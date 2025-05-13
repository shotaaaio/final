//#include"StraightBullet.h"
//#include"DeviceManager.h"
//
////コンストラクタ
//StraightBullet::StraightBullet(BulletManager*manager):Bullet(manager)
//{
//	model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\Sword\\Sword.cereal");
//
//	//モデルのスケール調整
//	const float scale_factor = 3.0f;
//	scale = { scale_factor, scale_factor, scale_factor };
//}
//
////デストラクタ
//StraightBullet::~StraightBullet()
//{
//
//}
//
////更新処理
//void StraightBullet::update(float elapsedTime)
//{
//	//経過時間を引いていく
//	lifeTime -= elapsedTime;
//	if (lifeTime <= 0.0f)
//	{
//		//時間が０以下になったら自分を破棄
//		destroy();
//	}
//
//	//移動
//	float speed = this->speed * elapsedTime;
//
//	//位置 += 方向 * 速さ
//	DirectX::XMStoreFloat3(&position,
//	DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position),
//		DirectX::XMVectorScale(DirectX::XMLoadFloat3(&direction), speed)));
//
//	//ワールド行列の更新
//	updateTransform();
//}
//
////描画処理
//void StraightBullet::render(ID3D11DeviceContext* dc)
//{
//	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
//}