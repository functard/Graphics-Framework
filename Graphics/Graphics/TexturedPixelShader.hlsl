struct LightConstantBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
	float4 specularColor;
	float3 cameraPosition;
};

ConstantBuffer<LightConstantBuffer> lightData : register(b0);

Texture2D mainTexture : register(t0);
SamplerState staticSampler : register(s0);

struct PixelInput
{
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float3 worldPosition : POSITION1;
};


#define r(a) float2x2(cos(a + .78*float4(0,6,2,0)))
float4 TexturedPixelShader(PixelInput _in) : SV_Target
{
	//float4 c;
	//float2 u;

	////float4 n = float4(u, sqrt(max(0., 1. - dot(u, u))), 1);

	//c.xyz = _in.worldPosition;
	//u = (u + u - c.xy) / c.y * 1.1;

	//float4 n = float4(u, sqrt(max(0., 1. - dot(u,u))), 1);

	//n.xy *= r(0.8);
	//n.xz *= r(-.6);
	//n.xy *= r(n.z * 4. + .3);
	//c = (n * .6 + .9) * max(.2, n.z) * mainTexture.Sample(staticSampler, n.xy * .1).r;
	//c.r *= .6;

	//return c;

	float2 uv = _in.worldPosition.xy;
	uv = fmod(uv, 1.0f);
	float4 color = mainTexture.Sample(staticSampler, uv);
	return mainTexture.Sample(staticSampler, 1 - _in.uv);
}