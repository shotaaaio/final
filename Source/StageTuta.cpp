#include "StageTuta.h"
#include "DeviceManager.h"
#include "Graphics/GraphicsManager.h"
#include"Graphics/ImGuiRenderer.h"
#include "Camera.h"
#include "EnemyPlantune.h"
#include "EnemyManager.h"
#include "EffectManager.h"
#include "StageManager.h"
#include "Input/InputManager.h"

#include "Graphics/Texture.h"
#include "Mathf.h"
#include "Ivy.h"
#include "ObjManager.h"
#include "Trash.h"
#include "Goal.h"

// ������
void StageTuta::initialize()
{
	DeviceManager* deviceMgr = DeviceManager::instance();

	// �V�[���萔�o�b�t�@�̍쐬
	createBuffer<StageTuta::SceneConstants>(DeviceManager::instance()->getDevice(), buffer.GetAddressOf());

	//�X�e�[�W�̍쐬
	stage = std::make_unique<Stage>();
	StageManager::instance()->regist(stage.get());

	//�G�t�F�N�g�̏�����
	EffectManager::instance()->initialize();

	//�v���C���[�̍쐬
	player = std::make_unique<Player>();

	//�J��������̏�����
	cameraCtrl = std::make_unique<CameraController>();

	//�G�̍쐬�Ə�����
		EnemyPlantune* plantune = new EnemyPlantune();
		plantune->setPosition({ 2.0f,0.0f,5.0f });

		//�G�Ǘ��N���X�Ɏ��t��
		EnemyManager* eneMgr = EnemyManager::instance();
		eneMgr->regist(plantune);

	//�Q�[�W�p�X�v���C�g
	gauge = std::make_unique<Sprite>(deviceMgr->getDevice(), nullptr);

	//�J���������ݒ�
	Camera* camera = Camera::instance();
	camera->setLookAt(
		DirectX::XMFLOAT3(0, 10, -10),// �J�����̎��_�i�ʒu�j
		DirectX::XMFLOAT3(0, 0, 0),	  // �J�����̒����_�i�^�[�Q�b�g�j
		DirectX::XMFLOAT3(0, 1, 0)    // �J�����̏����
	);

	camera->setPerspectiveFov(
		DirectX::XMConvertToRadians(45),	// ����p
		deviceMgr->getScreenWidth() / deviceMgr->getScreenHeight(),	// ��ʏc���䗦
		0.1f,		// �J�������f���o���̍ŋߋ���
		1000.0f		// �J�������f���o���̍ŉ�����
	);

	skymap = std::make_unique<Skymap>(deviceMgr->getDevice(), L"Resources/Image/incskies_043_16k.png");

	//	�����A�j���[�V�����t���p�[�e�B�N��
	{
		D3D11_TEXTURE2D_DESC texture2d_desc;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;

		//	�p�[�e�B�N���p�摜���[�h
		TextureManager::instance()->loadTextureFromFile(deviceMgr->getDevice(), L"Resources/Image/Explosion01_5x5.png", shader_resource_view.GetAddressOf(), &texture2d_desc);

		//	�p�[�e�B�N���V�X�e������
		particle_bomb = std::make_unique<ParticleSystem>(deviceMgr->getDevice(), shader_resource_view, 5, 5);
	}

	Ivy* ivy = new Ivy();
	ivy->setPosition({ -11, 0, -1 });
	ObjManager::instance()->regist(ivy);

	/*Trash* trash = new Trash();
	trash->setPosition({ -10, 0, 2 });
	ObjManager::instance()->regist(trash);*/

	Goal* goal = new Goal();
	goal->setPosition({ -20, 0, 2 });
	ObjManager::instance()->regist(goal);

	//Key_c = std::bind(&StageTuta::Reset, this);

	scoreManager = std::make_unique<ScoreManager>();
}

// �I������
void StageTuta::finalize()
{
	//�G�l�~�[�I����
	EnemyManager::instance()->clear();
	EffectManager::instance()->finalize();
	ObjManager::instance()->clear();
}

// �X�V����
void StageTuta::update(float elapsedTime)
{
	GamePad* gamePad = InputManager::instance()->getGamePad();
	if (gamePad->getButtonDown() & GamePad::BTN_P)
	{
		isPaused = !isPaused;
	}
	if (isPaused) return;

	//�J��������̍X�V
	DirectX::XMFLOAT3 target = *(player->getPosition());
	target.y += options.x; //�v���C���[�̍�������ɒ����_�ݒ�
	//DirectX::XMFLOAT3 vel = player->getVelocity();
	
	//DirectX::XMFLOAT3 dir = Mathf::GetForwardDirection(player->getAngle()->y);
	//target.x += options.y * dir.x;
	//target.z += options.y * dir.z;
	cameraCtrl->setTarget(target);
	//Camera* camera = Camera::instance();

	//float damping = 2.0f * sqrt(damp);
	//idealPos = *camera->getEye();
	//DirectX::XMFLOAT3 actualpos = cameraCtrl->computeEye();

	//DirectX::XMFLOAT3 diff;
	//DirectX::XMStoreFloat3(&diff,
	//	DirectX::XMVector3Normalize(
	//	DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&idealPos), DirectX::XMLoadFloat3(&actualpos)))
	//);

	//DirectX::XMVECTOR accel =
	//	DirectX::XMVectorSubtract(
	//		DirectX::XMVectorScale(DirectX::XMLoadFloat3(&vel), damping),
	//		DirectX::XMVectorScale(DirectX::XMLoadFloat3(&diff), -damp)
	//	);

	//DirectX::XMFLOAT3 eye;
	//DirectX::XMStoreFloat3(&eye,
	//DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&idealPos), DirectX::XMVectorScale(accel, elapsedTime)));

	//cameraCtrl->rotateCameraAngle(elapsedTime);
	//camera->setLookAt(eye, target, {0,1,0});
	cameraCtrl->update(elapsedTime, player->getAngle()->y);

	//�X�e�[�W�X�V����
	stage->update(elapsedTime);

	//�v���C���[�X�V����
	player->update(elapsedTime);

	//�^�b�`�ɂ��G�̏o��
	enemyPlacementByTouch(DeviceManager::instance()->getDeviceContext(), elapsedTime);

	//�G�̍X�V����
	EnemyManager::instance()->update(elapsedTime);

	//�G�t�F�N�g�̍X�V
	EffectManager::instance()->update(elapsedTime);

	ObjManager::instance()->update(elapsedTime);

	////	�p�[�e�B�N���V�X�e���X�V
	//if (particle_bomb)
	//{
	//	//	�������o
	//	if (::GetAsyncKeyState('R') & 0x8000)
	//	{
	//		particle_bomb->Set(
	//			0, 1.0f,
	//			*player.get()->getPosition(),
	//			DirectX::XMFLOAT3(
	//				(rand() % 200 - 100) * 0.001f,
	//				(rand() % 100) * 0.005f,
	//				0),
	//			DirectX::XMFLOAT3(0, 0.5f, 0),
	//			DirectX::XMFLOAT2(3.0f, 3.0f),
	//			true,
	//			24.0f
	//		);
	//	}
	//	particle_bomb->Update(elapsedTime);
	//}
}


// �`�揈��
void StageTuta::render()
{
	DeviceManager* mgr = DeviceManager::instance();
	GraphicsManager* graphics = GraphicsManager::instance();

	ID3D11DeviceContext* dc = mgr->getDeviceContext();
	ID3D11RenderTargetView* rtv = mgr->getRenderTargetView();
	ID3D11DepthStencilView* dsv = mgr->getDepthStencilView();

	// ��ʃN���A�������_�[�^�[�Q�b�g�ݒ�
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0�`1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT4X4* view = camera->getView();
	const DirectX::XMFLOAT4X4* proj = camera->getProjection();

	// 3D ���f���̕`��ɕK�v�ȏ��
	SceneConstants sc;

	//�r���[�s��
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(view);

	//�v���W�F�N�V�����s��
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(proj);

	DirectX::XMStoreFloat4x4(&sc.viewProjection, View * Projection);

	// ���C�g�����i�������j
	sc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };

	// 3D �`��ɗ��p����萔�o�b�t�@�̍X�V�Ɛݒ�
	bindBuffer(dc, 1, buffer.GetAddressOf(), &sc);

	//�X�J�C�}�b�v
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// �T���v���[�X�e�[�g�̐ݒ�i�A�j�\�g���s�b�N�j
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
		// �u�����h�X�e�[�g�̐ݒ�i�A���t�@�j
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// �[�x�X�e���V���X�e�[�g�̐ݒ�i�[�x�e�X�g�I���A�[�x�������݃I�t�j
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_OFF)].Get(), 0);
		// ���X�^���C�U�X�e�[�g�̐ݒ�i�\���b�h�A���ʕ\���I�t�j
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
		});
	{
		DirectX::XMFLOAT4X4 inverse_view_projection;
		//�J�����̋t�s��
		DirectX::XMStoreFloat4x4(&inverse_view_projection, DirectX::XMMatrixInverse(nullptr, View * Projection));
		skymap->Render(dc, camera->getEye(), inverse_view_projection);
	}

	// 3D �`��ݒ�
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// �T���v���[�X�e�[�g�̐ݒ�i�A�j�\�g���s�b�N�j
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
		// �u�����h�X�e�[�g�̐ݒ�i�A���t�@�j
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// �[�x�X�e���V���X�e�[�g�̐ݒ�i�[�x�e�X�g�I���A�[�x�������݃I���j
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_ON)].Get(), 0);
		// ���X�^���C�U�X�e�[�g�̐ݒ�i�\���b�h�A���ʕ\���I�t�j
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
		});

	// 3D �`��
	{
		//�X�e�[�W�̕`��
		stage->render(dc);

		//�v���C���[�̕`��
		player->render(dc);

		//�G�`�揈��
		EnemyManager::instance()->render(dc);

		ObjManager::instance()->render(dc);
	}

	//3D�G�t�F�N�g�`��
	{
		EffectManager::instance()->render(view, proj);
	}


	//�p�[�e�B�N���`��
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// �T���v���[�X�e�[�g�̐ݒ�i�A�j�\�g���s�b�N�j
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
		// �u�����h�X�e�[�g�̐ݒ�i�A���t�@�j
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// �[�x�X�e���V���X�e�[�g�̐ݒ�i�[�x�e�X�g�I���A�[�x�������݃I�t�j
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_OFF)].Get(), 0);
		// ���X�^���C�U�X�e�[�g�̐ݒ�i�\���b�h�A���ʕ\���I�t�j
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
		});
	{
		if (particle_bomb)particle_bomb->Render(dc, *view, *proj);
	}

	// 3D�f�o�b�O�`��
	{
		// ���C�������_���`����s
		graphics->getLineRenderer()->render(dc, *view, *proj);

		// �f�o�b�O�����_���`����s
		graphics->getDebugRenderer()->render(dc, *view, *proj);

		player->drawDrawPrimitive();

		EnemyManager::instance()->drawDebugPrimitive();
	}

	// 2D �`��ݒ�
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// �T���v���[�X�e�[�g�̐ݒ�i���j�A�j
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
		// �u�����h�X�e�[�g�̐ݒ�i�A���t�@�j
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// �[�x�X�e���V���X�e�[�g�̐ݒ�i�[�x�e�X�g�I�t�A�[�x�������݃I�t�j
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::OFF_OFF)].Get(), 0);
		// ���X�^���C�U�X�e�[�g�̐ݒ�i�\���b�h�A���ʕ\���I�t�j
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
		});

	// 2D �`��
	{
		RenderEnemyGauge(dc, *view, *proj);
	}

	// 2D�f�o�b�OGUI�`��
	{
		DirectX::XMFLOAT3 eye = *(camera->getEye());
		//�v���C���[�̃f�o�b�N�̕`��
		player->drawDebugGui();
		ImGui::Begin("game");
		ImGui::SliderFloat3("options", &options.x, -5.0f, 5.0f);
		ImGui::InputFloat3("camera position", &eye.x);
		ImGui::SliderFloat("damp", &damp, 2.0f, 5.0f);
		ImGui::End();
	}
}


//�G���C�t�Q�[�W�̕`��
void StageTuta::RenderEnemyGauge(
	ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection)
{
	// �r���[�|�[�g�擾
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// �ϊ��s��
	DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	// �}�E�X����
	Mouse* mouse = InputManager::instance()->getMouse();
	static Enemy* selectedEnemy = nullptr;
	static bool isDragging = false;

	// �G�}�l�[�W���[�擾
	EnemyManager* enemyMgr = EnemyManager::instance();
	int enemyCount = enemyMgr->getEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyMgr->getEnemy(i);

		// �G�l�~�[�̓���̃��[���h���W
		DirectX::XMFLOAT3 worldPosition = *enemy->getPosition();
		worldPosition.y += enemy->getHeight();

		DirectX::XMVECTOR worldPositionVec = DirectX::XMLoadFloat3(&worldPosition);

		// ���[���h���W����X�N���[�����W�֕ϊ�
		DirectX::XMVECTOR screenPositionVec = DirectX::XMVector3Project(
			worldPositionVec,
			viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height,
			viewport.MinDepth, viewport.MaxDepth,
			projMat, viewMat, worldMat
		);

		// �X�N���[�����W�擾
		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMStoreFloat2(&screenPosition, screenPositionVec);

		// �Q�[�W�̒���
		const float gaugeWidth = 30.0f;
		const float gaugeHeight = 5.0f;

		float healthRate = enemy->getHealth() / static_cast<float>(enemy->getMaxHealth());

		if (!clickGauge[i])
		{
			gaugeOffsets[enemy] = { screenPosition.x - gaugeWidth * 0.5f, screenPosition.y - gaugeHeight };
		}

		float& gaugeOffsetX = gaugeOffsets[enemy].x;
		float& gaugeOffsetY = gaugeOffsets[enemy].y;

		// �}�E�X�I������
		if (mouse->getButtonDown() & Mouse::BTN_LEFT)
		{
			float mouseX = static_cast<float>(mouse->getPositionX());
			float mouseY = static_cast<float>(mouse->getPositionY());

			// �}�E�X���Q�[�W�͈͓̔��ɂ��邩�`�F�b�N
			if (mouseX >= gaugeOffsetX && mouseX <= gaugeOffsetX + gaugeWidth &&
				mouseY >= gaugeOffsetY && mouseY <= gaugeOffsetY + gaugeHeight)
			{
				selectedEnemy = enemy;
				isDragging = true;
			}
		}

		// �h���b�O���Ɉʒu�X�V
		if (isDragging && selectedEnemy == enemy && (mouse->getButton() & Mouse::BTN_LEFT))
		{
			gaugeOffsetX = static_cast<float>(mouse->getPositionX()) - gaugeWidth * 0.5f;
			gaugeOffsetY = static_cast<float>(mouse->getPositionY()) - gaugeHeight;
			clickGauge[i] = true;
		}

		// �}�E�X�{�^���𗣂�����h���b�v
		if (isDragging && selectedEnemy == enemy && !(mouse->getButton() & Mouse::BTN_LEFT))
		{
			isDragging = false;

			
			
		
			if (gaugeOffsetX + gaugeWidth < 0 || gaugeOffsetX>1280 || gaugeOffsetY + gaugeHeight < 0 || gaugeOffsetY>720)
			{
				InputManager::instance()->attack = false;
			}
			selectedEnemy = nullptr;
		}
		ImGui::Begin("gauge");
		ImGui::SliderFloat3("gp", &gaugeOffsetX, -5.0f, 5.0f);
		ImGui::End();

			// �Q�[�W�`��
			gauge->render(
				dc,
				gaugeOffsetX, gaugeOffsetY,
				gaugeWidth * healthRate, gaugeHeight,
				1.0f, 0.0f, 0.0f, 1.0f,
				0.0f,
				0, 0,
				static_cast<float>(gauge->getTextureWidth()),
				static_cast<float>(gauge->getTextureHeight())
			);
	}
}

//�^�b�`�ɂ��G�̔z�u
void StageTuta::enemyPlacementByTouch(ID3D11DeviceContext* dc, float elapsedTime)
{
	static Enemy* selectedEnemy = nullptr;
	static bool isDragging = false;

	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT4X4* view = camera->getView();
	const DirectX::XMFLOAT4X4* proj = camera->getProjection();

	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(view);
	DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(proj);
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	Mouse* mouse = InputManager::instance()->getMouse();
	/*const float doubleClickThreshold = 0.3f;
	if (click) lastClickTime += elapsedTime;
	if (lastClickTime > doubleClickThreshold)
	{
		click = !click;
		lastClickTime = 0.0f;
	}*/

	// --- �G�I������ ---
	if (mouse->getButtonDown() & Mouse::BTN_LEFT)
	{
		/*click = !click;
		if (click) return;
		if (lastClickTime > doubleClickThreshold)
		{
			lastClickTime = 0.0f;
			return;
		}
		lastClickTime = 0.0f;*/
		DirectX::XMFLOAT3 screenPosition{
			static_cast<float>(mouse->getPositionX()),
			static_cast<float>(mouse->getPositionY()),
			0.0f
		};

		DirectX::XMVECTOR screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		DirectX::XMVECTOR worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec, viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth,
			projMat, viewMat, worldMat
		);

		DirectX::XMFLOAT3 rayStart;
		DirectX::XMStoreFloat3(&rayStart, worldPositionVec);

		screenPosition.z = 1.0f;
		screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec, viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth,
			projMat, viewMat, worldMat
		);

		DirectX::XMFLOAT3 rayEnd;
		DirectX::XMStoreFloat3(&rayEnd, worldPositionVec);

		if (!isDragging)
		{
			selectedEnemy = EnemyManager::instance()->getEnemyByRay(rayStart, rayEnd);
			if (selectedEnemy)
			{
				isDragging = true;
			}
		}
	}

	// --- �h���b�O���̓G�̈ʒu���X�V ---
	if (isDragging && selectedEnemy && mouse->getButton() & Mouse::BTN_LEFT)
	{
		DirectX::XMFLOAT3 screenPosition{
			static_cast<float>(mouse->getPositionX()),
			static_cast<float>(mouse->getPositionY()),
			0.0f
		};

		DirectX::XMVECTOR screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		DirectX::XMVECTOR worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec, viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth,
			projMat, viewMat, worldMat
		);

		DirectX::XMFLOAT3 rayStart;
		DirectX::XMStoreFloat3(&rayStart, worldPositionVec);

		screenPosition.z = 1.0f;
		screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec, viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth,
			projMat, viewMat, worldMat
		);

		DirectX::XMFLOAT3 rayEnd;
		DirectX::XMStoreFloat3(&rayEnd, worldPositionVec);

		/*HitResult hit;
		if (stage->raycast(rayStart, rayEnd, hit))
		{
			selectedEnemy->setPosition(hit.position);
		}*/
	}

	// --- �h���b�v���� ---
	if (isDragging && selectedEnemy && !(mouse->getButton() & Mouse::BTN_LEFT))
	{
		isDragging = false;

		selectedEnemy = nullptr;
	}
}

void StageTuta::Reset()
{
	finalize();  // �܂��Â����\�[�X��������Ə���
	stage.reset();
	player.reset();
	cameraCtrl.reset();
	gauge.reset();
	skymap.reset();
	particle_bomb.reset();

	Key_c = nullptr;

	initialize(); // ���̌�A���������Ȃ���

	Key_c = std::bind(&StageTuta::Reset, this);
}