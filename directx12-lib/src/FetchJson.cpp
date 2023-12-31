#include "Json.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>


/// jsonを読み込み構造を返す
nlohmann::json Json::load(const char* file_path)
{
	std::ifstream ifs(file_path);
	if (!ifs.is_open()) {
		throw std::runtime_error(("FAILED Json::load %ds", file_path));
		return nullptr;
	}

	nlohmann::json json;
	ifs >> json;
	ifs.close();

	return json;
}
