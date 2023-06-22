
// 頂点シェーダーへの入力頂点構造体
struct VSInput
{
    float4 pos : POSITION;
    float3 color : COLOR;
    float2 uv : TEXCOORD; // 頂点からUV座標のデータを引っ張ってくる
};

// 頂点シェーダーの出力
struct VSOutput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
};

float4x4 g_worldMatrix : register(b0);
sampler g_sampler : register(s0);