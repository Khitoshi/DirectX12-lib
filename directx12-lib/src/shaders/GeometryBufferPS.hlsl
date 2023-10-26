#include "./GeometryBuffer.hlsli"


/*
*
*	Position
*
*/
float4 PSPosition(VSPositionOutput o) : SV_TARGET0
{
    return o.world_pos;
}

/*
*
*	Normal
*
*/
float4 PSNormal(VSNormalOutput o) : SV_TARGET0
{
    return o.normal;
}

/*
*
*	Color
*
*/
float4 PSColor(VSColorOutput o) : SV_TARGET0
{
    return o.color;
}

/*
*
*   Albedo
*
*/
float4 PSAlbedo(VSAlbedoOutput o) : SV_TARGET0
{
    return o.albedo;
}