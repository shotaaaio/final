#include"EnemySlime.h"
#include"DeviceManager.h"
#include"Graphics/GraphicsManager.h"

EnemySlime::EnemySlime()
{
	model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\Slime\\Slime.cereal");

	// モデルが大きいのでスケール調整
	const float scale_factor = 0.01f;
	scale = { scale_factor, scale_factor, scale_factor };

	radius = 0.5;  //幅
	height = 1.0f; //高さ
}

EnemySlime::~EnemySlime()
{

}

//更新処理
void EnemySlime::update(float elapsedTime)
{
	//速度処理更新
	updateVelocity(elapsedTime);

	//無敵時間の更新
	updateInvincibleTimer(elapsedTime);

	//ワールド行列更新
	updateTransform();
}

//描画処理
void EnemySlime::render(ID3D11DeviceContext* dc)
{
	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}

//死亡した時に呼ばれる
void EnemySlime::onDead()
{
	destroy();
}