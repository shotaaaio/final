#include"HomingBullet.h"
#include"DeviceManager.h"

//�R���X�g���N�^
HomingBullet::HomingBullet(BulletManager* manager) :Bullet(manager)
{
	model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\Sword\\Sword.cereal");

	const float scale_factor = 3.0f;
	scale = { scale_factor ,scale_factor ,scale_factor };
}

//�f�X�g���N�^
HomingBullet::~HomingBullet()
{

}

//�X�V����
void HomingBullet::update(float elapsedTime)
{
	//�o�ߎ��Ԃ������Ă���
	lifeTimer -= elapsedTime;
	//���Ԃ��O�ȉ��ɂȂ����玩����j��
	if (lifeTimer <= 0.0f) destroy();

	//�ړ�
	float speed = this->speed * elapsedTime;

	//�ʒu += ���� * ����
	DirectX::XMStoreFloat3(&position,
		DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position),
			DirectX::XMVectorScale(DirectX::XMLoadFloat3(&direction), speed)));

	//�^�[�Q�b�g�Ƃ̋������`�F�b�N
	DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&target);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(targetVec, posVec);

	//�[���x�N�g���łȂ��Ȃ���񏈗�
	DirectX::XMVECTOR lengthSqVec = DirectX::XMVector3Length(vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, lengthSqVec);
	if (lengthSq > 0.00001f)
	{
		//�^�[�Q�b�g�܂ł̃x�N�g��������ɕϊ�
		vec = DirectX::XMVector3Normalize(vec);

		//�i�s�����x�N�g���ƃ^�[�Q�b�g�ւ̕����x�N�g������ςŊp�x���v�Z
		DirectX::XMVECTOR directVec = DirectX::XMLoadFloat3(&direction);
		DirectX::XMVECTOR dotVec = DirectX::XMVector3Dot(directVec, vec);

		float dot;
		DirectX::XMStoreFloat(&dot, dotVec);

		//�^�[�Q�b�g�ɋ߂Â����قǏ���������
		float rot = 1.0f - dot;
		if (rot > turnSpeed) rot = turnSpeed;

		//��]�p�x������Ȃ��]����
		if (fabsf(rot) > 0.0001f)
		{
			//�i�s�����x�N�g���ƃ^�[�Q�b�g�ւ̃x�N�g�������]����ׂ������v�Z
			DirectX::XMVECTOR axisVec = DirectX::XMVector3Cross(directVec, vec);

			//��]���Ɖ�]�ʂ����]�s����v�Z
			DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationAxis(axisVec, rot);

			//���݂̍s��ɉ�]�𔽉f������
			DirectX::XMMATRIX transformMat = DirectX::XMLoadFloat4x4(&transform);
			transformMat = DirectX::XMMatrixMultiply(transformMat, rotationMat);

			//��]������̑O���������o��
			directVec = DirectX::XMVector3Normalize(transformMat.r[2]);
			//�i�s���@�x�N�g���Ƃ��Đݒ肷��
			DirectX::XMStoreFloat3(&direction, directVec);
		}
	}
	//���[���h�s��̍X�V
	updateTransform();
}

//�`�揈��
void HomingBullet::render(ID3D11DeviceContext* dc)
{
	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}