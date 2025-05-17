#include "UIManager.h"

void UIManager::Initialize()
{
	for (auto& ui : m_uiElements)
	{
		ui->Initialize();
	}
}

void UIManager::Update(float elapsedTime)
{
	for (auto& ui : m_uiElements)
	{
		ui->Update(elapsedTime);
	}
}

void UIManager::Render(ID3D11DeviceContext* dc)
{
	for (auto& ui : m_uiElements)
	{
		ui->Render(dc);
	}
}

//動的追加用(シーンによって切り替え)
void UIManager::AddUI(std::unique_ptr<UI> ui)
{
	m_uiElements.emplace_back(std::move(ui));
}
