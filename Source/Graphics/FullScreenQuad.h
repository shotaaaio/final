#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>

#include "Graphics/Sprite.h"

class FullScreenQuad
{
public:
	FullScreenQuad(ID3D11Device* device);
	~FullScreenQuad() = default;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> embedded_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> embedded_pixel_shader;

public:
	void blit(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView** shader_resource_view, uint32_t start_slot,
		uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader = nullptr);
};