cbuffer cameraData : register(b0)
{
	float4x4 view;
	float4x4 proj;
	float4x4 vp;
}

struct VSInput
{
	float4 position : POSITION;
	float4 color: COLOR;
	float2 tex : TEXCOORD0;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float4 color: COLOR;
	float2 tex : TEXCOORD0;
};

Texture2D fontTexture;
SamplerState SampleType;

PSInput VSMain(VSInput input)
{
	PSInput output;

	output.position = input.position;
	output.color = input.color;
	output.tex = input.tex;

	return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	float4 color;

	// Sample the texture pixel at this location.
	color = fontTexture.Sample(SampleType, input.tex);

	// If the color is black on the texture then treat this pixel as transparent.
	if (color.r == 0.0f)
	{
		color.a = 0.0f;
	}

	// If the color is other than black on the texture then this is a pixel in the font so draw it using the font pixel color.
	else
	{
		color.a = 1.0f;
		color = color * input.color;
	}

	return color;
}