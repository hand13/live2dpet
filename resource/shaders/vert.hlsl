struct VS_INPUT
{
	float3 vPos: POSITION;
};

struct PS_INPUT {
	float4 vPos: SV_POSITION;
};

PS_INPUT main(VS_INPUT input)
{
	PS_INPUT OUTPUT;
	float4 pos = float4(input.vPos,1.0f);
	OUTPUT.vPos = pos;
	return OUTPUT;
};