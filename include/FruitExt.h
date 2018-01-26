#pragma once 

#include "FruitExtTypes.h"

template <typename T>
class FruitVector
{
protected:

	T* mappedPtr;
	size_t count;
	size_t capacity;

public:

	//getters
	virtual FruitBuffer GetBuffer() = 0;

	// reinitialize the vector leaving it unmapped
	virtual void init(size_t size) = 0;
	virtual void destroy() = 0;

	// mapping and unmapping
	virtual void map(int flags = eNvFlexMapWait) = 0;
	virtual void unmap() = 0;

	// operator []
	virtual const T& operator[](size_t index) const = 0;
	virtual T& operator[](size_t index) = 0;
	
	// get and set for buffer
	virtual T& get(size_t index) = 0;
	virtual void set(size_t index, T value) = 0;

	virtual void push_back(const T& t) = 0;
	virtual void assign(const T* srcPtr, size_t newCount) = 0;

	virtual void copyto(T* dest, size_t count) = 0;

	virtual size_t size() const { return count; }
	virtual bool empty() const { return size() == 0; }

	virtual const T& back() const = 0;

	virtual void reserve(size_t minCapacity) = 0;

	// resizes mapped buffer and leaves new buffer mapped 
	virtual void resize(size_t newCount) = 0;

	virtual void resize(size_t newCount, const T& val) = 0;

	// need rewrite for Fruit Vector
	virtual void erase(size_t start, size_t end) = 0;

	// operators
	bool operator==(const FruitVector<T> &rVector) const {
		if (this->size() != rVector.size())
			return false;

		for (size_t i = 0; i < this->size(); i++) {
			if (this->mappedPtr[i] != rVector[i])
				return false;
		}

		return true;
	}

	bool operator!=(const FruitVector<T> &rVector) const {
		return !(*this == rVector);
	}
};