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
#include "Ivy.h"
#include "Graphics/Sprite.h"
#include "Goal.h"

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
	/*Key_c = std::bind(&Player::FlyingStraightBullet,this);
	Key_v = std::bind(&Player::FlyingHomingBullet,this);*/
	Key_z = std::bind(&Player::JumpAction, this);
	Key_x = std::bind(&Player::Attack, this);

	shizukuImage = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Image\\shizuku.png");

	wakuImage = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Image\\waku.png");

	sakuImage = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Image\\柵.png");

	whiteImage = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Image\\white.jpg");

	clearText = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Font\\clear.png");

	timerText = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Font\\font4.png");

	rankText = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Font\\rank.png");

	secondsText = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Font\\seconds.png");
}

//デストラクタ
Player::~Player()
{
}

//更新処理
void Player::update(float elapsedTime)
{
	/*if (!IsLookEnemy(elapsedTime))
	{
		return;
	}*/
	if (IsGoal) {
		updateTransform();
		return;
	}

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

	Hashigo();

	inGoal();

	Timer(elapsedTime);

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

	if (attack_count >= 10)
	{
		shizukuImage->render(dc,
			600, 550, 100, 100,
			1, 1, 1, 1,
			0,
			0, 0, 768, 768
		);

		wakuImage->render(dc,
			565, 540, 170, 130,
			1, 1, 1, 1,
			0,
			0, 0, 800, 600
		);
	}

	if (IsGoal)
	{
		if (white_a <= 0.5)white_a += 0.002f;

		whiteImage->render(dc,
			0, 0, 1280, 720,
			1, 1, 1, white_a,
			0,
			0, 0, 850, 478
		);

		for (int i = 0; i < 10; i++)
		{
			posY1 += 0.5;
			posYMax = posY1 - (100 * (i + 1));
			if (posYMax >= 0) posYMax = 0;
			sakuImage->render(dc,
				140 * i - 50, posYMax, 200, 500,
				1, 1, 1, 1,
				180,
				-170, -290, 1200, 1200
			);
		}

		for (int i = 0; i < 10; i++)
		{
			posY2 -= 0.5;
			posYMax = posY2 + (100 * (i + 1));
			if (posYMax <= 470)
			{
				posYMax = 470;
				if (i == 9) clear_b = true;
			}
			sakuImage->render(dc,
				140 * (9 - i) - 50, posYMax, 200, 500,
				1, 1, 1, 1,
				0,
				170, 300, 1200, 1200
			);
		}

		if (clear_b) {
			if (clearTime <= 1) clearTime += 0.01f;
			clearText->render(dc,
				440, 310, 400, 76,
				1, 1, 1, clearTime,
				0,
				0, 0, 171, 38
			);
		}
	}

		/*if (clear_b)
		{*/
			if (two)
			{
				timerText->render(dc,
					920, 280, 60, 60,
					0, 0, 0, 1,
					0,
					time1, 60, 20, 20);
			}

			timerText->render(dc,
				970, 280, 60, 60,
				0, 0, 0, 1,
				0,
				time2, 60, 20, 20);

			secondsText->render(dc,
				1040, 280, 60, 60,
				1, 1, 1, 1,
				0,
				0, 0, 47, 46);

			rankText->render(dc,
				950, 350, 90, 90,
				1, 1, 0, 1,
				0,
				rank, 0, 36, 38);
		//}

	//弾描画処理
	bulletMgr.render(dc);
}

//デバック用の描画
void Player::drawDebugGui()
{
	ImGui::Begin("Player");

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

	ImGui::End();
}

void Player::drawDrawPrimitive()
{
	DebugRenderer* debugRenderer = GraphicsManager::instance()->getDebugRenderer();

	//衝突用のデバック球を描画
	//debugRenderer->drawSphere(position, radius, { 0,0,0,1 });
	debugRenderer->drawCylinder(position, radius, height, { 0,0,0,1 });

	//弾デバックプリミティブ描画
	bulletMgr.drawDebugPrimitive();

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
	if (!isOnLadder)
	{
		//進行ベクトル取得
		DirectX::XMFLOAT3 moveVec = getMoveVec();

		//移動処理
		Move(moveVec.x, moveVec.z, moveSpeed);

		//旋回処理
		turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
	}
	else
	{
		Move(0, 0, 0);
	}
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

void Player::collisionBulletAndEnemies()
{
	EnemyManager* enemyMgr = EnemyManager::instance();

	//全ての弾と全ての敵を総当たりで衝突処理
	int bulletCount = bulletMgr.getBulletCount();
	int enemyCount = enemyMgr->getEnemyCount();

	for (int i = 0;i < bulletCount;++i)
	{
		Bullet* bullet = bulletMgr.getBullet(i);
		for (int j = 0;j < enemyCount;++j)
		{
			Enemy* enemy = EnemyManager::instance()->getEnemy(j);

			//衝突処理
			DirectX::XMFLOAT3 outVec;
			if (Collision::intersectSphereAndCylinder(
				*bullet->getPosition(),
				bullet->getRadius(),
				*enemy->getPosition(),
				enemy->getRadius(),
				enemy->getHeight(),
				outVec
			))
			{
				//仮のダメージ
				int damage = 3;
				if (enemy->applyDamage(damage,0.1f))
				{
					//吹き飛ばし
					float power = 10.0f;
					DirectX::XMFLOAT3 impulse;
					impulse.x = outVec.x * power;
					impulse.y = power * 0.5f;
					impulse.z = outVec.z * power;
					enemy->addImpulse(impulse);

					//ヒットエフェクトの再生
					DirectX::XMFLOAT3 enePos = *enemy->getPosition();
					enePos.y += enemy->getHeight() * 0.5f;
					Effekseer::Handle handle = hitEffect->play(&enePos, 0.5f);

					//弾の破棄
					bullet->destroy();
				}
			}
		}
	}
}

//行動受付
void Player::inputAction()
{
	GamePad* gamePad = InputManager::instance()->getGamePad();
	if (gamePad->getButtonDown() & GamePad::BTN_X) Key_c();
	if (gamePad->getButtonDown() & GamePad::BTN_Y) Key_v();
	if (gamePad->getButtonDown() & GamePad::BTN_A) Key_z();
	if (gamePad->getButtonDown() & GamePad::BTN_B) Key_x();
}

//ジャンプ入力処理
void Player::JumpAction()
{
	//ジャンプ
	jumpCount++;
	if (jumpCount <= jumpLimit) jump(jumpSpeed);
}

void Player::Attack()
{
	playAnimation(7, false);
}


void Player::FlyingStraightBullet()
{
	//前方向
	DirectX::XMFLOAT3 dir = Mathf::GetForwardDirection(angle.y);

	//発射位置(プレイヤーの腰あたり)
	DirectX::XMFLOAT3 pos;
	pos.x = position.x;
	pos.y = position.y + height * 0.5f;
	pos.z = position.z;

	//発射
	StraightBullet* bullet = new StraightBullet(&bulletMgr);
	bullet->setDirection(dir);
	bullet->setPosition(pos);
}

void Player::FlyingHomingBullet()
{
	//前方向
	DirectX::XMFLOAT3 dir = Mathf::GetForwardDirection(angle.y);

	//発射位置(プレイヤーの腰あたり)
	DirectX::XMFLOAT3 pos;
	pos.x = position.x;
	pos.y = position.y + height * 0.5f;
	pos.z = position.z;

	//ターゲット
	DirectX::XMFLOAT3 target;
	target.x = pos.x + dir.x * 1000.0f;
	target.y = pos.y + dir.y * 1000.0f;
	target.z = pos.z + dir.z * 1000.0f;

	//一番近くの敵をターゲットに設定
	Enemy* enemy = EnemyManager::instance()->searchEnemy(&pos);
	if (enemy != nullptr)
	{
		target = *enemy->getPosition();
		target.y += enemy->getHeight() * 0.5f;
	}
	//発射
	HomingBullet* bullet = new HomingBullet(&bulletMgr);
	bullet->setDirection(dir);
	bullet->setPosition(pos);
	bullet->lockOnTarget(target);
}

void Player::onLanding()
{
	jumpCount = 0;
	Effekseer::Handle handle = hitEffect->play(&position, 0.5f);
}

void Player::Hashigo()
{
	ObjManager* eneMgr = ObjManager::instance();
	int enemyCount = eneMgr->getObjCount();
	isOnLadder = false;

	for (int i = 0; i < enemyCount; i++)
	{
		Obj* enemy = eneMgr->getObj(i);
		if (!enemy) continue;  // nullチェック

		// Ivyにキャストできるか
		Ivy* ivy = dynamic_cast<Ivy*>(enemy);
		const DirectX::XMFLOAT3* enePos = enemy->getPosition();
		if (!enePos) continue; // nullチェック

		DirectX::XMFLOAT3 outVec;
		if (Collision::intersectCylinderAndCylinder(position, radius, height, *enePos, enemy->getRadius(), enemy->getHeight(), outVec))
		{
			//Ivy以外の当たり判定
			DirectX::XMFLOAT3 toEnemyVec = {
				enePos->x - position.x,
				0.0f,
				enePos->z - position.z
			};

			GamePad* gamePad = InputManager::instance()->getGamePad();
			float ax = gamePad->getAxisLX();
			float ay = gamePad->getAxisLY();

			DirectX::XMVECTOR inputVec = DirectX::XMVector3Normalize(DirectX::XMVectorSet(ax, 0, ay, 0));
			DirectX::XMVECTOR toEnemy = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&toEnemyVec));
			float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(inputVec, toEnemy));

			if ((ax != 0 || ay != 0) && dot > 0.7f)
			{
				isOnLadder = true;
				velocity = { 0, ivy ? 2.0f : 0.0f, 0 }; // Ivy の場合だけ上昇
			}
			else
			{
				velocity.y = 0;
			}

			// Ivy かつ Fキーで大きくする処理
			if (ivy && attack_count >= 10 && GetKeyState('F') < 0)
			{
				const DirectX::XMFLOAT3* scale = enemy->getScale();
				if (scale)
				{
					DirectX::XMFLOAT3 eneScale = *scale;
					eneScale.y = 0.01f;
					eneScale.z = 0.01f;
					enemy->setScale(eneScale);
					enemy->setHeight(7);
				}
			}
		}
	}

}

void Player::inGoal()
{
	ObjManager* eneMgr = ObjManager::instance();

	int enemyCount = eneMgr->getObjCount();

	for (int i = 0; i < enemyCount; i++)
	{
		Obj* enemy = eneMgr->getObj(i);

		Goal* ivy = dynamic_cast<Goal*>(enemy);
		if (ivy)
		{
			DirectX::XMFLOAT3 outVec;

			if (Collision::intersectCylinderAndCylinder(position, radius, height, *ivy->getPosition(), ivy->getRadius(), ivy->getHeight(), outVec))
			{
				IsGoal = true;
				//SceneManager::instance()->changeScene(new SceneClear);
			}
		}
	}
}

void Player::Timer(float elapsedTime)
{
	// フレーム毎の更新処理
	seconds += elapsedTime;
	static float timerForTime2 = 0.0f;
	timerForTime2 += elapsedTime;

	// 毎秒（1秒ごと）に time2 を一気に 20 右へ
	if (timerForTime2 >= 1.0f)
	{
		time2 += 20;
		timerForTime2 -= 1.0f;  // 1秒分だけ引く（10秒までカウントできるように）
	}

	// 10秒経ったら time1 を20右に、time2をリセット
	if (time2 >= 200) // 1秒20px × 10秒 = 200px
	{
		time1 += 20;
		time2 = 0;
		two = true;
	}

	if (seconds < 10) rank = 0;
	else if (seconds < 20) rank = 38;
	else if (seconds < 30) rank = 72;

	/*if (timer > 1000)
	{
		if (position.x >= 23 && position.x <= 24 && position.z >= -0.5f && position.z <= 0.5f)
		{
			stop = true;
		}
	}*/
}

void Player::onRayCastHit(DirectX::XMFLOAT3 s, DirectX::XMFLOAT3 e)
{
	// velocityベクトル
	DirectX::XMVECTOR vVel = DirectX::XMLoadFloat3(&velocity);

	// カメラの右ベクトル（Right Vector）を取得する必要があります
	// 仮に cameraRight が取得済みとする
	//カメラの前方向を取得
	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT3* cameraRight = camera->getRight();
	DirectX::XMVECTOR vCameraRight = DirectX::XMLoadFloat3(cameraRight);

	// 内積をとって、右方向に進んでいるか確認
	float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(vVel, vCameraRight));
	if (dot > 0) {
		if (IsStageEast) IsGoal = true;
	}
}

void Player::Chase(float elapsedTime)
{
	EnemyManager* enemyMgr = EnemyManager::instance();

	int enemyCount = enemyMgr->getEnemyCount();

		for (int j = 0; j < enemyCount; ++j)
		{
			Enemy* enemy = EnemyManager::instance()->getEnemy(j);
			DirectX::XMVECTOR ePos = DirectX::XMLoadFloat3(enemy->getPosition());
			DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(ePos, DirectX::XMLoadFloat3(&position)));

			// 速度を定義（例：1.0f 単位／秒）
			float speed = 1.0f;
			DirectX::XMVECTOR velocity = DirectX::XMVectorScale(direction, speed * elapsedTime);

			// 移動
			ePos = DirectX::XMVectorAdd(ePos, velocity);

			//enemy->setPosition(DirectX::ePos)
		}

}

