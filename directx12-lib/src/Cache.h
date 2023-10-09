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
	void regenerate(Type type, ID3D12Device* device) {
		for (Param* param : this->cache_[type]) {
			if (param) {
				param->init(device);
			}
		}

	}

	void release(Type type) {
		for (Param* param : this->cache_[type]) {
			if (param) {
				param->release();
			}
		}

		//ƒLƒƒƒbƒVƒ…‚©‚çíœ
		this->cache_.erase(type);
	}

private:
	std::map<Type, std::vector<Param*>>cache_;

};