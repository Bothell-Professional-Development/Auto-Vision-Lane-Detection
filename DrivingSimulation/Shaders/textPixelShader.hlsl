Texture2D shaderTexture;
SamplerState SampleType;

cbuffer PixelBuffer
{
    float4 pixelColor;
}

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 TextPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    
    if(textureColor.r == 0.0f)
    {
        textureColor.a = 0.0f;
    }
    else
    {
        textureColor.a = 1.0f;
        textureColor = textureColor * pixelColor; 
    }

    return textureColor;
}