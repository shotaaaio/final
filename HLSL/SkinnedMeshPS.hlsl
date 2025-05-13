#include "SkinnedMesh.hlsli"

SamplerState samplerState : register(s0);
Texture2D textureMap : register(t0);
Texture2D normalMap : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = textureMap.Sample(samplerState, pin.texcoord);
    float alpha = color.a;
	
    float3 normal = normalMap.Sample(samplerState, pin.texcoord).xyz * 2.0f - 1.0f;
    float3x3 CM = float3x3(pin.tangent.xyz, pin.binormal.xyz, pin.world_normal.xyz);
    float3 N = normalize(mul(normal,CM));
    
    float3 L = normalize(-light_direction.xyz);
    float3 diffuse = color.rgb * max(0, dot(N, L) * 0.5f + 0.5f);
    
    return float4(diffuse, alpha) * pin.color;
}