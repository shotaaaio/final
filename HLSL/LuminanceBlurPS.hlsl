#include "FullScreenQuad.hlsli"

SamplerState sampler0 : register(s0);
Texture2D texture_maps[4] : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
	uint mip_level = 0, width, height, number_of_levels;
	texture_maps[1].GetDimensions(mip_level, width, height, number_of_levels);

	float4 color = texture_maps[0].Sample(sampler0, pin.texcoord);
	float alpha = color.a;

	if (blur_use_flag > 0) return color;

	float3 blur_color = 0;
	float gaussian_kernel_total = 0;

	const int gaussian_half_kernel_size = 3;
	const float gaussian_sigma = 1.0f;
	[unroll]
	for (int x = -gaussian_half_kernel_size; x <= gaussian_half_kernel_size; x += 1)
	{
		[unroll]
		for (int y = -gaussian_half_kernel_size; y <= gaussian_half_kernel_size; y += 1)
		{
			float gaussian_kernel = exp(-(x * x + y * y) / (2.0f * gaussian_sigma * gaussian_sigma)) /
				(2.0f * 3.14159265358979 * gaussian_sigma * gaussian_sigma);
			blur_color += texture_maps[1].Sample(sampler0, pin.texcoord +
				float2(x * 1.0f / width, y * 1.0f / height)).rgb * gaussian_kernel;
			gaussian_kernel_total += gaussian_kernel;
		}
	}
	blur_color /= gaussian_kernel_total;
	const float bloom_intensity = 1.0f;
	color.rgb += blur_color * bloom_intensity;

	return float4(color.rgb, alpha);
}