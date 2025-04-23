struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

cbuffer LUMINANCE_EXTRACT : register(b5)
{
	float3 luminanceColor;
	float threshold;
	float intensity;
	int luminance_use_flag;
	int blur_use_flag;
	int luminancedummy;
};