Texture2D MainTexture : register(t0);
Texture2D RenderOutput : register(t1);

SamplerState Sampler : register(s0);

cbuffer FrameData : register(b0)
{
	float4x4 mvp[64];
	bool depthEnabled;
};

cbuffer SceneData : register(b1)
{
	float nearClip;
	float farClip;
};

struct VSInput
{
	float4 pos : POSITION;
	float4 albedo : COLOR;
	float3 uv : TEXCOORD0;
	float3 normal : NORMAL;
	float2 roughness_metallic : TEXCOORD1;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float4 albedo : COLOR;
	float2 uv : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float2 depth: TEXCOORD2;
	float2 roughness_metallic : TEXCOORD3;
};

struct PSOutput
{
	float4 albedo_roughness : SV_Target0;
	float4 normal_metallic : SV_Target1;
	float2 uv : SV_Target2;
	float depth : SV_Target3;
};

PSInput VSMain(VSInput i)
{
	PSInput o;

	i.pos.w = 1.0f;
	o.pos = mul(i.pos, mvp[i.uv.z]);

	o.albedo = i.albedo;
	o.normal = i.normal;
	o.uv = i.uv.xy;
	o.depth = float2(o.pos.z, o.pos.w);
	o.roughness_metallic = i.roughness_metallic;

	return o;
}

PSOutput PSMain(PSInput i)
{
	PSOutput o;

	float depth = i.depth.x / i.depth.y;
	float4 albedo = MainTexture.Sample(Sampler, i.uv);
	albedo.rgb *= i.albedo.rgb;

	o.albedo_roughness = float4(albedo.r, albedo.g, albedo.b, i.roughness_metallic.x);
	o.normal_metallic = float4(i.normal.x, i.normal.y, i.normal.z, i.roughness_metallic.y);
	o.uv = float2(i.uv.x, i.uv.y);
	o.depth = depth;

	return o;
}