#include"Enemy.h"
#include"Graphics/GraphicsManager.h"
#include"EnemyManager.h"

//�f�o�b�N�v���~�e�B�u
void Enemy::drawDebugPrimitive()
{
	DebugRenderer* debugRenderer = GraphicsManager::instance()->getDebugRenderer();

	//�Փ˔���p�̃f�o�b�N����`��
	//debugRenderer->drawSphere(position, radius, { 0,0,0,1 });
	debugRenderer->drawCylinder(position, radius, height, { 0,0,0,1 });
}

//�j������
void Enemy::destroy()
{
	EnemyManager::instance()->remove(this);
}