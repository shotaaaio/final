#pragma once
#include"Graphics/SkinnedMesh.h"
#include"Enemy.h"

//スライム
class EnemySlime : public Enemy
{
public:
	EnemySlime();
	~EnemySlime();

	//更新処理
	void update(float elapsedTime);

	//描画処理
	void render(ID3D11DeviceContext* dc) override;

	//死亡したときに呼ばれる
	void onDead()override;
};