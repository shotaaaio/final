#pragma once

#include <d3d11.h>
#include <wrl.h>

#define OBJECT_BUFFER 0;

/// <summary>
/// �萔�o�b�t�@�̍쐬
/// </summary>
/// <param name="buffer">��������萔�o�b�t�@</param>
template<typename Ty>
inline HRESULT createBuffer(ID3D11Device* device, ID3D11Buffer** buffer)
{
	// �V�[���p�萔�o�b�t�@
	D3D11_BUFFER_DESC desc;
	::memset(&desc, 0, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.ByteWidth = sizeof(Ty);
	desc.StructureByteStride = 0;

	return device->CreateBuffer(&desc, 0, buffer);
}

/// <summary>
/// �萔�o�b�t�@�̍X�V�Ɛݒ�
/// </summary>
/// <param name="dc">�f�o�C�X�R���e�L�X�g</param>
/// <param name="buffer">�X�V�����萔�o�b�t�@</param>
/// <param name="constants">�X�V����萔�̃f�[�^</param>
template<typename Ty>
inline void bindBuffer(ID3D11DeviceContext* dc, int slot, ID3D11Buffer** buffer, Ty* constants)
{
	// �萔�o�b�t�@�̓o�^
	dc->UpdateSubresource(*buffer, 0, 0, constants, 0, 0);
	dc->VSSetConstantBuffers(slot, 1, buffer);
	dc->PSSetConstantBuffers(slot, 1, buffer);
}