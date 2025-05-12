#include "Texture.h"
#include "misc.h"

#include <WICTextureLoader.h>
#include<string.h>
using namespace DirectX;
using namespace Microsoft::WRL;

#include <memory>
#include <filesystem>
#include <DDSTextureLoader.h>
#include"DeviceManager.h"
#include"DirectXTex.h"

using namespace std;

TextureManager::~TextureManager()
{
	releaseAllTextures();
}

// �t�@�C������e�N�X�`�����쐬
HRESULT TextureManager::loadTextureFromFile(
	ID3D11Device* device, 
	const wchar_t* filename, 
	ID3D11ShaderResourceView** shader_resource_view,
	D3D11_TEXTURE2D_DESC* texture2d_desc
)
{
	HRESULT hr{ S_OK };

	// �摜�t�@�C������V�F�[�_���\�[�X�r���[�𐶐�
	ComPtr<ID3D11Resource> resource;

	auto it = resources.find(filename);
	if (it != resources.end())
	{
		*shader_resource_view = it->second.Get();
		(*shader_resource_view)->AddRef();
		(*shader_resource_view)->GetResource(resource.GetAddressOf());
	}
	else
	{
		filesystem::path filename_p(filename);
		wstring ext = filename_p.extension().wstring();
		if (ext == L".dds")
		{
			hr = CreateDDSTextureFromFile(device, filename_p.c_str(), resource.GetAddressOf(), shader_resource_view);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}
		else if (ext == L".tga")
		{
			DirectX::ScratchImage image;
			DirectX::TexMetadata metadata;
			hr = DirectX::LoadFromTGAFile(filename_p.c_str(), &metadata, image);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

			hr = DirectX::CreateShaderResourceView(device, image.GetImages(), image.GetImageCount(), metadata, shader_resource_view);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

			(*shader_resource_view)->GetResource(resource.GetAddressOf());
		}
		else
		{
			hr = CreateWICTextureFromFile(device, filename_p.c_str(), resource.GetAddressOf(), shader_resource_view);
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}
		resources.insert({ filename, *shader_resource_view });
	}
	
	if (texture2d_desc == nullptr)
		return hr;

	// �e�N�X�`���̈�ʏ����擾�i���Ƃ������Ƃ��j
	ComPtr<ID3D11Texture2D> texture2d;
	hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	texture2d->GetDesc(texture2d_desc);

	return hr;
}

// �_�~�[�e�N�X�`���̍쐬
HRESULT TextureManager::makeDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view, DWORD value, UINT dimension)
{
	HRESULT hr{ S_OK };

	// �e�N�X�`�� 2D �����p�̐ݒ�
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d_desc.Width = dimension;
	texture2d_desc.Height = dimension;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	
	// �F���𕝍������ɑ΂��Đݒ肵�Ă���
	size_t texels = dimension * dimension;
	unique_ptr<DWORD[]> sysmem{ std::make_unique<DWORD[]>(texels) };
	for (size_t i = 0; i < texels; ++i) sysmem[i] = value;

	// �F���z��ƐF���z��̃T�C�Y��ݒ�
	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = sysmem.get();
	subresource_data.SysMemPitch = sizeof(DWORD) * dimension;

	// �e�N�X�`�� 2D �𐶐�
	ComPtr<ID3D11Texture2D> texture2d;
	hr = device->CreateTexture2D(&texture2d_desc, &subresource_data, &texture2d);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// �V�F�[�_�[���\�[�X�r���[�����p�̐ݒ�
	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
	shader_resource_view_desc.Format = texture2d_desc.Format;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MipLevels = 1;

	// ���������e�N�X�`�� 2D ���g���ăV�F�[�_�[���\�[�X�r���[�𐶐�
	hr = device->CreateShaderResourceView(texture2d.Get(), &shader_resource_view_desc, shader_resource_view);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	return hr;
}

// �쐬�����S�Ẵe�N�X�`�����J��
void TextureManager::releaseAllTextures()
{
	resources.clear();
}

void TextureManager::createShaderResourceView(ID3D11ShaderResourceView *srv)
{
	ID3D11Device* device = DeviceManager::instance()->getDevice();
	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<ID3D11Texture2D> buffer{};
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d_desc.Width = 1280;
	texture2d_desc.Height = 720;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2d_desc, NULL, buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	hr = device->CreateShaderResourceView(buffer.Get(), NULL, &srv);
}