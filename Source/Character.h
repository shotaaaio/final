#pragma once
#include"Object.h"
#include<memory>
#include"StageManager.h"

//�L�����N�^�[
class Character : public Object
{
public:
	Character() {};
	virtual ~Character(){}

	//���a�̎擾
	float getRadius()const { return radius; }

	//���n�������ǂ����̃t���O�̎擾
	bool isGrounded() const { return isGroundedFlag; }

	//�����擾
	float getHeight() const { return height; }

	//���C�t���擾
	int getHealth()const { return health; }

	//�ő僉�C�t���擾
	int getMaxHealth()const { return maxHealth; }

	//���x�擾
	DirectX::XMFLOAT3 getVelocity() { return velocity; }

	void setVelocity(const DirectX::XMFLOAT3& v) { this->velocity = v; }

	/// <summary>
	/// �_���[�W����
	/// </summary>
	/// <param name="damage">�_���[�W��</param>
	/// <param name="invincibleTime">�ݒ肷�閳�G���� �f�t�H���g�l��0.5f</param>
	/// <returns>true...HP�ɕω���������</returns>
	bool applyDamage(int damage, float invincibleTime = 0.5f);

	//�Ռ���^����
	void addImpulse(const DirectX::XMFLOAT3& impulse);

	//���񏈗�
	void turn(float elapsedTime, float vx, float vz, float speed);
protected:
	//���x�����X�V
	void updateVelocity(float elapsedTime);

	//�������x�X�V����
	void updateVerticalVelocity(float elapsedTime);

	//�����ړ��X�V����
	void updateVerticalMove(float elapsedTime);

	//�������x�X�V����
	void updateHorizontalVelocity(float elapsedTime);

	//�����ړ��X�V����
	void updateHorizontalMove(float elapsedTime);

	//���n�����Ƃ��ɌĂяo�����
	virtual void onLanding(){}

	//�_���[�W���󂯂��Ƃ��ɌĂ΂��
	virtual void onDamaged(){}

	//���S�����Ƃ��ɌĂ΂��
	virtual void onDead(){}

	//���G���Ԃ̍X�V
	void updateInvincibleTimer(float elapsedTimer);

	//�W�����v����
	void jump(float speed);

	//�ړ�
	void Move(float vx, float vz, float speed);

protected:
	//���a
	float radius = 0.5f;

	//�d��
	float gravity = -1.0f;

	//���x
	DirectX::XMFLOAT3 velocity = { 0,0,0 };

	//true...���n����
	bool isGroundedFlag = false;

	//����
	float height = 2.0f;

	//���C�t
	int health = 5;

	//�ő僉�C�t
	int maxHealth = 5;

	//���G����
	float invincibleTimer = 0.5f;

	//���C
	float friction = 0.5f;

	//�����x
	float acceleration = 1.0f;

	//�ő呬�x
	float maxMoveSpeed = 5.0f;

	//�ړ�����
	DirectX::XMFLOAT3 direction = { 0.0f,0.0f,0.0f };

	//�󒆐���p�W��
	float airControl = 0.3f;

	//�X�Η�(�f�t�H���g = 1.0f)
	float slopeRate = 1.0f;


	//�A�j���[�V����

	//�A�j���[�V�����X�V
	void updateAnimation(float elapsedTime);
	//�A�j���[�V�����Đ�
	void playAnimation(int index, bool loop, float blendRate = 0.2f);
	//�A�j���[�V�����Đ�����
	bool isPlayAnimation()const;

	//�A�j���[�V�����ԍ�
	int animation_index = 1;
	animation::keyframe keyframe;

	//�A�j���[�V�����Đ�����
	float animation_tick = 0;

	//true...�A�j���[�V���������[�v����
	bool animationLoopFlag = false;

	//true...�A�j���[�V�������I������
	bool animationEndFlag = false;

	//�A�j���[�V�����u�����h�̌o�߂̍��v����
	float animationBlendTime = 0.0f;

	//�A�j���[�V�����u�����h�̎���
	float animationBlendSeconds = 0.0f;
public:
	virtual void onRayCastHit(DirectX::XMFLOAT3 s, DirectX::XMFLOAT3 e);
};