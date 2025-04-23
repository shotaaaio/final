#include "FullScreenQuad.hlsli"
#include "FilterFunctions.hlsli"

cbuffer COLOR_FILTER : register(b7)
{
	float4 filterColor;
	float hueShift;
	float saturation;
	float brightness;
	float dummy;
};

Texture2D color_map : register(t0);
SamplerState color_sampler_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = color_map.Sample(color_sampler_state, pin.texcoord) * filterColor;
	
	color.rgb = RGB2HSV(color.rgb);

	color.r += hueShift;
	color.g *= saturation;
	color.b *= brightness;

	color.rgb = HSV2RGB(color.rgb);

	return color;
}