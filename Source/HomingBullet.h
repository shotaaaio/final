#pragma once
#include"Buller.h"

//追尾弾
class HomingBullet : public Bullet
{
public:
	HomingBullet(BulletManager* manager);
	~HomingBullet()override;

	//更新処理
	void update(float elapsedTime)override;

	//描画処理
	void render(ID3D11DeviceContext* dc)override;

	void lockOnTarget(DirectX::XMFLOAT3 target)
	{
		this->target = target;
		//ターゲットが変更した後に姿勢が変更されるのでこのタイミングで姿勢行列の更新が必要
		updateTransform();
	}
private:
	DirectX::XMFLOAT3 target = { 0,0,0 };
	float speed = 10.0f; //移動速度
	float turnSpeed = DirectX::XMConvertToRadians(180); // 最大回転速度
	float lifeTimer = 3.0f; // 寿命(３秒)
};