
#include "./Rotation.hlsli"

//�A�t�B���ϊ��s��
cbuffer ConstantBuffer : register(b0)
{
    float4x4 transformMatrix;
}

// ���_�V�F�[�_�[�ւ̓��͒��_�\����
struct VSInput
{
    float4 pos : POSITION; //XYZW
    float4 color : COLOR; //RGBA
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    //output.position = input.pos;
    output.position = mul(input.pos, transformMatrix);
    //output.position = mul(transformMatrix, input.pos);
    output.color = input.color;

    return output;
}