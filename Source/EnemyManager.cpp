#include"EnemyManager.h"
#include"Collision.h"

//更新処理
void EnemyManager::update(float elapsedTime)
{
	for (Enemy* enemy : enemyArray)
	{
		enemy->update(elapsedTime);
	}

	//破棄処理
	for (Enemy* enemy : removeArray)
	{
		auto itr = std::find(enemyArray.begin(), enemyArray.end(), enemy);
		if (itr != enemyArray.end())
		{
			// 破棄すべき bullet が bullets 内に存在する場合、削除
			delete enemy;
			enemyArray.erase(itr);
		}
	}

	//破棄リストをクリア
	removeArray.clear();

	//敵同士の衝突判定
	collisionEnemiesAndEnemies();
}

//描画処理
void EnemyManager::render(ID3D11DeviceContext* dc)
{
	for (Enemy* enemy : enemyArray)
	{
		enemy->render(dc);
	}
}

//敵を登録
void EnemyManager::regist(Enemy* enemy)
{
	enemyArray.emplace_back(enemy);
}

void EnemyManager::clear()
{
	for (Enemy* enemy : enemyArray)
	{
		delete enemy;
	}
	enemyArray.clear();
}

void EnemyManager::drawDebugPrimitive()
{
	for (Enemy* enemy : enemyArray)
	{
		enemy->drawDebugPrimitive();
	}
}

void EnemyManager::collisionEnemiesAndEnemies()
{
	int max = getEnemyCount();
	for (int i = 0;i < max;++i)
	{
		Enemy* enemy1 = getEnemy(i);
		for (int j = 0;j < max;++j)
		{
			//自分と自分で比べないように
			if (i == j)continue;
			Enemy* enemy2 = getEnemy(j);
			DirectX::XMFLOAT3 outVec;

			if(Collision::intersectCylinderAndCylinder(
				*(enemy1->getPosition()),
				enemy1->getRadius(),
				enemy1->getHeight(),
				*(enemy2->getPosition()),
				enemy2->getRadius(),
				enemy2->getHeight(),
				outVec
			))
			{
				//半径の合計
				float range = enemy1->getRadius() + enemy2->getRadius();

				//衝突方向のベクトルを合計の長さにスケーリング
				DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), range);

				//そのベクトルを衝突する側の位置に足す
				vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(enemy1->getPosition()), vec);

				//その位置ベクトルを衝突される側に設定する
				DirectX::XMFLOAT3 pos;
				DirectX::XMStoreFloat3(&pos, vec);
				enemy2->setPosition(pos);
			}
		}
	}
}

Enemy* EnemyManager::searchEnemy(DirectX::XMFLOAT3* position)
{
	float dist = FLT_MAX;
	int enemyCount = getEnemyCount();
	Enemy* cacheEnemy = nullptr;
	for (int i = 0;i < enemyCount;++i)
	{
		//敵との距離を判定
		Enemy* enemy = getEnemy(i);
		DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(position);
		DirectX::XMVECTOR eneVec = DirectX::XMLoadFloat3(enemy->getPosition());
		DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(eneVec, posVec);
		DirectX::XMVECTOR lengthSqVec = DirectX::XMVector3LengthSq(vec);
		float lengthSq;
		DirectX::XMStoreFloat(&lengthSq, lengthSqVec);
		if (lengthSq < dist)
		{
			//距離の更新
			dist = lengthSq;
			//現状一番近いエネミー
			cacheEnemy = enemy;
		}
	}
	return cacheEnemy;
}

void EnemyManager::remove(Enemy* enemy)
{
	auto itr = std::find(removeArray.begin(), removeArray.end(), enemy);

	//重複していたら追加しない
	if (itr != removeArray.end())return;

	//破棄リストに追加
	removeArray.push_back(enemy);
}

Enemy* EnemyManager::getEnemyByRay(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd)
{
	DirectX::XMVECTOR startVec = DirectX::XMLoadFloat3(&rayStart);
	DirectX::XMVECTOR endVec = DirectX::XMLoadFloat3(&rayEnd);
	DirectX::XMVECTOR rayDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(endVec, startVec));

	float closestDist = FLT_MAX;
	Enemy* closestEnemy = nullptr;

	for (int i = 0; i < getEnemyCount(); ++i)
	{
		Enemy* enemy = getEnemy(i);
		DirectX::XMFLOAT3 enemyPos = *enemy->getPosition();
		DirectX::XMVECTOR enemyVec = DirectX::XMLoadFloat3(&enemyPos);

		DirectX::XMVECTOR toEnemyVec = DirectX::XMVectorSubtract(enemyVec, startVec);
		float t;
		DirectX::XMStoreFloat(&t, DirectX::XMVector3Dot(toEnemyVec, rayDir));

		if (t < 0)
			continue;

		DirectX::XMVECTOR closestPoint = DirectX::XMVectorAdd(startVec, DirectX::XMVectorScale(rayDir, t));
		DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(enemyVec, closestPoint);

		float distSq;
		DirectX::XMStoreFloat(&distSq, DirectX::XMVector3LengthSq(diffVec));

		float radius = enemy->getRadius();
		if (distSq <= radius * radius && t < closestDist)
		{
			closestDist = t;
			closestEnemy = enemy;
		}
	}
	return closestEnemy;
}