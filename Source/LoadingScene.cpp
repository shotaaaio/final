#include"LoadingScene.h"
#include"SceneManager.h"
#include"DeviceManager.h"
#include"Graphics/GraphicsManager.h"

//����������
void LoadingScene::initialize()
{
    //���[�f�B���O�摜�ǂݍ���
	loadingImage = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Image\\LoadingIcon.png");

    //�ʂ̃X���b�h�̋N��
    thread = new std::thread(LoadingThread, this);
}

//�I��������
void LoadingScene::finalize()
{
    if (thread != nullptr)
    {
        thread->join(); //�X���b�h�����R�ɏI������̂�҂�
        delete thread;
        thread = nullptr;
    }
}

//�X�V����
void LoadingScene::update(float elapsedTime)
{
	constexpr float speed = 180.0f;
	angle += speed * elapsedTime;

    //���̃V�[���̏����������ʒm���������Ă�����V�[����؂�ւ���
    if (nextScene->isReady())
    {
        SceneManager::instance()->changeScene(nextScene);
        nextScene = nullptr;
    }
}

//�`�揈��
void LoadingScene::render()
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
        // ��ʂ̉E���Ƀ��[�f�B���O�摜��\��
        float screenWidth = mgr->getScreenWidth();
        float screenHeight = mgr->getScreenHeight();
        float textureWidth = 256.0f;
        float textureHeight = 256.0f;
        float posX = screenWidth - textureWidth;
        float posY = screenHeight - textureHeight;

        loadingImage->render(dc,
            posX, posY, textureWidth, textureHeight,
            1, 1, 1, 1,
            angle,
            0, 0, textureWidth, textureHeight
        );
    }
}

void LoadingScene::LoadingThread(LoadingScene* scene)
{
    //DirectX�֘A�̃X���b�h�̏�����
    HREFTYPE hr = CoInitialize(nullptr);

    //���̃V�[���̏��������s��
    scene->nextScene->initialize();

    //DirectX �֘A�̃X���b�h�̏I����
    CoUninitialize();

    //���̃V�[���̏��������ݒ�
    scene->nextScene->setReady();
}