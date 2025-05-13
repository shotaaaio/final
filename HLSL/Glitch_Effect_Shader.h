#pragma once
#include "d3d11.h"
#include "memory"
#include "wrl.h"
#include <memory>
#include "Graphics/Sprite.h"
#include "grap"

class GlitchEffectShader
{
public:
	GlitchEffectShader(ID3D11Device* device);
	~GlitchEffectShader() {};

	void Render(ID3D11DeviceContext* idc,Render);


private:
	struct glitch_shader_constants
	{
		float				time = 0.0f; //�o�ߎ���
		float				density = 0.0f; //���x
		float				shift = 0.0f; //���炷��
		float				line_impact = 10.0f; //��{���̉e���x

		DirectX::XMFLOAT2	x_shift = { 0.0f,0.0f }; //x�̉����ǂ̒��x���炷�̂�
		DirectX::XMFLOAT2	y_shift = { 0.0f,0.0f }; //y�̉����ǂ̒��x���炷�̂�

		float				rand_float = 0.0f; //�����_����
		float				x_shifting = 0.0f; //�F������x�̈ʒu�����炷
		float				y_shifting = 0.0f; //�F������y�̈ʒu�����炷    
		float				extension = 0.0f; //uv�̊g��

		float				uv_slider = 0.0f; //����ɗ���
		float				brightness = 0.0f; //���邳
		float				glitch_mask_radius = 0; //�}�X�N���a
		int					glitch_sampling_count = 1; //��

		float gage;
		DirectX::XMFLOAT2	center = { 0.5f,0.5f }; //���S
		float dummy;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>Glitch_Constant_Buffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> Glitch_Vertex_Shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> Glitch_Pixel_Shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> Glitch_Input_Layout;
	Microsoft::WRL::ComPtr<ID3D11BlendState> Glitch_Blend_State;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>Glitch_Rasterizer_State;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>Glitch_DepthStencil_State;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>Glitch_Sampler_State;
};