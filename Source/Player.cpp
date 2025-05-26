#include"Player.h"
#include"DeviceManager.h"
#include"Input/InputManager.h"
#include"Graphics/ImGuiRenderer.h"
#include"Camera.h"
#include"Graphics/DebugRenderer.h"
#include"Graphics/GraphicsManager.h"
#include"EnemyManager.h"
#include"Collision.h"
#include"StraightBullet.h"
#include"HomingBullet.h"
#include"Mathf.h"
#include "ObjManager.h"
#include "Graphics/Sprite.h"

//コンストラクタ
Player::Player()
{
	//model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\Jummo\\Jummo.fbx");
	model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\nico\\nico.fbx");

	//model->append_animations("Resources\\Model\\Jummo\\Animations\\Attack.fbx",0);
	const float scale_factor = 0.01f; //モデルが大きいのでスケーリング
	scale = { scale_factor,scale_factor,scale_factor };

	hitEffect = std::make_unique<Effect>("Resources\\Effect\\01_AndrewFM01\\hit_eff.efk");
	onLandingEffect = std::make_unique<Effect>("Resources\\Effect\\01_AndrewFM01\\blue_laser.efk");

	playAnimation(0,true);
	meshIndex = model->findMeshIndex("NIC:magic_wand");
	boneIndex = model->findBoneIndex("NIC:wand2_BK",meshIndex);

	//それぞれのボタン入力の行動を設定
	//Key_c = std::bind(&Player::FlyingStraightBullet,this);
	//Key_v = std::bind(&Player::FlyingHomingBullet,this);
	Key_z = std::bind(&Player::JumpAction, this);
	Key_x = std::bind(&Player::Attack, this);
}

//デストラクタ
Player::~Player()
{
}

//更新処理
void Player::update(float elapsedTime)
{
	if (!isPlayAnimation())
	{
		playAnimation(0, true);
	}

	//移動受付
	InputMove(elapsedTime);

	//アクション受付
	inputAction();

	//速度処理更新
	updateVelocity(elapsedTime);

	//弾更新処理
	//bulletMgr.update(elapsedTime);

	//プレイヤーと敵との衝突処置
	collisionPlayerAndEnemies();

	//敵と弾の衝突判定
	//collisionBulletAndEnemies();

	//アニメーションの更新
	updateAnimation(elapsedTime);

	//ワールド行列の更新
	updateTransform();

	bonePosition = model->getBonePosition(meshIndex, boneIndex, keyframe, transform);
}

//描画処理
void Player::render(ID3D11DeviceContext* dc)
{
	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, &keyframe);

	//弾描画処理
	//bulletMgr.render(dc);
}

//デバック用の描画
void Player::drawDebugGui()
{
	//位置
	ImGui::InputFloat3("Position", &position.x);

	//回転
	DirectX::XMFLOAT3 a{ DirectX::XMConvertToDegrees(angle.x),DirectX::XMConvertToDegrees(angle.y),DirectX::XMConvertToDegrees(angle.z) };
	ImGui::InputFloat3("Angle", &a.x);
	angle = { DirectX::XMConvertToRadians(a.x),DirectX::XMConvertToRadians(a.y), DirectX::XMConvertToRadians(a.z) };

	//拡大縮小
	ImGui::InputFloat3("Scale", &scale.x);

	//速度
	ImGui::InputFloat3("Velocity", &velocity.x);

	ImGui::InputFloat3("lod pos", &bonePosition.x);

	
}

void Player::drawDrawPrimitive()
{
	DebugRenderer* debugRenderer = GraphicsManager::instance()->getDebugRenderer();

	//衝突用のデバック球を描画
	//debugRenderer->drawSphere(position, radius, { 0,0,0,1 });
	debugRenderer->drawCylinder(position, radius, height, { 0,0,0,1 });

	//弾デバックプリミティブ描画
	//bulletMgr.drawDebugPrimitive();

	debugRenderer->drawSphere(
		bonePosition,
		1.0f,
		DirectX::XMFLOAT4(1, 0, 0, 1)
	);
}

//入力値から移動ベクトルを取得
DirectX::XMFLOAT3 Player::getMoveVec()const
{
	//入力情報を取得
	GamePad* gamePad = InputManager::instance()->getGamePad();
	float ax = gamePad->getAxisLX();
	float ay = gamePad->getAxisLY();

	//カメラの前方向を取得
	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT3* cameraFront = camera->getFront();
	const DirectX::XMFLOAT3* cameraRight = camera->getRight();

	// カメラ前方向ベクトルを XZ 単位ベクトルに変換
	float cameraFrontX = cameraFront->x;
	float cameraFrontZ = cameraFront->z;

	// カメラ前方向ベクトルを単位ベクトル化
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		cameraFrontX = cameraFrontX / cameraFrontLength;
		cameraFrontZ = cameraFrontZ / cameraFrontLength;
	}

	//カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight->x;
	float cameraRightZ = cameraRight->z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		cameraRightX = cameraRightX / cameraRightLength;
		cameraRightZ = cameraRightZ / cameraRightLength;
	}

	// 垂直入力値をカメラ前方向に、
	// 水平方向をカメラ右方向に反映し進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = cameraFrontX * ay + cameraRightX * ax;
	vec.z = cameraFrontZ * ay + cameraRightZ * ax;

	//Y軸方向に移動しない
	vec.y = 0.0f;
	return vec;
}

void Player::InputMove(float elapsedTime)
{
	//進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = getMoveVec();

	//移動処理
	Move(moveVec.x, moveVec.z, moveSpeed);

	//旋回処理
	turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}

//プレイヤーと敵との衝突判定
void Player::collisionPlayerAndEnemies()
{
	EnemyManager* eneMgr = EnemyManager::instance();

	//全ての敵と総当たり判定
	int enemyCount = eneMgr->getEnemyCount();
	for (int i = 0;i < enemyCount;++i)
	{
		Enemy* enemy = eneMgr->getEnemy(i);

		//衝突方向
		DirectX::XMFLOAT3 outVec;

		//衝突処理
		if (Collision::intersectCylinderAndCylinder(
			position,
			radius,
			height,
			*(enemy->getPosition()),
			enemy->getRadius(),
			height,
			outVec
		))
		{
			// プレイヤーの下端の高さ
			float playerBottom = position.y;
			float enemyTop = enemy->getPosition()->y + enemy->getHeight() * 0.5f;

			float verticalVelocity = velocity.y; // プレイヤーのY方向速度が必要（下に落ちてきてるか）

			// 踏みつけと判定する条件
			if (playerBottom > enemyTop && verticalVelocity < -0.1f)
			{
				jump(jumpSpeed * 0.5f);
				attack_count++;

				int damage = 2; // 仮のダメージとして１
				if (InputManager::instance()->attack)
				{
					if (enemy->applyDamage(damage, 0.1f))
					{
					}
				}
			}
			else
			{
				//半径の合計
				float range = radius + enemy->getRadius();

				//衝突方向のベクトルを合計の長さにスケーリング
				DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), range);

				//そのベクトルを衝突する側の位置に足す
				vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position), vec);

				//その位置ベクトルを衝突される側に設定する
				DirectX::XMFLOAT3 pos;
				DirectX::XMStoreFloat3(&pos, vec);
				pos.y = enemy->getPosition()->y;
				enemy->setPosition(pos);
			}
		}
	}
}
//
//void Player::collisionBulletAndEnemies()
//{
//	EnemyManager* enemyMgr = EnemyManager::instance();
//
//	//全ての弾と全ての敵を総当たりで衝突処理
//	int bulletCount = bulletMgr.getBulletCount();
//	int enemyCount = enemyMgr->getEnemyCount();
//
//	for (int i = 0;i < bulletCount;++i)
//	{
//		Bullet* bullet = bulletMgr.getBullet(i);
//		for (int j = 0;j < enemyCount;++j)
//		{
//			Enemy* enemy = EnemyManager::instance()->getEnemy(j);
//
//			//衝突処理
//			DirectX::XMFLOAT3 outVec;
//			if (Collision::intersectSphereAndCylinder(
//				*bullet->getPosition(),
//				bullet->getRadius(),
//				*enemy->getPosition(),
//				enemy->getRadius(),
//				enemy->getHeight(),
//				outVec
//			))
//			{
//				//仮のダメージ
//				int damage = 3;
//				if (enemy->applyDamage(damage,0.1f))
//				{
//					//吹き飛ばし
//					float power = 10.0f;
//					DirectX::XMFLOAT3 impulse;
//					impulse.x = outVec.x * power;
//					impulse.y = power * 0.5f;
//					impulse.z = outVec.z * power;
//					enemy->addImpulse(impulse);
//
//					//ヒットエフェクトの再生
//					DirectX::XMFLOAT3 enePos = *enemy->getPosition();
//					enePos.y += enemy->getHeight() * 0.5f;
//					Effekseer::Handle handle = hitEffect->play(&enePos, 0.5f);
//
//					//弾の破棄
//					bullet->destroy();
//				}
//			}
//		}
//	}
//}

//行動受付
void Player::inputAction()
{
	GamePad* gamePad = InputManager::instance()->getGamePad();
	//if (gamePad->getButtonDown() & GamePad::BTN_X) Key_c();
	//if (gamePad->getButtonDown() & GamePad::BTN_Y) Key_v();
	if (gamePad->getButtonDown() & GamePad::BTN_A) Key_z();
	if (gamePad->getButtonDown() & GamePad::BTN_B) Key_x();
}

//ジャンプ入力処理
void Player::JumpAction()
{
	//ジャンプ
	jumpCount++;
	if (jumpCount <= jumpLimit) jump(jumpSpeed);
	playAnimation(5, false);
}

void Player::Attack()
{
	// 最も近い敵を取得
	Enemy* enemy = EnemyManager::instance()->searchEnemy(&position);
	if (!enemy) return;

	DirectX::XMFLOAT3 enemyPos = *enemy->getPosition();

	// プレイヤー → 敵方向ベクトル
	float dx = enemyPos.x - position.x;
	float dz = enemyPos.z - position.z;

	float length = sqrtf(dx * dx + dz * dz);
	if (length < 0.0001f) return;

	dx /= length;
	dz /= length;

	// プレイヤーの前方向（angle.y による）
	DirectX::XMFLOAT3 forward = Mathf::GetForwardDirection(angle.y);

	// 内積でcosθを計算（前方向と敵方向）
	float dot = forward.x * dx + forward.z * dz;

	// 閾値（例：±45度以内にいる敵だけ許可 → cos(45°) ≒ 0.707）
	const float thresholdCos = cosf(DirectX::XMConvertToRadians(45.0f));

	// 攻撃アニメーション再生（常に）
	playAnimation(7, false);

	// 扇の中に敵がいれば、ダッシュ
	if (dot >= thresholdCos)
	{
		//向きは常に敵に向ける
		angle.y = atan2f(dx, dz);

		// ダッシュ速度
		float dashSpeed = 10.0f;
		velocity.x = dx * dashSpeed;
		velocity.z = dz * dashSpeed;
	}
}

//攻撃しながら後方にステップする処理
void Player::BackStepAttack()
{
}

//
//void Player::FlyingStraightBullet()
//{
//	//前方向
//	DirectX::XMFLOAT3 dir = Mathf::GetForwardDirection(angle.y);
//
//	//発射位置(プレイヤーの腰あたり)
//	DirectX::XMFLOAT3 pos;
//	pos.x = position.x;
//	pos.y = position.y + height * 0.5f;
//	pos.z = position.z;
//
//	//発射
//	StraightBullet* bullet = new StraightBullet(&bulletMgr);
//	bullet->setDirection(dir);
//	bullet->setPosition(pos);
//}
//
//void Player::FlyingHomingBullet()
//{
//	//前方向
//	DirectX::XMFLOAT3 dir = Mathf::GetForwardDirection(angle.y);
//
//	//発射位置(プレイヤーの腰あたり)
//	DirectX::XMFLOAT3 pos;
//	pos.x = position.x;
//	pos.y = position.y + height * 0.5f;
//	pos.z = position.z;
//
//	//ターゲット
//	DirectX::XMFLOAT3 target;
//	target.x = pos.x + dir.x * 1000.0f;
//	target.y = pos.y + dir.y * 1000.0f;
//	target.z = pos.z + dir.z * 1000.0f;
//
//	//一番近くの敵をターゲットに設定
//	Enemy* enemy = EnemyManager::instance()->searchEnemy(&pos);
//	if (enemy != nullptr)
//	{
//		target = *enemy->getPosition();
//		target.y += enemy->getHeight() * 0.5f;
//	}
//	//発射
//	HomingBullet* bullet = new HomingBullet(&bulletMgr);
//	bullet->setDirection(dir);
//	bullet->setPosition(pos);
//	bullet->lockOnTarget(target);
//}

void Player::onLanding()
{
	jumpCount = 0;
	Effekseer::Handle handle = hitEffect->play(&position, 0.5f);
}
