#include"HomingBullet.h"
#include"DeviceManager.h"

//コンストラクタ
HomingBullet::HomingBullet(BulletManager* manager) :Bullet(manager)
{
	model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\Sword\\Sword.cereal");

	const float scale_factor = 3.0f;
	scale = { scale_factor ,scale_factor ,scale_factor };
}

//デストラクタ
HomingBullet::~HomingBullet()
{

}

//更新処理
void HomingBullet::update(float elapsedTime)
{
	//経過時間を引いていく
	lifeTimer -= elapsedTime;
	//時間が０以下になったら自分を破棄
	if (lifeTimer <= 0.0f) destroy();

	//移動
	float speed = this->speed * elapsedTime;

	//位置 += 方向 * 速さ
	DirectX::XMStoreFloat3(&position,
		DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position),
			DirectX::XMVectorScale(DirectX::XMLoadFloat3(&direction), speed)));

	//ターゲットとの距離をチェック
	DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&target);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(targetVec, posVec);

	//ゼロベクトルでないなら旋回処理
	DirectX::XMVECTOR lengthSqVec = DirectX::XMVector3Length(vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, lengthSqVec);
	if (lengthSq > 0.00001f)
	{
		//ターゲットまでのベクトルを方向に変換
		vec = DirectX::XMVector3Normalize(vec);

		//進行方向ベクトルとターゲットへの方向ベクトルを内積で角度を計算
		DirectX::XMVECTOR directVec = DirectX::XMLoadFloat3(&direction);
		DirectX::XMVECTOR dotVec = DirectX::XMVector3Dot(directVec, vec);

		float dot;
		DirectX::XMStoreFloat(&dot, dotVec);

		//ターゲットに近づく程ほど小さくする
		float rot = 1.0f - dot;
		if (rot > turnSpeed) rot = turnSpeed;

		//回転角度があるなら回転処理
		if (fabsf(rot) > 0.0001f)
		{
			//進行方向ベクトルとターゲットへのベクトルから回転するべき軸を計算
			DirectX::XMVECTOR axisVec = DirectX::XMVector3Cross(directVec, vec);

			//回転軸と回転量から回転行列を計算
			DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationAxis(axisVec, rot);

			//現在の行列に回転を反映させる
			DirectX::XMMATRIX transformMat = DirectX::XMLoadFloat4x4(&transform);
			transformMat = DirectX::XMMatrixMultiply(transformMat, rotationMat);

			//回転方向後の前方向を取り出す
			directVec = DirectX::XMVector3Normalize(transformMat.r[2]);
			//進行方法ベクトルとして設定する
			DirectX::XMStoreFloat3(&direction, directVec);
		}
	}
	//ワールド行列の更新
	updateTransform();
}

//描画処理
void HomingBullet::render(ID3D11DeviceContext* dc)
{
	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}