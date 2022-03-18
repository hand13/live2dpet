sampler MySampler;
Texture2D MyTex;
struct PS_INPUT {
	float4 vPos: SV_POSITION;
	float2 texCoord:TEXCOORD;
};

float4 main(PS_INPUT In):SV_TARGET
{
	return MyTex.Sample(MySampler,In.texCoord);
}