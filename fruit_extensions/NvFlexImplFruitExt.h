#pragma once

#include "../include/NvFlex.h"
#include "../include/NvFlexExt.h"

#include "../include/FruitExt.h"

template <typename T>
class FruitNvFlexVector : public FruitVector<T> {

private:
	NvFlexLibrary* lib;
	NvFlexBuffer* buffer;

public:

	FruitNvFlexVector() {};
	
	FruitNvFlexVector(NvFlexLibrary* l, int size = 0) : lib(l) {
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

	FruitNvFlexVector(NvFlexLibrary* l, const T* ptr, int size) : lib(l) {
		this->mappedPtr = NULL;
		this->buffer = NULL;
		this->count = 0;
		this->capacity = 0;

		assign(ptr, size);
		unmap();
	}

	~FruitNvFlexVector() {
		destroy();
	}

	FruitBuffer GetBuffer() {
		FruitBuffer fruitBuffer;
		fruitBuffer.SetBuffer(buffer);

		return fruitBuffer;
	}

	void init(int size) {
		destroy();
		resize(size);
		unmap();
	}

	void destroy() {
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

	const T& operator[](int index) const {
		assert(mappedPtr);
		assert(index < count);

		return mappedPtr[index];
	}

	T& operator[](int index) {
		assert(mappedPtr);
		assert(index < count);

		return mappedPtr[index];
	}

	T& get(int index) {
		assert(mappedPtr);
		assert(index < count);

		return mappedPtr[index];
	}

	void set(int index, T value) {
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

	void assign(const T* srcPtr, int newCount) {
		assert(mappedPtr || !buffer);

		resize(newCount);
		memcpy(mappedPtr, srcPtr, newCount * sizeof(T));
	}

	void copyto(T* dest, int count)
	{
		assert(mappedPtr);

		memcpy(dest, mappedPtr, sizeof(T)*count);
	}

	int size() const { return count; }

	bool empty() const { return size() == 0; }

	const T& back() const {
		assert(mappedPtr);
		assert(!empty());

		return mappedPtr[count - 1];
	}

	void reserve(int minCapacity) {
		if (minCapacity > capacity) {
			// growth factor of 1.5
			const int newCapacity = minCapacity * 3 / 2;

			NvFlexBuffer* newBuf = NvFlexAllocBuffer(lib, newCapacity, sizeof(T), eNvFlexBufferHost);

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
	void resize(int newCount) {
		assert(mappedPtr || !buffer);

		reserve(newCount);

		// resize but do not initialize new entries
		count = newCount;
	}

	void resize(int newCount, const T& val) {
		assert(mappedPtr || !buffer);

		const int startInit = count;
		const int endInit = newCount;

		resize(newCount);

		// init any new entries
		for (int i = startInit; i < endInit; ++i)
			mappedPtr[i] = val;
	}

	// need rewrite for Fruit Vector
	void erase(int start, int end) {
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