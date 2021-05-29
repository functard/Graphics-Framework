struct mvpMatrix
{
	matrix mvp;
};

struct worldMatrix
{
	matrix world;
};

ConstantBuffer<mvpMatrix> mvpMatrixCB : register(b0);
ConstantBuffer<worldMatrix> worldMatrixCB : register(b1);

struct VertexInput
{
	float3 position : POSITION;
	float4 color : COLOR;
	float3 normal: NORMAL;
	float2 uv : TEXCOORD;
};

struct VertexOutput
{
	float4 color : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float4 position : SV_Position;
};

VertexOutput HeightBlendingVertexShader(VertexInput _in)
{
	VertexOutput o;

	o.color = _in.color;
	o.normal = mul(worldMatrixCB.world, float4(_in.normal, 1)).xyz;
	o.position = mul(mvpMatrixCB.mvp, float4(_in.position, 1));
	o.uv = _in.uv;

	return o;
}