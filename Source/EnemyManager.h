#pragma once
#include"Enemy.h"

//敵管理
class EnemyManager
{
private:
	EnemyManager(){}
	~EnemyManager() {}
public:
	//シングルトン
	static EnemyManager* instance()
	{
		static EnemyManager inst;
		return &inst;
	}

	//更新処理
	void update(float elapsedTime);

	//描画処理
	void render(ID3D11DeviceContext* dc);

	//敵同士の衝突判定
	void collisionEnemiesAndEnemies();

	//敵の登録
	void regist(Enemy* enemy);

	//敵の破棄
	void clear();

	//デバックプリミティブ描画
	void drawDebugPrimitive();

	//敵の総数を取得
	int getEnemyCount()const { return static_cast<int>(enemyArray.size()); }

	//敵を取得
	Enemy* getEnemy(int index) { return enemyArray.at(index); }

	//引数で貰ったポジションから一番近い敵をサーチ
	Enemy* searchEnemy(DirectX::XMFLOAT3* position);

	//敵削除
	void remove(Enemy* enemy);
private:
	std::vector<Enemy*>enemyArray;
	std::vector<Enemy*>removeArray;
};