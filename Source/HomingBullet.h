//#pragma once
//#include"Buller.h"
//
////�ǔ��e
//class HomingBullet : public Bullet
//{
//public:
//	HomingBullet(BulletManager* manager);
//	~HomingBullet()override;
//
//	//�X�V����
//	void update(float elapsedTime)override;
//
//	//�`�揈��
//	void render(ID3D11DeviceContext* dc)override;
//
//	void lockOnTarget(DirectX::XMFLOAT3 target)
//	{
//		this->target = target;
//		//�^�[�Q�b�g���ύX������Ɏp�����ύX�����̂ł��̃^�C�~���O�Ŏp���s��̍X�V���K�v
//		updateTransform();
//	}
//private:
//	DirectX::XMFLOAT3 target = { 0,0,0 };
//	float speed = 10.0f; //�ړ����x
//	float turnSpeed = DirectX::XMConvertToRadians(180); // �ő��]���x
//	float lifeTimer = 3.0f; // ����(�R�b)
//};