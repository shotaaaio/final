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
};
