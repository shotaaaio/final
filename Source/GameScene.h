#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl.h>
#include "Stage.h"
#include "Player.h"
#include "CameraController.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "Graphics/Skymap.h"
#include "particle/particle_system.h"

#include "Graphics/FrameBuffer.h"
#include "Graphics/FullScreenQuad.h"
#include "misc.h"

// ゲームシーン
class GameScene :public Scene
{
public:
	GameScene() {}
	~GameScene() {}

	// 初期化
	void initialize()override;

	// 終了処理
	void finalize()override;

	// 更新処理
	void update(float elapsedTime)override;

	// 描画処理
	void render()override;
private:
	//敵ライフゲージ描画
	void RenderEnemyGauge(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection
	);

	//タッチによる敵の出現
	void enemyPlacementByTouch(ID3D11DeviceContext* dc);
private:
	// シーン定数
	struct SceneConstants
	{
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		lightDirection;
	};

	// シーン定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

	std::unique_ptr<Stage>				stage;
	std::unique_ptr<Player>				player;
	std::unique_ptr<CameraController>	cameraCtrl;
	std::unique_ptr<Sprite>				gauge;
	std::unique_ptr<Skymap>				skymap;


	std::unique_ptr<ParticleSystem>particle_bomb;

	DirectX::XMFLOAT3 options = { 0.5f,0.3f,5.0f };
	DirectX::XMFLOAT3 idealPos;
	float damp = 2.0f;

	std::unique_ptr<FrameBuffer> frameBuffers[8];
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader[8];

	int postEffectIndex = 0;

	struct LuminanceExtract
	{
		DirectX::XMFLOAT3 luminanceColor{ 0.0f, 0.0f, 0.0f };
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

private:
	void CreateShader(ID3D11Device* device);
	void UsePostEffect(ID3D11DeviceContext* dc, int index);
	void ComputeVignette();
};
