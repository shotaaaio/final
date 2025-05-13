#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <string>
#include <map>

class TextureManager
{
private:
	TextureManager() {}
	~TextureManager();

public:
	static TextureManager* instance()
	{
		static TextureManager inst;
		return &inst;
	}

	// �t�@�C������e�N�X�`�����쐬
	HRESULT loadTextureFromFile(
		ID3D11Device* device,
		const wchar_t* filename,
		ID3D11ShaderResourceView** shader_resource_view,
		D3D11_TEXTURE2D_DESC* texture2d_desc
		);

	// �_�~�[�e�N�X�`���̍쐬
	HRESULT makeDummyTexture(
		ID3D11Device* device,
		ID3D11ShaderResourceView** shader_resource_view,
		DWORD value,
		UINT dimension);

	// �쐬�����S�Ẵe�N�X�`�����J��
	void releaseAllTextures();

private:
	std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> resources;
};

