#pragma once
#include "ShaderManager.h"
#include "Shader.h"

class GlitchEffectShader : public SpriteShader
{
public:
	GlitchEffectShader(ID3D11Device* device);
	~GlitchEffectShader()  override{}

	void Begin(ID3D11DeviceContext* immediate_context)override;
	void Draw(ID3D11DeviceContext* immediate_context,const RenderContext& rc,const Sprite* sprite)override;
	void End( ID3D11DeviceContext* immediate_context)override;
private:

	struct glitch_shader_constants
	{
        float               time = 0.0f;              //�o�ߎ���
        float               density = 0.0f;           //���x
        float               shift = 0.0f;             //���炷��
        float               line_impact = 10.0f;      //��{���̉e���x

        DirectX::XMFLOAT2   x_shift = { 0.0f, 0.0f }; //x�̉����ǂ̒��x���炷�̂�
        DirectX::XMFLOAT2   y_shift = { 0.0f, 0.0f }; //y�̉����ǂ̒��x���炷�̂�

        float               rand_float = 0.0f;        //�����_����
        float               x_shifting = 0.0f;        //�F������x�̈ʒu�����炷
        float               y_shifting = 0.0f;        //�F������y�̈ʒu�����炷    
        float               extension = 0.0f;          //uv�̊g��

        float               uv_slider = 0.0f;         //����ɗ���
        float               brightness = 0.0f;        //���邳
        float               glitch_mask_radius = 0;    //�}�X�N���a
        int                 glitch_sampling_count = 1; //��

        float               gage = 0.0f;              //�Q�[�W

        DirectX::XMFLOAT2   center = { 0.5f, 0.5f };  //���S

        //-------------------------------------
        // �� ��`�m�C�Y�i�u���b�N�m�C�Y�j
        //-------------------------------------
        float               rect_noise_width = 0.05f;  // �m�C�Y��`�̕��iUV��ԁj
        float               rect_noise_height = 0.05f; // �m�C�Y��`�̍����iUV��ԁj
        float               rect_noise_strength = 0.3f; // �m�C�Y���x�i0?1�j

        //-------------------------------------
        // �� ���]�t���b�V��
        //-------------------------------------
        float               flash_frequency = 1.0f;    // �t���b�V�������i�b�j
        float               flash_strength = 0.2f;     // �t���b�V�����]�����i0?1�j
        DirectX::XMFLOAT4 dummy;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>glitchConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexxShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>samplerState;
};