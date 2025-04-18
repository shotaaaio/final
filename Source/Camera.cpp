#include"Camera.h"

//�J�����̈ʒu�Ǝp���̐ݒ�
void Camera::setLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& forcus, const DirectX::XMFLOAT3& up)
{
	//���_�A�����_�A���������r���[�s����쐬
	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&forcus);
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View);

	//�r���[�s����t�s��v�Z���A���[���h�s��ɖ߂�
	DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, World);

	//�J�����̉E���������o��
	this->right.x = world._11;
	this->right.y = world._12;
	this->right.z = world._13;

	//�J�����̏���������o��
	this->up.x = world._21;
	this->up.y = world._22;
	this->up.z = world._23;

	//�J�����̑O���������o��
	this->front.x = world._31;
	this->front.y = world._32;
	this->front.z = world._33;

	//���_�A�����_��ۑ�
	this->eye = eye;
	this->focus = forcus;
}

//�J�����̃����Y�̐ݒ�
void Camera::setPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	//����p�A�c���䗦�A�N���b�v��������v���W�F�N�V�����s����쐬
	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
	DirectX::XMStoreFloat4x4(&projection, Projection);
}