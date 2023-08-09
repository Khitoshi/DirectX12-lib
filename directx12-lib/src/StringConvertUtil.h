#pragma once
#include <string>
#include <assimp\types.h>


namespace {
    /// <summary>
    /// aiStringをstd::stringに変換する
    /// </summary>
    /// <param name="ai_str">変換したい文字列</param>
    /// <returns>
    /// wstring型に変換されたai_str
    /// </returns>
    std::wstring Convert(const aiString& ai_str)
    {
        wchar_t temp[256] = {};
        size_t  size;
        mbstowcs_s(&size, temp, ai_str.C_Str(), 256);
        return std::wstring(temp);
    }

}
