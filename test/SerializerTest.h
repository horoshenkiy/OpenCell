#pragma once

#include <Serializer.h>

class SerializerTest {
private:
	Serializer serializer;

public:

	SerializerTest() = default;

	// for single
	/////////////////////////////////////////////////////
	template<class Archive, typename T>
	void Save(Archive &archive, T &t) {
		serializer.Save(archive, t);
	}

	template<class Archive, typename T>
	void Load(Archive &archive, T &t) {
		serializer.Load(archive, t);
	}
	
	// for massive 
	/////////////////////////////////////////////////////
	template<class Archive, typename T>
	void Save(Archive &archive, T *t, int size) {
		archive.saveBinary(t, sizeof(T) * size);
	}

	template<class Archive, typename T>
	void Load(Archive &archive, T **t, int size) {
		*t = new T[size];
		archive.loadBinary(*t, sizeof(T) * size);
	}

};