#pragma once
#include"Enemy.h"
//“GŠÇ—
class EnemyManager
{
private:
	EnemyManager(){}
	~EnemyManager() {}
public:
	//ƒVƒ“ƒOƒ‹ƒgƒ“
	static EnemyManager* instance()
	{
		static EnemyManager inst;
		return &inst;
	}

	//XVˆ—
	void update(float elapsedTime);

	//•`‰æˆ—
	void render(ID3D11DeviceContext* dc);

	//“G“¯m‚ÌÕ“Ë”»’è
	void collisionEnemiesAndEnemies();

	//“G‚Ì“o˜^
	void regist(Enemy* enemy);

	//“G‚Ì”jŠü
	void clear();

	//ƒfƒoƒbƒNƒvƒŠƒ~ƒeƒBƒu•`‰æ
	void drawDebugPrimitive();

	//“G‚Ì‘”‚ğæ“¾
	int getEnemyCount()const { return static_cast<int>(enemyArray.size()); }

	//“G‚ğæ“¾
	Enemy* getEnemy(int index) { return enemyArray.at(index); }

	//ˆø”‚Å–á‚Á‚½ƒ|ƒWƒVƒ‡ƒ“‚©‚çˆê”Ô‹ß‚¢“G‚ğƒT[ƒ`
	Enemy* searchEnemy(DirectX::XMFLOAT3* position);

	//“Gíœ
	void remove(Enemy* enemy);

	Enemy* getEnemyByRay(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd);
private:
	std::vector<Enemy*>enemyArray;
	std::vector<Enemy*>removeArray;	
};