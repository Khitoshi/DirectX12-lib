
#include "./Rotation.hlsli"

// ���_�V�F�[�_�[�ւ̓��͒��_�\����
struct VSInput
{
    float4 pos : POSITION; //XYZW
    float4 color : COLOR; //RGBA
};

VSOutput VSMain(VSInput input) 
{
    VSOutput output;
    output.position = input.pos;
    output.color = input.color;
    
	return output;
}