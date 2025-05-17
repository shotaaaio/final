#include"TitleScene.h"
#include"DeviceManager.h"
#include"Graphics/GraphicsManager.h"
#include"Input/InputManager.h"
#include"SceneManager.h"
#include"GameScene.h"
#include"LoadingScene.h"
#include "imgui.h"


//����������
void TitleScene::initialize()
{
	//�^�C�g���摜�ǂݍ���
	titleImage = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Image\\Title.png");
}

//�I����
void TitleScene::finalize()
{

}

//�X�V����
void TitleScene::update(float elapsedTime)
{
	GamePad* gamePad = InputManager::instance()->getGamePad();

	//�{�^������������Q�[���V�[���ɑJ��
	if (gamePad->getButtonDown() & GamePad::BTN_A)
	{
		SceneManager::instance()->changeScene(new LoadingScene(new GameScene));
	}
}

//�`�揈��
void TitleScene::render()
{
	DeviceManager* mgr = DeviceManager::instance();
	GraphicsManager* graphics = GraphicsManager::instance();

	ID3D11DeviceContext* dc = mgr->getDeviceContext();
	ID3D11RenderTargetView* rtv = mgr->getRenderTargetView();
	ID3D11DepthStencilView* dsv = mgr->getDepthStencilView();

	//��ʃN���A�������_�[�^�[�Q�b�g�r���[�ݒ�
	FLOAT color[] = { 0.0f,0.0f,0.5f,1.0f };//RGBA(0.0f~1.0f)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);
	
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
		titleImage->render(dc,
			0, 0, mgr->getScreenWidth(), mgr->getScreenHeight(),
			1, 1, 1, 1,
			0,
			0, 0, 1280, 720
		);
	}

	// �T�[�o�[�Ɛڑ�
	if (ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::Button(u8"Stage:1"))
		{
			SceneManager::instance()->changeScene(new LoadingScene(new GameScene));
		}
	}
	ImGui::End();
}