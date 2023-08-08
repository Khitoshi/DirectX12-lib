#include "d3dx12.h"

#ifdef _DEBUG
#pragma comment(lib,"assimp-vc142-mtd.lib")
#else
#pragma comment(lib,"assimp-vc142-md.lib")
#endif // _DEBUG

class Model
{
public:
    Model() {};
    ~Model() {};

    void init(ID3D12Device* device, const char* model_file_path);
    void draw();
private:

};
