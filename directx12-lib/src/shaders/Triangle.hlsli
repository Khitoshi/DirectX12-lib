
// ���_�V�F�[�_�[�ւ̓��͒��_�\����
struct VSInput
{
    float4 pos : POSITION;
    float3 color : COLOR;
    float2 uv : TEXCOORD; // ���_����UV���W�̃f�[�^�����������Ă���
};

// ���_�V�F�[�_�[�̏o��
struct VSOutput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
};

float4x4 g_worldMatrix : register(b0);
sampler g_sampler : register(s0);