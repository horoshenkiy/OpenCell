#pragma once

#include "../include/NvFlex.h"
#include "../include/NvFlexExt.h"

#include "../include/FruitExt.h"

#include <cereal/access.hpp>
#include "../include/serialize_types.h"

template <typename T>
class FruitNvFlexVector : public FruitVector<T> {

private:
	NvFlexLibrary* lib = nullptr;
	NvFlexBuffer* buffer = nullptr;

public:

	FruitNvFlexVector() {}
	
	FruitNvFlexVector(NvFlexLibrary* l, size_t size = 0) : lib(l) {
		this->mappedPtr = NULL;
		this->buffer = NULL;
		this->count = 0;
		this->capacity = 0;

		if (size) {
			resize(size);

			// resize implicitly maps, unmap initial allocation
			unmap();
		}
	}

	FruitNvFlexVector(NvFlexLibrary* l, const T* ptr, size_t size) : lib(l) {
		this->mappedPtr = NULL;
		this->buffer = NULL;
		this->count = 0;
		this->capacity = 0;

		assign(ptr, size);
		unmap();
	}

	~FruitNvFlexVector() {
		//destroy();
	}

	FruitBuffer GetBuffer() override {
		FruitBuffer fruitBuffer;
		fruitBuffer.SetBuffer(buffer);

		return fruitBuffer;
	}

	void init(size_t size) override {
		destroy();
		resize(size);
		unmap();
	}

	void destroy() override {
		if (mappedPtr)
			NvFlexUnmap(buffer);

		if (buffer)
			NvFlexFreeBuffer(buffer);

		mappedPtr = NULL;
		buffer = NULL;
		capacity = 0;
		count = 0;
	}

	void map(int flags = eNvFlexMapWait) {
		if (!buffer)
			return;

		assert(!mappedPtr);
		mappedPtr = (T*)NvFlexMap(buffer, flags);
	}

	void unmap() {
		if (!buffer)
			return;

		assert(mappedPtr);

		NvFlexUnmap(buffer);
		mappedPtr = 0;
	}

	const T& operator[](size_t index) const override {
		assert(mappedPtr);
		assert(index < count);

		return mappedPtr[index];
	}

	T& operator[](size_t index) override {
		assert(mappedPtr);
		assert(index < count);

		return mappedPtr[index];
	}

	T& get(size_t index) override {
		assert(mappedPtr);
		assert(index < count);

		return mappedPtr[index];
	}

	void set(size_t index, T value) {
		assert(mappedPtr);
		assert(index < count);

		mappedPtr[index] = value;
	}

	void push_back(const T& t) {
		assert(mappedPtr || !buffer);

		reserve(count + 1);

		// copy element
		mappedPtr[count++] = t;
	}

	void assign(const T* srcPtr, size_t newCount) override {
		assert(mappedPtr || !buffer);

		resize(newCount);
		memcpy(mappedPtr, srcPtr, newCount * sizeof(T));
	}

	void copyto(T* dest, size_t count) override {
		assert(mappedPtr);
		memcpy(dest, mappedPtr, sizeof(T)*count);
	}

	size_t size() const override { return count; }

	bool empty() const override { return size() == 0; }

	const T& back() const override {
		assert(mappedPtr);
		assert(!empty());

		return mappedPtr[count - 1];
	}

	void reserve(size_t minCapacity) override {
		if (minCapacity > capacity) {
			// growth factor of 1.5
			const size_t newCapacity = minCapacity * 3 / 2;

			NvFlexBuffer* newBuf = NvFlexAllocBuffer(lib, newCapacity, sizeof(T), eNvFlexBufferHost); //-V107

			// copy contents to new buffer			
			void* newPtr = NvFlexMap(newBuf, eNvFlexMapWait);
			memcpy(newPtr, mappedPtr, count * sizeof(T));

			// unmap old buffer, but leave new buffer mapped
			unmap();

			if (buffer)
				NvFlexFreeBuffer(buffer);

			// swap
			buffer = newBuf;
			mappedPtr = (T*)newPtr;
			capacity = newCapacity;
		}
	}

	// resizes mapped buffer and leaves new buffer mapped 
	void resize(size_t newCount) override {
		assert(mappedPtr || !buffer);

		reserve(newCount);

		// resize but do not initialize new entries
		count = newCount;
	}

	void resize(size_t newCount, const T& val) override {
		assert(mappedPtr || !buffer);

		const size_t startInit = count;
		const size_t endInit = newCount;

		resize(newCount);

		// init any new entries
		for (size_t i = startInit; i < endInit; ++i)
			mappedPtr[i] = val;
	}

	// need rewrite for Fruit Vector
	void erase(size_t start, size_t end) override {
		assert(mappedPtr || !buffer);

		if (end == count) {
			count = start;
		}
		else {
			memcpy(&mappedPtr[start], &mappedPtr[end], sizeof(T) * (count - end));
			count -= end - start;
		}
	}
};