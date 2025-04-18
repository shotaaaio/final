#include "../Misc.h"
#include "AudioManager.h"

// �f�X�g���N�^
AudioManager::~AudioManager()
{
	// �}�X�^�����O�{�C�X�j��
	if (masteringVoice != nullptr)
	{
		masteringVoice->DestroyVoice();
		masteringVoice = nullptr;
	}

	// XAudio�I����
	if (xaudio != nullptr)
	{
		xaudio->Release();
		xaudio = nullptr;
	}

	// COM�I����
	CoUninitialize();
}

AudioManager* AudioManager::initialize()
{
	HRESULT hr;

	// COM�̏�����
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	UINT32 createFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	//createFlags |= XAUDIO2_DEBUG_ENGINE;
#endif

	// XAudio������
	hr = XAudio2Create(&xaudio, createFlags);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// �}�X�^�����O�{�C�X����
	hr = xaudio->CreateMasteringVoice(&masteringVoice);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	return this;
}

// �I�[�f�B�I�\�[�X�ǂݍ���
std::unique_ptr<Audio> AudioManager::loadAudioSource(const char* filename)
{
	std::shared_ptr<AudioResource> resource = std::make_shared<AudioResource>(filename);
	return std::make_unique<Audio>(xaudio, resource);
}
