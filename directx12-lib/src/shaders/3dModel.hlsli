// 頂点シェーダーの出力
struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float3 tangent : TANGENT;
    float4 color : COLOR;

    float4 diffuse_color : DIFFUSE_COLOR;
};
