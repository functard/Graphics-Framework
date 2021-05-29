struct LightData
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
	float4 specularColor;
	float3 cameraPos;
};

ConstantBuffer<LightData> lightDataCB : register(b0);

struct PixelInput
{
	float4 color : COLOR;
	float3 normal : NORMAL;
};

float sdCircle(float2 p, float r)
{
	return length(p) - r;
}

float4 CelShadingPixelShader(PixelInput _in) : SV_Target
{
	return float4(0,0,0,1);
}