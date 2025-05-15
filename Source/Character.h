#pragma once
#include"Object.h"
#include<memory>
#include"StageManager.h"

//キャラクター
class Character : public Object
{
public:
	Character() {};
	virtual ~Character(){}

	//半径の取得
	float getRadius()const { return radius; }

	//着地したかどうかのフラグの取得
	bool isGrounded() const { return isGroundedFlag; }

	//高さ取得
	float getHeight() const { return height; }

	//ライフを取得
	int getHealth()const { return health; }

	//最大ライフを取得
	int getMaxHealth()const { return maxHealth; }

	//速度取得
	DirectX::XMFLOAT3 getVelocity() { return velocity; }

	void setVelocity(const DirectX::XMFLOAT3& v) { this->velocity = v; }

	/// <summary>
	/// ダメージ処理
	/// </summary>
	/// <param name="damage">ダメージ量</param>
	/// <param name="invincibleTime">設定する無敵時間 デフォルト値は0.5f</param>
	/// <returns>true...HPに変化があった</returns>
	bool applyDamage(int damage, float invincibleTime = 0.5f);

	//衝撃を与える
	void addImpulse(const DirectX::XMFLOAT3& impulse);

	//旋回処理
	void turn(float elapsedTime, float vx, float vz, float speed);
protected:
	//速度処理更新
	void updateVelocity(float elapsedTime);

	//垂直速度更新処理
	void updateVerticalVelocity(float elapsedTime);

	//垂直移動更新処理
	void updateVerticalMove(float elapsedTime);

	//水平速度更新処理
	void updateHorizontalVelocity(float elapsedTime);

	//水平移動更新処理
	void updateHorizontalMove(float elapsedTime);

	//着地したときに呼び出される
	virtual void onLanding(){}

	//ダメージを受けたときに呼ばれる
	virtual void onDamaged(){}

	//死亡したときに呼ばれる
	virtual void onDead(){}

	//無敵時間の更新
	void updateInvincibleTimer(float elapsedTimer);

	//ジャンプ処理
	void jump(float speed);

	//移動
	void Move(float vx, float vz, float speed);

protected:
	//半径
	float radius = 0.5f;

	//重力
	float gravity = -1.0f;

	//速度
	DirectX::XMFLOAT3 velocity = { 0,0,0 };

	//true...着地した
	bool isGroundedFlag = false;

	//高さ
	float height = 2.0f;

	//ライフ
	int health = 5;

	//最大ライフ
	int maxHealth = 5;

	//無敵時間
	float invincibleTimer = 0.5f;

	//摩擦
	float friction = 0.5f;

	//加速度
	float acceleration = 1.0f;

	//最大速度
	float maxMoveSpeed = 5.0f;

	//移動方向
	DirectX::XMFLOAT3 direction = { 0.0f,0.0f,0.0f };

	//空中制御用係数
	float airControl = 0.3f;

	//傾斜率(デフォルト = 1.0f)
	float slopeRate = 1.0f;


	//アニメーション

	//アニメーション更新
	void updateAnimation(float elapsedTime);
	//アニメーション再生
	void playAnimation(int index, bool loop, float blendRate = 0.2f);
	//アニメーション再生中か
	bool isPlayAnimation()const;

	//アニメーション番号
	int animation_index = 1;
	animation::keyframe keyframe;

	//アニメーション再生時間
	float animation_tick = 0;

	//true...アニメーションがループした
	bool animationLoopFlag = false;

	//true...アニメーションが終了した
	bool animationEndFlag = false;

	//アニメーションブレンドの経過の合計時間
	float animationBlendTime = 0.0f;

	//アニメーションブレンドの時間
	float animationBlendSeconds = 0.0f;
public:
	virtual void onRayCastHit(DirectX::XMFLOAT3 s, DirectX::XMFLOAT3 e);
};