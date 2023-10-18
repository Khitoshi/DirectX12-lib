#include "./GeometryBuffer.hlsli"
/*
*
*	Poisition
*
*/
float4 PSPosition(float4 pos : SV_POSITION) : SV_TARGET0
{
    return pos;
}



/*
*
*	Normal
*
*/
float4 PSNormal(VSNormalOutput o) : SV_TARGET1
{
    return o.normal;
}



/*
*
*	Texcoord
*
*/
float4 PSTexcoord(VSTexcoordOutput o) : SV_TARGET2
{
    return o.texcoord;
}



/*
*
*	Tangent
*
*/
float4 PSTangent(VSTangentOutput o) : SV_TARGET3
{
    return o.tangent;
}



/*
*
*	Color
*
*/
float4 PSColor(VSColorOutput o) : SV_TARGET4
{
    return o.color;
}