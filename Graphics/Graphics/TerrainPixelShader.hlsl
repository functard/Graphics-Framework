struct scale
{
	float x;
};

ConstantBuffer<scale> scaleCB : register(b0);

struct PixelInput
{
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float3 worldPosition : POSITION1;
};

Texture2D splatmap : register(t0);
Texture2D atlas : register(t1);
SamplerState wrapSampler : register(s0);
SamplerState clampSampler : register(s1);

float4 TerrainPixelShader(PixelInput _in) : SV_Target
{
	int tileSize = scaleCB.x;
	float2 worldUV = fmod(_in.worldPosition.xz, tileSize);
	worldUV /= tileSize;

	float2 uv = splatmap.Sample(wrapSampler, worldUV).xy;
	uv += abs(worldUV) / 4;
	return atlas.Sample(clampSampler, uv);
}