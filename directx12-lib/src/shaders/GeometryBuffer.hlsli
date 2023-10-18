struct VSNormalOutput{
    float4 pos : SV_POSITION;
    float4 normal : NORMAL;
};

struct VSTexcoordOutput{
    float4 pos : SV_POSITION;
    float4 texcoord : TEXCOORD;
};

struct VSTangentOutput{
    float4 pos : SV_POSITION;
    float4 tangent : TANGENT;
};

struct VSColorOutput{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};
