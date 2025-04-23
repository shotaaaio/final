#include "FullScreenQuad.hlsli"

SamplerState sampler_state0 : register(s0);
Texture2D texture_map : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
	return texture_map.Sample(sampler_state0, pin.texcoord);
}