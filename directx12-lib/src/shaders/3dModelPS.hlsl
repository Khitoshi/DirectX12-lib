#include "./3dModel.hlsli"

Texture2D<float4> tex : register(t0); 

//Texture2D<float4> normal_tex : register(t1); 
//Texture2D<float4> color_tex : register(t2); 
//Texture2D<float4> albedo_tex : register(t3); 

SamplerState smp : register(s0); //0�ԃX���b�g�ɐݒ肳�ꂽ�T���v��

float4 PSMain(VSOutput input) : SV_TARGET
{ 
    // �X�N���[����Ԃ̍��W���v�Z
    //float2 screenCoord = input.position.xy / input.position.w;

    //float4 normal = normal_tex.Sample(smp, screenCoord);
    //float4 color = color_tex.Sample(smp, screenCoord);
    //float4 albedo = albedo_tex.Sample(smp, screenCoord);

    return float4(tex.Sample(smp, input.texcoord));
}