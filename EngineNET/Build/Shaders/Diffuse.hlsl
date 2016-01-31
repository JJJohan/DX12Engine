cbuffer cameraData : register(b0)
{
	float4x4 view;
	float4x4 proj;
	float4x4 vp;
}

struct VSInput
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PSInput VSMain(VSInput input)
{
	PSInput output;

	input.position.w = 1.0f;
	output.position = input.position;
	output.position = mul(output.position, vp);

	output.color = input.color;

	return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return input.color;
}