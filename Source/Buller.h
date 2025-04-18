#pragma once
#include"Object.h"

//’eŠÇ—‚Ì‘O•ûéŒ¾
class BulletManager;

//’e
class Bullet : public Object
{
public:
	Bullet(BulletManager* manager);
	virtual ~Bullet(){}

	//XVˆ—
	virtual void update(float elapedTime) = 0;

	//•`‰æˆ—
	virtual void render(ID3D11DeviceContext* dc) = 0;

	//ƒfƒoƒbƒNƒvƒŠƒ~ƒeƒBƒu•`‰æ
	virtual void drawDebugPrimitive();

	void updateTransform() override;

	//•ûŒüæ“¾
	const DirectX::XMFLOAT3* getDirection() { return &direction; }

	//•ûŒü‚Ìİ’è
	void setDirection(const DirectX::XMFLOAT3& d) { this->direction = d; }

	//”¼Œaæ“¾
	float getRadius()const { return radius; }
	//”jŠü
	void destroy();
protected:
	DirectX::XMFLOAT3 direction{ 0,0,1 }; //•ûŒü

	//’eŠÇ—
	BulletManager* manager;

	//”¼Œa
	float radius = 0.5;
};