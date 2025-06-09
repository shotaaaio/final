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


	//�R���X�g���N�^
	Player::Player()
	{
		model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\nico\\nico.fbx");
		//model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\Jummo\\Jummo.fbx");

		const float scale_factor = 0.01f; //���f�����傫���̂ŃX�P�[�����O
		scale = { scale_factor,scale_factor,scale_factor };

		hitEffect = std::make_unique<Effect>("Resources\\Effect\\01_AndrewFM01\\hit_eff.efk");
		onLandingEffect = std::make_unique<Effect>("Resources\\Effect\\01_AndrewFM01\\blue_laser.efk");

		meshIndex = model->findMeshIndex("NIC:magic_wand");
		boneIndex = model->findBoneIndex("NIC:wand2_BK",meshIndex);

		//�X�e�[�g�Q�̐ݒ�
		SettingStates();

		comboTransitionMap = {
			{"WeakAttackCombo2",StateID::WeakAttackCombo2},
			{"WeakAttackCombo3",StateID::WeakAttackCombo3},
		};


		//���ꂼ��̃{�^�����͂̍s����ݒ�
		Key_x = [this]() {
			StateID current = stateMachine->GetCurrentState();

			// �R���{���n�܂��Ă��Ȃ���΁A��U���X�^�[�g
			if (comboStep == ComboStep::None && !IsComboState()) {
				comboStep = ComboStep::Combo1;
				stateMachine->ChangeState(StateID::WeakAttack);
			}
			else {
				// �A�j���[�V�����������ȏ�i��ł��āA�R���{�p���\�Ȃ�
				if (/*IsCurrentAnimationFhinised()&&*/ CanComboContinue()) {
					int nextStep = static_cast<int>(comboStep) + 1;

					// nextStep���R���{�͈͓��Ȃ�
					if (nextStep < static_cast<int>(ComboStep::Total)) {
						std::string comboKey = "WeakAttackCombo" + std::to_string(nextStep);
						auto it = comboTransitionMap.find(comboKey);
						if (it != comboTransitionMap.end()) {
							// ���̃R���{���L���[
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

	//�f�X�g���N�^
	Player::~Player()
	{
	}

	//�X�V����
	void Player::update(float elapsedTime)
	{
		// �X���[��������
		//ResetSlowAnimation(elapsedTime);

		hitStop->Update(elapsedTime);

		//�ړ���t
		InputMove(elapsedTime);

		//�A�N�V������t
		inputAction();

		//�v���C���[�ƓG�Ƃ̏Փˏ��u
		collisionPlayerAndEnemies();

		//�X�e�[�g�}�V���̍X�V
		stateMachine->Update(elapsedTime);

		//�R���{�X�V����
		UpdateCombo(elapsedTime);

		//�A�j���[�V�����X�V
		updateAnimation(elapsedTime);

		//���[���h�s��̍X�V
		updateTransform();

		bonePosition = model->getBonePosition(meshIndex, boneIndex, keyframe, transform);
	}

//�`�揈��
void Player::render(ID3D11DeviceContext* dc)
{
	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, &keyframe);
}

//�f�o�b�N�p�̕`��
void Player::drawDebugGui()
{
	//�ʒu
	ImGui::InputFloat3("Position", &position.x);

	//��]
	DirectX::XMFLOAT3 a{ DirectX::XMConvertToDegrees(angle.x),DirectX::XMConvertToDegrees(angle.y),DirectX::XMConvertToDegrees(angle.z) };
	ImGui::InputFloat3("Angle", &a.x);
	angle = { DirectX::XMConvertToRadians(a.x),DirectX::XMConvertToRadians(a.y), DirectX::XMConvertToRadians(a.z) };

	//�g��k��
	ImGui::InputFloat3("Scale", &scale.x);

	//���x
	ImGui::InputFloat3("Velocity", &velocity.x);

	//�v���C���[�̃{�[���ʒu
	ImGui::InputFloat3("lod pos", &bonePosition.x);

	//�O���ړ����x
	ImGui::SliderFloat("Desh speed", &dashSpeed, 1.0f, 50.0f);

	//����ړ����x
	ImGui::SliderFloat("backStep Speed", &backStepSpeed, 1.0f, 50.0f);
	//�q�b�g�X�g�b�v���
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

	//�X�e�[�g�ƃR���{���
	ImGui::Separator();
	ImGui::Text("State: %s",stateMachine->GetCurrentStateName());
	ImGui::Text("Combo Step: %d", static_cast<int>(comboStep));
	ImGui::Text("Queued Next Combo: %s", queuedNextCombo ? "Yes" : "No");
	ImGui::Text("Next Step: %d", nextStep);
	ImGui::Text("Is Attacking: %s", isAttacking ? "True" : "False");

	//�A�j���[�V�������
	ImGui::Separator();
	ImGui::Text("Animation Playing: %s", isPlayAnimation() ? "Yes" : "No");
	ImGui::Text("Animation Tick: %.2f", animation_tick);
	ImGui::Text("Animation Blend Time: %.2f / %.2f", animationBlendTime, animationBlendSeconds);
	
	//�o�ߎ���
	ImGui::Separator();
	ImGui::Text("Current Time: %.2f", currentTime);
	ImGui::Text("Attack Start Time: %.2f", attackStartTime);
}

void Player::drawDrawPrimitive()
{
	DebugRenderer* debugRenderer = GraphicsManager::instance()->getDebugRenderer();

	//�Փ˗p�̃f�o�b�N����`��
	debugRenderer->drawSphere(position, radius, { 0,0,0,1 });
	debugRenderer->drawCylinder(position, radius, height, { 0,0,0,1 });

	debugRenderer->drawSphere(
		bonePosition,
		1.0f,
		DirectX::XMFLOAT4(1, 0, 0, 1)
	);
}

//���͒l����ړ��x�N�g�����擾
DirectX::XMFLOAT3 Player::getMoveVec()const
{
	//���͏����擾
	GamePad* gamePad = InputManager::instance()->getGamePad();
	float ax = gamePad->getAxisLX();
	float ay = gamePad->getAxisLY();

	//�J�����̑O�������擾
	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT3* cameraFront = camera->getFront();
	const DirectX::XMFLOAT3* cameraRight = camera->getRight();

	// �J�����O�����x�N�g���� XZ �P�ʃx�N�g���ɕϊ�
	float cameraFrontX = cameraFront->x;
	float cameraFrontZ = cameraFront->z;

	// �J�����O�����x�N�g����P�ʃx�N�g����
	float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraFrontLength > 0.0f)
	{
		cameraFrontX = cameraFrontX / cameraFrontLength;
		cameraFrontZ = cameraFrontZ / cameraFrontLength;
	}

	//�J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraRightX = cameraRight->x;
	float cameraRightZ = cameraRight->z;
	float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLength > 0.0f)
	{
		cameraRightX = cameraRightX / cameraRightLength;
		cameraRightZ = cameraRightZ / cameraRightLength;
	}

	// �������͒l���J�����O�����ɁA
	// �����������J�����E�����ɔ��f���i�s�x�N�g�����v�Z����
	DirectX::XMFLOAT3 vec;
	vec.x = cameraFrontX * ay + cameraRightX * ax;
	vec.z = cameraFrontZ * ay + cameraRightZ * ax;

	//Y�������Ɉړ����Ȃ�
	vec.y = 0.0f;
	return vec;
}

//�e�X�e�[�g�̐ݒ�
void Player::SettingStates()
{
	//�X�e�[�g�̏�����
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
			[](Player* p,float elapsedTime) {/*�W�����v���̍X�V����  ��Ƀp�[�e�B�N����������*/},
			[](Player* p) {//�X�e�[�g�ɓ����Ă������̏���
			p->playAnimation(5,false,1.0f);
			p->Jump(); },
			[](Player* p) {},//�X�e�[�g���甲���鎞�̏���
			false//�����I��Idle�ɑJ�ڂ���
		},
		{
			"Dash",
			[](Player* p,float elapsedTime) {/*�_�b�V�����̍X�V����*/},
			[](Player* p) { // �X�e�[�g�ɓ����Ă������̏���
			p->playAnimation(1, true, 1.0f);
		},
			[](Player* p) {},//�X�e�[�g���甲���鎞�̏���
			true//�����I��Idle�ɑJ�ڂ���
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
			[](Player* p,float elapsedTime) {/*�U�����̍X�V����*/},
			[](Player* p) {//�X�e�[�g�ɓ����Ă������̏���
			p->playAnimation(7,false,1.0f);
			p->WeakAttack(); },
			[](Player* p) {},//�X�e�[�g���甲���鎞�̏���
			true//�����I��Idle�ɑJ�ڂ���
		},
		{
			"WeakAttackCombo2",
			[](Player* p,float elapsedTime) {/*�U�����̍X�V����*/},
			[](Player* p) {//�X�e�[�g�ɓ����Ă������̏���
			p->playAnimation(7,false,1.0f);
			p->WeakAttack(); },
			[](Player* p) {},//�X�e�[�g���甲���鎞�̏���
			true//�����I��Idle�ɑJ�ڂ���
		},
		{
			"WeakAttackCombo3",
			[](Player* p,float elapsedTime) {/*�U�����̍X�V����*/},
			[](Player* p) {//�X�e�[�g�ɓ����Ă������̏���
			p->playAnimation(7,false,1.0f);
			p->WeakAttack(); },
			[](Player* p) {},//�X�e�[�g���甲���鎞�̏���
			true//�����I��Idle�ɑJ�ڂ���
		},
		{
			"BackStep",
			[](Player* p,float elapsedTime) {/*�o�b�N�X�e�b�v���̍X�V����*/},
			[](Player* p) {//�X�e�[�g�ɓ����Ă������̏���
			p->playAnimation(5,false,1.0f);
			p->BackStep(); },
			[](Player* p) {},//�X�e�[�g���甲���鎞�̏���
			true//�����I��Idle�ɑJ�ڂ���
		}
	};

	//�X�e�[�g�}�V���ɑ΂��ăX�e�[�g���Z�b�g
	stateMachine = std::make_unique<StateMachine<Player>>(this, playerStates);
}
void Player::InputMove(float elapsedTime)
{
	//�i�s�x�N�g���擾
	DirectX::XMFLOAT3 moveVec = getMoveVec();

	//���x�����X�V
	updateVelocity(elapsedTime);

	//�ړ�����
	Move(moveVec.x, moveVec.z, moveSpeed);

	//���񏈗�
	turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	//�ړ����x�ɉ����ăX�e�[�g��؂�ւ��鏈��
	//ChangeStateBySpeed(elapsedTime,moveSpeed);
}

//�v���C���[�ƓG�Ƃ̏Փ˔���
void Player::collisionPlayerAndEnemies()
{
	EnemyManager* eneMgr = EnemyManager::instance();

	//�S�Ă̓G�Ƒ������蔻��
	int enemyCount = eneMgr->getEnemyCount();
	for (int i = 0;i < enemyCount;++i)
	{
		Enemy* enemy = eneMgr->getEnemy(i);

		//�Փ˕���
		DirectX::XMFLOAT3 outVec;

		//�Փˏ���
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
			// �v���C���[�̉��[�̍���
			float playerBottom = position.y;
			float enemyTop = enemy->getPosition()->y + enemy->getHeight() * 0.5f;

			float verticalVelocity = velocity.y; // �v���C���[��Y�������x���K�v�i���ɗ����Ă��Ă邩�j

			// ���݂��Ɣ��肷�����
			if (playerBottom > enemyTop && verticalVelocity < -0.1f)
			{
				jump(jumpSpeed * 0.5f);
				attack_count++;

				int damage = 2; // ���̃_���[�W�Ƃ��ĂP
				if (InputManager::instance()->attack)
				{
					if (enemy->applyDamage(damage, 0.1f))
					{
					}
				}
			}
			else
			{
				//���a�̍��v
				float range = radius + enemy->getRadius();

				//�Փ˕����̃x�N�g�������v�̒����ɃX�P�[�����O
				DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), range);

				//���̃x�N�g�����Փ˂��鑤�̈ʒu�ɑ���
				vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position), vec);

				//���̈ʒu�x�N�g�����Փ˂���鑤�ɐݒ肷��
				DirectX::XMFLOAT3 pos;
				DirectX::XMStoreFloat3(&pos, vec);
				pos.y = enemy->getPosition()->y;
				enemy->setPosition(pos);
			}
		}
	}
}

//�s����t
void Player::inputAction()
{
	GamePad* gamePad = InputManager::instance()->getGamePad();
	if (gamePad->getButtonDown() & GamePad::BTN_A) if(Key_z)Key_z();
	if (gamePad->getButtonDown() & GamePad::BTN_B) Key_x();
	if (gamePad->getButtonDown() & GamePad::BTN_X) Key_c();
}

//�W�����v���͏���
void Player::Jump()
{
	//�W�����v
	jumpCount++;
	if (jumpCount <= jumpLimit) jump(jumpSpeed);
}

void Player::WeakAttack()
{
	// �ł��߂��G���擾
	Enemy* enemy = EnemyManager::instance()->searchEnemy(&position);
	if (!enemy) return;

	DirectX::XMFLOAT3 enemyPos = *enemy->getPosition();

	// �v���C���[ �� �G�����x�N�g��
	float dx = enemyPos.x - position.x;
	float dz = enemyPos.z - position.z;

	float length = sqrtf(dx * dx + dz * dz);
	if (length < 0.0001f) return;

	dx /= length;
	dz /= length;

	// �v���C���[�̑O�����iangle.y �ɂ��j
	DirectX::XMFLOAT3 forward = Mathf::GetForwardDirection(angle.y);

	// ���ς�cos�Ƃ��v�Z�i�O�����ƓG�����j
	float dot = forward.x * dx + forward.z * dz;

	// 臒l�i��F�}45�x�ȓ��ɂ���G�������� �� cos(45��) �� 0.707�j
	const float thresholdCos = cosf(DirectX::XMConvertToRadians(45.0f));

	// ��̒��ɓG������΁A�_�b�V��
	if (dot >= thresholdCos)
	{
		//�����͏�ɓG�Ɍ�����
		angle.y = atan2f(dx, dz);

		// �_�b�V�����x
		velocity.x = dx * dashSpeed;
		velocity.z = dz * dashSpeed;
	}
}

//�R���{���̍U��
void Player::WeakAttackCombo2()
{
	//��R��
}

//����
void Player::WeakAttackCombo3()
{
	//���]���R��
}

//�R���{��t����
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

//�R���{�X�V����
void Player::UpdateCombo(float elapsedTime)
{
	currentTime += elapsedTime;//�o�ߎ���

	StateID currentState = stateMachine->GetCurrentState();

	//�U���J�n��ԂɂȂ����u�ԂɊJ�n���Ԃ��L�^
	// �U���J�n��Ԃɓ������u�ԂɃ^�C�����L�^
	if (!isAttacking && IsComboState())
	{
		attackStartTime = currentTime;
		isAttacking = true;
	}
	else if (!IsComboState())
	{
		isAttacking = false;
	}

	//�R���{�������Ă��Ȃ���Α������^�[��
	if (comboStep == ComboStep::None)return;

	//�A�j���[�V�������Đ����I�����Ă���΃R���{�I��
	if (!isPlayAnimation())
	{
		comboStep = ComboStep::None;
		queuedNextCombo = false;
		nextStep = -1;
		return;
	}

	//�R���{�p���\�Ŏ��̃R���{�̗\�񂪂���ꍇ
	if (CanComboContinue() && queuedNextCombo)
	{
		comboStep = static_cast<ComboStep>(nextStep);

		//���̃R���{�p�L�[�𐶐�
		std::string comboKey = "WeakAttackCombo" + std::to_string(nextStep);

		//�}�b�v����J�ڐ�X�e�[�g��T��
		auto it = comboTransitionMap.find(comboKey);
		if (it != comboTransitionMap.end())
		{
			//�X�e�[�g�J��
			stateMachine->ChangeState(it->second);
		}
		else
		{
			//�J�ڐ悪�Ȃ���΃R���{�I��
			comboStep = ComboStep::None;
		}
		queuedNextCombo = false;
		nextStep = -1;
	}

}

//�U�����Ȃ������ɃX�e�b�v���鏈��
void Player::BackStep()
{
	// �v���C���[�̌���������v�Z
	DirectX::XMFLOAT3 backward = { -Mathf::GetForwardDirection(angle.y).x, 0.0f, -Mathf::GetForwardDirection(angle.y).z };

	//�␳�l
	float correction = 2.0f;

	// ����ւ̈ړ����x��ݒ�  
	velocity.x = backward.x * backStepSpeed;
	velocity.z = backward.z * backStepSpeed;

	// �U���G�t�F�N�g���Đ�  
	Effekseer::Handle handle = hitEffect->play(&position, 1.0f);
}

void Player::onLanding()
{
	jumpCount = 0;
	Effekseer::Handle handle = hitEffect->play(&position, 0.5f);
	stateMachine->ChangeState(StateID::Idle);
}

