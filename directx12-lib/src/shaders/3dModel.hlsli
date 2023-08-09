// 頂点シェーダーの出力
struct VSOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float3 normal : NORMAL;
    //float4 color : COLOR;
    float3 tangent : TANGENT;
};

