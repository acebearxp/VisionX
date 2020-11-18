
cbuffer ConstantBuffer : register(b0)
{
    matrix WorldViewProjection;
    uint Textured;
}

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

VS_OUTPUT main(float4 Pos : POSITION, float4 Color : COLOR)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Pos = mul(Pos, WorldViewProjection);
    output.Color = Color;
    return output;
}