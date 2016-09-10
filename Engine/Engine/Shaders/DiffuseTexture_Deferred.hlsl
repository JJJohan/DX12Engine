Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

cbuffer FrameData : register(b0)
{
	float4x4 mvp[64];
};

cbuffer ObjectData : register(b1)
{
	bool depthEnabled;
};

cbuffer GlobalData : register(b2)
{
	float nearClip;
	float farClip;
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
	float4 depth : SV_Target3;
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

PSOutput PSMain(PSInput i)
{
	PSOutput o;

   	float depth = 2.0f * log(i.pos.w / nearClip) / log(farClip / nearClip) - 1.0f; 
    	depth = depth * i.pos.w;


	o.pos = i.pos;
	o.col = g_texture.Sample(g_sampler, i.uv) * i.col;
	o.uv = float4(i.uv.x, i.uv.y, 0.0f, 1.0f);
	o.depth = float4(depth, depth, depth, 1.0f);

	return o;
}