#include "GaussianBlurShader.h"
#include "Buffer.h"
GaussianBlurShader::GaussianBlurShader(ID3D11Device* device)
{
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	ShaderManager* shader = ShaderManager::instance();
	shader->createVsFromCso(device, "Shader/GaussianBlur_vs.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));
	shader->createPsFromCso(device, "Shader/GaussianBlur_ps.cso", pixelShader.GetAddressOf());

	createBuffer<GaussianBlurShader::CBFilter>(device, filterConstantBuffer.GetAddressOf());
	
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

		device->CreateDepthStencilState(&desc, depthStancilState.GetAddressOf());

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
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		device->CreateSamplerState(&desc, samplerState.GetAddressOf());
	}
}

void GaussianBlurShader::Begin(ID3D11DeviceContext* immdiaite_context)
{
	immdiaite_context->VSSetShader(vertexShader.Get(), nullptr, 0);
	immdiaite_context->PSSetShader(pixelShader.Get(), nullptr, 0);
	immdiaite_context->IASetInputLayout(inputLayout.Get());

	immdiaite_context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	immdiaite_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11Buffer* constantBuffers[] =
	{
		filterConstantBuffer.Get(),
	};
	immdiaite_context->VSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);
	immdiaite_context->PSSetConstantBuffers(0, ARRAYSIZE(constantBuffers), constantBuffers);

	const float blend_factor[4] = { 1.0f,1.0f,1.0f,1.0f };
	immdiaite_context->OMSetBlendState(blendState.Get(), blend_factor, 0xFFFFFFFF);
	immdiaite_context->OMSetDepthStencilState(depthStancilState.Get(),0);
	immdiaite_context->RSSetState(rasterizerState.Get());
	immdiaite_context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
}

void GaussianBlurShader::Draw(ID3D11DeviceContext* immdiaite_context, const RenderContext& rc, const Sprite* sprite)
{
	//ガウスフィルター値の計算
	CBFilter cbFilter;
	CalcGaussianFliter(cbFilter, rc.gaussianFilterData);
	immdiaite_context->UpdateSubresource(filterConstantBuffer.Get(), 0, 0, &cbFilter, 0, 0);
	
	UINT stride = sizeof(Sprite::vertex);
	UINT offset = 0;
	immdiaite_context->IASetVertexBuffers(0, 1, sprite->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	immdiaite_context->PSSetShaderResources(0, 1, sprite->GetShaderResourceView().GetAddressOf());
	immdiaite_context->Draw(4, 0);
}

void GaussianBlurShader::End(ID3D11DeviceContext* immdiaite_context)
{
	immdiaite_context->VSSetShader(nullptr, nullptr, 0);
	immdiaite_context->PSSetShader(nullptr, nullptr, 0);
	immdiaite_context->IASetInputLayout(nullptr);
}

void GaussianBlurShader::CalcGaussianFliter(CBFilter& cbFilter, const GaussianFilterData& gaussianFilterData)
{
	int kernelSize = gaussianFilterData.kernelSize;
	//偶数の場合は奇数に修正
	if (kernelSize % 2 == 0) kernelSize++;
	kernelSize = max(MaxKernelSize - 1, kernelSize);
	cbFilter.KernelSize = kernelSize;
	cbFilter.texcel.x = 1.0f / gaussianFilterData.textureSize.x;
	cbFilter.texcel.y = 1.0f / gaussianFilterData.textureSize.y;

	float deviationPow2 = 2.0f * gaussianFilterData.deviation * gaussianFilterData.deviation;
	float sum = 0.0f;
	int id = 0;
	for (int y = -kernelSize / 2; y <= kernelSize / 2; ++y)
	{
		for (int x = -kernelSize / 2; x <= kernelSize / 2; ++x)
		{
			cbFilter.weights[id].x = (float)x;
			cbFilter.weights[id].y = (float)y;
			cbFilter.weights[id].z = (1.0f / (2.0f * DirectX::XM_PI * deviationPow2)) * expf(-(x * x + y * y) / (2.0f * deviationPow2));

			sum += cbFilter.weights[id].z;
			id++;
		}
	}
	for (int i = 0; i < cbFilter.KernelSize * cbFilter.KernelSize;i++)
	{
		cbFilter.weights[i].z /= sum;
	}
}


