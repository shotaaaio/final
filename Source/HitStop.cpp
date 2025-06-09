#include "HitStop.h"

HitStop::HitStop()
{
	hitType = HitType::None;
	isSlowMotion = false;
	motionSpeed = 1.0f;
}

void HitStop::Update(float elapsedTime)
{
	//スローモーション終了処理
	ResetSlowMotion(elapsedTime);
}

void HitStop::StartHitStop(HitType type)
{
	hitType = type;

	switch (type)
	{
	case HitStop::HitType::Weak:
		motionSpeed = 0.8f;
		motionTime = 0.15f;
		break;
	case HitStop::HitType::Strong:
		motionSpeed = 0.3f;
		motionTime = 0.35f;
		break;
	case HitStop::HitType::Combo:
		motionSpeed = 0.9f;
		motionTime = 0.2f;
		break;
	case HitStop::HitType::Finish:
		motionSpeed = 0.2f;
		motionTime = 0.8f;
		break;
	case HitStop::HitType::None:
		motionSpeed = 1.0f;
		motionTime = 0.0f;
		break;
	}
	StartSlowMotion(motionSpeed, motionTime);
}

void HitStop::StartSlowMotion(float slowSpeed, float time)
{
	isSlowMotion = true;
	motionTime = time;
	motionSpeed = slowSpeed;
}

void HitStop::ResetSlowMotion(float elapsedTime)
{
	if (!isSlowMotion)return;

	motionTime -= elapsedTime;
	if (motionTime <= 0.0f)
	{
		isSlowMotion = false;
		motionTime = 0.0f;
		motionSpeed = 1.0f;
	}
}
