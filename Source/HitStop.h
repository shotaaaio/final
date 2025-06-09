#pragma once
#include <DirectXMath.h>

// アニメーション用スローモーション処理
class HitStop {
public:
	enum class HitType {
		None,
		Weak,
		Strong,
		Combo,
		Finish,
	};

	HitStop();
	~HitStop() = default;

	// スローモーション更新
	void Update(float elapsedTime);

	// スローモーション開始（ヒットタイプ別に強度を設定）
	void StartHitStop(HitType hitType);

	float GetMotionTime()const { return motionTime; }

	// 現在のスローモーション倍率を取得（アニメーション用）
	float GetMotionSpeed() const { return motionSpeed; }

	// スローモーション中かどうか
	bool IsSlowMotion() const { return isSlowMotion; }

	// 現在のヒットタイプ
	HitType GetHitType() const { return hitType; }

private:
	void StartSlowMotion(float slowRate, float time);
	void ResetSlowMotion(float elapsedTime);

private:
	// ヒット種別
	HitType hitType = HitType::None;

	// スローモーション制御
	bool isSlowMotion = false;
	float motionTime = 0.0f;//ヒットストップ時の長さ
	float motionSpeed = 1.0f;//ヒットストップ時の速さ
};