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
		model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\nico\\nico.fbx");
		//model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\Jummo\\Jummo.fbx");

		const float scale_factor = 0.01f; //モデルが大きいのでスケーリング
		scale = { scale_factor,scale_factor,scale_factor };

		hitEffect = std::make_unique<Effect>("Resources\\Effect\\01_AndrewFM01\\hit_eff.efk");
		onLandingEffect = std::make_unique<Effect>("Resources\\Effect\\01_AndrewFM01\\blue_laser.efk");

		meshIndex = model->findMeshIndex("NIC:magic_wand");
		boneIndex = model->findBoneIndex("NIC:wand2_BK",meshIndex);

		//ステート群の設定
		SettingStates();

		comboTransitionMap = {
			{"WeakAttackCombo2",StateID::WeakAttackCombo2},
			{"WeakAttackCombo3",StateID::WeakAttackCombo3},
		};


		//それぞれのボタン入力の行動を設定
		Key_x = [this]() {
			StateID current = stateMachine->GetCurrentState();

			// コンボが始まっていなければ、弱攻撃スタート
			if (comboStep == ComboStep::None && !IsComboState()) {
				comboStep = ComboStep::Combo1;
				stateMachine->ChangeState(StateID::WeakAttack);
			}
			else {
				// アニメーションが半分以上進んでいて、コンボ継続可能なら
				if (/*IsCurrentAnimationFhinised()&&*/ CanComboContinue()) {
					int nextStep = static_cast<int>(comboStep) + 1;

					// nextStepがコンボ範囲内なら
					if (nextStep < static_cast<int>(ComboStep::Total)) {
						std::string comboKey = "WeakAttackCombo" + std::to_string(nextStep);
						auto it = comboTransitionMap.find(comboKey);
						if (it != comboTransitionMap.end()) {
							// 次のコンボをキュー
							this->nextStep = nextStep; 
							queuedNextCombo = true;
						}
					}
				}
			}
			};
		Key_z = [this]() {this->stateMachine->ChangeState(StateID::Jump);};
		Key_c = [this]() {this->stateMachine->ChangeState(StateID::BackStep); };
	}

	//デストラクタ
	Player::~Player()
	{
	}

	//更新処理
	void Player::update(float elapsedTime)
	{
		// スロー解除処理
		//ResetSlowAnimation(elapsedTime);

		hitStop->Update(elapsedTime);

		//移動受付
		InputMove(elapsedTime);

		//アクション受付
		inputAction();

		//プレイヤーと敵との衝突処置
		collisionPlayerAndEnemies();

		//ステートマシンの更新
		stateMachine->Update(elapsedTime);

		//コンボ更新処理
		UpdateCombo(elapsedTime);

		//アニメーション更新
		updateAnimation(elapsedTime);

		//ワールド行列の更新
		updateTransform();

		bonePosition = model->getBonePosition(meshIndex, boneIndex, keyframe, transform);
	}

//描画処理
void Player::render(ID3D11DeviceContext* dc)
{
	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, &keyframe);
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

	//プレイヤーのボーン位置
	ImGui::InputFloat3("lod pos", &bonePosition.x);

	//前方移動速度
	ImGui::SliderFloat("Desh speed", &dashSpeed, 1.0f, 50.0f);

	//後方移動速度
	ImGui::SliderFloat("backStep Speed", &backStepSpeed, 1.0f, 50.0f);
	//ヒットストップ情報
	if (ImGui::CollapsingHeader("HitStop Debug"))
	{
		ImGui::Text("Current Hit Type: %s",
			[&]() {
				switch (hitStop->GetHitType()) {
				case HitStop::HitType::Weak: return "Weak";
				case HitStop::HitType::Strong: return "Strong";
				case HitStop::HitType::Combo: return "Combo";
				case HitStop::HitType::Finish: return "Finish";
				default: return "None";
				}
			}());

		ImGui::Text("Active: %s", hitStop->IsSlowMotion() ? "True" : "False");
		ImGui::Text("Speed Rate: %.2f", hitStop->GetMotionSpeed());
		ImGui::Text("Remaining Time: %.2f", hitStop->GetMotionTime());

		ImGui::Separator();

		ImGui::Text("Manual Trigger:");
		if (ImGui::Button("Weak"))   hitStop->StartHitStop(HitStop::HitType::Weak);
		ImGui::SameLine();
		if (ImGui::Button("Strong")) hitStop->StartHitStop(HitStop::HitType::Strong);
		ImGui::SameLine();
		if (ImGui::Button("Combo"))  hitStop->StartHitStop(HitStop::HitType::Combo);
		ImGui::SameLine();
		if (ImGui::Button("Finish")) hitStop->StartHitStop(HitStop::HitType::Finish);
	}

	//ステートとコンボ情報
	ImGui::Separator();
	ImGui::Text("State: %s",stateMachine->GetCurrentStateName());
	ImGui::Text("Combo Step: %d", static_cast<int>(comboStep));
	ImGui::Text("Queued Next Combo: %s", queuedNextCombo ? "Yes" : "No");
	ImGui::Text("Next Step: %d", nextStep);
	ImGui::Text("Is Attacking: %s", isAttacking ? "True" : "False");

	//アニメーション情報
	ImGui::Separator();
	ImGui::Text("Animation Playing: %s", isPlayAnimation() ? "Yes" : "No");
	ImGui::Text("Animation Tick: %.2f", animation_tick);
	ImGui::Text("Animation Blend Time: %.2f / %.2f", animationBlendTime, animationBlendSeconds);
	
	//経過時間
	ImGui::Separator();
	ImGui::Text("Current Time: %.2f", currentTime);
	ImGui::Text("Attack Start Time: %.2f", attackStartTime);
}

void Player::drawDrawPrimitive()
{
	DebugRenderer* debugRenderer = GraphicsManager::instance()->getDebugRenderer();

	//衝突用のデバック球を描画
	debugRenderer->drawSphere(position, radius, { 0,0,0,1 });
	debugRenderer->drawCylinder(position, radius, height, { 0,0,0,1 });

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

//各ステートの設定
void Player::SettingStates()
{
	//ステートの初期化
	static const StateInfo<Player> playerStates[] = {
		{
			"Idle",
			[](Player* p,float elapsedTime) 
		{
			DirectX::XMFLOAT3 velocity = p->getVelocity();
			float speedXZ = std::sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (speedXZ > 0.1f)
			{
				p->stateMachine->ChangeState(StateID::Walk);
			}
		},
			[](Player* p) {p->playAnimation(0,true,1.0f); },
			[](Player* p) {},
			false
		},
		{
			"Jump",
			[](Player* p,float elapsedTime) {/*ジャンプ時の更新処理  後にパーティクル生成処理*/},
			[](Player* p) {//ステートに入ってきた時の処理
			p->playAnimation(5,false,1.0f);
			p->Jump(); },
			[](Player* p) {},//ステートから抜ける時の処理
			false//自動的にIdleに遷移する
		},
		{
			"Dash",
			[](Player* p,float elapsedTime) {/*ダッシュ時の更新処理*/},
			[](Player* p) { // ステートに入ってきた時の処理
			p->playAnimation(1, true, 1.0f);
		},
			[](Player* p) {},//ステートから抜ける時の処理
			true//自動的にIdleに遷移する
		},
		{
			"Walk",
			[](Player* p,float elapsedTime) 
		{
			DirectX::XMFLOAT3 velocity = p->getVelocity();
			float speedXZ = std::sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (speedXZ <= 0.1f)
			{
				p->stateMachine->ChangeState(StateID::Idle);
			}
		},
			[](Player* p)
		{
				p->playAnimation(1, true, 0.2f);
		},
			[](Player* p) {},
			false
		},
		{
			"WeakAttack",
			[](Player* p,float elapsedTime) {/*攻撃時の更新処理*/},
			[](Player* p) {//ステートに入ってきた時の処理
			p->playAnimation(7,false,1.0f);
			p->WeakAttack(); },
			[](Player* p) {},//ステートから抜ける時の処理
			true//自動的にIdleに遷移する
		},
		{
			"WeakAttackCombo2",
			[](Player* p,float elapsedTime) {/*攻撃時の更新処理*/},
			[](Player* p) {//ステートに入ってきた時の処理
			p->playAnimation(7,false,1.0f);
			p->WeakAttack(); },
			[](Player* p) {},//ステートから抜ける時の処理
			true//自動的にIdleに遷移する
		},
		{
			"WeakAttackCombo3",
			[](Player* p,float elapsedTime) {/*攻撃時の更新処理*/},
			[](Player* p) {//ステートに入ってきた時の処理
			p->playAnimation(7,false,1.0f);
			p->WeakAttack(); },
			[](Player* p) {},//ステートから抜ける時の処理
			true//自動的にIdleに遷移する
		},
		{
			"BackStep",
			[](Player* p,float elapsedTime) {/*バックステップ時の更新処理*/},
			[](Player* p) {//ステートに入ってきた時の処理
			p->playAnimation(5,false,1.0f);
			p->BackStep(); },
			[](Player* p) {},//ステートから抜ける時の処理
			true//自動的にIdleに遷移する
		}
	};

	//ステートマシンに対してステートをセット
	stateMachine = std::make_unique<StateMachine<Player>>(this, playerStates);
}
void Player::InputMove(float elapsedTime)
{
	//進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = getMoveVec();

	//速度処理更新
	updateVelocity(elapsedTime);

	//移動処理
	Move(moveVec.x, moveVec.z, moveSpeed);

	//旋回処理
	turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	//移動速度に応じてステートを切り替える処理
	//ChangeStateBySpeed(elapsedTime,moveSpeed);
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

//行動受付
void Player::inputAction()
{
	GamePad* gamePad = InputManager::instance()->getGamePad();
	if (gamePad->getButtonDown() & GamePad::BTN_A) if(Key_z)Key_z();
	if (gamePad->getButtonDown() & GamePad::BTN_B) Key_x();
	if (gamePad->getButtonDown() & GamePad::BTN_X) Key_c();
}

//ジャンプ入力処理
void Player::Jump()
{
	//ジャンプ
	jumpCount++;
	if (jumpCount <= jumpLimit) jump(jumpSpeed);
}

void Player::WeakAttack()
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

	// 扇の中に敵がいれば、ダッシュ
	if (dot >= thresholdCos)
	{
		//向きは常に敵に向ける
		angle.y = atan2f(dx, dz);

		// ダッシュ速度
		velocity.x = dx * dashSpeed;
		velocity.z = dz * dashSpeed;
	}
}

//コンボ時の攻撃
void Player::WeakAttackCombo2()
{
	//弱蹴り
}

//同上
void Player::WeakAttackCombo3()
{
	//反転強蹴り
}

//コンボ受付時間
bool Player::CanComboContinue() const
{
	return (currentTime - attackStartTime) <= comboGracePeriod;
}


bool Player::IsComboState() const
{
	StateID current = stateMachine->GetCurrentState();
	return current == StateID::WeakAttack ||
		current == StateID::WeakAttackCombo2 ||
		current == StateID::WeakAttackCombo3;
}

bool Player::IsCurrentAnimationFhinised()
{
	return false;

}

//コンボ更新処理
void Player::UpdateCombo(float elapsedTime)
{
	currentTime += elapsedTime;//経過時間

	StateID currentState = stateMachine->GetCurrentState();

	//攻撃開始状態になった瞬間に開始時間を記録
	// 攻撃開始状態に入った瞬間にタイムを記録
	if (!isAttacking && IsComboState())
	{
		attackStartTime = currentTime;
		isAttacking = true;
	}
	else if (!IsComboState())
	{
		isAttacking = false;
	}

	//コンボが入っていなければ早期リターン
	if (comboStep == ComboStep::None)return;

	//アニメーションが再生が終了していればコンボ終了
	if (!isPlayAnimation())
	{
		comboStep = ComboStep::None;
		queuedNextCombo = false;
		nextStep = -1;
		return;
	}

	//コンボ継続可能で次のコンボの予約がある場合
	if (CanComboContinue() && queuedNextCombo)
	{
		comboStep = static_cast<ComboStep>(nextStep);

		//次のコンボ用キーを生成
		std::string comboKey = "WeakAttackCombo" + std::to_string(nextStep);

		//マップから遷移先ステートを探す
		auto it = comboTransitionMap.find(comboKey);
		if (it != comboTransitionMap.end())
		{
			//ステート遷移
			stateMachine->ChangeState(it->second);
		}
		else
		{
			//遷移先がなければコンボ終了
			comboStep = ComboStep::None;
		}
		queuedNextCombo = false;
		nextStep = -1;
	}

}

//攻撃しながら後方にステップする処理
void Player::BackStep()
{
	// プレイヤーの後方方向を計算
	DirectX::XMFLOAT3 backward = { -Mathf::GetForwardDirection(angle.y).x, 0.0f, -Mathf::GetForwardDirection(angle.y).z };

	//補正値
	float correction = 2.0f;

	// 後方への移動速度を設定  
	velocity.x = backward.x * backStepSpeed;
	velocity.z = backward.z * backStepSpeed;

	// 攻撃エフェクトを再生  
	Effekseer::Handle handle = hitEffect->play(&position, 1.0f);
}

void Player::onLanding()
{
	jumpCount = 0;
	Effekseer::Handle handle = hitEffect->play(&position, 0.5f);
	stateMachine->ChangeState(StateID::Idle);
}

