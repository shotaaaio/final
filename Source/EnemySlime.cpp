#include"EnemySlime.h"
#include"DeviceManager.h"
#include"Graphics/GraphicsManager.h"

EnemySlime::EnemySlime()
{
	model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\Slime\\Slime.cereal");

	// ���f�����傫���̂ŃX�P�[������
	const float scale_factor = 0.01f;
	scale = { scale_factor, scale_factor, scale_factor };

	radius = 0.5;  //��
	height = 1.0f; //����
}

EnemySlime::~EnemySlime()
{

}

//�X�V����
void EnemySlime::update(float elapsedTime)
{
	//���x�����X�V
	updateVelocity(elapsedTime);

	//���G���Ԃ̍X�V
	updateInvincibleTimer(elapsedTime);

	//���[���h�s��X�V
	updateTransform();
}

//�`�揈��
void EnemySlime::render(ID3D11DeviceContext* dc)
{
	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}

//���S�������ɌĂ΂��
void EnemySlime::onDead()
{
	destroy();
}