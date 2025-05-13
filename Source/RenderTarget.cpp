#include "stdlib.h"
#include "Graphics/GraphicsManager.h"
#include "RenderTarget.h"

#include "Misc.h"
RenderTarget::RenderTarget(UINT width, UINT height, DXGI_FORMAT format)
{
	ID3D11Device* device = GraphicsManager::instance()->GetDevice();

	texture2dDesc = {};
	texture2dDesc.Width = width;
	texture2dDesc.Height = height;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = format;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags = 0;
	texture2dDesc.MiscFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
	device->CreateTexture2D(&texture2dDesc, 0, texture2d.ReleaseAndGetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resorce_view_desc = {};
	shader_resorce_view_desc.Format = texture2dDesc.Format;
	shader_resorce_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resorce_view_desc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(texture2d.Get(), &shader_resorce_view_desc, shaderResorceView.ReleaseAndGetAddressOf());
	

	D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc{};
	render_target_view_desc.Format = texture2dDesc.Format;
	render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	device->CreateRenderTargetView(texture2d.Get(), &render_target_view_desc,
		renderTargetView.ReleaseAndGetAddressOf());
}