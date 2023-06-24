#include "./Triangle.hlsli"


    // ���_�V�F�[�_�[�ւ̓��͒��_�\����
struct VSInput
{
    float4 pos : POSITION; //XYZW
    float4 color : COLOR; //RGBA
};

// ���_�V�F�[�_�[
// 1. �����͕ϊ��O�̒��_���
// 2. �߂�l�͕ϊ���̒��_���
VSOutput VSMain(VSInput In)
{
    VSOutput result = (VSOutput) 0;
    result.pos = In.pos;
    result.color = In.color;
    return result;
}