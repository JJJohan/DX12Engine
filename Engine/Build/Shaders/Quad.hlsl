Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

cbuffer CBuffer : register(b0)
{
	float4x4 mvp[64];
};

struct VSInput
{
	float4 pos : POSITION;
	float4 col: COLOR;
	float3 uv:  TEXCOORD;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
	float2 uv : TEXCOORD;
};

PSInput VSMain(VSInput i)
{
	PSInput o;

	i.pos.w = 1.0f;
	o.pos = i.pos;
	o.pos = mul(o.pos, mvp[i.uv.z]);

	o.col = i.col;
	o.uv = i.uv.xy;

	return o;
}

float4 PSMain(PSInput i) : SV_TARGET
{
	return g_texture.Sample(g_sampler, i.uv) * i.col;
}