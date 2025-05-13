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
        float               time = 0.0f;              //経過時間
        float               density = 0.0f;           //密度
        float               shift = 0.0f;             //ずらす幅
        float               line_impact = 10.0f;      //一本線の影響度

        DirectX::XMFLOAT2   x_shift = { 0.0f, 0.0f }; //xの横をどの程度ずらすのか
        DirectX::XMFLOAT2   y_shift = { 0.0f, 0.0f }; //yの横をどの程度ずらすのか

        float               rand_float = 0.0f;        //ランダム性
        float               x_shifting = 0.0f;        //色収差のxの位置をずらす
        float               y_shifting = 0.0f;        //色収差のyの位置をずらす    
        float               extension = 0.0f;          //uvの拡張

        float               uv_slider = 0.0f;         //左上に流す
        float               brightness = 0.0f;        //明るさ
        float               glitch_mask_radius = 0;    //マスク半径
        int                 glitch_sampling_count = 1; //回数

        float               gage = 0.0f;              //ゲージ

        DirectX::XMFLOAT2   center = { 0.5f, 0.5f };  //中心

        //-------------------------------------
        // ▼ 矩形ノイズ（ブロックノイズ）
        //-------------------------------------
        float               rect_noise_width = 0.05f;  // ノイズ矩形の幅（UV空間）
        float               rect_noise_height = 0.05f; // ノイズ矩形の高さ（UV空間）
        float               rect_noise_strength = 0.3f; // ノイズ強度（0?1）

        //-------------------------------------
        // ▼ 反転フラッシュ
        //-------------------------------------
        float               flash_frequency = 1.0f;    // フラッシュ周期（秒）
        float               flash_strength = 0.2f;     // フラッシュ反転割合（0?1）
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