#pragma once

#include"Character.h"
#include"Effect.h"
#include<functional>
#include "StateMachine.h"

//�v���C���[
class Player : public Character
{
public:
	Player();
	~Player()override;

	//�X�V����
	void update(float elapsedTime);

	//�`�揈��
	void render(ID3D11DeviceContext* dc);

	//�f�o�b�N�p�̕`��
	void drawDebugGui();

	//�f�o�b�N�v���~�e�B�u�̕`��
	void drawDrawPrimitive();

	//�v���C���[�ƓG�Ƃ̏Փˏ���
	void collisionPlayerAndEnemies();

	//�ړ����͏���
	void InputMove(float elapsedTime);

	//���͎�t
	void inputAction();


protected:
	//���n�����Ƃ��Ăяo�����
	void onLanding()override;

private:
	//���͒l����ړ��x�N�g�����擾
	DirectX::XMFLOAT3 getMoveVec()const;

private:
	//�ړ��X�s�[�h
	float moveSpeed = 5.0f;

	//����X�s�[�h
	float turnSpeed = DirectX::XMConvertToRadians(720);

	//�W�����v��
	float jumpSpeed = 20.0f;

	// �W�����v��
	int jumpCount = 0;

	// �W�����v�����i�ő�W�����v���A�ЂƂ܂��Q�i�W�����v�j
	int jumpLimit = 2;

	//�_�b�V���X�s�[�h
	float dashSpeed = 10.0f;

	// ����ւ̃X�e�b�v���x
	float backStepSpeed = 20.0f;

	bool isSlowAnimation = true;
	float slowAnimationTimer = 0.0f;
	float slowAnimationSpeed = 0.2f;   //�X���[���̑��x
	float slowAnimationDuration = 0.5f;//�X���[��������
	float animationSpeed = 1.0f;       //���̑��x

	std::unique_ptr<Effect> hitEffect;
	std::unique_ptr<Effect> onLandingEffect;

	int boneIndex;
	int meshIndex;
	DirectX::XMFLOAT3 bonePosition = { 0,0,0 };

	//�v���C���[�s���p�|�C���^�[
	std::function<void()> Key_c;
	std::function<void()> Key_v;
	std::function<void()> Key_z;
	std::function<void()> Key_x;
	int attack_count = 2;

	//�X�e�[�g�}�V�������o�ϐ�
	std::unique_ptr<StateMachine<Player>>stateMachine;

	//�R���{�X�e�[�g�J�ڃ}�b�v
	std::unordered_map<std::string, StateID>comboTransitionMap;

	enum class ComboStep
	{
		None,
		Combo1,
		Combo2,
		Combo3,
		Total,
	};
	ComboStep comboStep = ComboStep::None;
	int nextStep = -1;
	bool queuedNextCombo = false;//���̃R���{���͂��L���[���ꂽ��
	float attackStartTime = 0.0f;      // �U�����n�܂�������
	float currentTime = 0.0f;          // ���݂̎��ԁi�Q�[�����ԂȂǁj
	const float comboGracePeriod = 0.8f; // �R���{�P�\���ԁi�b�j
	bool isAttacking = false;
	bool canMove = true;//�ړ��\��

	//�e�X�e�[�g�̐ݒ菈��
	void SettingStates();

	//�ړ�����
	void Jump();           //�W�����v����

	//��ޏ���
	void BackStep();

	//�U�������̐錾
	void WeakAttack(); //��U������
	void WeakAttackCombo2();
	void WeakAttackCombo3();



	//��ԃ`�F�b�N
	bool CanComboContinue() const;//�R���{�p���\��(�P�\���Ԃ�)
	bool IsComboState() const;//�R���{�X�e�[�g��
	bool IsCurrentAnimationFhinised();//�w�肵���A�j���[�V�����܂ŏI�����Ă��邩

	//�R���{�X�V����
	void UpdateCombo(float elapsedTime);

};