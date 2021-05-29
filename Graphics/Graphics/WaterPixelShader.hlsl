struct PixelInput
{
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float3 worldPosition : POSITION1;
};

Texture2D normalMap : register(t0);
SamplerState staticSampler : register(s0);

float4 WaterPixelShader(PixelInput _in) : SV_Target
{
	float heigth = _in.worldPosition.y / 10;
	float white;

	white = sign(heigth - 0.7);
	white = saturate(white);

	return float4(white * heigth, white * heigth, heigth, heigth);
}