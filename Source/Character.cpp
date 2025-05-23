#include"Character.h"

bool Character::applyDamage(int damage,float invincibleTime)
{
	//ダメージが０以下の場合はHPに変化なし
	if (damage <= 0)return false;

	//既にHPが０以下（死亡）の場合はHPに変化なし
	if (health <= 0)return false;

	//無敵中はHPに変化なし
	if (invincibleTimer > 0.0f)return false;
		
	//ダメージを受けた際に無敵時間を再設定
	invincibleTimer = invincibleTime;

	//ダメージ計算
	health -= damage;

	//ダメージ計算後の残りHPで処理を変化
	if (health <= 0)
	{
		//死亡
		onDead();
	}
	else
	{
		//ダメージを受けた
		onDamaged();
	}

	return true;
}

void Character::addImpulse(const DirectX::XMFLOAT3& impulse)
{
	//速度に力を加える
	//velocity += impulse
	DirectX::XMStoreFloat3(&velocity,
			DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&velocity), DirectX::XMLoadFloat3(&impulse)));
}

//移動方向を決定
void Character::Move(float vx, float vz, float speed)
{
	//移動方向ベクトルを決定
	direction.x = vx;
	direction.z = vz;

	//最大速度設定
	maxMoveSpeed = speed;
}

void Character::turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	//進むべき進行ベクトルがゼロの場合は旋回処理の必要なし
	float length = sqrtf(vx * vx + vz * vz);
	if (length < 0.001f) return;

	//進行ベクトルを単位ベクトル化
	vx /= length;
	vz /= length;

	//前方向ベクトルのXZ成分を取得し単位ベクトル化
	float frontX = transform._31; //前方向ベクトルのｘ成分
	float frontZ = transform._33; //前方向ベクトルのZ成分
	float frontLength = sqrtf(frontX * frontX + frontZ * frontZ);
	if (frontLength > 0.0f)
	{
		frontX /= frontLength;
		frontZ /= frontLength;
	}

	//左右チェックの為の外積計算
	float cross = (frontZ * vx) - (frontX * vz);

	//回転角を求める為、2つの単位ベクトルの内積を計算する
	//2つのベクトルの内積値は-1.0~1.0で表現されます
	float dot = (frontX * vx) + (frontZ * vz);

	//2つのベクトルが重なった時、回転速度は0.0fになる
	float rot = (1.0f - dot);

	//あまり離れすぎると回転速度が早くなりすぎるのでspeed以上の回転速度にはならないよう制限
	if (rot > speed)rot = speed;

	//外積が正の場合は右回転、負の場合は左回転
	if (cross < 0.0f)
	{
		angle.y -= rot;
	}
	else
	{
		angle.y += speed;
	}
}

//ジャンプ処理
void Character::jump(float speed)
{
	//上方向の力を設定
	velocity.y = speed;
}

//速度処理更新
void Character::updateVelocity(float elapsedTime)
{
	//垂直速度更新処理
	updateVerticalVelocity(elapsedTime);

	//垂直移動更新処理
	updateVerticalMove(elapsedTime);

	//水平速度更新処理
	updateHorizontalVelocity(elapsedTime);

	//水平移動更新処理
	updateHorizontalMove(elapsedTime);
}

//無敵時間の更新
void Character::updateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
	{
		invincibleTimer -= elapsedTime;
	}
}

//垂直移動更新処理
void Character::updateVerticalVelocity(float elapsedTime)
{
	//重力処理（フレーム単位で計算）
	velocity.y += gravity * elapsedTime * 60.0f;
}

//垂直移動処理
void Character::updateVerticalMove(float elapsedTime)
{
	//垂直方向の移動量
	float moveY = velocity.y * elapsedTime;

	if (moveY < 0.0f)
	{
		//落下中

		//レイの開始位置を設定(足もとより少し上)
		DirectX::XMFLOAT3 start = { position.x, position.y + 1.0f, position.z };
		//レイの終端位置を設定（移動後の位置）
		DirectX::XMFLOAT3 end = { position.x, position.y + moveY, position.z };

		//レイキャストによる地面判定
		HitResult hit;
		if (StageManager::instance()->getStage(0)->raycast(start, end, hit))
		{
			// 地面に設置している（ヒット結果の y 位置を反映）
			position.y = hit.position.y;
			velocity.y = 0.0f;

			//傾斜率の計算
			float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
			slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

			// 着地した
			if (isGroundedFlag == false)
			{
				onLanding();
			}
			isGroundedFlag = true;
		}
		else
		{
			// 空中に浮いている
			position.y += moveY;
			isGroundedFlag = false;
		}
	}
	else if (moveY > 0.0f)
	{
		//上昇中
		position.y += moveY;
		isGroundedFlag = false;
	}


}

//水平移動更新処理
void Character::updateHorizontalVelocity(float elapsedTime)
{
	//速度に力が加わっていたら（0 じゃなかったら）減速処理を行う
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length > 0.0f)
	{
		//摩擦力（フレーム単位で計算）
		float friction = this->friction * elapsedTime * 60.0f;

		//空中にいる時は摩擦力を減少
		if (isGroundedFlag == false) friction*= airControl;

		if (length > friction)
		{
			//摩擦による横方向の減速処理
			float vx = velocity.x / length;
			float vz = velocity.z / length;

			velocity.x -= vx * friction;
			velocity.z -= vz * friction;
		}
		else
		{
			//横方向の速度が摩擦力以下になったので速度を無効化
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	//最大速度以下なら加速処理を行う
	if (length <= maxMoveSpeed)
	{
		//方向ベクトルが０でないなら加速処理を行う
		float directionLength = sqrtf(direction.x * direction.x + direction.z * direction.z);
		if (directionLength > 0.0f)
		{
			//加速度（フレーム単位で計算）
			float acceleration = this->acceleration * elapsedTime * 60.0f;

			//空中にいる時は摩擦力を減少
			if (isGroundedFlag == false) acceleration*= airControl;

			//加速処理
			//方向ベクトルに加速度えをスケーリングした速度ベクトルに加算
			velocity.x += direction.x * acceleration;
			velocity.z += direction.z * acceleration;


			//傾斜率が高い場合は落ちてくように計算
			if (isGroundedFlag == true && slopeRate > 0.0f)
				velocity.y -= length * slopeRate * elapsedTime * 60.0f;

			//最大速度制限
			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

			if (length > maxMoveSpeed)
			{
				//方向ベクトルに最大移動速度をスケーリングした値
				velocity.x = direction.x * maxMoveSpeed;
				velocity.z = direction.z * maxMoveSpeed;
			}
		}
	}
}

//水平移動処理
void Character::updateHorizontalMove(float elapsedTime)
{
	float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f)
	{
		//計算用の移動後の速度
		float moveX = velocity.x * elapsedTime;
		float moveZ = velocity.z * elapsedTime;

		//レイの始点位置と終点位置
		DirectX::XMFLOAT3 start = { position.x,position.y + 1.0f,position.z };
		DirectX::XMFLOAT3 end = { position.x + moveX,position.y + 1.0f,position.z + moveZ };

		//レイキャスト
		HitResult hit;
		if (StageManager::instance()->getStage(0)->raycast(start, end, hit))
		{
			DirectX::XMVECTOR startVec = DirectX::XMLoadFloat3(&hit.position);
			DirectX::XMVECTOR endVec = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(endVec, startVec);

			//壁の法線ベクトル
			DirectX::XMVECTOR normalVec = DirectX::XMLoadFloat3(&hit.normal);

			//入射ベクトルを逆ベクトルに変換
			vec = DirectX::XMVectorNegate(vec);

			//入射ベクトルを法線で射影（移動位置から壁までの距離）
			DirectX::XMVECTOR lengthVec = DirectX::XMVector3Dot(vec, normalVec);
			float length;
			DirectX::XMStoreFloat(&length, lengthVec);

			//補正位置へのベクトルを計算
			DirectX::XMVECTOR correctVec = DirectX::XMVectorScale(normalVec, length * 1.1f);

			//最終的な補正位置ベクトルを計算
			DirectX::XMVECTOR correctPosVec = DirectX::XMVectorAdd(endVec, correctVec);
			DirectX::XMFLOAT3 correctPos;
			DirectX::XMStoreFloat3(&correctPos, correctPosVec);

			position.x = correctPos.x;
			position.z = correctPos.z;
		}
		else
		{
			//壁に当たっていない場合は通常の移動
			position.x += moveX;
			position.z += moveZ;
		}
	}
}


void Character::updateAnimation(float elapsedTime)
{
	if (animation_index >= 0)
	{
		//ブレンド率の計算
		float blendRate = 1.0f;
		if (animationBlendTime < animationBlendTime)
		{
			animationBlendTime += elapsedTime;
			if (animationBlendTime >= animationBlendSeconds)
			{
				animationBlendTime = animationBlendSeconds;
			}
			blendRate = animationBlendTime / animationBlendSeconds;

			//二次関数的に補完をする
			blendRate *= blendRate;
		}


		//ブレンド補完
		if (blendRate < 1.0f)
		{
			//結果出力用のキーフレーム
			animation::keyframe outKeyframe;
			//開始アニメーションと終了アニメーション
			const animation::keyframe* keyframes[2] =
			{
				&keyframe,
				&model->animation_clips.at(animation_index).sequence.at(0)
			};

			//ブレンドアニメーション
			model->blend_animations(keyframes, blendRate, outKeyframe);

			//ブレンド補完したキーフレームで更新
			model->update_animation(outKeyframe);

			keyframe = outKeyframe;
		}
		//通常処理
		else {
			//モデルアニメーション更新
			int frame_index{ 0 };
			animation& animation{ model->animation_clips.at(animation_index) };

			//アニメーションが終了していなかったらフレームの計算を行う
			if (animationEndFlag == false)
				frame_index = static_cast<int>(animation_tick * animation.sampling_rate);

			if (frame_index > animation.sequence.size() - 1)
			{
				frame_index = 0;
				animation_tick = 0;
				if (animationLoopFlag == false)
				{
					animationEndFlag = true;
					animation_index = -1;
				}
			}
			else
			{
				if (animationEndFlag == false)
					animation_tick += elapsedTime;
			}
			keyframe = animation.sequence.at(frame_index);

			model->update_animation(keyframe);
		}
	}
}

void Character::playAnimation(int index, bool loop, float blendRate)
{
	animation_index = index;
	animation_tick = 0.0f;
	animationLoopFlag = loop;
	animationEndFlag = false;
	animationBlendTime = 0.0;
	animationBlendSeconds = blendRate;
}

bool Character::isPlayAnimation() const
{
	if (animation_index < 0)return false;
	if (animation_index >= model->animation_clips.size())return false;
	return true;
}