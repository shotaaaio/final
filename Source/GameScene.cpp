#include "GameScene.h"
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
#include "ObjManager.h"

#include "Utilities.h"

// ������
void GameScene::initialize()
{
	DeviceManager* deviceMgr = DeviceManager::instance();

	// �V�[���萔�o�b�t�@�̍쐬
	createBuffer<GameScene::SceneConstants>(DeviceManager::instance()->getDevice(), buffer.GetAddressOf());

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
	hp = std::make_unique<Sprite>(deviceMgr->getDevice(), L"Resources/Image/HPgage1.png");
	hp->SetShaderResourceView(hp->GetShaderResourceView(),hp->getTextureWidth(),hp->getTextureHeight());
	
	//	�����A�j���[�V�����t���p�[�e�B�N��
	{
		D3D11_TEXTURE2D_DESC texture2d_desc;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;

		//	�p�[�e�B�N���p�摜���[�h
		TextureManager::instance()->loadTextureFromFile(deviceMgr->getDevice(), L"Resources/Image/Explosion01_5x5.png", shader_resource_view.GetAddressOf(), &texture2d_desc);

		//	�p�[�e�B�N���V�X�e������
		particle_bomb = std::make_unique<ParticleSystem>(deviceMgr->getDevice(), shader_resource_view, 5, 5);
	}

	//�V�����`��^�[�Q�b�g�̐���
	{
		renderTarget = std::make_unique<RenderTarget>(
			static_cast<UINT>(deviceMgr->getScreenWidth()),
			static_cast<UINT>(deviceMgr->getScreenHeight()),
			DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	//�|�X�g�v���Z�X�`��N���X����
	{
		//postprosessingRenderer = std::make_unique<PostprosessingRenderer>();
		////�V�[���e�N�X�`����ݒ肵�Ă���
		//ShaderResourceViewData srvData;
		//srvData.srv = renderTarget->GetShaderResorceView();
		//srvData.width = renderTarget->GetWidth();
		//srvData.height = renderTarget->GetHeight();
		//postprosessingRenderer->SetSceneData(srvData);
	}

}

// �I������
void GameScene::finalize()
{
	//�G�l�~�[�I����
	EnemyManager::instance()->clear();
	EffectManager::instance()->finalize();
}

// �X�V����
void GameScene::update(float elapsedTime)
{
	//�J��������̍X�V
	DirectX::XMFLOAT3 target = *(player->getPosition());
	target.y += options.x; //�v���C���[�̍�������ɒ����_�ݒ�
	
	cameraCtrl->setTarget(target);

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

	glitchData.time += elapsedTime;
	if (glitchData.time > 1.0f)
	{
		glitchData.time = 0.1f;
	}
}


// �`�揈��
void GameScene::render()
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
		RenderContext rc;

		SpriteShader* gaussianShader = graphics->GetShader(SpriteShaderID::GaussianBlur);
		SpriteShader* glitchShader = graphics->GetShader(SpriteShaderID::Glitch);
		SpriteShader* luminanceShader = graphics->GetShader(SpriteShaderID::LuminanceExtraction);

		glitchShader->Begin(dc);
		rc.glitchData = glitchData;
		glitchShader->Draw(dc, rc, hp.get());
		glitchShader->End(dc);

	/*	rc.gaussianFilterData = gaussianFilterData;
		rc.gaussianFilterData.textureSize.x = hp->getTextureWidth();
		rc.gaussianFilterData.textureSize.y = hp->getTextureHeight();
		rc.luminanceExtractionData = lumianceExtractionData;
		luminanceShader->Begin(dc);
		luminanceShader->Draw(dc,rc, hp.get());
		luminanceShader->End(dc);*/

		/*rc.gaussianFilterData = gaussianFilterData;
		rc.gaussianFilterData.textureSize.x = hp->getTextureWidth();
		rc.gaussianFilterData.textureSize.y = hp->getTextureHeight();
		gaussianShader->Begin(dc);
		gaussianShader->Draw(dc, rc, hp.get());
		gaussianShader->End(dc);*/
		
	}

	if (ImGui::Begin("Debug"))
	{
		if (ImGui::BeginTabBar("DebugTabs"))
		{
			// === 2D Debug ===
			if (ImGui::BeginTabItem("2D Debug"))
			{
				if (ImGui::TreeNode("Glitch Shader"))
				{
					if (ImGui::CollapsingHeader("Param", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::InputFloat("Time", &glitchData.time);
						ImGui::SliderFloat("Density", &glitchData.density, 0.0f, 1.0f);
						ImGui::SliderFloat2("Shift Range", &glitchData.shift, 0.0f, 1.0f);
						ImGui::SliderFloat("Line Impact", &glitchData.line_impact, 0.0f, 100.0f);
						ImGui::SliderFloat("X Chromatic Shift", &glitchData.x_shifting, 0.0f, 1.0f);
						ImGui::SliderFloat("Y Chromatic Shift", &glitchData.y_shifting, 0.0f, 1.0f);
						ImGui::SliderFloat("HP(gage)", &glitchData.gage, 0.0f, 1.0f);
						ImGui::SliderFloat("Brightness", &glitchData.brightness, 0.0f, 1.0f);
						ImGui::SliderInt("Sampling Count", &glitchData.glitch_sampling_count, 0, 10);
					}

					if (ImGui::CollapsingHeader("Rectangular Noise", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::SliderFloat("Rect Noise Width", &glitchData.rect_noise_width, 0.0f, 1.0f);
						ImGui::SliderFloat("Rect Noise Height", &glitchData.rect_noise_height, 0.0f, 1.0f);
						ImGui::SliderFloat("Rect Noise Strength", &glitchData.rect_noise_strength, 0.0f, 1.0f);
					}

				

					// === Save / Load Buttons ===
					const std::string path = "glitch_settings.json";
					if (ImGui::Button("Save Glitch Settings")) {
						SaveSettings(glitchData, path);
					}
					ImGui::SameLine();
					if (ImGui::Button("Load Glitch Settings")) {
						LoadSettings(glitchData, path);
					}
					ImGui::SameLine();
					if (ImGui::Button("Reset All")) {
						// ���ׂẴp�����[�^�� 0 �ɏ�����
						glitchData.time = 0.0f;
						glitchData.density = 0.0f;
						glitchData.shift = 0.0f;
						glitchData.line_impact = 0.0f;
						glitchData.x_shifting = 0.0f;
						glitchData.y_shifting = 0.0f;
						glitchData.brightness = 0.0f;
						glitchData.glitch_sampling_count = 0;
						glitchData.rect_noise_width = 0.0f;
						glitchData.rect_noise_height = 0.0f;
						glitchData.rect_noise_strength = 0.0f;
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Gaussian Filter"))
				{
					if (ImGui::CollapsingHeader("Param", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::SliderInt("kernelSize", &gaussianFilterData.kernelSize, 1, MaxKernelSize - 1);
						ImGui::SliderFloat("deviation", &gaussianFilterData.deviation, 1.0f, 10.0f);
					}
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Bloom"))
				{
					ImGui::SliderFloat("threshold", &lumianceExtractionData.threshold, 0.0f, 1.0f);
					ImGui::SliderFloat("intensity", &lumianceExtractionData.intensity, 0.0f, 10.0f);
					ImGui::TreePop();
				}

				ImGui::EndTabItem();
			}


			// === 3D Debug ===
			if (ImGui::BeginTabItem("3D Debug"))
			{
				if (ImGui::TreeNode("Player3D"))
				{
					DirectX::XMFLOAT3 eye = *(camera->getEye());
					player->drawDebugGui();
					ImGui::SliderFloat3("options", &options.x, -5.0f, 5.0f);
					ImGui::InputFloat3("camera position", &eye.x);
					ImGui::SliderFloat("damp", &damp, 2.0f, 5.0f);
					ImGui::TreePop();
				}
				ImGui::EndTabItem();
			}

			// === Map Debug ===
			if (ImGui::BeginTabItem("Map Debug"))
			{
				ImGui::Text("Map Debug �����ɒǉ�");
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}



//�G���C�t�Q�[�W�̕`��
void GameScene::RenderEnemyGauge(
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


		float& gaugeOffsetX = gaugeOffsets[enemy].x;
		float& gaugeOffsetY = gaugeOffsets[enemy].y;

		if (gauge_b)
		{
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
}

//�^�b�`�ɂ��G�̔z�u
void GameScene::enemyPlacementByTouch(ID3D11DeviceContext* dc, float elapsedTime)
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