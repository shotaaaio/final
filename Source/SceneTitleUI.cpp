#include "SceneTitleUI.h"

void TitleLogo::Initialize()
{
	m_TitleLogo = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Image\\TitleLogo.png");
}

void TitleLogo::Update(float elapsedTime)
{
	//TitleLogoの処理
}

void TitleLogo::Render(ID3D11DeviceContext* dc)
{
	//画面サイズ
	float scrW = static_cast<float>(m_TitleLogo->getTextureWidth());
	float scrH = static_cast<float>(m_TitleLogo->getTextureHeight());

	//スプライトサイズ
	float sprW = static_cast<float>(m_TitleLogo->getTextureWidth());
	float sprH = static_cast<float>(m_TitleLogo->getTextureHeight());

	m_TitleLogo->render(dc, scrW, scrH, sprW, sprH);
}
