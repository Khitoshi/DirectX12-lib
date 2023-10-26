#pragma once
#include <string>
#include <memory>
#include <map>

class ResourceManager
{
private:
	ResourceManager() :
		resources_()
	{};
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
public:
	static ResourceManager* getInstance() {
		static ResourceManager instance;
		return &instance;
	}

	template <typename T>
	void registerResource(const std::string& name, std::shared_ptr<T> resource) {
		auto it = this->resources_.find(name);
		if (it != this->resources_.end()) {
			throw std::runtime_error("Resource already exists: " + name);
			return;
		}

		this->resources_[name] = std::move(resource);
	}

	template<typename T>
	std::shared_ptr<T> getResource(const std::string& name) {
		//–¼‘O‚ÅŒŸõ
		auto it = this->resources_.find(name);
		if (it != this->resources_.end()) {
			return std::static_pointer_cast<T>(it->second);
		}

		throw std::runtime_error("Resource not found: " + name);
	}

private:
	std::map<std::string, std::shared_ptr<void>> resources_;
};
