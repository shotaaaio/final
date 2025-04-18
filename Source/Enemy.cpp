#include"Enemy.h"
#include"Graphics/GraphicsManager.h"
#include"EnemyManager.h"

//デバックプリミティブ
void Enemy::drawDebugPrimitive()
{
	DebugRenderer* debugRenderer = GraphicsManager::instance()->getDebugRenderer();

	//衝突判定用のデバック球を描画
	//debugRenderer->drawSphere(position, radius, { 0,0,0,1 });
	debugRenderer->drawCylinder(position, radius, height, { 0,0,0,1 });
}

//破棄処理
void Enemy::destroy()
{
	EnemyManager::instance()->remove(this);
}