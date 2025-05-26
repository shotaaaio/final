#include "SceneGameUI.h"

void HPGaugeUI::Initialize()
{
	m_currentHp = player->getHealth();
	m_maxHp = player->getMaxHealth();

	m_HP_frameSprite = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Image\\HPFrame.png");
	m_HP_barSprite = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Image\\HPBar.png");
}

//HPGaugeUIの更新
void HPGaugeUI::Update(float elapsedTime)
{
	//HPの割合を計算
	float hpRatio = m_currentHp / m_maxHp;
	if (hpRatio < 0.0f)hpRatio = 0.0f;
	if (hpRatio > 1.0f)hpRatio = 1.0f;

	//HPバーSpriteの幅をHPの割合に応じて横方向を調整


}

void HPGaugeUI::Render(ID3D11DeviceContext* dc)
{
	DeviceManager* deiceM = DeviceManager::instance();

	if (!m_HP_barSprite || !m_HP_frameSprite || !m_currentHp || m_maxHp <= 0.0f)
		return;

	float percent = m_currentHp / m_maxHp;
	float fullWidth = static_cast<float>(m_HP_barSprite->getTextureWidth());
	float height = static_cast<float>(m_HP_barSprite->getTextureHeight());

	float screenHeight = deiceM->getScreenWidth();
	float posX = 0.0f;
	float posY = screenHeight - height;

	// HPBar可変幅
	m_HP_barSprite->render(dc, posX, posY, fullWidth * percent, height);

	// HPFrame固定サイズ
	m_HP_frameSprite->render(dc, posX, posY, fullWidth, height);
}

//TimeDisplayUIの初期化
void TimeDisplayUI::Initialize()
{
	m_totalTime = 0.0f;
	m_Time_FrameSprite = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Textures\\TimeDisplay.png");
}

//TimeDisplayUI
void TimeDisplayUI::Update(float elapsedTime)
{
	m_totalTime += elapsedTime;
}

void TimeDisplayUI::Render(ID3D11DeviceContext* dc)
{
	DeviceManager* deiceM = DeviceManager::instance();
	//画面サイズ
	float screenWidth = deiceM->getScreenWidth();
	float screenHeight = deiceM->getScreenHeight();

	//スプライトの幅と高さを取得
	float spriteWidth = static_cast<float>(m_Time_FrameSprite->getTextureWidth());
	float spriteHeight = static_cast<float>(m_Time_FrameSprite->getTextureHeight());
	//描画位置　画面上部中央
	float posX = (screenWidth - spriteWidth) / 2.0f;
	float posY = 0.0f; //上端

	// スプライトを描画
	m_Time_FrameSprite->render(dc, posX, posY, spriteWidth, spriteHeight);
}


//現在は画面右下にひし形の各頂点にUIを配置する形にしようと思っているが、
//後に画面左したで、1枚絵として表示する可能性大
void ActionButtonsUI::Initialize()
{
	m_buttonSprites.clear();

	ID3D11Device* device = DeviceManager::instance()->getDevice();

	// テクスチャパスを関数内で固定
	std::vector<const wchar_t*> buttonTextures =
	{
		L"Resources\\Textures\\ButtonAttackIcon.png",
		L"Resources\\Textures\\ButtonJumpIcon.png",
		L"Resources\\Textures\\ButtonDodgeIcon.png",
		L"Resources\\Textures\\ButtonComboIcon.png"
	};

	for (const auto& tex : buttonTextures)
	{
		m_buttonSprites.emplace_back(std::make_unique<Sprite>(device, tex));
	}
}

void ActionButtonsUI::Update(float elapsedTime)
{
	//ボタンの点滅処理(円周上に)
}

void ActionButtonsUI::Render(ID3D11DeviceContext* dc)
{
	DeviceManager* deiceM = DeviceManager::instance();

	float screenWidth = deiceM->getScreenWidth();
	float screenHeight = deiceM->getScreenHeight();

	//画面右下の基準点 右端・下端から100px内側に設定
	float baseX = screenWidth - 100.0f;
	float baseY = screenHeight - 100.0f;

	float buttonWidth = static_cast<float>(m_buttonSprites[0]->getTextureWidth());
	float buttonHeight = static_cast<float>(m_buttonSprites[0]->getTextureHeight());

	//ひし形の頂点オフセット
	const float offsetX = buttonWidth;
	const float offsetY = buttonHeight;

	//ひし形4頂点の座標 上右下左
	struct Pos { float x, y; };
	Pos positions[4] = {
		{ baseX,               baseY - offsetY }, //上頂点
		{ baseX + offsetX,     baseY           }, //右頂点
		{ baseX,               baseY + offsetY }, //下頂点
		{ baseX - offsetX,     baseY           }  //左頂点
	};

	for (int i = 0; i < 4; ++i)
	{
		if (m_buttonSprites[i])
		{
			//renderは左上基準なので中心合わせのために調整
			m_buttonSprites[i]->render(dc, positions[i].x - buttonWidth / 2.0f, positions[i].y - buttonHeight / 2.0f, buttonWidth, buttonHeight);
		}
	}
}