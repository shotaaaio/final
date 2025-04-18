#pragma once
#include<vector>
#include"Buller.h"

//’eŠÇ—
class BulletManager
{
public:
	BulletManager();
	~BulletManager();

	//XVˆ—
	void update(float elapsedTime);

	//•`‰æˆ—
	void render(ID3D11DeviceContext* dc);
	
	//ƒfƒoƒbƒNƒvƒŠƒ~ƒeƒBƒu
	void drawDebugPrimitive();

	//’e“o˜^
	void regist(Bullet* bullet);

	//’e‘S‰ğœ
	void clear();

	// ’e”æ“¾
	int getBulletCount() const { return static_cast<int>(bulletArray.size()); }

	// ’eæ“¾
	Bullet* getBullet(int index) { return bulletArray.at(index); }

	//’eíœ
	void remove(Bullet* bullet);
private:
	std::vector<Bullet*>bulletArray;
	std::vector<Bullet*>removes;
};