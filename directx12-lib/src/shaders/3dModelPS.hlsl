#include "./3dModel.hlsli"

Texture2D<float4> tex : register(t0); //0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v��

float4 PSMain(VSOutput input) : SV_TARGET
{
    //return float4(tex.Sample(smp, input.texcoord));
    return float4(1.0, 0, 0, 1.0f);

}