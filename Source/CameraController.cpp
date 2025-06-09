#include"CameraController.h"
#include"Camera.h"
#include"Input/InputManager.h"
#include"Mathf.h"
#include<algorithm>
#include"Player.h"
#include"Mathf.h"

//�X�V����
void CameraController::update(float elapsedTime,float rot)
{
	//�J�����̉�]
	rotateCameraAngle(elapsedTime);

	//�т��т��̃J�����ʒu
	DirectX::XMFLOAT3 eye = computeEye(rot);

	//���[�v�Ŋ��炩�ɂ���
	eye = Mathf::Leap(eye, target, elapsedTime);

	//�J�����̎��_�ƒ����_��ݒ�
	Camera::instance()->setLookAt(eye, target, { 0,1,0 });
}

void CameraController::rotateCameraAngle(float elapsedTime)
{
	GamePad* gamePad = InputManager::instance()->getGamePad();
	float ax = gamePad->getAxisRX(); //J�L�[��L�L�[
	float ay = gamePad->getAxisRY(); //I�L�[��K�L�[

	//�J�����̉�]���x
	constexpr float rollSpeed = DirectX::XMConvertToRadians(90);
	float speed = rollSpeed * elapsedTime;

	//�X�e�B�b�N�ɍ��킹�ĉ�]
	angle.x += ay * speed; //������]
	angle.y += ax * speed; //Y����]

	constexpr float maxAngle = DirectX::XMConvertToRadians(45);
	constexpr float minAngle = DirectX::XMConvertToRadians(-45);

	//x���̃J������]����
	if (angle.x < minAngle) angle.x = minAngle;
	if (angle.x > maxAngle) angle.x = maxAngle;

	//Y ���̉�]�l�� -3.14 �` 3.14 �Ɏ��܂�悤����
	if (angle.y > DirectX::XM_PI) angle.y -= DirectX::XM_2PI;
	if (angle.y < DirectX::XM_PI) angle.y += DirectX::XM_2PI;
}


DirectX::XMFLOAT3 CameraController::computeEye(float rot)
{
	//�J������]�l����]�s��ɕϊ�
	DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	//�J�����̉�]�s�񂩂�O���@�x�N�g�������o��
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, transform.r[2]);

	//�����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�(eye = target - front * range)
	DirectX::XMFLOAT3 eye;
	eye.x = target.x - front.x * range;
	eye.y = target.y - front.y * range;
	eye.z = target.z - front.z * range;

	return eye;
}

DirectX::XMFLOAT3 CameraController::computeEyeTPS()
{
	// ��]�s�񂩂�O���x�N�g�����擾�iZ�����O���j
	DirectX::XMMATRIX transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, transform.r[2]);

	// Y�����̏㉺�Ԃ�𖳎����āAXZ���������g��
	DirectX::XMFLOAT3 flatFront = { front.x, 0.0f, front.z };

	// ���K�����Ă����i���S�j
	DirectX::XMVECTOR vec = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&flatFront));
	DirectX::XMStoreFloat3(&flatFront, vec);

	// �J���������E�����ݒ�
	float backDistance = 5.0f;    // �������
	float cameraHeight = 3.0f;    // �v���C���[��荂���ʒu

	// �����_��Y���W�i���ʒu�j�{����
	DirectX::XMFLOAT3 eye;
	eye.x = target.x - flatFront.x * backDistance;
	eye.z = target.z - flatFront.z * backDistance;
	eye.y = target.y + cameraHeight;

	return eye;
}