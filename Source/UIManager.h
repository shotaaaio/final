#pragma once
#include "UIComponent.h"

class UIManager
{
public:
	UIManager(){}
	~UIManager(){}

	void Initialize();                         //初期化
	void Update(float elapsedTime);            //更新
	void Render(ID3D11DeviceContext* dc);      //描画
	void AddUI(std::shared_ptr<UIComponent>ui);//各UIを外部から登録する関数

private:
	//各抽象クラスの数を取得する為のメンバ変数
	std::vector<std::shared_ptr<UIComponent>>m_uiElements;
};