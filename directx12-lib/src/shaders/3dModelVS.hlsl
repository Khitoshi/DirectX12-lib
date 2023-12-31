#include "./3dModel.hlsli"


struct VSInput
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
    float3 tangent : TANGENT;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 model; // モデル行列
    float4x4 view; // ビュー行列
    float4x4 projection; // プロジェクション行列
}

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    float4 pos = float4(input.position, 1.0f); // 頂点座標を4次元に拡張

    //オブジェクトをワールド空間に配置
    pos = mul(pos, model); // モデル行列を適用
    //オブジェクトをビュー空間に配置
    pos = mul(pos, view); //ビュー行列適用
    //オブジェクトを3d空間から2d空間に配置
    pos = mul(pos, projection); //プロジェクション行列適用


    output.position = pos;
    output.texcoord = input.texcoord;
    output.tangent = input.tangent;
    return output;
}