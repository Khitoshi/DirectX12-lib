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
		this->cache_[type].push_back(param);
	}

	//Ä¶¬
	void regenerate() {
		for (auto& typeVectorPair : cache_) {
			for (Param* param : typeVectorPair.second) {
				if (param) {
					param->init();
				}
			}
		}
	}

private:
	std::map<Type, std::vector<Param*>>cache_;

};