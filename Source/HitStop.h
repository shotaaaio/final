#pragma once
#include <DirectXMath.h>

// �A�j���[�V�����p�X���[���[�V��������
class HitStop {
public:
	enum class HitType {
		None,
		Weak,
		Strong,
		Combo,
		Finish,
	};

	HitStop();
	~HitStop() = default;

	// �X���[���[�V�����X�V
	void Update(float elapsedTime);

	// �X���[���[�V�����J�n�i�q�b�g�^�C�v�ʂɋ��x��ݒ�j
	void StartHitStop(HitType hitType);

	float GetMotionTime()const { return motionTime; }

	// ���݂̃X���[���[�V�����{�����擾�i�A�j���[�V�����p�j
	float GetMotionSpeed() const { return motionSpeed; }

	// �X���[���[�V���������ǂ���
	bool IsSlowMotion() const { return isSlowMotion; }

	// ���݂̃q�b�g�^�C�v
	HitType GetHitType() const { return hitType; }

private:
	void StartSlowMotion(float slowRate, float time);
	void ResetSlowMotion(float elapsedTime);

private:
	// �q�b�g���
	HitType hitType = HitType::None;

	// �X���[���[�V��������
	bool isSlowMotion = false;
	float motionTime = 0.0f;//�q�b�g�X�g�b�v���̒���
	float motionSpeed = 1.0f;//�q�b�g�X�g�b�v���̑���
};