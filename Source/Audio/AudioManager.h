#pragma once

#include <xaudio2.h>
#include "Audio.h"

// �I�[�f�B�I�Ǘ�
class AudioManager
{
private:
	AudioManager() {}
	~AudioManager();

public:
	static AudioManager* instance()
	{
		static AudioManager inst;
		return &inst;
	}

	AudioManager* initialize();

	/// <summary>
	/// �ǂݍ���
	/// </summary>
	/// <param name="filename">�ǂݍ��ރt�@�C����</param>
	/// <returns>�쐬�����I�[�f�B�I</returns>
	std::unique_ptr<Audio> loadAudioSource(const char* filename);

private:
	IXAudio2*				xaudio = nullptr;
	IXAudio2MasteringVoice* masteringVoice = nullptr;
};
