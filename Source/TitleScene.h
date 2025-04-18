#pragma once
#include"Graphics/Sprite.h"
#include"Scene.h"
#include<memory>

//タイトルシーン
class TitleScene :public Scene
{
public:
	TitleScene(){}
	~TitleScene()override{}

	//初期化
	void initialize() override;

	//終了化
	void finalize()override;

	//更新処理
	void update(float elapsedTime)override;

	//描画処理
	void render()override;
private:
	//タイトル画像
	std::unique_ptr<Sprite>titleImage;
};