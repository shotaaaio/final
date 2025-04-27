#pragma once

#include"Character.h"
#include"BulletManager.h"
#include"Effect.h"

#include<functional>

//プレイヤー
class Player : public Character
{
public:
	Player();
	~Player()override;

	//更新処理
	void update(float elapsedTime);

	//描画処理
	void render(ID3D11DeviceContext* dc);

	//デバック用の描画
	void drawDebugGui();

	//デバックプリミティブの描画
	void drawDrawPrimitive();

	//プレイヤーと敵との衝突処理
	void collisionPlayerAndEnemies();

	//敵と弾の衝突判定
	void collisionBulletAndEnemies();

	//移動入力処理
	void InputMove(float elapsedTime);

	//アクション
	void JumpAction();
	void Attack();
	void FlyingStraightBullet();
	void FlyingHomingBullet();

	//入力受付
	void inputAction();
protected:
	//着地したとき呼び出される
	void onLanding()override;

private:
	//入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 getMoveVec()const;

	void Hashigo();

	void inGoal();
private:
	//移動スピード
	float moveSpeed = 5.0f;

	//旋回スピード
	float turnSpeed = DirectX::XMConvertToRadians(720);

	//ジャンプ力
	float jumpSpeed = 20.0f;

	// ジャンプ回数
	int jumpCount = 0;

	// ジャンプ制限（最大ジャンプ数、ひとまず２段ジャンプ可）
	int jumpLimit = 2;

	//弾管理
	BulletManager bulletMgr;

	std::unique_ptr<Effect> hitEffect;
	std::unique_ptr<Effect> onLandingEffect;

	int boneIndex;
	int meshIndex;
	DirectX::XMFLOAT3 bonePosition = { 0,0,0 };

	//プレイヤー行動用ポインター
	std::function<void()> Key_c;
	std::function<void()> Key_v;
	std::function<void()> Key_z;
	std::function<void()> Key_x;

	bool isOnLadder;

	int attack_count = 0;

	bool IsGoal = false;//true...ゴールした

	bool move_b = true;//true...プレイヤーが動ける

	float posY1 = 0.0f;//クリア演出用
	float posY2 = 720.0f;//クリア演出用

	float posYMax = 0.0f;//クリア演出用

	float white_a = 0.0f;//クリア演出用

	bool clear_b = false;//クリア演出用

	float clearTime = 0.0f;//クリア演出用

	std::unique_ptr<Sprite> shizukuImage;

	std::unique_ptr<Sprite> wakuImage;

	std::unique_ptr<Sprite> sakuImage;

	std::unique_ptr<Sprite> whiteImage;

	std::unique_ptr<Sprite> clearText;
};

enum class NodeState
{
	Idle = 0,
	FwdRun,
	FwdStart,
	FwdStop,
	FwdAir,
	JumpRecovery,
	JumpStart,
	Attack,
	Death,
};

class Node
{
public:
	virtual ~Node() = default;
	virtual NodeState Execute() = 0; // 各ノードが実装するメソッド
};

class PlayAnimationNode : public Node
{
public:
	PlayAnimationNode(NodeState node) { this->node = node; }
	NodeState Execute() override{}
private:
	NodeState node;
};

