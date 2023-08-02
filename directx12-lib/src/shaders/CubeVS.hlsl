#include "./Cube.hlsli"

// ���_�V�F�[�_�[�ւ̓��͒��_�\����
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
    float4 pos = float4(input.pos, 1.0f); // ���_���W��4�����Ɋg��
    //�I�u�W�F�N�g�����[���h��Ԃɔz�u
    pos = mul(pos, model); // ���f���s���K�p
    //�I�u�W�F�N�g���r���[��Ԃɔz�u
    pos = mul(pos, view); //�r���[�s��K�p
    //�I�u�W�F�N�g��3d��Ԃ���2d��Ԃɔz�u
    pos = mul(pos, projection); //�v���W�F�N�V�����s��K�p
    output.position = pos;
    output.texcoord = input.texcoord;


    return output;
}