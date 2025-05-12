#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

// �X�v���C�g
class Sprite
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	D3D11_TEXTURE2D_DESC texture2d_desc;

	bool isLoadFile = true;
public:
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	/// <param name="filename">�t�@�C����</param>
	Sprite(ID3D11Device* device, const wchar_t* filename);
	Sprite(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>shader_resource_view);
	virtual ~Sprite();

	// �`�揈��
	void render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh,
		float r, float g, float b, float a,
		float angle,
		float sx, float sy, float sw, float sh
	);

	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw,float dh);

	float getTextureWidth() { return static_cast<float>(texture2d_desc.Width); }
	float getTextureHeight() { return static_cast<float>(texture2d_desc.Height); }

	void setShaderResourceView(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv, int texWidth, int texHeight);
private:
	static void rotate(float& x, float& y, float cx, float cy, float angle)
	{
		x -= cx;
		y -= cy;

		float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
		float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
		float tx{ x }, ty{ y };
		x = cos * tx + -sin * ty;
		y = sin * tx + cos * ty;

		x += cx;
		y += cy;
	};
};