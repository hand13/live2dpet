cbuffer MVP : register(b0)
{
	matrix mWorld;
	matrix view;
	matrix projection;
};

struct VS_INPUT
{
	float3 vPos: POSITION;
	float2 texCoord: TEXCOORD;
};

struct PS_INPUT {
	float4 vPos: SV_POSITION;
	float2 texCoord:TEXCOORD;
};

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT OUTPUT;

	float4 pos = float4(input.vPos,1.0f);
	// pos = mul(pos,mWorld);
	// pos = mul(pos,view);
	// pos = mul(pos,projection);

	OUTPUT.vPos = pos;
	OUTPUT.texCoord = input.texCoord;
	return OUTPUT;
};