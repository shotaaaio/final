#include"EnemyPlantune.h"
#include"DeviceManager.h"
#include"Graphics/GraphicsManager.h"

EnemyPlantune::EnemyPlantune()
{
	model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\Enemy\\plantune.fbx");

	// モデルが大きいのでスケール調整
	const float scale_factor = 0.001f;
	scale = { scale_factor, scale_factor, scale_factor };

	radius = 0.5;  //幅
	height = 1.0f; //高さ
}

EnemyPlantune::~EnemyPlantune()
{

}

//更新処理
void EnemyPlantune::update(float elapsedTime)
{
	//速度処理更新
	updateVelocity(elapsedTime);

	//無敵時間の更新
	updateInvincibleTimer(elapsedTime);

	//ワールド行列更新
	updateTransform();
}

//描画処理
void EnemyPlantune::render(ID3D11DeviceContext* dc)
{
	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}

//死亡した時に呼ばれる
void EnemyPlantune::onDead()
{
	destroy();
}