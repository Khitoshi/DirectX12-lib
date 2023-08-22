#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>

struct Mesh;

class AssimpLoader
{
private:
    AssimpLoader() {};
    ~AssimpLoader() {};
public:
    static std::vector<Mesh> Load(const char* model_file_path, bool inverse_u = false, bool inverse_v = false);
};
