#include "UIManager.h"

//�e�V�[���̏���������AddUI�֐��̌�ɋL��
void UIManager::Initialize()
{
	for (auto& ui : m_uiElements)
	{
		ui->Initialize();
	}
}

//�e�V�[���̍X�V���ɋL��
void UIManager::Update(float elapsedTime)
{
	for (auto& ui : m_uiElements)
	{
		ui->Update(elapsedTime);
	}
}

//�e�V�[���̕`�掞�ɋL��
void UIManager::Render(ID3D11DeviceContext* dc)
{
	for (auto& ui : m_uiElements)
	{
		ui->Render(dc);
	}
}

//Scene�̏�������AddUI�֐����Ă�
void UIManager::AddUI(std::shared_ptr<UIComponent> ui)
{
	//�e�q�N���X���J�E���g
	m_uiElements.emplace_back(ui);
}
                         