#include "../misc.h"
#include "Audio.h"

// コンストラクタ
Audio::Audio(IXAudio2* xaudio, std::shared_ptr<AudioResource>& resource)
	: resource(resource)
{
	HRESULT hr;

	// ソースボイスを生成
	hr = xaudio->CreateSourceVoice(&sourceVoice, &resource->getWaveFormat());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

// デストラクタ
Audio::~Audio()
{
	if (sourceVoice != nullptr)
	{
		sourceVoice->DestroyVoice();
		sourceVoice = nullptr;
	}
}

// 再生
void Audio::play(bool loop)
{
	stop();

	// ソースボイスにデータを送信
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = resource->getAudioBytes();
	buffer.pAudioData = resource->getAudioData();
	buffer.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	
	sourceVoice->SubmitSourceBuffer(&buffer);

	HRESULT hr = sourceVoice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

// 停止
void Audio::stop()
{
	sourceVoice->Stop();
}

//ワンショットのオーディオ再生 
void Audio::DCPlay()
{
	stop();
	sourceVoice->FlushSourceBuffers();
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = resource->getAudioBytes();
	buffer.pAudioData = resource->getAudioData();
	buffer.LoopCount = 0;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	sourceVoice->SubmitSourceBuffer(&buffer);

	HRESULT hr = sourceVoice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

//途中から再生
void Audio::middlePlay(float time)
{
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = resource->getAudioBytes();
	buffer.pAudioData = resource->getAudioData();
	buffer.LoopCount = 0;
	buffer.PlayBegin = resource->getWaveFormat().nSamplesPerSec * time;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	sourceVoice->SubmitSourceBuffer(&buffer);
	HRESULT hr = sourceVoice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

//指定した所をループ
void Audio::sustainLoop(float loop_start, float loop_end)
{
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.AudioBytes = resource->getAudioBytes();
	buffer.pAudioData = resource->getAudioData();
	buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	buffer.LoopBegin = resource->getWaveFormat().nSamplesPerSec * loop_start;
	buffer.LoopLength = resource->getWaveFormat().nSamplesPerSec * loop_end;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	sourceVoice->SubmitSourceBuffer(&buffer);
	HRESULT hr = sourceVoice->Start();
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

//ボリュームの変更
void Audio::setVolume(float volume)
{
	this->volume = volume;
	sourceVoice->SetVolume(volume);
}

//ピッチ
void Audio::setPitch(float Pitch)
{
	sourceVoice->SetFrequencyRatio(Pitch);
}

//ピッチのリセット
void Audio::resetPitch()
{
	sourceVoice->SetFrequencyRatio(1.0f);
}

//ポーズ
void Audio::pause()
{
	sourceVoice->Stop(XAUDIO2_PLAY_TAILS);
}

//リスタート
void Audio::reStart()
{
	sourceVoice->Start();
}

//フィルター
void Audio::setFilter(int type)
{
	XAUDIO2_FILTER_PARAMETERS paramers{};

	switch (type)
	{
	case Fliter_Type::LOW_PASS_FILTER:
		paramers.Type = XAUDIO2_FILTER_TYPE::LowPassFilter;
		paramers.Frequency = 0.5f / resource->getWaveFormat().nSamplesPerSec * 6.0f;
		paramers.OneOverQ = 1.4142f;
		break;
	case Fliter_Type::HIGH_PASS_FILTER:
		paramers.Type = XAUDIO2_FILTER_TYPE::HighPassFilter;
		paramers.Frequency = 0.5f / resource->getWaveFormat().nSamplesPerSec * 6.0f;
		paramers.OneOverQ = 1.4142f;
		break;
	case Fliter_Type::BAND_PASS_FILTER:
		paramers.Type = XAUDIO2_FILTER_TYPE::BandPassFilter;
		paramers.Frequency = 0.5f;
		paramers.OneOverQ = 1.0f;
		break;
	case Fliter_Type::NOTCH_FILTER:
		paramers.Type = XAUDIO2_FILTER_TYPE::NotchFilter;
		paramers.Frequency = 0.5f;
		paramers.OneOverQ = 1.0f;
		break;
	case Fliter_Type::LOW_PASS_ONE_POLE_FILTER:
		paramers.Type = XAUDIO2_FILTER_TYPE::LowPassOnePoleFilter;
		paramers.Frequency = XAudio2CutoffFrequencyToOnePoleCoefficient(0.5f, resource->getWaveFormat().nSamplesPerSec);
		break;
	case Fliter_Type::HIGH_PASS_ONE_POLE_FILTER:
		paramers.Type = XAUDIO2_FILTER_TYPE::HighPassOnePoleFilter;
		paramers.Frequency = XAudio2CutoffFrequencyToOnePoleCoefficient(0.5f, resource->getWaveFormat().nSamplesPerSec);
		break;
	}


	sourceVoice->SetFilterParameters(&paramers);
}


bool Audio::isPlay()
{
	XAUDIO2_VOICE_STATE state;
	sourceVoice->GetState(&state);
	//BuffersQueued が 0 でない、または SamplesPlayed が 0 でない場合、再生中とみなす
	if (state.BuffersQueued > 0 || state.SamplesPlayed > 0)return true;
	return false;
}
