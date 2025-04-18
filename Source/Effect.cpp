#include"Effect.h"
#include"EffectManager.h"
#include"DeviceManager.h"

//�R���X�g���N�^
Effect::Effect(const char* filename)
{
	// �r������
	std::lock_guard<std::mutex> lock(DeviceManager::instance()->getMutex());

	//�t�@�C������UTF-16�����Ή����ĂȂ��̂ŕ����R�[�h��ϊ�
	char16_t utf16Filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);
	
	Effekseer::ManagerRef effekseerMgr = EffectManager::instance()->getEffekseerMgr();

	//Effekseer�̃G�t�F�N�g��ǂݍ���
	effekseerEffect = Effekseer::Effect::Create(effekseerMgr, (EFK_CHAR*)utf16Filename);
}

//�Đ�
Effekseer::Handle Effect::play(const DirectX::XMFLOAT3* position, float scale)
{
	Effekseer::ManagerRef effekseerMgr = EffectManager::instance()->getEffekseerMgr();

	//�Đ��ƈʒu�ݒ�
	Effekseer::Handle handle = effekseerMgr->Play(effekseerEffect, position->x, position->y, position->z);

	//�g�嗦�̕ύX
	effekseerMgr->SetScale(handle, scale, scale, scale);

	return handle;
}

//��~
void Effect::stop(Effekseer::Handle handle)
{
	Effekseer::ManagerRef effeckseerMgr = EffectManager::instance()->getEffekseerMgr();

	effeckseerMgr->StopEffect(handle);
}

//�ʒu�ݒ�
void Effect::setPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3* position)
{
	Effekseer::ManagerRef effekseerMgr = EffectManager::instance()->getEffekseerMgr();

	effekseerMgr->SetLocation(handle, position->x, position->y, position->z);
}

//�g��k����
void Effect::setScale(Effekseer::Handle handle, const DirectX::XMFLOAT3* scale)
{
	Effekseer::ManagerRef effekseerMgr = EffectManager::instance()->getEffekseerMgr();

	effekseerMgr->SetScale(handle, scale->x, scale->y, scale->z);
}