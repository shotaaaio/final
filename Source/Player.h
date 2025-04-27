#pragma once

#include"Character.h"
#include"BulletManager.h"
#include"Effect.h"

#include<functional>

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

	//�G�ƒe�̏Փ˔���
	void collisionBulletAndEnemies();

	//�ړ����͏���
	void InputMove(float elapsedTime);

	//�A�N�V����
	void JumpAction();
	void Attack();
	void FlyingStraightBullet();
	void FlyingHomingBullet();

	//���͎�t
	void inputAction();
protected:
	//���n�����Ƃ��Ăяo�����
	void onLanding()override;

private:
	//���͒l����ړ��x�N�g�����擾
	DirectX::XMFLOAT3 getMoveVec()const;

	void Hashigo();

	void inGoal();
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

	//�e�Ǘ�
	BulletManager bulletMgr;

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

	bool isOnLadder;

	int attack_count = 0;

	bool IsGoal = false;//true...�S�[������

	bool move_b = true;//true...�v���C���[��������

	float posY1 = 0.0f;//�N���A���o�p
	float posY2 = 720.0f;//�N���A���o�p

	float posYMax = 0.0f;//�N���A���o�p

	float white_a = 0.0f;//�N���A���o�p

	bool clear_b = false;//�N���A���o�p

	float clearTime = 0.0f;//�N���A���o�p

	std::unique_ptr<Sprite> shizukuImage;

	std::unique_ptr<Sprite> wakuImage;

	std::unique_ptr<Sprite> sakuImage;

	std::unique_ptr<Sprite> whiteImage;

	std::unique_ptr<Sprite> clearText;
};

enum class NodeState
{
	Idle = 0,
	FwdRun,
	FwdStart,
	FwdStop,
	FwdAir,
	JumpRecovery,
	JumpStart,
	Attack,
	Death,
};

class Node
{
public:
	virtual ~Node() = default;
	virtual NodeState Execute() = 0; // �e�m�[�h���������郁�\�b�h
};

class PlayAnimationNode : public Node
{
public:
	PlayAnimationNode(NodeState node) { this->node = node; }
	NodeState Execute() override{}
private:
	NodeState node;
};

