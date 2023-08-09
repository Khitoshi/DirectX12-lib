#pragma once
#include <string>
#include <assimp\types.h>


namespace {
    /// <summary>
    /// aiString‚ðstd::string‚É•ÏŠ·‚·‚é
    /// </summary>
    /// <param name="ai_str">•ÏŠ·‚µ‚½‚¢•¶Žš—ñ</param>
    /// <returns>
    /// wstringŒ^‚É•ÏŠ·‚³‚ê‚½ai_str
    /// </returns>
    std::wstring Convert(const aiString& ai_str)
    {
        wchar_t temp[256] = {};
        size_t  size;
        mbstowcs_s(&size, temp, ai_str.C_Str(), 256);
        return std::wstring(temp);
    }

}
