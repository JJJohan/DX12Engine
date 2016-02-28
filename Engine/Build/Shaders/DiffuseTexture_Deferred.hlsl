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

struct PSOutput
{
	float4 pos : SV_Target0;
	float4 col : SV_Target1;
	float4 uv : SV_Target2;
};

PSInput VSMain(VSInput i)
{
	PSInput o;

	i.pos.w = 1.0;
	o.pos = i.pos;
	o.pos = mul(o.pos, mvp[i.uv.z]);

	o.col = i.col;
	o.uv = i.uv.xy;

	return o;
}

PSOutput PSMain(PSInput i)
{
	PSOutput o;

	o.pos = i.pos;
	o.col = g_texture.Sample(g_sampler, i.uv) * i.col;
	o.uv = float4(i.uv.x, i.uv.y, 0.0, 1.0);

	return o;
}