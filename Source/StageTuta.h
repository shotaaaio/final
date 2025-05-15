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
#include "Enemy.h"
#include "ScoreManager.h"

// �Q�[���V�[��
class StageTuta :public Scene
{
public:
	StageTuta() {}
	~StageTuta() {}

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
	void enemyPlacementByTouch(ID3D11DeviceContext* dc, float elapsedTime);

	void Reset();

	void clickMoveGauge(Enemy* enemy,
		float gaugeWidth,
		float gaugeHeight,
		float& gaugeOffsetX,
		float& gaugeOffsetY,
		bool& isDragging,
		Enemy*& selectedEnemy,
		int enemyIndex);
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

	bool clickGauge[20];
	std::unordered_map<Enemy*, DirectX::XMFLOAT2> gaugeOffsets;

	bool gauge_b = true;

	std::function<void()> Key_c;

	std::unique_ptr<ScoreManager> scoreManager;

	bool isPaused = false;//�|�[�Y���
};
