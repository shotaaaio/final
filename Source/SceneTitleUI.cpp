#include "SceneTitleUI.h"

void TitleLogo::Initialize()
{
	m_TitleLogo = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Image\\TitleLogo.png");
}

void TitleLogo::Update(float elapsedTime)
{
	//TitleLogo�̏���
}

void TitleLogo::Render(ID3D11DeviceContext* dc)
{
	//��ʃT�C�Y
	float scrW = static_cast<float>(m_TitleLogo->getTextureWidth());
	float scrH = static_cast<float>(m_TitleLogo->getTextureHeight());

	//�X�v���C�g�T�C�Y
	float sprW = static_cast<float>(m_TitleLogo->getTextureWidth());
	float sprH = static_cast<float>(m_TitleLogo->getTextureHeight());

	m_TitleLogo->render(dc, scrW, scrH, sprW, sprH);
}
