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
	//model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\Jummo\\Jummo.fbx");
	model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\nico\\nico.fbx");

	//model->append_animations("Resources\\Model\\Jummo\\Animations\\Attack.fbx",0);
	const float scale_factor = 0.01f; //���f�����傫���̂ŃX�P�[�����O
	scale = { scale_factor,scale_factor,scale_factor };

	hitEffect = std::make_unique<Effect>("Resources\\Effect\\01_AndrewFM01\\hit_eff.efk");
	onLandingEffect = std::make_unique<Effect>("Resources\\Effect\\01_AndrewFM01\\blue_laser.efk");

	playAnimation(0,true);
	meshIndex = model->findMeshIndex("NIC:magic_wand");
	boneIndex = model->findBoneIndex("NIC:wand2_BK",meshIndex);

	//���ꂼ��̃{�^�����͂̍s����ݒ�
	//Key_c = std::bind(&Player::FlyingStraightBullet,this);
	//Key_v = std::bind(&Player::FlyingHomingBullet,this);
	Key_z = std::bind(&Player::JumpAction, this);
	Key_x = std::bind(&Player::Attack, this);
}

//�f�X�g���N�^
Player::~Player()
{
}

//�X�V����
void Player::update(float elapsedTime)
{
	if (!isPlayAnimation())
	{
		playAnimation(0, true);
	}

	//�ړ���t
	InputMove(elapsedTime);

	//�A�N�V������t
	inputAction();

	//���x�����X�V
	updateVelocity(elapsedTime);

	//�e�X�V����
	//bulletMgr.update(elapsedTime);

	//�v���C���[�ƓG�Ƃ̏Փˏ��u
	collisionPlayerAndEnemies();

	//�G�ƒe�̏Փ˔���
	//collisionBulletAndEnemies();

	//�A�j���[�V�����̍X�V
	updateAnimation(elapsedTime);

	//���[���h�s��̍X�V
	updateTransform();

	bonePosition = model->getBonePosition(meshIndex, boneIndex, keyframe, transform);
}

//�`�揈��
void Player::render(ID3D11DeviceContext* dc)
{
	model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, &keyframe);

	//�e�`�揈��
	//bulletMgr.render(dc);
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

	ImGui::InputFloat3("lod pos", &bonePosition.x);

	
}

void Player::drawDrawPrimitive()
{
	DebugRenderer* debugRenderer = GraphicsManager::instance()->getDebugRenderer();

	//�Փ˗p�̃f�o�b�N����`��
	//debugRenderer->drawSphere(position, radius, { 0,0,0,1 });
	debugRenderer->drawCylinder(position, radius, height, { 0,0,0,1 });

	//�e�f�o�b�N�v���~�e�B�u�`��
	//bulletMgr.drawDebugPrimitive();

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

void Player::InputMove(float elapsedTime)
{
	//�i�s�x�N�g���擾
	DirectX::XMFLOAT3 moveVec = getMoveVec();

	//�ړ�����
	Move(moveVec.x, moveVec.z, moveSpeed);

	//���񏈗�
	turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
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
//
//void Player::collisionBulletAndEnemies()
//{
//	EnemyManager* enemyMgr = EnemyManager::instance();
//
//	//�S�Ă̒e�ƑS�Ă̓G�𑍓�����ŏՓˏ���
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
//			//�Փˏ���
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
//				//���̃_���[�W
//				int damage = 3;
//				if (enemy->applyDamage(damage,0.1f))
//				{
//					//������΂�
//					float power = 10.0f;
//					DirectX::XMFLOAT3 impulse;
//					impulse.x = outVec.x * power;
//					impulse.y = power * 0.5f;
//					impulse.z = outVec.z * power;
//					enemy->addImpulse(impulse);
//
//					//�q�b�g�G�t�F�N�g�̍Đ�
//					DirectX::XMFLOAT3 enePos = *enemy->getPosition();
//					enePos.y += enemy->getHeight() * 0.5f;
//					Effekseer::Handle handle = hitEffect->play(&enePos, 0.5f);
//
//					//�e�̔j��
//					bullet->destroy();
//				}
//			}
//		}
//	}
//}

//�s����t
void Player::inputAction()
{
	GamePad* gamePad = InputManager::instance()->getGamePad();
	//if (gamePad->getButtonDown() & GamePad::BTN_X) Key_c();
	//if (gamePad->getButtonDown() & GamePad::BTN_Y) Key_v();
	if (gamePad->getButtonDown() & GamePad::BTN_A) Key_z();
	if (gamePad->getButtonDown() & GamePad::BTN_B) Key_x();
}

//�W�����v���͏���
void Player::JumpAction()
{
	//�W�����v
	jumpCount++;
	if (jumpCount <= jumpLimit) jump(jumpSpeed);
	playAnimation(5, false);
}

void Player::Attack()
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

	// �U���A�j���[�V�����Đ��i��Ɂj
	playAnimation(7, false);

	// ��̒��ɓG������΁A�_�b�V��
	if (dot >= thresholdCos)
	{
		//�����͏�ɓG�Ɍ�����
		angle.y = atan2f(dx, dz);

		// �_�b�V�����x
		float dashSpeed = 10.0f;
		velocity.x = dx * dashSpeed;
		velocity.z = dz * dashSpeed;
	}
}

//�U�����Ȃ������ɃX�e�b�v���鏈��
void Player::BackStepAttack()
{
}

//
//void Player::FlyingStraightBullet()
//{
//	//�O����
//	DirectX::XMFLOAT3 dir = Mathf::GetForwardDirection(angle.y);
//
//	//���ˈʒu(�v���C���[�̍�������)
//	DirectX::XMFLOAT3 pos;
//	pos.x = position.x;
//	pos.y = position.y + height * 0.5f;
//	pos.z = position.z;
//
//	//����
//	StraightBullet* bullet = new StraightBullet(&bulletMgr);
//	bullet->setDirection(dir);
//	bullet->setPosition(pos);
//}
//
//void Player::FlyingHomingBullet()
//{
//	//�O����
//	DirectX::XMFLOAT3 dir = Mathf::GetForwardDirection(angle.y);
//
//	//���ˈʒu(�v���C���[�̍�������)
//	DirectX::XMFLOAT3 pos;
//	pos.x = position.x;
//	pos.y = position.y + height * 0.5f;
//	pos.z = position.z;
//
//	//�^�[�Q�b�g
//	DirectX::XMFLOAT3 target;
//	target.x = pos.x + dir.x * 1000.0f;
//	target.y = pos.y + dir.y * 1000.0f;
//	target.z = pos.z + dir.z * 1000.0f;
//
//	//��ԋ߂��̓G���^�[�Q�b�g�ɐݒ�
//	Enemy* enemy = EnemyManager::instance()->searchEnemy(&pos);
//	if (enemy != nullptr)
//	{
//		target = *enemy->getPosition();
//		target.y += enemy->getHeight() * 0.5f;
//	}
//	//����
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
