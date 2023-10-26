struct VSPositionOutput{
    float4 pos : SV_POSITION;
    float4 world_pos : TEXCOORD0;
};

struct VSNormalOutput{
    float4 pos : SV_POSITION;
    float4 normal : NORMAL;
};

struct VSColorOutput{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

struct VSAlbedoOutput{
    float4 pos : SV_POSITION;
    float4 albedo : DIFFUSE_COLOR;
};