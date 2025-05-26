#pragma once

#include <functional>
#include <memory>
#include "DebugRenderer.h"
#include "LineRenderer.h"
#include "Buffer.h"
#include "Shader.h"
enum class SAMPLER_STATE
{
	POINT,
	LINEAR,
	ANISOTROPIC,
};

enum class DEPTH_STENCIL_STATE
{
	ON_ON,		// 深度テストを ON 深度書き込みを ON
	ON_OFF,		// 深度テストを ON 深度書き込みを OFF
	OFF_ON,		// 深度テストを OFF 深度書き込みを ON
	OFF_OFF,	// 深度テストを OFF 深度書き込みを OFF
};

enum class BLEND_STATE
{
	NONE,
	ALPHABLENDING,
	ADD,
	MULTIPLE
};

enum class RASTERIZER_STATE
{
	SOLID_CULLBACK,
	WIRE_CULLBACK,
	SOLID_CULLNONE,
	WIRE_CULLNONE
};

//ガウスフィルター
struct GaussianFilterData
{
	int kernelSize = 8;           //カーネルサイズ
	float deviation = 10.0f;      //標準偏差
	DirectX::XMFLOAT2 textureSize;//暈すテクスチャサイズ
};
static const int MaxKernelSize = 16;//ガウスフィルタの最大カーネル数

//高輝度抽出用情報
struct LumianceExtractionData
{
	float threshold = 0.5f;
	float intensity = 1.0f;//ブルームの強度
	DirectX::XMFLOAT2 dummy;
};

//グリッチ情報
struct GlitchData
{
	float time;                //経過時間
    float density;             //密度
    float shift;               //ずらす幅
    float line_impact;         //一本線の影響度

    DirectX::XMFLOAT2 x_shift; //xの横をどの程度ずらすのか
    DirectX::XMFLOAT2 y_shift; //yの横をどの程度ずらすのか

    float rand_float;          //ランダム性
    float x_shifting;          //色収差のxの位置をずらす
    float y_shifting;          //色収差のyの位置をずらす    
    float extension;           //uvの拡張

    float uv_slider;           //左上に流す
    float brightness;          //明るさ
    float glitch_mask_radius;  //マスク半径
    int   glitch_sampling_count; //回数

    DirectX::XMFLOAT2 center = { 0.5f, 0.5f };    //中心
    float gage;

    float rect_noise_width;    //ノイズ矩形の幅（UV空間）
    float rect_noise_height;   //ノイズ矩形の高さ（UV空間）
    float rect_noise_strength; //ノイズ強度（0〜1）
};

enum class SpriteShaderID
{
	Default,
	Glitch,
	GaussianBlur,
	ColorGrading,
	LuminanceExtraction,
	Max
};

// レンダーコンテキスト
struct RenderContext
{
	// サンプラーステート
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStates[3];
	// 深度ステンシルステート
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates[4];
	// ブレンドステート
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates[4];
	// ラスタライザステート
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[4];

	GlitchData glitchData;

	GaussianFilterData gaussianFilterData;

	LumianceExtractionData luminanceExtractionData;

};

class GraphicsManager
{
private:
	GraphicsManager() {}
	~GraphicsManager() {}

public:
	static GraphicsManager* instance()
	{
		static GraphicsManager inst;
		return &inst;
	}

	bool initialize(ID3D11Device* device, ID3D11DeviceContext* dc);

	Microsoft::WRL::ComPtr<ID3D11SamplerState> getSamplerState(SAMPLER_STATE state) const { return renderContext.samplerStates[static_cast<int>(state)]; }

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> getDepthStencilStates(DEPTH_STENCIL_STATE state) const { return renderContext.depthStencilStates[static_cast<int>(state)]; }

	Microsoft::WRL::ComPtr<ID3D11BlendState> getBlendStates(BLEND_STATE state) const { return renderContext.blendStates[static_cast<int>(state)]; }

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> getRasterizerStates(RASTERIZER_STATE state) const { return renderContext.rasterizerStates[static_cast<int>(state)]; }

	void SettingRenderContext(void(*settingCallback)(ID3D11DeviceContext*, RenderContext*));

	DebugRenderer* getDebugRenderer() const { return debugRenderer.get(); }

	LineRenderer* getLineRenderer() const { return lineRenderer.get(); }

	// スプライトシェーダー取得
	SpriteShader* GetShader(SpriteShaderID id) const { return spriteShaders[static_cast<int>(id)].get(); }

private:
	ID3D11DeviceContext* deviceContext = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	RenderContext renderContext;
	
	std::unique_ptr<SpriteShader>	spriteShaders[static_cast<int>(SpriteShaderID::Max)];

	std::unique_ptr<DebugRenderer> debugRenderer;
	std::unique_ptr<LineRenderer> lineRenderer;

	float	screenWidth;
	float	screenHeight;
};