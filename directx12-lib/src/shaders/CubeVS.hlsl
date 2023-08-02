#include "./Cube.hlsli"

// 頂点シェーダーへの入力頂点構造体
struct VSInput
{
    float3 pos : POSITION; //XYZ
    float2 texcoord : TEXCOORD; //UV
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 model;
    float4x4 view;
    float4x4 projection;
}

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    float4 pos = float4(input.pos, 1.0f); // 頂点座標を4次元に拡張
    //オブジェクトをワールド空間に配置
    pos = mul(pos, model); // モデル行列を適用
    //オブジェクトをビュー空間に配置
    pos = mul(pos, view); //ビュー行列適用
    //オブジェクトを3d空間から2d空間に配置
    pos = mul(pos, projection); //プロジェクション行列適用
    output.position = pos;
    output.texcoord = input.texcoord;


    return output;
}