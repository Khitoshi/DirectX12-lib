#pragma once
#include <string>
#include <filesystem>
#include <direct.h>


/// <summary>
/// �t�@�C���p�X����f�B���N�g���p�X���擾����
/// path = "C:\test\test.txt"�̏ꍇ�A"C:\test\"��Ԃ�
/// </summary>
/// <param name="path"></param>
/// <returns>�f�B���N�g���p�X</returns>
std::string GetDirFromPath(const std::string& path)
{
	const std::string::size_type pos1 = path.find_last_of('/');
	const std::string::size_type pos2 = path.find_last_of('\\');
	const std::string::size_type pos = (pos1 == std::string::npos) ? pos2 : (pos2 == std::string::npos) ? pos1 : max(pos1, pos2);

	return (pos == std::string::npos) ? std::string() : path.substr(0, pos + 1);
}


/// <summary>
/// �t�@�C���p�X����t�@�C�������擾����
/// path = "C:\test\test.txt"�̏ꍇ�A"test.txt"��Ԃ�
/// </summary>
/// <param name="path"></param>
/// <returns>�t�@�C����</returns>
std::string GetFileNameFromPath(const std::string& path)
{
	const std::string::size_type pos1 = path.find_last_of('/');
	const std::string::size_type pos2 = path.find_last_of('\\');
	const std::string::size_type pos = (pos1 == std::string::npos) ? pos2 : (pos2 == std::string::npos) ? pos1 : max(pos1, pos2);

	return (pos == std::string::npos) ? path : path.substr(pos + 1);
}


/// <summary>
/// �t�@�C���������̓f�B���N�g�������݂��邩�ǂ���
/// </summary>
/// <param name="name"></param>
/// <returns>
/// true:���݂���
/// false:���݂��Ȃ�
/// </returns>
bool DoesPathExist(const std::string& name)
{
	return std::filesystem::exists(name);
}


//�f�B���N�g�������݂��邩�ǂ���
//true:���� false:���s
bool CreateDirectoryIfNotExists(const std::string& path)
{
	if (DoesPathExist(path))return true;
	return _mkdir(path.c_str()) >= 0 ? true : false;
}
