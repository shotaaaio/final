#pragma once
#include<DirectXMath.h>
#include "Graphics/SkinnedMesh.h"
//�q�b�g���ʁi���C�L���X�g��Ɏ擾���������j
struct HitResult
{
	DirectX::XMFLOAT3 position = { 0,0,0 };//���C�ƃ|���S���̌�_
	DirectX::XMFLOAT3 normal = { 0,0,0 }; //�Փ˂����|���S���̖@���x�N�g��
	float distance = 0.0f; //���C�̎��_�����_�܂ł̋���
	int materialIndex = -1; //�Փ˂����|���S���̃}�e���A���ԍ�
};

class Collision
{
public:
	/// <summary>
	/// ���Ƌ��̌�������
	/// </summary>
	/// <param name="positionA">��A</param>
	/// <param name="radiusA">���aA</param>
	/// <param name="positionB">��B</param>
	/// <param name="radiusB">���aB</param>
	/// <param name="outVec">�Փ˕����̃x�N�g���i�o�͗p�j</param>
	/// <returns>true...�Փ˂���</returns>
	static bool intersectSphereAndSphere(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		DirectX::XMFLOAT3& outVec);

	/// <summary>
	/// �~���Ɖ~���̌�������
	/// </summary>
	/// <param name="positionA">�ʒuA</param>
	/// <param name="radiusA">���aA</param>
	/// <param name="heightA">����A</param>
	/// <param name="positionB">�ʒuB</param>
	/// <param name="radiusB">���aB</param>
	/// <param name="heightB">����B</param>
	/// <param name="outVec">�Փ˕����̃x�N�g���i�o�͗p�j</param>
	/// <returns>true...�Փ˂���</returns>
	static bool intersectCylinderAndCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outVec);

	/// <summary>
	/// ���Ɖ~���̌������f
	/// </summary>
	/// <param name="positionA">���̈ʒu</param>
	/// <param name="radiusA">���̔��a</param>
	/// <param name="positionB">�~���̈ʒu</param>
	/// <param name="radiusB">�~���̔��a</param>
	/// <param name="heightB">�~���̍���</param>
	/// <param name="outVec">�Փ˕����̃x�N�g���i�o�͗p�j</param>
	/// <returns>true...�Փ˂���</returns>
	static bool intersectSphereAndCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outVec);

	/// <summary>
	/// ���C�ƃ��f���̌�������
	/// </summary>
	/// <param name="start">���C�̎n�_</param>
	/// <param name="end">���C�̏I�_</param>
	/// <param name="model">���f��</param>
	/// <param name="result">�q�b�g����</param>
	/// <returns>true...�Փ˂���</returns>
	static bool intersectRayAndModel(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const SkinnedMesh* model,
		HitResult& result);

	static bool squareVsSquare(
		DirectX::XMFLOAT2 posA,
		DirectX::XMFLOAT2 sizeA,
		DirectX::XMFLOAT2 posB,
		DirectX::XMFLOAT2 sizeB
	);
};