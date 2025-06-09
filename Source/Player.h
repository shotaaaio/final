#pragma once

#include"Character.h"
#include"Effect.h"
#include<functional>
#include "StateMachine.h"

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

	//移動入力処理
	void InputMove(float elapsedTime);

	//入力受付
	void inputAction();


protected:
	//着地したとき呼び出される
	void onLanding()override;

private:
	//入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 getMoveVec()const;

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

	//ダッシュスピード
	float dashSpeed = 10.0f;

	// 後方へのステップ速度
	float backStepSpeed = 20.0f;

	bool isSlowAnimation = true;
	float slowAnimationTimer = 0.0f;
	float slowAnimationSpeed = 0.2f;   //スロー時の速度
	float slowAnimationDuration = 0.5f;//スロー持続時間
	float animationSpeed = 1.0f;       //元の速度

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
	int attack_count = 2;

	//ステートマシンメンバ変数
	std::unique_ptr<StateMachine<Player>>stateMachine;

	//コンボステート遷移マップ
	std::unordered_map<std::string, StateID>comboTransitionMap;

	enum class ComboStep
	{
		None,
		Combo1,
		Combo2,
		Combo3,
		Total,
	};
	ComboStep comboStep = ComboStep::None;
	int nextStep = -1;
	bool queuedNextCombo = false;//次のコンボ入力がキューされたか
	float attackStartTime = 0.0f;      // 攻撃が始まった時間
	float currentTime = 0.0f;          // 現在の時間（ゲーム時間など）
	const float comboGracePeriod = 0.8f; // コンボ猶予時間（秒）
	bool isAttacking = false;
	bool canMove = true;//移動可能か

	//各ステートの設定処理
	void SettingStates();

	//移動処理
	void Jump();           //ジャンプ処理

	//後退処理
	void BackStep();

	//攻撃処理の宣言
	void WeakAttack(); //弱攻撃処理
	void WeakAttackCombo2();
	void WeakAttackCombo3();



	//状態チェック
	bool CanComboContinue() const;//コンボ継続可能か(猶予時間か)
	bool IsComboState() const;//コンボステートか
	bool IsCurrentAnimationFhinised();//指定したアニメーションまで終了しているか

	//コンボ更新処理
	void UpdateCombo(float elapsedTime);

};