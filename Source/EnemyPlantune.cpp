#include"EnemyPlantune.h"
#include"DeviceManager.h"
#include"Graphics/GraphicsManager.h"

EnemyPlantune::EnemyPlantune()
{
	model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\Enemy\\plantune.fbx");

	// ���f�����傫���̂ŃX�P�[������
	const float scale_factor = 0.001f;
	scale = { scale_factor, scale_factor, scale_factor };

	radius = 0.5;  //��
	height = 1.0f; //����
}

EnemyPlantune::~EnemyPlantune()
{

}

//�X�V����
void EnemyPlantune::update(float elapsedTime)
{
	//���x�����X�V
	updateVelocity(elapsedTime);

	//���G���Ԃ̍X�V
	updateInvincibleTimer(elapsedTime);

	//���[���h�s��X�V
	updateTransform();
}

//�`�揈��
void EnemyPlantune::render(ID3D11DeviceContext* dc)
{
	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}

//���S�������ɌĂ΂��
void EnemyPlantune::onDead()
{
	destroy();
}