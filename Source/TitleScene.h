#pragma once
#include"Scene.h"
#include<memory>
#include "UIManager.h"

//タイトルシーン
class TitleScene :public Scene
{
public:
	TitleScene(){}
	~TitleScene()override{}

	//初期化
	void initialize() override;

	void finalize() override;

	//更新処理
	void update(float elapsedTime)override;

	//描画処理
	void render()override;
private:
	//タイトル画像
	std::unique_ptr<UIManager>m_UIManager;
};