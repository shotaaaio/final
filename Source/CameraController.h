#pragma once
#include<DirectXMath.h>
#include "Enemy.h"
#include "EnemyManager.h"
#include "Player.h"

//�J��������
class CameraController
{
public:
	CameraController() {}
	~CameraController(){}

	//�X�V����
	void update(float elapsedTime,float rot);

	//�^�[�Q�b�g�ʒu�ݒ�
	void setTarget(const DirectX::XMFLOAT3& t) { this->target = t; }
	
	//���ۂɗ��Ăق�Eye�̈ʒu
	DirectX::XMFLOAT3 computeEye(float rot);
	DirectX::XMFLOAT3 computeEyeTPS();

	//�J�����̉�]
	void rotateCameraAngle(float elapsedTime);

	//void setPlayer(Player* player) { player_ = player; } // ���������ɌĂ�
private:
	DirectX::XMFLOAT3 target = { 0,0,0 }; //�^�[�Q�b�g�ʒu
	DirectX::XMFLOAT3 angle  = { 0.5f,0,0 }; //�J�����̊p�x
	float range = 10.0f;

	float enemyVisibleTime = 0.0f;
	//Player* player_ = nullptr;
};