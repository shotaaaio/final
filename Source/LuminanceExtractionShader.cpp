#include "LuminanceExtractionShader.h"

LumianceExtractionShader::LumianceExtractionShader(ID3D11Device* device)
{
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	ShaderManager* shader = ShaderManager::instance();
	shader->createVsFromCso(device, "Shader/Glitch_Effect_vs.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));
	shader->createPsFromCso(device, "Shader/Glitch_Effect_ps.cso", pixelShader.GetAddressOf());

	createBuffer<LumianceExtractionShader::CBLuminanceExtrantiion>(device, luminanceExtractionConstantBuffer.GetAddressOf());

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

void LumianceExtractionShader::Begin(ID3D11DeviceContext* immediate_context)
{
	immediate_context->VSSetShader(vertexShader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixelShader.Get(), nullptr, 0);
	immediate_context->IASetInputLayout(inputLayout.Get());

	immediate_context->IASetIndexBuffer(nullptr,DXGI_FORMAT_UNKNOWN,0);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11Buffer* constantBuffers[] =
	{
		luminanceExtractionConstantBuffer.Get(),
	};
	immediate_context->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
	immediate_context->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);

	const float blend_factor[4] = { 1.0f,1.0f, 1.0f, 1.0f };
	immediate_context->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
	immediate_context->OMSetDepthStencilState(depthStencilState.Get(), 0);
	immediate_context->RSSetState(rasterizerState.Get());
	immediate_context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
}

void LumianceExtractionShader::Draw(ID3D11DeviceContext* immediate_context, const RenderContext& rc, const Sprite* sprite)
{
	CBLuminanceExtrantiion cbLuminanceExtrantiion;
	cbLuminanceExtrantiion.threshold = rc.luminanceExtractionData.threshold;
	cbLuminanceExtrantiion.intensity = rc.luminanceExtractionData.intensity;
	immediate_context->UpdateSubresource(luminanceExtractionConstantBuffer.Get(),0,0,&cbLuminanceExtrantiion,0,0);

	UINT stride = sizeof(Sprite::vertex);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0,1,sprite->GetVertexBuffer().GetAddressOf(),&stride,&offset);
	immediate_context->PSSetShaderResources(0,1,sprite->GetShaderResourceView().GetAddressOf());
	immediate_context->Draw(4,0);
}

void LumianceExtractionShader::End(ID3D11DeviceContext* immediate_context)
{
	immediate_context->VSSetShader(nullptr,nullptr,0);
	immediate_context->PSSetShader(nullptr,nullptr,0);
	immediate_context->IASetInputLayout(nullptr);
}
