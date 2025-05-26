#include "SceneGameUI.h"

void HPGaugeUI::Initialize()
{
	m_currentHp = player->getHealth();
	m_maxHp = player->getMaxHealth();

	m_HP_frameSprite = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Image\\HPFrame.png");
	m_HP_barSprite = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Image\\HPBar.png");
}

//HPGaugeUI�̍X�V
void HPGaugeUI::Update(float elapsedTime)
{
	//HP�̊������v�Z
	float hpRatio = m_currentHp / m_maxHp;
	if (hpRatio < 0.0f)hpRatio = 0.0f;
	if (hpRatio > 1.0f)hpRatio = 1.0f;

	//HP�o�[Sprite�̕���HP�̊����ɉ����ĉ������𒲐�


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

	// HPBar�ϕ�
	m_HP_barSprite->render(dc, posX, posY, fullWidth * percent, height);

	// HPFrame�Œ�T�C�Y
	m_HP_frameSprite->render(dc, posX, posY, fullWidth, height);
}

//TimeDisplayUI�̏�����
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
	//��ʃT�C�Y
	float screenWidth = deiceM->getScreenWidth();
	float screenHeight = deiceM->getScreenHeight();

	//�X�v���C�g�̕��ƍ������擾
	float spriteWidth = static_cast<float>(m_Time_FrameSprite->getTextureWidth());
	float spriteHeight = static_cast<float>(m_Time_FrameSprite->getTextureHeight());
	//�`��ʒu�@��ʏ㕔����
	float posX = (screenWidth - spriteWidth) / 2.0f;
	float posY = 0.0f; //��[

	// �X�v���C�g��`��
	m_Time_FrameSprite->render(dc, posX, posY, spriteWidth, spriteHeight);
}


//���݂͉�ʉE���ɂЂ��`�̊e���_��UI��z�u����`�ɂ��悤�Ǝv���Ă��邪�A
//��ɉ�ʍ������ŁA1���G�Ƃ��ĕ\������\����
void ActionButtonsUI::Initialize()
{
	m_buttonSprites.clear();

	ID3D11Device* device = DeviceManager::instance()->getDevice();

	// �e�N�X�`���p�X���֐����ŌŒ�
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
	//�{�^���̓_�ŏ���(�~�����)
}

void ActionButtonsUI::Render(ID3D11DeviceContext* dc)
{
	DeviceManager* deiceM = DeviceManager::instance();

	float screenWidth = deiceM->getScreenWidth();
	float screenHeight = deiceM->getScreenHeight();

	//��ʉE���̊�_ �E�[�E���[����100px�����ɐݒ�
	float baseX = screenWidth - 100.0f;
	float baseY = screenHeight - 100.0f;

	float buttonWidth = static_cast<float>(m_buttonSprites[0]->getTextureWidth());
	float buttonHeight = static_cast<float>(m_buttonSprites[0]->getTextureHeight());

	//�Ђ��`�̒��_�I�t�Z�b�g
	const float offsetX = buttonWidth;
	const float offsetY = buttonHeight;

	//�Ђ��`4���_�̍��W ��E����
	struct Pos { float x, y; };
	Pos positions[4] = {
		{ baseX,               baseY - offsetY }, //�㒸�_
		{ baseX + offsetX,     baseY           }, //�E���_
		{ baseX,               baseY + offsetY }, //�����_
		{ baseX - offsetX,     baseY           }  //�����_
	};

	for (int i = 0; i < 4; ++i)
	{
		if (m_buttonSprites[i])
		{
			//render�͍����Ȃ̂Œ��S���킹�̂��߂ɒ���
			m_buttonSprites[i]->render(dc, positions[i].x - buttonWidth / 2.0f, positions[i].y - buttonHeight / 2.0f, buttonWidth, buttonHeight);
		}
	}
}