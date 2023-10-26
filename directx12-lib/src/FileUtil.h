#pragma once
#include <string>
#include <filesystem>
#include <direct.h>


/// <summary>
/// ファイルパスからディレクトリパスを取得する
/// path = "C:\test\test.txt"の場合、"C:\test\"を返す
/// </summary>
/// <param name="path"></param>
/// <returns>ディレクトリパス</returns>
std::string GetDirFromPath(const std::string& path)
{
	const std::string::size_type pos1 = path.find_last_of('/');
	const std::string::size_type pos2 = path.find_last_of('\\');
	const std::string::size_type pos = (pos1 == std::string::npos) ? pos2 : (pos2 == std::string::npos) ? pos1 : max(pos1, pos2);

	return (pos == std::string::npos) ? std::string() : path.substr(0, pos + 1);
}


/// <summary>
/// ファイルパスからファイル名を取得する
/// path = "C:\test\test.txt"の場合、"test.txt"を返す
/// </summary>
/// <param name="path"></param>
/// <returns>ファイル名</returns>
std::string GetFileNameFromPath(const std::string& path)
{
	const std::string::size_type pos1 = path.find_last_of('/');
	const std::string::size_type pos2 = path.find_last_of('\\');
	const std::string::size_type pos = (pos1 == std::string::npos) ? pos2 : (pos2 == std::string::npos) ? pos1 : max(pos1, pos2);

	return (pos == std::string::npos) ? path : path.substr(pos + 1);
}


/// <summary>
/// ファイルもしくはディレクトリが存在するかどうか
/// </summary>
/// <param name="name"></param>
/// <returns>
/// true:存在する
/// false:存在しない
/// </returns>
bool DoesPathExist(const std::string& name)
{
	return std::filesystem::exists(name);
}


//ディレクトリが存在するかどうか
//true:成功 false:失敗
bool CreateDirectoryIfNotExists(const std::string& path)
{
	if (DoesPathExist(path))return true;
	return _mkdir(path.c_str()) >= 0 ? true : false;
}
