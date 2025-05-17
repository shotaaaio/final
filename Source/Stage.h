#pragma once
#include"Graphics/SkinnedMesh.h"
#include"Collision.h"

//�X�e�[�W
class Stage
{
public:
	Stage();
	~Stage();

	//�X�V����
	void update(float elapsedTime);

	//�`�揈��
	void render(ID3D11DeviceContext* dc);

	//���C�L���X�g
	bool raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	//�X�t�B�A�L���X�g
	bool SphereCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, float radius, HitResult& hitResult);
private:
	std::unique_ptr<SkinnedMesh>model;
};