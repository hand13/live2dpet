struct PS_INPUT {
	float4 vPos: SV_POSITION;
};

float4 main(PS_INPUT k):SV_TARGET
{
	// return In.vPos;
	return float4(1.0f,1.0f,0,1.0f);
	// return MyTex.Sample(MySampler,In.texCoord);
}