#pragma once
#include <map>
#include <vector>

template<typename Type, class Param>
class Cache
{
public:

	Cache() {}
	~Cache() {}

	void addDescriptor(Type type, Param* param) {
		cache[type].push_back(param);
	}

	//Ä¶¬
	void regenerate() {

	}

private:
	std::map<Type, std::vector<Param*>>cache;

};