#include"Collision.h"
#include"Graphics/SkinnedMesh.h"

	/// <summary>
	/// ���Ƌ��̌�������
	/// </summary>
	/// <param name="positionA">��A</param>
	/// <param name="radiusA">���aA</param>
	/// <param name="positionB">��B</param>
	/// <param name="radiusB">���aB</param>
	/// <param name="outVec">�Փ˕����̃x�N�g���i�o�͗p�j</param>
	/// <returns>true...�Փ˂���</returns>
bool Collision::intersectSphereAndSphere(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	DirectX::XMFLOAT3& outVec
)
{
	DirectX::XMVECTOR posA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR posB = DirectX::XMLoadFloat3(&positionB);
	//��_�Ԃ̈ʒu�x�N�g��������
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(posB, posA);

	//�P��̒������v�Z
	DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);

	//float�^�ɕϊ�
	float len;
	DirectX::XMStoreFloat(&len, length);

	//��������
	float range = radiusA + radiusB;
	if (len > range)return false;

	//�Փ˂��Ă�����Փ˕����̃x�N�g����ݒ�
	vec = DirectX::XMVector3Normalize(vec);
	DirectX::XMStoreFloat3(&outVec, vec);

	return true;
}

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

bool Collision::intersectCylinderAndCylinder(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	float heightA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	float heightB,
	DirectX::XMFLOAT3& outVec)
{
	//A�̑�����B�̓�����Ȃ�Փ˂��Ă��Ȃ�
	if (positionA.y > positionB.y + heightB) return false;

	//A�̓���B�̑�����艺�Ȃ�Փ˂��Ă��Ȃ�
	if (positionA.y + heightA < positionB.y) return false;

	//XZ���ʂł͈̔̓`�F�b�N
	float vx = positionB.x - positionA.x;
	float vz = positionB.z - positionA.z;
	float range = radiusA + radiusB;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range) return false;

	//�Փ˕����̃x�N�g��
	outVec.x = vx / distXZ;
	outVec.y = 0.0f;
	outVec.z = vz / distXZ;
	return true;
}

bool Collision::intersectSphereAndCylinder(
	const DirectX::XMFLOAT3& positionA,
	float radiusA,
	const DirectX::XMFLOAT3& positionB,
	float radiusB,
	float heightB,
	DirectX::XMFLOAT3& outVec)
{
	//A(��)�̑�����B��(�~��)�̓�����Ȃ�Փ˂��ĂȂ�
	if (positionA.y - radiusA > positionB.y + heightB)return false;

	//A�i���j�̓��� B�i�~���j�̑����Ƃ�艺�Ȃ�Փ˂��Ă��Ȃ�
	if (positionA.y + radiusA < positionB.y)return false;

	//XZ���ʂł͈̔̓`�F�b�N
	float vx = positionB.x - positionA.x;
	float vz = positionB.z - positionA.z;
	float range = radiusA + radiusB;
	float distXZ = sqrtf(vx * vx + vz * vz);
	if (distXZ > range)return false;

	//�Փ˕����̃x�N�g��
	outVec.x = vx / distXZ;
	outVec.y = 0.0f;
	outVec.z = vz / distXZ;

	return true;
}

	/// <summary>
	/// ���C�ƃ��f���̌�������
	/// </summary>
	/// <param name="start">���C�̎n�_</param>
	/// <param name="end">���C�̏I�_</param>
	/// <param name="model">���f��</param>
	/// <param name="result">�q�b�g����</param>
	/// <returns>true...�Փ˂���</returns>
bool Collision::intersectRayAndModel(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const SkinnedMesh* model,
	HitResult& result)
{
	//���[���h��ԏ�ł̃��C�̎��_
	DirectX::XMVECTOR worldRayStartVec = DirectX::XMLoadFloat3(&start);
	//���[���h��ԏ�ł̃��C�̏I�_
	DirectX::XMVECTOR worldRayEndVec = DirectX::XMLoadFloat3(&end);
	//���[���h��ԏ�ł̃��C�̎n�_����I�_�܂ł̃x�N�g��
	DirectX::XMVECTOR worldRayVec = DirectX::XMVectorSubtract(worldRayEndVec, worldRayStartVec);
	//���[���h��ԏ�ł̃��C�̒���
	DirectX::XMVECTOR worldRayLength = DirectX::XMVector3Length(worldRayVec);
	DirectX::XMStoreFloat(&result.distance, worldRayLength);

	
	//true...�Փ˂���
	bool hit = false;

	for (const SkinnedMesh::mesh&mesh : model->meshes)
	{
		//���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ�
		DirectX::XMMATRIX worldTransformMat = DirectX::XMLoadFloat4x4(&mesh.default_global_transform);
		DirectX::XMMATRIX inverseWorldTransformMat = DirectX::XMMatrixInverse(nullptr, worldTransformMat);

		//���[�J����ԏ�ł̃��C�̎n�_
		DirectX::XMVECTOR localRayStartVec = DirectX::XMVector3TransformCoord(worldRayStartVec, inverseWorldTransformMat);
		//���[�J����ԏ�ł̃��C�̏I�_
		DirectX::XMVECTOR localRayEndVec = DirectX::XMVector3TransformCoord(worldRayEndVec, inverseWorldTransformMat);
		//���[�J����ԏ�ł̃��C�̎n�_����I�_�܂ł̃x�N�g��
		DirectX::XMVECTOR localRayVec = DirectX::XMVectorSubtract(localRayEndVec, localRayStartVec);
		//���[�J����ԏ�ł̃��C�̕����x�N�g��
		DirectX::XMVECTOR localRayDirectVec = DirectX::XMVector3Normalize(localRayVec);
		//���[�J����ԏ�ł̒���
		DirectX::XMVECTOR localRayLengthVec = DirectX::XMVector3Length(localRayVec);
		float localRayLength;
		DirectX::XMStoreFloat(&localRayLength, localRayLengthVec);


		//���_�f�[�^���擾
		const std::vector<SkinnedMesh::vertex>& vertices = mesh.vertices;
		const std::vector<UINT>& indices = mesh.indices;

		//���ƂȂ���
		int materialIndex = -1;
		DirectX::XMVECTOR hitPosition;
		DirectX::XMVECTOR hitNormal;

		for (const SkinnedMesh::mesh::subset& subset : mesh.subsets)
		{
			for (UINT i = 0;i < subset.index_count; i+=3)
			{
				UINT index = subset.start_index_location + i;

				//�O�p�`�̒��_�̒��o
				const SkinnedMesh::vertex& a = vertices.at(indices.at(index));
				const SkinnedMesh::vertex& b = vertices.at(indices.at(index + 1));
				const SkinnedMesh::vertex& c = vertices.at(indices.at(index + 2));

				DirectX::XMVECTOR aVec = DirectX::XMLoadFloat3(&a.position);
				DirectX::XMVECTOR bVec = DirectX::XMLoadFloat3(&b.position);
				DirectX::XMVECTOR cVec = DirectX::XMLoadFloat3(&c.position);

				DirectX::XMVECTOR abVec = DirectX::XMVectorSubtract(bVec, aVec);
				DirectX::XMVECTOR bcVec = DirectX::XMVectorSubtract(cVec, bVec);
				DirectX::XMVECTOR caVec = DirectX::XMVectorSubtract(aVec, cVec);

				//�O�p�`�̖@�����v�Z
				DirectX::XMVECTOR normalVec = DirectX::XMVector3Cross(abVec, bcVec);

				//���C�̕����Ɩʂ̕������`�F�b�N�i���ʂ̕\������j
				DirectX::XMVECTOR dotVec = DirectX::XMVector3Dot(localRayDirectVec, normalVec);
				float dot;
				DirectX::XMStoreFloat(&dot, dotVec);
				if (dot >= 0)continue;

				//���C�̎n�_�����_�܂ł̒������v�Z
				DirectX::XMVECTOR tmp = DirectX::XMVectorSubtract(aVec, localRayStartVec);
				float length = DirectX::XMVectorGetX(DirectX::XMVector3Dot(tmp, normalVec)) / dot;

				//�������O�ȉ��Ȃ�ȍ~�̏����͔�΂�
				if (length < 0.0f) continue;

				//���C�ƕ��ʂ̌�_���v�Z
				DirectX::XMVECTOR hitPosVec = DirectX::XMVectorAdd(localRayStartVec, DirectX::XMVectorScale(localRayDirectVec, length));

				//��_���O�p�`�̓����ɂ��邩�ǂ�������

				//��_����_A�܂ł̃x�N�g��
				DirectX::XMVECTOR paVac = DirectX::XMVectorSubtract(aVec, hitPosVec);
				//��_����_A�܂ł̃x�N�g���Ɠ_A�_B�̃x�N�g���̊O��
				DirectX::XMVECTOR crossVec1 = DirectX::XMVector3Cross(paVac, abVec);
				//�O�ςƖ@���̌������`�F�b�N
				DirectX::XMVECTOR dotVec1 = DirectX::XMVector3Dot(crossVec1, normalVec);
				float dot1;
				DirectX::XMStoreFloat(&dot1, dotVec1);
				if (dot1 < 0.0f)continue;

				
				//��_����_B�܂ł̃x�N�g��
				DirectX::XMVECTOR pbVec = DirectX::XMVectorSubtract(bVec, hitPosVec);
				//��_����_B�܂ł̃x�N�g���Ɠ_B�_C�̃x�N�g���̊O��
				DirectX::XMVECTOR crossVec2 = DirectX::XMVector3Cross(pbVec, bcVec);
				//�O�ςƖ@���̌������`�F�b�N
				DirectX::XMVECTOR dotVec2 = DirectX::XMVector3Dot(crossVec2, normalVec);
				float dot2;
				DirectX::XMStoreFloat(&dot2, dotVec2);
				if (dot2 < 0.0f)continue;

				
				//��_����_C�܂ł̃x�N�g��
				DirectX::XMVECTOR pcVec = DirectX::XMVectorSubtract(cVec, hitPosVec);
				//��_����_C�܂ł̃x�N�g���Ɠ_C�_A�̃x�N�g���̊O��
				DirectX::XMVECTOR crossVec3 = DirectX::XMVector3Cross(pcVec, caVec);
				//�O�ςƖ@���̌������`�F�b�N
				DirectX::XMVECTOR dotVec3 = DirectX::XMVector3Dot(crossVec3, normalVec);
				float dot3;
				DirectX::XMStoreFloat(&dot3, dotVec3);
				if (dot3 < 0.0f)continue;


				//�������O�ȉ��Ȃ�ȍ~�̏����͔�΂�
				if (length < 0.0f)continue;

				//��_�܂ł̋��������܂łɌv�Z�����ŋߋ������傫�����̓X�L�b�v
				if (length > localRayLength) continue;


				//�ŋߋ������X�V
				localRayLength = length;

				//�}�e���A���ԍ����X�V
				materialIndex = static_cast<int>(subset.material_unique_id);

				//��_�Ɩ@�����X�V
				hitPosition = hitPosVec;
				hitNormal = normalVec;
			}
		}

		if (materialIndex >= 0)
		{
			//��_���W�����[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
			DirectX::XMVECTOR worldPositionVec = DirectX::XMVector3TransformCoord(hitPosition, worldTransformMat);
			//���[���h��ԏ�ł̃��C�̎n�_�����_�܂ł̃x�N�g��
			DirectX::XMVECTOR worldVec = DirectX::XMVectorSubtract(worldPositionVec, worldRayStartVec);
			//���[���h��ԏ�ł̃��C�̎n�_�����_�܂ł̒���
			DirectX::XMVECTOR worldLengthVec = DirectX::XMVector3Length(worldVec);
			float distance;
			DirectX::XMStoreFloat(&distance, worldLengthVec);

			//�q�b�g���ʏ��ۑ�
			if (result.distance > distance)
			{
				//�q�b�g���̖ʂ�@�������[�J����Ԃ��烏�[���h��Ԃ֕ϊ�
				DirectX::XMVECTOR worldNormal = DirectX::XMVector3Transform(hitNormal, worldTransformMat);

				result.distance = distance;
				result.materialIndex = materialIndex;
				DirectX::XMStoreFloat3(&result.position, worldPositionVec);
				DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(worldNormal));
				hit = true;
			}
		}
	}

	return hit;
}

bool Collision::squareVsSquare(
	DirectX::XMFLOAT2 posA,
	DirectX::XMFLOAT2 sizeA,
	DirectX::XMFLOAT2 posB,
	DirectX::XMFLOAT2 sizeB
)
{

	return true;
}