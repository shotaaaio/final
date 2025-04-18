#include"BulletManager.h"

//コンストラクタ
BulletManager::BulletManager()
{

}

//デストラクタ
BulletManager::~BulletManager()
{
	clear();
}

//更新処理
void BulletManager::update(float elapsedTime)
{
	for (Bullet* bullet : bulletArray)
	{
		bullet->update(elapsedTime);
	}

	//破棄処理
	for (Bullet* bullet : removes)
	{
		auto itr = std::find(bulletArray.begin(), bulletArray.end(), bullet);
		if (itr != bulletArray.end())
		{
			// 破棄すべき bullet が bullets 内に存在する場合、削除
			delete bullet;
			bulletArray.erase(itr);
		}
	}

	//破棄リストをクリア
	removes.clear();
}

//描画処理
void BulletManager::render(ID3D11DeviceContext* dc)
{
	for (Bullet* bullet : bulletArray)
	{
		bullet->render(dc);
	}
}

//デバックプリミティブ描画
void BulletManager::drawDebugPrimitive()
{
	for (Bullet* bullet : bulletArray)
	{
		bullet->drawDebugPrimitive();
	}
}

//弾登録
void BulletManager::regist(Bullet* bullet)
{
	bulletArray.emplace_back(bullet);
}

//弾全解除
void BulletManager::clear()
{
	for (Bullet* bullet : bulletArray)
	{
		delete bullet;
	}
	bulletArray.clear();
}

//弾削除
void BulletManager::remove(Bullet* bullet)
{
	auto itr = std::find(removes.begin(), removes.end(), bullet);

	//重複していたら追加しない
	if (itr != removes.end())return;

	//破棄リストに追加
	removes.push_back(bullet);
}