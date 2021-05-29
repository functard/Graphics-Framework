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
	float3 pos : POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
};

struct VertexOutput
{
	float4 color : COLOR;
	float3 normal : NORMAL;
	float4 pos : SV_Position;
};

VertexOutput CelShadingVertexShader(VertexInput _in)
{
	VertexOutput o;

	o.color = _in.color;
	o.pos = mul(mvpMatrixCB.mvp, float4(_in.pos, 1));
	o.normal = mul(worldMatrixCB.world, float4(_in.normal, 1)).xyz;

	return o;
}