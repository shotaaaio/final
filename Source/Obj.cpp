#include "Obj.h"
#include "Mathf.h"

// �s��X�V����
void Obj::updateTransform()
{
    // �X�P�[���s����쐬
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    // ��]�s����쐬
    DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
    DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
    DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
    DirectX::XMMATRIX R = Y * X * Z;
    // �ʒu�s����쐬
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    // �R�̍s���g�ݍ��킹�A���[���h�s����쐬
    DirectX::XMMATRIX W = S * R * T;
    // �v�Z�������[���h�s��� transform �Ɏ��o��
    DirectX::XMStoreFloat4x4(&transform, W);
}

void Obj::addImpulse(const DirectX::XMFLOAT3& impulse)
{
    // ���x�ɗ͂�������
    velocity.x += impulse.x;
    velocity.y += impulse.y;
    velocity.z += impulse.z;
}

void Obj::updateVelocity(float elapsedTime)
{
    // �������x�X�V����
    updateVerticalVelocity(elapsedTime);

    // �����ړ��X�V����
    updateVerticalMove(elapsedTime);

    updateHorizontalVelocity(elapsedTime);

    updateHorizontalMove(elapsedTime);
}

void Obj::updateVerticalVelocity(float elapsedTime)
{
    // �d�͏����i�t���[���P�ʂŌv�Z�j
    velocity.y += gravity * elapsedTime * 60.0f;
}

void Obj::updateVerticalMove(float elapsedTime)
{
    // ���������̈ړ���
    float moveY = velocity.y * elapsedTime;

    // �p������p�@���x�N�g���i�f�t�H���g�͏�����j
    DirectX::XMFLOAT3 normal = { 0,1,0 };

    if (moveY < 0.0f)
    {
        // ������

        // ���C�̊J�n�ʒu��ݒ�i�����Ƃ�菭����j
        DirectX::XMFLOAT3 start = { position.x, position.y + 1.0f, position.z };
        // ���C�̏I�_�ʒu��ݒ�i�ړ���̈ʒu�j
        DirectX::XMFLOAT3 end = { position.x, position.y + moveY, position.z };

        // ���C�L���X�g�ɂ��n�ʔ���
        HitResult hit;
        if (StageManager::instance()->RayCast(start, end, hit))
        {
            // �n�ʂ̌������p������p�@���x�N�g���ɐݒ�
            normal = hit.normal;

            // �n�ʂɐݒu���Ă���i�q�b�g���ʂ� y �ʒu�𔽉f�j
            position.y = hit.position.y;
            velocity.y = 0.0f;

            // �X�Η��̌v�Z
            float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
            slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

            groundedFlag = true;
        }
        else
        {
            // �󒆂ɕ����Ă���
            position.y += moveY;
            groundedFlag = false;
        }
    }
    else if (moveY > 0.0f)
    {
        // �㏸��
        position.y += moveY;
        groundedFlag = false;
    }

    // �p������p�@���x�N�g������ x �� z �̊p�x���v�Z
    // y �����p������p�@���x�N�g�������Ɍ����悤�Ɋp�x���v�Z���܂�
    //float angleX = atan2f(normal.z, normal.y);
    //float angleZ = -atan2f(normal.x, normal.y);

    //// ���`��ԂŊ��炩�ɉ�]
    //angle.x = Mathf::Lerp(angle.x, angleX, 0.2f);
    //angle.z = Mathf::Lerp(angle.z, angleZ, 0.2f);
}

void Obj::updateHorizontalVelocity(float elapsedTime)
{
    // ���x�ɗ͂�������Ă�����i0 ����Ȃ�������j�����������s��
    float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    if (length > 0.0f)
    {
        // ���C�́i�t���[���P�ʂŌv�Z�j
        float friction = this->friction * elapsedTime * 60.0f;

        // �󒆂ɂ���Ƃ��͖��C�͂�����
        if (groundedFlag == false)
            friction *= airControl;

        if (length > friction)
        {
            // ���C�ɂ�鉡�����̌�������
            float vx = velocity.x / length;//���K�����Ă���
            float vz = velocity.z / length;

            velocity.x -= vx * friction;
            velocity.z -= vz * friction;
        }
        else
        {
            // �������̑��x�����C�͈ȉ��ɂȂ����̂ő��x�𖳌���
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
    }

    // �ő呬�x�ȉ��Ȃ�����������s��
    if (length <= maxMoveSpeed)
    {
        // �����x�N�g�����[���łȂ��Ȃ�����������s��
           // �[���̏ꍇ�͓��͂���Ă��Ȃ�
        float directionLength = sqrtf(direction.x * direction.x + direction.z * direction.z);
        if (directionLength > 0.0f)
        {
            // �����x�i�t���[���P�ʂŌv�Z�j
            float acceleration = this->acceleration * elapsedTime * 60.0f;

            // �󒆂ɂ���Ƃ��͉����x������
            if (groundedFlag == false)
                acceleration *= airControl;

            // ��������
                // �����x�N�g���ɉ����x���X�P�[�����O�����l�𑬓x�x�N�g���ɉ��Z
            velocity.x += direction.x * acceleration;
            velocity.z += direction.z * acceleration;

            // �X�Η��������ꍇ�͗����Ă����悤�Ɍv�Z
            if (groundedFlag == true && slopeRate > 0.0f)
                velocity.y -= length * slopeRate * elapsedTime * 60.0f;

            // �ő呬�x����
            float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
            if (length > maxMoveSpeed)
            {
                // �����x�N�g���ɍő�ړ����x���X�P�[�����O�����l�𑬓x�x�N�g���ɑ��
                velocity.x = direction.x * maxMoveSpeed;
                velocity.z = direction.z * maxMoveSpeed;
            }
        }
    }
}

void Obj::updateHorizontalMove(float elapsedTime)
{
    // �������x�ʌv�Z
    float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    if (velocityLengthXZ > 0.0f)
    {
        // �v�Z�p�̈ړ���̑��x
        float moveX = velocity.x * elapsedTime;
        float moveZ = velocity.z * elapsedTime;

        // ���C�̎n�_�ʒu�ƏI�_�ʒu
        DirectX::XMFLOAT3 start = { position.x, position.y + 1.0f, position.z };
        DirectX::XMFLOAT3 end = { position.x + moveX, position.y + 1.0f, position.z + moveZ };

        // ���C�L���X�g
        HitResult hit;
        if (StageManager::instance()->getStage(0)->raycast(start, end, hit))
        {
            DirectX::XMVECTOR startVec = DirectX::XMLoadFloat3(&hit.position);
            DirectX::XMVECTOR endVec = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(endVec, startVec);

            // �ǂ̖@���x�N�g��
            DirectX::XMVECTOR normalVec = DirectX::XMLoadFloat3(&hit.normal);

            // ���˃x�N�g�����t�x�N�g���ɕϊ�
            vec = DirectX::XMVectorNegate(vec);

            // ���˃x�N�g����@���Ŏˉe�i�ړ���̈ʒu����ǂ܂ł̋����j
            DirectX::XMVECTOR lengthVec = DirectX::XMVector3Dot(vec, normalVec);
            float length;
            DirectX::XMStoreFloat(&length, lengthVec);

            // �␳�ʒu�ւ̃x�N�g�����v�Z
            DirectX::XMVECTOR correctVec = DirectX::XMVectorScale(normalVec, length * 1.1f);

            // �ŏI�I�ȕ␳�ʒu�x�N�g�����v�Z
            DirectX::XMVECTOR correctPosVec = DirectX::XMVectorAdd(endVec, correctVec);
            DirectX::XMFLOAT3 correctPos;
            DirectX::XMStoreFloat3(&correctPos, correctPosVec);

            DirectX::XMVECTOR correctionVec = DirectX::XMVectorSubtract(correctPosVec, startVec);
            HitResult hit2;
            if (StageManager::instance()->getStage(0)->raycast(hit.position, correctPos, hit2))
            {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }
            else
            {
                position.x = correctPos.x;
                position.z = correctPos.z;
            }
        }
        else
        {
            // �ǂɂ������Ă��Ȃ��ꍇ�͒ʏ�̈ړ�
            position.x += moveX;
            position.z += moveZ;
        }
    }
}