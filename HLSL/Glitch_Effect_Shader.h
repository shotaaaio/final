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
		float				time = 0.0f; //経過時間
		float				density = 0.0f; //密度
		float				shift = 0.0f; //ずらす幅
		float				line_impact = 10.0f; //一本線の影響度

		DirectX::XMFLOAT2	x_shift = { 0.0f,0.0f }; //xの横をどの程度ずらすのか
		DirectX::XMFLOAT2	y_shift = { 0.0f,0.0f }; //yの横をどの程度ずらすのか

		float				rand_float = 0.0f; //ランダム性
		float				x_shifting = 0.0f; //色収差のxの位置をずらす
		float				y_shifting = 0.0f; //色収差のyの位置をずらす    
		float				extension = 0.0f; //uvの拡張

		float				uv_slider = 0.0f; //左上に流す
		float				brightness = 0.0f; //明るさ
		float				glitch_mask_radius = 0; //マスク半径
		int					glitch_sampling_count = 1; //回数

		float gage;
		DirectX::XMFLOAT2	center = { 0.5f,0.5f }; //中心
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