#include "./3dModel.hlsli"

// ���_�V�F�[�_�[�ւ̓��͒��_�\����
struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float3 tangent : TANGENT;
    float4 color : COLOR;
};

cbuffer ConstantBuffer : register(b0)
{
    float4x4 model; // ���f���s��
    float4x4 view; // �r���[�s��
    float4x4 projection; // �v���W�F�N�V�����s��
}


cbuffer MaterialConf : register(b1)
{
    float4 diffuse_color;
}

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    float4 pos = float4(input.position, 1.0f); // ���_���W��4�����Ɋg��

    //�I�u�W�F�N�g�����[���h��Ԃɔz�u
    pos = mul(pos, model); // ���f���s���K�p
    //�I�u�W�F�N�g���r���[��Ԃɔz�u
    pos = mul(pos, view); //�r���[�s��K�p
    //�I�u�W�F�N�g��3d��Ԃ���2d��Ԃɔz�u
    pos = mul(pos, projection); //�v���W�F�N�V�����s��K�p

    output.position = pos;
    output.normal = input.normal;
    output.texcoord = input.texcoord;
    output.tangent = input.tangent;
    output.color = input.color;
    output.diffuse_color = diffuse_color;

    return output;
}