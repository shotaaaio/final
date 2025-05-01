#include "FullScreenQuad.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0: register(s0);

float RGB2Luminance(float3 rgb)
{
	static const float3 luminance_value = float3(0.299, 0.587, 0.114);
	return dot(luminance_value, rgb);
}

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = texture0.Sample(sampler0, pin.texcoord);
	float alpha = color.a;
	
	if (luminance_use_flag > 0) return color;

	float luminance = RGB2Luminance(color.rgb);
	float contribution = max(0, luminance - threshold);

	color.rgb *= luminanceColor;
	color.rgb *= contribution / luminance;
	color.rgb *= intensity;

	
	return color;
}