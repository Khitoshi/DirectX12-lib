
#include "./GeometryBuffer.hlsli"

/*
*
*	Poisition
*
*/
float4 VSPosition(float4 pos : POSITION):SV_POSITION
{
    float4 normalized_pos = (pos + 1.0f) / 2.0f;
    //float4 normalized_pos = normalize(pos);
    return normalized_pos;
}



/*
*
*	Normal
*
*/
VSNormalOutput VSNormal(float4 pos : POSITION, float4 normal : NORMAL)
{
    VSNormalOutput o;
    //o.pos = pos;
    o.pos = (pos + 1.0f) / 2.0f;
    o.normal = normal;
    return o;
}



/*
*
*	Texcoord
*
*/
VSTexcoordOutput VSTexcoord(float4 pos : POSITION, float4 texcoord : TEXCOORD)
{
    VSTexcoordOutput o;
    //o.pos = pos;
    o.pos = (pos + 1.0f) / 2.0f;
    o.texcoord = texcoord;
    return o;
}



/*
*
*	Tangent
*
*/
VSTangentOutput VSTangent(float4 pos : POSITION, float4 tangent : TANGENT)
{
    VSTangentOutput o;
    //o.pos = pos;
    o.pos = (pos + 1.0f) / 2.0f;
    o.tangent = tangent;
    return o;
}



/*
*
*	Color
*
*/
VSColorOutput VSColor(float4 pos : POSITION, float4 color : COLOR)
{
    VSColorOutput o;
    //o.pos = pos;
    o.pos = (pos + 1.0f) / 2.0f;
    o.color = color;
    return o;
}