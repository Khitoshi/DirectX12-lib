#pragma once
#include <string>
#include <assimp\types.h>


namespace {
    /// <summary>
    /// aiString��std::string�ɕϊ�����
    /// </summary>
    /// <param name="ai_str">�ϊ�������������</param>
    /// <returns>
    /// wstring�^�ɕϊ����ꂽai_str
    /// </returns>
    std::wstring Convert(const aiString& ai_str)
    {
        wchar_t temp[256] = {};
        size_t  size;
        mbstowcs_s(&size, temp, ai_str.C_Str(), 256);
        return std::wstring(temp);
    }

}
