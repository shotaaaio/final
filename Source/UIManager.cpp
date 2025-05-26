#include "UIManager.h"

//各シーンの初期化時のAddUI関数の後に記入
void UIManager::Initialize()
{
	for (auto& ui : m_uiElements)
	{
		ui->Initialize();
	}
}

//各シーンの更新時に記入
void UIManager::Update(float elapsedTime)
{
	for (auto& ui : m_uiElements)
	{
		ui->Update(elapsedTime);
	}
}

//各シーンの描画時に記入
void UIManager::Render(ID3D11DeviceContext* dc)
{
	for (auto& ui : m_uiElements)
	{
		ui->Render(dc);
	}
}

//Sceneの初期化でAddUI関数を呼ぶ
void UIManager::AddUI(std::shared_ptr<UIComponent> ui)
{
	//各子クラスをカウント
	m_uiElements.emplace_back(ui);
}
                         