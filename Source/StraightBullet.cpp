//#include"StraightBullet.h"
//#include"DeviceManager.h"
//
////�R���X�g���N�^
//StraightBullet::StraightBullet(BulletManager*manager):Bullet(manager)
//{
//	model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\Sword\\Sword.cereal");
//
//	//���f���̃X�P�[������
//	const float scale_factor = 3.0f;
//	scale = { scale_factor, scale_factor, scale_factor };
//}
//
////�f�X�g���N�^
//StraightBullet::~StraightBullet()
//{
//
//}
//
////�X�V����
//void StraightBullet::update(float elapsedTime)
//{
//	//�o�ߎ��Ԃ������Ă���
//	lifeTime -= elapsedTime;
//	if (lifeTime <= 0.0f)
//	{
//		//���Ԃ��O�ȉ��ɂȂ����玩����j��
//		destroy();
//	}
//
//	//�ړ�
//	float speed = this->speed * elapsedTime;
//
//	//�ʒu += ���� * ����
//	DirectX::XMStoreFloat3(&position,
//	DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position),
//		DirectX::XMVectorScale(DirectX::XMLoadFloat3(&direction), speed)));
//
//	//���[���h�s��̍X�V
//	updateTransform();
//}
//
////�`�揈��
//void StraightBullet::render(ID3D11DeviceContext* dc)
//{
//	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
//}