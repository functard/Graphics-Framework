struct mvp
{
	matrix mvp;
};

struct worldMatrix
{
	matrix world;
};

ConstantBuffer<mvp> cbMvp : register(b0);
ConstantBuffer<worldMatrix> cbWorld : register(b1);

struct VertexInput
{
	float3 pos : POSITION;
	float4 col : COLOR;
	float3 normal : NORMAL;
};

struct VertexOutput
{
	float4 col : COLOR;
	float3 normal : NORMAL;
	float3 worldPos : POSITION1;
	float4 pos : SV_Position;
};

VertexOutput SimpleVertexShader(VertexInput _in)
{
	VertexOutput o;

	o.col = _in.col;
	// Object Space => Screen Space
	// Model Space => Projection Space
	//_in.pos += float3(sin(_in.pos.x), 0, 0);
	o.pos = mul(cbMvp.mvp, float4(_in.pos, 1));
	o.normal = mul(cbWorld.world, float4(_in.normal, 1)).xyz;
	o.worldPos = mul(cbWorld.world, float4(_in.pos, 1)).xyz;

	return o;
}