#include "Misc.h"
#include "Glitch_Effect_Shader.h"
#include "Buffer.h"

GlitchEffectShader::GlitchEffectShader(ID3D11Device* device)
{
		// 入力レイアウト
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		ShaderManager* shader = ShaderManager::instance();
		shader->createVsFromCso(device, "Shader/Glitch_Effect_vs.cso", vertexxShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));
		shader->createPsFromCso(device, "Shader/Glitch_Effect_ps.cso", pixelShader.GetAddressOf());

		createBuffer<GlitchEffectShader::glitch_shader_constants>(device, glitchConstantBuffer.GetAddressOf());

		// ブレンドステート
		{
			D3D11_BLEND_DESC desc;
			::memset(&desc, 0, sizeof(desc));
			desc.AlphaToCoverageEnable = false;
			desc.IndependentBlendEnable = false;
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			device->CreateBlendState(&desc, blendState.GetAddressOf());
		}

		// 深度ステンシルステート
		{
			D3D11_DEPTH_STENCIL_DESC desc;
			::memset(&desc, 0, sizeof(desc));
			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

			device->CreateDepthStencilState(&desc, depthStencilState.GetAddressOf());
		}

		// ラスタライザーステート
		{
			D3D11_RASTERIZER_DESC desc;
			::memset(&desc, 0, sizeof(desc));
			desc.FrontCounterClockwise = false;
			desc.DepthBias = 0;
			desc.DepthBiasClamp = 0;
			desc.SlopeScaledDepthBias = 0;
			desc.DepthClipEnable = true;
			desc.ScissorEnable = false;
			desc.MultisampleEnable = true;
			desc.FillMode = D3D11_FILL_SOLID;
			desc.CullMode = D3D11_CULL_BACK;
			desc.AntialiasedLineEnable = false;

			device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
		}

		// サンプラステート
		{
			D3D11_SAMPLER_DESC desc;
			::memset(&desc, 0, sizeof(desc));
			desc.MipLODBias = 0.0f;
			desc.MaxAnisotropy = 1;
			desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			desc.MinLOD = -FLT_MAX;
			desc.MaxLOD = FLT_MAX;
			desc.BorderColor[0] = 1.0f;
			desc.BorderColor[1] = 1.0f;
			desc.BorderColor[2] = 1.0f;
			desc.BorderColor[3] = 1.0f;
			desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

			device->CreateSamplerState(&desc, samplerState.GetAddressOf());
		}
}

void GlitchEffectShader::Begin(ID3D11DeviceContext* immediate_context)
{
	immediate_context->VSSetShader(vertexxShader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixelShader.Get(), nullptr, 0);
	immediate_context->IASetInputLayout(inputLayout.Get());

	immediate_context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11Buffer* constantBuffers[] =
	{
		glitchConstantBuffer.Get(),
	};
	immediate_context->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
	immediate_context->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);

	const float blend_factor[4] = {1.0f,1.0f, 1.0f, 1.0f};
	immediate_context->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
	immediate_context->OMSetDepthStencilState(depthStencilState.Get(), 0);
	immediate_context->RSSetState(rasterizerState.Get());
	immediate_context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
}
void GlitchEffectShader::Draw(ID3D11DeviceContext* immediate_context, const RenderContext& rc, const Sprite* sprite)
{
	// 定数バッファにセットするためのデータを格納する構造体
	glitch_shader_constants glitch_shader_constants;
	glitch_shader_constants.brightness = rc.glitchData.brightness;
	glitch_shader_constants.center = rc.glitchData.center;
	glitch_shader_constants.density = rc.glitchData.density;
	glitch_shader_constants.extension = rc.glitchData.extension;
	glitch_shader_constants.glitch_mask_radius = rc.glitchData.glitch_mask_radius;
	glitch_shader_constants.glitch_sampling_count = rc.glitchData.glitch_sampling_count;
	glitch_shader_constants.line_impact = rc.glitchData.line_impact;
	glitch_shader_constants.rand_float = rc.glitchData.rand_float;
	glitch_shader_constants.shift = rc.glitchData.shift;
	glitch_shader_constants.time = rc.glitchData.time;
	glitch_shader_constants.uv_slider = rc.glitchData.uv_slider;
	glitch_shader_constants.x_shift = rc.glitchData.x_shift;
	glitch_shader_constants.x_shifting = rc.glitchData.x_shifting;
	glitch_shader_constants.y_shift = rc.glitchData.y_shift;
	glitch_shader_constants.y_shifting = rc.glitchData.y_shifting;
	glitch_shader_constants.gage = rc.glitchData.gage;
	glitch_shader_constants.rect_noise_width = rc.glitchData.rect_noise_width;
	glitch_shader_constants.rect_noise_height = rc.glitchData.rect_noise_height;
	glitch_shader_constants.rect_noise_strength = rc.glitchData.rect_noise_strength;
	glitch_shader_constants.flash_frequency = rc.glitchData.flash_frequency;
	glitch_shader_constants.flash_strength = rc.glitchData.flash_strength;

	// 定数バッファの更新
	immediate_context->UpdateSubresource(glitchConstantBuffer.Get(), 0, nullptr, &glitch_shader_constants, 0, 0);

	// 頂点バッファの設定
	UINT stride = sizeof(Sprite::vertex);  // Sprite::vertexのサイズに合わせて設定
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, sprite->GetVertexBuffer().GetAddressOf(), &stride, &offset);

	// シェーダーリソースの設定（テクスチャなど）
	immediate_context->PSSetShaderResources(0, 1, sprite->GetShaderResourceView().GetAddressOf());

	// 描画コマンドを発行
	immediate_context->Draw(4, 0);
}

void GlitchEffectShader::End(ID3D11DeviceContext* immediate_context)
{
	immediate_context->VSSetShader(nullptr, nullptr, 0);
	immediate_context->PSSetShader(nullptr, nullptr, 0);
	immediate_context->IASetInputLayout(nullptr);
}
