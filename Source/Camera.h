#pragma once
#include<DirectXMath.h>

//�J����
class Camera
{
private:
	Camera() {}
	~Camera(){}
public:
	static Camera* instance()
	{
		static Camera inst;
		return &inst;
	}

	//�J�����̈ʒu�Ǝp���̐ݒ�
	void setLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& forcus,const DirectX::XMFLOAT3& up);

	//�J�����̃����Y�̐ݒ�
	void setPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	//�r���[�s��擾
	const DirectX::XMFLOAT4X4* getView()const { return &view; }

	//�v���W�F�N�V�����s��
	const DirectX::XMFLOAT4X4* getProjection()const { return &projection; }

	//���_�擾
	const DirectX::XMFLOAT3* getEye()const { return &eye; }

	//�����_�擾
	const DirectX::XMFLOAT3* getFocus()const { return &focus; }

	//�E�����x�N�g���擾
	const DirectX::XMFLOAT3* getRight()const { return &right; }

	//������x�N�g���擾
	const DirectX::XMFLOAT3* getUp()const { return &up; }

	//�O�����x�N�g��
	const DirectX::XMFLOAT3* getFront()const { return &front; }

	void setEye(DirectX::XMFLOAT3* eye) { this->eye = *eye; }
private:
	DirectX::XMFLOAT4X4 view = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	DirectX::XMFLOAT4X4 projection = {
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	DirectX::XMFLOAT3 eye	= { 0,0,0 };	//�J�����̎��_�i�J�����ʒu�j
	DirectX::XMFLOAT3 focus = { 0,0,0 };	//����

	DirectX::XMFLOAT3 right = { 0,0,0 };	//�E�����x�N�g��
	DirectX::XMFLOAT3 up	= { 0,0,0 };	//������x�N�g��
	DirectX::XMFLOAT3 front = { 0,0,0 };	//�O�����x�N�g��
};