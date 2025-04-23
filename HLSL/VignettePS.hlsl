#include "FullScreenQuad.hlsli"

cbuffer VIGNETTE_CONSTANT_BUFFER : register(b6)
{
	float4 vignette_color;
	float2 vignette_center;
	float vignette_intensity;
	float vignette_smoothness;

	float vignette_rounded;
	float vignette_roundness;
	float vignette_opacity;
	float vignette_dumy;
};

Texture2D scene_map : register(t0);
SamplerState linear_sampler_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	float2 scene_map_size;
	scene_map.GetDimensions(scene_map_size.x, scene_map_size.y);
	float4 color = scene_map.Sample(linear_sampler_state, pin.texcoord);

	float2 d = abs(pin.texcoord - vignette_center)* (vignette_intensity);
	d.x *= lerp(1.0f, scene_map_size.x / scene_map_size.y, vignette_rounded);

	d = pow(saturate(d), vignette_roundness);
	half vignette_factor = pow(saturate(1.0f - dot(d, d)), vignette_smoothness);
	color.rgb *= lerp(vignette_color.rgb, (float3)1.0f, vignette_factor);
	
	return color;
}