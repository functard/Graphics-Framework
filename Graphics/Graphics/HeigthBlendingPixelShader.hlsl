

struct PixelInput
{
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

Texture2D stoneAlbedo : register(t0);
Texture2D stoneHeigth : register(t1);
Texture2D sandAlbedo : register(t2);
Texture2D sandHeigth : register(t3);

SamplerState staticSampler : register(s0);

float4 HeigthBlending(float4 _colorOne, float _heightOne, float4 _colorTwo, float _heigthTwo)
{
	float maxHeigth = max(_heightOne, _heigthTwo);
	float blendValue = 0.7f;

	float differenceOne = max(1 - (maxHeigth - _heightOne) - blendValue, 0);
	float differenceTwo = max(1 - (maxHeigth - _heigthTwo) - blendValue, 0);

	return (_colorOne * differenceOne + _colorTwo * differenceTwo) / (differenceOne + differenceTwo);
}

float4 HeightBlendingPixelShader(PixelInput _in) : SV_Target
{
	float stoneH = stoneHeigth.Sample(staticSampler, _in.uv).r;
	float sandH = sandHeigth.Sample(staticSampler, _in.uv).r + 0.2f;
	float4 stoneColor = stoneAlbedo.Sample(staticSampler, _in.uv);
	float4 sandColor = sandAlbedo.Sample(staticSampler, _in.uv);

	return HeigthBlending(stoneColor, stoneH, sandColor, sandH);
	//return stoneAlbedo.Sample(staticSampler, _in.uv) * stoneH + sandAlbedo.Sample(staticSampler, _in.uv) * sandH;
}

