struct mvpMatrix
{
	matrix mvp; // Model - View - Projection
};

struct worldMatrix 
{
	matrix world;
};

ConstantBuffer<mvpMatrix> mvpCB : register(b0);
ConstantBuffer<worldMatrix> worldCB : register(b1);

struct VertexInput
{
	float3 position : POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct VertexOutput
{
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float3 worldPosition : POSITION1;
	float4 position : SV_Position;
};

Texture2D splatmap : register(t0);
SamplerState staticSampler : register(s0);

VertexOutput TerrainVertexShader(VertexInput _in)
{
	VertexOutput o;

	o.color = _in.color;
	o.uv = _in.uv;
	
	float heigth = splatmap.SampleLevel(staticSampler, _in.uv, 0).b;
	float3 position = _in.position + _in.normal * heigth * 10;

	o.position = mul(mvpCB.mvp, float4(position, 1));
	o.normal = mul(worldCB.world, float4(_in.normal, 1)).xyz;
	o.worldPosition = mul(worldCB.world, float4(position, 1)).xyz;

	return o;
}