#include "./Triangle.hlsli"

// �s�N�Z���V�F�[�_�[
float4 PSmain(VSOutput vsOut) : SV_Target0
{
    // step-5 �e�N�X�`���J���[���T���v�����O���ĕԂ�
    return float4(vsOut.color, 1.0f);
}