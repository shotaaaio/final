#include"Stage.h"
#include"DeviceManager.h"

Stage::Stage()
{
	model = std::make_unique<SkinnedMesh>(DeviceManager::instance()->getDevice(), "Resources\\Model\\ExampleStage\\ExampleStage.fbx");
}

Stage::~Stage()
{
}

void Stage::update(float elapsedTime)
{

}

void Stage::render(ID3D11DeviceContext* dc)
{
	model->render(dc, { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 }, { 0.8f,0.8f,0.8f,1.0f }, nullptr);
}

//レイキャスト
bool Stage::raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::intersectRayAndModel(start, end, model.get(), hit);
}

bool Stage::SphereCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, float radius, HitResult& hitResult)
{
	return Collision::intersectSphereCastAndModel(start,end,radius,model.get(),hitResult);
}
