#pragma once 

#include "FruitExtTypes.h"

template <typename T>
class FruitVector
{
protected:

	T* mappedPtr;
	int count;
	int capacity;

public:

	//getters
	virtual FruitBuffer GetBuffer() = 0;

	// reinitialize the vector leaving it unmapped
	virtual void init(int size) = 0;
	virtual void destroy() = 0;

	// mapping and unmapping
	virtual void map(int flags = eNvFlexMapWait) = 0;
	virtual void unmap() = 0;

	// operator []
	virtual const T& operator[](int index) const = 0;
	virtual T& operator[](int index) = 0;
	
	// get and set for buffer
	virtual T& get(int index) = 0;
	virtual void set(int index, T value) = 0;

	virtual void push_back(const T& t) = 0;
	virtual void assign(const T* srcPtr, int newCount) = 0;

	virtual void copyto(T* dest, int count) = 0;

	virtual int size() const { return count; }
	virtual bool empty() const { return size() == 0; }

	virtual const T& back() const = 0;

	virtual void reserve(int minCapacity) = 0;

	// resizes mapped buffer and leaves new buffer mapped 
	virtual void resize(int newCount) = 0;

	virtual void resize(int newCount, const T& val) = 0;

	// need rewrite for Fruit Vector
	virtual void erase(int start, int end) = 0;
};