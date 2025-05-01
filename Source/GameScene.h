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

// �Q�[���V�[��
class GameScene :public Scene
{
public:
	GameScene() {}
	~GameScene() {}

	// ������
	void initialize()override;

	// �I������
	void finalize()override;

	// �X�V����
	void update(float elapsedTime)override;

	// �`�揈��
	void render()override;
private:
	//�G���C�t�Q�[�W�`��
	void RenderEnemyGauge(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4& view,
		const DirectX::XMFLOAT4X4& projection
	);

	//�^�b�`�ɂ��G�̏o��
	void enemyPlacementByTouch(ID3D11DeviceContext* dc);
private:
	// �V�[���萔
	struct SceneConstants
	{
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		lightDirection;
	};

	// �V�[���萔�o�b�t�@
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
