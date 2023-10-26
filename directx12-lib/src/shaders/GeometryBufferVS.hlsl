
#include "./GeometryBuffer.hlsli"

cbuffer MVW_Matrix : register(b0)
{
    float4x4 model;
    float4x4 view;
    float4x4 projection;
}


/*
*
*	Normal
*
*/
VSPositionOutput VSPosition(float3 position : POSITION)
{
    float4 pos = float4(position, 1.0f);
    //ワールド空間に配置
    pos = mul(pos, model);
    //ビュー空間に配置
    pos = mul(pos, view);
    //3d空間から2d空間に配置
    pos = mul(pos, projection);
    
    VSPositionOutput o;
    o.world_pos = pos;
    o.pos = pos;
    return o;
}


/*
*
*	Normal
*
*/
VSNormalOutput VSNormal(float3 position : POSITION, float4 normal : NORMAL)
{
    float4 pos = float4(position, 1.0f);
    //ワールド空間に配置
    pos = mul(pos, model);
    //ビュー空間に配置
    pos = mul(pos, view);
    //3d空間から2d空間に配置
    pos = mul(pos, projection);

    VSNormalOutput o;
    o.pos = pos;
    o.normal = normal;
    return o;
}

/*
*
*	Color
*
*/
VSColorOutput VSColor(float3 position : POSITION, float4 color : COLOR)
{
    float4 pos = float4(position, 1.0f);
    //ワールド空間に配置
    pos = mul(pos, model);
    //ビュー空間に配置
    pos = mul(pos, view);
    //3d空間から2d空間に配置
    pos = mul(pos, projection);

    VSColorOutput o;
    o.pos = pos;
    o.color = color;
    return o;
}


/*
*
*	Albedo
*
*/
VSAlbedoOutput VSAlbedo(float3 position : POSITION, float4 aldedo : DIFFUSE_COLOR)
{
    float4 pos = float4(position, 1.0f);
    //ワールド空間に配置
    pos = mul(pos, model);
    //ビュー空間に配置
    pos = mul(pos, view);
    //3d空間から2d空間に配置
    pos = mul(pos, projection);

    VSAlbedoOutput o;
    o.pos = pos;
    o.albedo = aldedo;
    return o;
}