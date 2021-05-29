struct LightConstantBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDir;
	float padding;
	float4 specularColor;
	float3 cameraPos;
};

ConstantBuffer<LightConstantBuffer> lightCB : register(b0);


struct PixelInput
{
	float4 col : COLOR;
	float3 normal : NORMAL;
	float3 worldPos : POSITION1;
};

float4 hsv_to_rgb(float h, float s, float v, float a)
{
	float c = v * s;
	h = h * 6 % 6.0;// mod((h * 6.0), 6.0);
	float x = c * (1.0 - abs(/*mod(h, 2.0) - 1.0*/ (h % 2) - 1));
	float4 color;

	if (0.0 <= h && h < 1.0) {
		color = float4(c, x, 0.0, a);
	}
	else if (1.0 <= h && h < 2.0) {
		color = float4(x, c, 0.0, a);
	}
	else if (2.0 <= h && h < 3.0) {
		color = float4(0.0, c, x, a);
	}
	else if (3.0 <= h && h < 4.0) {
		color = float4(0.0, x, c, a);
	}
	else if (4.0 <= h && h < 5.0) {
		color = float4(x, 0.0, c, a);
	}
	else if (5.0 <= h && h < 6.0) {
		color = float4(c, 0.0, x, a);
	}
	else {
		color = float4(0.0, 0.0, 0.0, a);
	}

	color.rgb += v - c;

	return color;
}

float sdCircle(float2 p, float r)
{
	return length(p) - r;
}

float3 hue2rgb(float hue) {
	return clamp(
		abs((hue * 6.0 + float3(0.0, 4.0, 2.0) % 6.0) - 3.0) - 1.0,
		0.0, 1.0);
}

float4 SimplePixelShader(PixelInput _in) : SV_Target
{
	float x = _in.worldPos.x;
	float y = _in.worldPos.y;
	
	float r = length(float2(x,y));
	float angle = atan2(x,y) -  r / 360 + 2.0;
	float intensity = 0.5 + 0.25 * sin(15.0 * angle - 5);
	
	return float4(hsv_to_rgb(angle / 3.1415926535897932384626433832795, intensity, 1.0, 0.5));
}