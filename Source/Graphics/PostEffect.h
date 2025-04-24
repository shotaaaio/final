#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <memory>
#include "Graphics/FrameBuffer.h"
#include "Graphics/FullScreenQuad.h"

class PostEffect
{
public:
	PostEffect(ID3D11Device* device);
	~PostEffect() = default;

	void Update();
	//ImGui更新
	void UpdateImGui();
	//バッファの更新
	void UpdateBuffer(ID3D11DeviceContext* dc);
	//描画
	void Render(ID3D11DeviceContext* dc);

	//下記ではさんだところがオフスクリーンレンダリングで描画される
	//シーン情報をオフスクリーンでテクスチャ化する際の開始位置
	void OffScreenStart(ID3D11DeviceContext* dc);
	//シーン情報をオフスクリーンでテクスチャ化する際の終了位置
	void OffScrrenEnd(ID3D11DeviceContext* dc);
private:
	std::unique_ptr<FrameBuffer> frameBuffers[8];
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader[8];

	int postEffectIndex = 0;

	struct PostEffectSceneConstants
	{
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		lightDirection;
	};

	struct LuminanceExtract
	{
		DirectX::XMFLOAT3 luminanceColor{ 1.0f, 1.0f, 1.0f };
		float threshold = { 0.5f };
		float intensity = { 1.0f };
		int luminance_use_flag = 0;
		int blur_use_flag = 0;
		float dummy;
	};
	LuminanceExtract luminance;
	Microsoft::WRL::ComPtr<ID3D11Buffer> luminancebuffer;

	struct VignetteData
	{
		DirectX::XMFLOAT4 vignette_color = { 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT2 vignette_center = { 0.5f, 0.5f };
		float vignette_intensity = 0.5f;
		float vignette_smoothness = 0.2f;

		bool vignette_rounded = false;
		float vignette_roundness = 0.85f;
		bool vignette_compute_flag = false;
	};
	VignetteData vignettedata;
	struct VignetteConstants
	{
		DirectX::XMFLOAT4 vignette_color;
		DirectX::XMFLOAT2 vignette_center;
		float vignette_intensity;
		float vignette_smoothness;

		float vignette_rounded;
		float vignette_roundness;
		DirectX::XMFLOAT2 vignette_dummy;
	};
	VignetteConstants vignette;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vignettebuffer;

	struct ColorFilter
	{
		DirectX::XMFLOAT4 filterColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		float hueShift = 0.0f;
		float saturation = 2.0f;
		float brightness = 1.0f;
		float dummy;
	};
	ColorFilter colorfilter;
	Microsoft::WRL::ComPtr<ID3D11Buffer> colorfilterbuffer;
	DirectX::XMFLOAT4 colorfilterparameter{ 0.0f, 1.0f, 1.0f, 0.0f };
};