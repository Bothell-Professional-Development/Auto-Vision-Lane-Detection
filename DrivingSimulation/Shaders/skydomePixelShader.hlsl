cbuffer GradientBuffer
{
    float4 apexColor;
    float4 centerColor;
    float radius;
    float3 padding;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 domePosition : TEXCOORD0;
};

float4 SkyDomePixelShader(PixelInputType input) : SV_TARGET
{
    float height;
    float4 outputColor;

    height = input.domePosition.y / radius;

    if(height < 0.0)
    {
        height = 0.0f;
    }

    outputColor = lerp(centerColor, apexColor, height);
    
    return outputColor;
}
