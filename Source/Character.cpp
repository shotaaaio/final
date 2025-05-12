#include"Character.h"

bool Character::applyDamage(int damage,float invincibleTime)
{
	//�_���[�W���O�ȉ��̏ꍇ��HP�ɕω��Ȃ�
	if (damage <= 0)return false;

	//����HP���O�ȉ��i���S�j�̏ꍇ��HP�ɕω��Ȃ�
	if (health <= 0)return false;

	//���G����HP�ɕω��Ȃ�
	if (invincibleTimer > 0.0f)return false;
		
	//�_���[�W���󂯂��ۂɖ��G���Ԃ��Đݒ�
	invincibleTimer = invincibleTime;

	//�_���[�W�v�Z
	health -= damage;

	//�_���[�W�v�Z��̎c��HP�ŏ�����ω�
	if (health <= 0)
	{
		//���S
		onDead();
	}
	else
	{
		//�_���[�W���󂯂�
		onDamaged();
	}

	return true;
}

void Character::addImpulse(const DirectX::XMFLOAT3& impulse)
{
	//���x�ɗ͂�������
	//velocity += impulse
	DirectX::XMStoreFloat3(&velocity,
			DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&velocity), DirectX::XMLoadFloat3(&impulse)));
}

//�ړ�����������
void Character::Move(float vx, float vz, float speed)
{
	//�ړ������x�N�g��������
	direction.x = vx;
	direction.z = vz;

	//�ő呬�x�ݒ�
	maxMoveSpeed = speed;
}

void Character::turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	//�i�ނׂ��i�s�x�N�g�����[���̏ꍇ�͐��񏈗��̕K�v�Ȃ�
	float length = sqrtf(vx * vx + vz * vz);
	if (length < 0.001f) return;

	//�i�s�x�N�g����P�ʃx�N�g����
	vx /= length;
	vz /= length;

	//�O�����x�N�g����XZ�������擾���P�ʃx�N�g����
	float frontX = transform._31; //�O�����x�N�g���̂�����
	float frontZ = transform._33; //�O�����x�N�g����Z����
	float frontLength = sqrtf(frontX * frontX + frontZ * frontZ);
	if (frontLength > 0.0f)
	{
		frontX /= frontLength;
		frontZ /= frontLength;
	}

	//���E�`�F�b�N�ׂ̈̊O�όv�Z
	float cross = (frontZ * vx) - (frontX * vz);

	//��]�p�����߂�ׁA2�̒P�ʃx�N�g���̓��ς��v�Z����
	//2�̃x�N�g���̓��ϒl��-1.0~1.0�ŕ\������܂�
	float dot = (frontX * vx) + (frontZ * vz);

	//2�̃x�N�g�����d�Ȃ������A��]���x��0.0f�ɂȂ�
	float rot = (1.0f - dot);

	//���܂藣�ꂷ����Ɖ�]���x�������Ȃ肷����̂�speed�ȏ�̉�]���x�ɂ͂Ȃ�Ȃ��悤����
	if (rot > speed)rot = speed;

	//�O�ς����̏ꍇ�͉E��]�A���̏ꍇ�͍���]
	if (cross < 0.0f)
	{
		angle.y -= rot;
	}
	else
	{
		angle.y += speed;
	}
}

//�W�����v����
void Character::jump(float speed)
{
	//������̗͂�ݒ�
	velocity.y = speed;
}

//���x�����X�V
void Character::updateVelocity(float elapsedTime)
{
	//�������x�X�V����
	updateVerticalVelocity(elapsedTime);

	//�����ړ��X�V����
	updateVerticalMove(elapsedTime);

	//�������x�X�V����
	updateHorizontalVelocity(elapsedTime);

	//�����ړ��X�V����
	updateHorizontalMove(elapsedTime);
}

//���G���Ԃ̍X�V
void Character::updateInvincibleTimer(float elapsedTime)
{
	if (invincibleTimer > 0.0f)
	{
		invincibleTimer -= elapsedTime;
	}
}

//�����ړ��X�V����
void Character::updateVerticalVelocity(float elapsedTime)
{
	//�d�͏����i�t���[���P�ʂŌv�Z�j
	velocity.y += gravity * elapsedTime * 60.0f;
}

//�����ړ�����
void Character::updateVerticalMove(float elapsedTime)
{
	//���������̈ړ���
	float moveY = velocity.y * elapsedTime;

	if (moveY < 0.0f)
	{
		//������

		//���C�̊J�n�ʒu��ݒ�(�����Ƃ�菭����)
		DirectX::XMFLOAT3 start = { position.x, position.y + 1.0f, position.z };
		//���C�̏I�[�ʒu��ݒ�i�ړ���̈ʒu�j
		DirectX::XMFLOAT3 end = { position.x, position.y + moveY, position.z };

		//���C�L���X�g�ɂ��n�ʔ���
		HitResult hit;
		if (StageManager::instance()->getStage(0)->raycast(start, end, hit))
		{
			// �n�ʂɐݒu���Ă���i�q�b�g���ʂ� y �ʒu�𔽉f�j
			position.y = hit.position.y;
			velocity.y = 0.0f;

			//�X�Η��̌v�Z
			float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
			slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

			// ���n����
			if (isGroundedFlag == false)
			{
				onLanding();
			}
			isGroundedFlag = true;
		}
		else
		{
			// �󒆂ɕ����Ă���
			position.y += moveY;
			isGroundedFlag = false;
		}
	}
	else if (moveY > 0.0f)
	{
		//�㏸��
		position.y += moveY;
		isGroundedFlag = false;
	}


}

//�����ړ��X�V����
void Character::updateHorizontalVelocity(float elapsedTime)
{
	//���x�ɗ͂�������Ă�����i0 ����Ȃ�������j�����������s��
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length > 0.0f)
	{
		//���C�́i�t���[���P�ʂŌv�Z�j
		float friction = this->friction * elapsedTime * 60.0f;

		//�󒆂ɂ��鎞�͖��C�͂�����
		if (isGroundedFlag == false) friction*= airControl;

		if (length > friction)
		{
			//���C�ɂ�鉡�����̌�������
			float vx = velocity.x / length;
			float vz = velocity.z / length;

			velocity.x -= vx * friction;
			velocity.z -= vz * friction;
		}
		else
		{
			//�������̑��x�����C�͈ȉ��ɂȂ����̂ő��x�𖳌���
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	//�ő呬�x�ȉ��Ȃ�����������s��
	if (length <= maxMoveSpeed)
	{
		//�����x�N�g�����O�łȂ��Ȃ�����������s��
		float directionLength = sqrtf(direction.x * direction.x + direction.z * direction.z);
		if (directionLength > 0.0f)
		{
			//�����x�i�t���[���P�ʂŌv�Z�j
			float acceleration = this->acceleration * elapsedTime * 60.0f;

			//�󒆂ɂ��鎞�͖��C�͂�����
			if (isGroundedFlag == false) acceleration*= airControl;

			//��������
			//�����x�N�g���ɉ����x�����X�P�[�����O�������x�x�N�g���ɉ��Z
			velocity.x += direction.x * acceleration;
			velocity.z += direction.z * acceleration;


			//�X�Η��������ꍇ�͗����Ă��悤�Ɍv�Z
			if (isGroundedFlag == true && slopeRate > 0.0f)
				velocity.y -= length * slopeRate * elapsedTime * 60.0f;

			//�ő呬�x����
			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

			if (length > maxMoveSpeed)
			{
				//�����x�N�g���ɍő�ړ����x���X�P�[�����O�����l
				velocity.x = direction.x * maxMoveSpeed;
				velocity.z = direction.z * maxMoveSpeed;
			}
		}
	}
}

//�����ړ�����
void Character::updateHorizontalMove(float elapsedTime)
{
	float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f)
	{
		//�v�Z�p�̈ړ���̑��x
		float moveX = velocity.x * elapsedTime;
		float moveZ = velocity.z * elapsedTime;

		//���C�̎n�_�ʒu�ƏI�_�ʒu
		DirectX::XMFLOAT3 start = { position.x,position.y + 1.0f,position.z };
		DirectX::XMFLOAT3 end = { position.x + moveX,position.y + 1.0f,position.z + moveZ };

		//���C�L���X�g
		HitResult hit;
		if (StageManager::instance()->getStage(0)->raycast(start, end, hit))
		{
			onRayCastHit(start,end);
			DirectX::XMVECTOR startVec = DirectX::XMLoadFloat3(&hit.position);
			DirectX::XMVECTOR endVec = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(endVec, startVec);

			//�ǂ̖@���x�N�g��
			DirectX::XMVECTOR normalVec = DirectX::XMLoadFloat3(&hit.normal);

			//���˃x�N�g�����t�x�N�g���ɕϊ�
			vec = DirectX::XMVectorNegate(vec);

			//���˃x�N�g����@���Ŏˉe�i�ړ��ʒu����ǂ܂ł̋����j
			DirectX::XMVECTOR lengthVec = DirectX::XMVector3Dot(vec, normalVec);
			float length;
			DirectX::XMStoreFloat(&length, lengthVec);

			//�␳�ʒu�ւ̃x�N�g�����v�Z
			DirectX::XMVECTOR correctVec = DirectX::XMVectorScale(normalVec, length * 1.1f);

			//�ŏI�I�ȕ␳�ʒu�x�N�g�����v�Z
			DirectX::XMVECTOR correctPosVec = DirectX::XMVectorAdd(endVec, correctVec);
			DirectX::XMFLOAT3 correctPos;
			DirectX::XMStoreFloat3(&correctPos, correctPosVec);

			position.x = correctPos.x;
			position.z = correctPos.z;
		}
		else
		{
			//�ǂɓ������Ă��Ȃ��ꍇ�͒ʏ�̈ړ�
			position.x += moveX;
			position.z += moveZ;
		}
	}
}


void Character::updateAnimation(float elapsedTime)
{
	if (animation_index >= 0)
	{
		//�u�����h���̌v�Z
		float blendRate = 1.0f;
		if (animationBlendTime < animationBlendTime)
		{
			animationBlendTime += elapsedTime;
			if (animationBlendTime >= animationBlendSeconds)
			{
				animationBlendTime = animationBlendSeconds;
			}
			blendRate = animationBlendTime / animationBlendSeconds;

			//�񎟊֐��I�ɕ⊮������
			blendRate *= blendRate;
		}


		//�u�����h�⊮
		if (blendRate < 1.0f)
		{
			//���ʏo�͗p�̃L�[�t���[��
			animation::keyframe outKeyframe;
			//�J�n�A�j���[�V�����ƏI���A�j���[�V����
			const animation::keyframe* keyframes[2] =
			{
				&keyframe,
				&model->animation_clips.at(animation_index).sequence.at(0)
			};

			//�u�����h�A�j���[�V����
			model->blend_animations(keyframes, blendRate, outKeyframe);

			//�u�����h�⊮�����L�[�t���[���ōX�V
			model->update_animation(outKeyframe);

			keyframe = outKeyframe;
		}
		//�ʏ폈��
		else {
			//���f���A�j���[�V�����X�V
			int frame_index{ 0 };
			animation& animation{ model->animation_clips.at(animation_index) };

			//�A�j���[�V�������I�����Ă��Ȃ�������t���[���̌v�Z���s��
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

void Character::onRayCastHit(DirectX::XMFLOAT3 s, DirectX::XMFLOAT3 e) {
	// ���N���X�ł͉������Ȃ��Ă�OK
}