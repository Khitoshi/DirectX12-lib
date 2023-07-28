#pragma once
#include <unordered_map>

/// <summary>
/// �n�b�V���l���v�Z����
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="seed">�n�b�V���l</param>
/// <param name="v"></param>
template <class T>
inline void hash_combine(std::size_t& seed, const T& t)
{
    std::hash<T> hasher;
    seed ^= hasher(t) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
};