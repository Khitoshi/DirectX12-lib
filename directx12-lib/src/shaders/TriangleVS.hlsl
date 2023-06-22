#include "./Triangle.hlsli"

// ���_�V�F�[�_�[
// 1. �����͕ϊ��O�̒��_���
// 2. �߂�l�͕ϊ���̒��_���
VSOutput VSmain(VSInput In)
{
    VSOutput vsOut = (VSOutput) 0;
    vsOut.pos = mul(g_worldMatrix, In.pos);
    vsOut.uv = In.uv;
    vsOut.color = In.color; //
    return vsOut;
}