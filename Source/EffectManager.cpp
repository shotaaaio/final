#include"DeviceManager.h"
#include"EffectManager.h"

//����������
void EffectManager::initialize()
{
    // �r������
    std::lock_guard<std::mutex> lock(DeviceManager::instance()->getMutex());

    DeviceManager* deviceMgr = DeviceManager::instance();

    // Effekseer �����_���𐶐�
    effekseerRenderer = EffekseerRendererDX11::Renderer::Create(deviceMgr->getDevice(),
        deviceMgr->getDeviceContext(), 2048);

    // Effekseer �Ǘ�����
    effekseerMgr = Effekseer::Manager::Create(2048);

    // Effekseer �`�����̊e��ݒ�i����ȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͈ȉ��̐ݒ�j
    effekseerMgr->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
    effekseerMgr->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
    effekseerMgr->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
    effekseerMgr->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
    effekseerMgr->SetModelRenderer(effekseerRenderer->CreateModelRenderer());
    // Effekseer ���ł� Loader �̐ݒ�i����ȃJ�X�^�}�C�Y�����Ȃ��ꍇ�͈ȉ��̐ݒ�j
    effekseerMgr->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
    effekseerMgr->SetModelLoader(effekseerRenderer->CreateModelLoader());
    effekseerMgr->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());

    // Effekseer ��������W�n�Ōv�Z����
    effekseerMgr->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

//�I����
void EffectManager::finalize()
{
	//effekseerManager�Ȃǂ̓X�}�[�g�|�C���^�[���Ŕj�󂳂��̂ŉ������Ȃ�
}


//�X�V����
void EffectManager::update(float elapsedTime)
{
	//�G�t�F�N�g�X�V�����i�t���[���P�ʁj
	effekseerMgr->Update(elapsedTime * 60.0f);
}

//�`�揈��
void EffectManager::render(const DirectX::XMFLOAT4X4* view, const DirectX::XMFLOAT4X4* projection)
{
	//�r���[���v���W�F�N�V�����s���Effeksser�����_���ɐݒ�
	effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(view));
	effekseerRenderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(projection));

	//Effekseer�̕`����J�n
	effekseerRenderer->BeginRendering();

	// Effekseer �̕`������s
	// ���� effekseerMgr ���琶�������G�t�F�N�g�͂����ňꊇ�`�悳���
	effekseerMgr->Draw();

	// Effekseer �̕`����I��
	effekseerRenderer->EndRendering();
}