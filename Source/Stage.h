#pragma once
#include"Graphics/SkinnedMesh.h"
#include"Collision.h"

//ステージ
class Stage
{
public:
	Stage();
	~Stage();

	//更新処理
	void update(float elapsedTime);

	//描画処理
	void render(ID3D11DeviceContext* dc);

	//レイキャスト
	bool raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
private:
	std::unique_ptr<SkinnedMesh>model;
};