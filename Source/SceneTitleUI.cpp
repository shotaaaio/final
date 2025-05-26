#include "SceneTitleUI.h"

#include "Input/InputManager.h"
#include "Input/GamePad.h"
#include "SceneManager.h"
#include "LoadingScene.h"
#include "GameScene.h"

void TitleUI::Initialize()
{
	//�f�o�C�X�擾
	auto device = DeviceManager::instance()->getDevice();
	//�^�C�g�����S�̓ǂݍ���
	m_TitleLogo = std::make_unique<Sprite>(device, L"Resources\\Image\\TItle_Images\\Title.png");
	//���j���[������
	SetupMenus();
	
	m_CurrentMenu = m_MainMenu; //�������j���[�̓��C�����j���[
	m_selectIndex = 0;          //�����I���C���f�b�N�X��0�ɐݒ�

	//���C�����j���[��������X���C�h�C�������鏀��
	for (auto& Menu : m_CurrentMenu)
	{
		Menu.position.x= -500.0f; //�����ʒu����ʊO���ɐݒ�
	}
	StartMenuSlideIn(m_CurrentMenu,-500.0f,100.0f);//��ʊO�������ʓ����ɃX���C�h�C������悤�ɐݒ�
}

void TitleUI::Update(float elapsedTime)
{
	GamePad* gamepad = InputManager::instance()->getGamePad();
		
	 // �A�j���[�V��������
    for (auto& menu : m_CurrentMenu)
    {
        if (menu.is_Animating)
        {
            float dx = menu.targetPosition.x - menu.position.x; // �ڕW�ʒu�ƌ��݈ʒux�̍���
            float dy = menu.targetPosition.y - menu.position.y; // �ڕW�ʒu�ƌ��݈ʒuy�̍���
            float dist = std::sqrtf(dx * dx + dy * dy);         // �ڕW�ʒu�܂ł̋���
            float step = menu.animationSpeed * elapsedTime;

            if (dist <= step)
            {
                menu.position.x = menu.targetPosition.x; // �ڕW�ʒux�ɓ��B
                menu.position.y = menu.targetPosition.y; // �ڕW�ʒuy�ɓ��B
                menu.is_Animating = false;                // �A�j���[�V�����I���t���O���I�t
            }
            else
            {
                // �ڕW�ʒu�Ɍ������Ĉړ�
                menu.position.x += (dx / dist) * step;
                menu.position.y += (dy / dist) * step;
            }
        }
    }

	//���{�^���Ŏ��̃��j���[�ցA��{�^���őO�̃��j���[�ֈړ�
	if (gamepad->getButtonDown() & GamePad::BTN_DOWN)
	{
		m_selectIndex = (m_selectIndex + 1) % m_CurrentMenu.size(); //���L�[�Ŏ��̃��j���[��
	}
	if (gamepad->getButtonDown() & GamePad::BTN_UP)
	{
		m_selectIndex = (m_selectIndex - 1 + m_CurrentMenu.size()) % m_CurrentMenu.size(); //��L�[�őO�̃��j���[��
	}

	//����{�^��(A)�őI�����ꂽ���j���[�����s
	if (gamepad->getButtonDown() & GamePad::BTN_A)
	{
		if (m_selectIndex >= 0 && m_selectIndex < m_CurrentMenu.size())
		{
			m_CurrentMenu[m_selectIndex].SelectCallback();
		}
	}

	//�L�����Z���{�^��(B)�Ń��C�����j���[�ɖ߂�
	if (gamepad->getButtonDown() & GamePad::BTN_B)
	{
		if (m_State != MenuType::MAIN_MENU) //���C�����j���[�ȊO�̏�ԂȂ�
		{
			m_State = MenuType::MAIN_MENU; //���C�����j���[��ԂɑJ��
			m_CurrentMenu = m_MainMenu;      //���C�����j���[�ɐ؂�ւ�
			m_selectIndex = 0;               //�I���C���f�b�N�X��0�Ƀ��Z�b�g
			// m_CurrentMenu �ɑ΂��ČĂ�
			for (auto& menu : m_CurrentMenu)
			{
				menu.position.x = -500.0f; // �X���C�h�J�n�ʒu�Ƀ��Z�b�g
			}
			StartMenuSlideIn(m_CurrentMenu, -500.0f, 100.0f);
		}
	}
}

void TitleUI::Render(ID3D11DeviceContext* dc)
{
	RenderTitleLogo(dc);          //�^�C�g�����S�`��
	
	switch (m_State)
	{
	case MenuType::MAIN_MENU:
		RenderMainMenu(dc);      //���C�����j���[�`��
		break;
	case MenuType::SELECT_DIFFICULTY:
		RenderDifficultyMenu(dc);//��Փx�I�����j���[�`��
		break;
	case MenuType::SETTINGS:
		RenderSettingsMenu(dc); //�ݒ胁�j���[�`��
		break;
	case MenuType::EXIT:
		break;
	}
}

//���C�����j���[�̍쐬(Start,settings,Exit)
void TitleUI::SetupMainMenuCallbacks()
{
	m_MainMenu.clear();
	m_MainMenuSprites.clear();
	auto device = DeviceManager::instance()->getDevice();

	// Start ���j���[
	{
		Menu startMenu;
		startMenu.SelectCallback = [this]()
			{
				m_State = MenuType::SELECT_DIFFICULTY;
				m_selectIndex = 0;
				StartMenuSlideIn(m_DifficultyMenu, -500.0f, 100.0f);
			};
		//�ʒu���̐ݒ�
		startMenu.position.x = 100.0f;
		startMenu.position.y = 200.0f;
		startMenu.targetPosition.x = 100.0f;
		startMenu.targetPosition.y = 200.0f;

		m_MainMenu.push_back(startMenu);
		m_MainMenuSprites.push_back(std::make_unique<Sprite>(device, L"Resources\\Image\\TItle_Images\\MainMenu_Images\\Start.png"));
	}

	// Settings ���j���[
	{

		Menu settingsMenu;
		settingsMenu.SelectCallback = [this]()
			{
				m_State = MenuType::SETTINGS;
				m_selectIndex = 0;
				StartMenuSlideIn(m_SettingsMenu, 800.0f, 100.0f);
			};
		settingsMenu.position.x = 100.0f;
		settingsMenu.position.y = 300.0f;
		settingsMenu.targetPosition.x = 100.0f;
		settingsMenu.targetPosition.y = 300.0f;

		m_MainMenu.push_back(settingsMenu);
		m_MainMenuSprites.push_back(std::make_unique<Sprite>(device, L"Resources\\Image\\TItle_Images\\MainMenu_Images\\Settings.png"));
	}

	// Exit ���j���[
	{
		Menu exitMenu;
		exitMenu.SelectCallback = []()
			{
				// �I������
				PostQuitMessage(0);
			};
		exitMenu.position.x = 100.0f;
		exitMenu.position.y = 400.0f;
		exitMenu.targetPosition.x = 100.0f;
		exitMenu.targetPosition.y = 400.0f;

		m_MainMenu.push_back(exitMenu);
		m_MainMenuSprites.push_back(std::make_unique<Sprite>(device, L"Resources\\Image\\TItle_Images\\MainMenu_Images\\Exit.png"));
	}
}

//��Փx�I�����j���[�̍쐬(Easy�ANormal�AHard�A�߂�)
void TitleUI::BuildDifficultyMenu()
{
	m_DifficultyMenu.clear();
	m_DifficultyMenuSprites.clear();
	auto device = DeviceManager::instance()->getDevice();
	
	// Easy ���j���[
	{
		Menu easyMenu;
		easyMenu.SelectCallback = [this]()
			{
				SceneManager::instance()->changeScene(new LoadingScene(new GameScene));
				m_State = MenuType::MAIN_MENU;  // ���C�����j���[�ɖ߂���
			};
		easyMenu.position.x = 100.0f;
		easyMenu.position.y = 200.0f;
		easyMenu.targetPosition.x = 100.0f;
		easyMenu.targetPosition.y = 200.0f;

		m_DifficultyMenu.push_back(easyMenu);
		m_DifficultyMenuSprites.push_back(std::make_unique<Sprite>(device, L"Resources\\Image\\TItle_Images\\Difficult_Images\\Easy.png"));
	}

	// Normal ���j���[
	{
		Menu normalMenu;
		normalMenu.SelectCallback = [this]()
			{
				// ��ՓxNormal�I�����̏���
				m_State = MenuType::MAIN_MENU;
			};
		normalMenu.position.x = 100.0f;
		normalMenu.position.y = 300.0f;
		normalMenu.targetPosition.x = 100.0f;
		normalMenu.targetPosition.y = 300.0f;

		m_DifficultyMenu.push_back(normalMenu);
		m_DifficultyMenuSprites.push_back(std::make_unique<Sprite>(device, L"Resources\\Image\\TItle_Images\\Difficult_Images\\Normal.png"));
	}


	// Hard ���j���[
	{
		Menu hardMenu;
		hardMenu.SelectCallback = [this]()
			{
				// ��ՓxHard�I�����̏���
				m_State = MenuType::MAIN_MENU;
			};
		hardMenu.position.x= 100.0f;
		hardMenu.position.y = 400.0f;
		hardMenu.targetPosition.x = 100.0f;
		hardMenu.targetPosition.y = 400.0f;

		m_DifficultyMenu.push_back(hardMenu);
		m_DifficultyMenuSprites.push_back(std::make_unique<Sprite>(device, L"Resources\\Image\\TItle_Images\\Difficult_Images\\Hard.png"));
	}

	// Back ���j���[�i���C�����j���[�ɖ߂�j
	{
		Menu backMenu;
		backMenu.SelectCallback = [this]()
			{
				m_State = MenuType::MAIN_MENU;
				m_selectIndex = 0;
				StartMenuSlideIn(m_MainMenu, 800.0f, 100.0f);
			};
		backMenu.position.x = 100.0f;
		backMenu.position.y = 500.0f;
		backMenu.targetPosition.x = 100.0f;
		backMenu.targetPosition.y = 500.0f;

		m_DifficultyMenu.push_back(backMenu);
		m_DifficultyMenuSprites.push_back(std::make_unique<Sprite>(device, L"Resources\\Image\\TItle_Images\\Difficult_Images\\Back.png"));
	}
}

//�ݒ胁�j���[
void TitleUI::BuildSettingsMenu()
{
	auto device = DeviceManager::instance()->getDevice();

	// �����̐ݒ胁�j���[�ƃX�v���C�g���N���A
	m_SettingsMenu.clear();
	m_SettingsMenuSprites.clear();

	// ��: ���ʐݒ胁�j���[
	{
		Menu volumeMenu;
		volumeMenu.SelectCallback = []()
			{
				// ���ʐݒ��ʂ֑J�ڂ�ݒ菈���Ȃ�
			};
		volumeMenu.position.x = 100.0f;
		volumeMenu.position.y = 200.0f;
		volumeMenu.targetPosition.x = 100.0f;
		volumeMenu.targetPosition.y = 200.0f;

		m_SettingsMenu.push_back(volumeMenu);
		m_SettingsMenuSprites.push_back(std::make_unique<Sprite>(device, L"Resources\\Image\\Title_Images\\Settings_Images\\Volume.png"));
	}

	// Back ���j���[�i���C�����j���[�ɖ߂�j
	{
		Menu backMenu;
		backMenu.SelectCallback = [this]()
			{
				m_State = MenuType::MAIN_MENU;
				m_selectIndex = 0;
				StartMenuSlideIn(m_MainMenu, 800.0f, 100.0f);
			};
		backMenu.position.x = 100.0f;
		backMenu.position.y = 400.0f;
		backMenu.targetPosition.x = 100.0f;
		backMenu.targetPosition.y = 400.0f;

		m_SettingsMenu.push_back(backMenu);
		m_SettingsMenuSprites.push_back(std::make_unique<Sprite>(device, L"Resources\\Image\\Title_Images\\Settings_Images\\Back.png"));
	}
}
//�e���j���[�̏����ݒ菈��
void TitleUI::SetupMenus()
{
	SetupMainMenuCallbacks();   //���C�����j���[�̃R�[���o�b�N�o�^
	BuildDifficultyMenu();      //��Փx���j���[�̍쐬
	BuildSettingsMenu();        //�ݒ胁�j���[�̍쐬
}

//�^�C�g�����S�̕`�揈��
void TitleUI::RenderTitleLogo(ID3D11DeviceContext* dc)
{
	DeviceManager* deviceM = DeviceManager::instance();
	float scrW = deviceM->getScreenWidth();
	float scrH = deviceM->getScreenHeight();
	float sprW = m_TitleLogo->getTextureWidth();
	float sprH = m_TitleLogo->getTextureHeight();

	//�^�C�g�����S�̕`��ʒu����ʒ����ɐݒ�
	m_TitleLogo->render(
		dc,
		0, 0, scrW, scrH,
		1, 1, 1, 1,
		0,
		0, 0, sprW, sprH
	);
}

//���j���[�̕`�揈��
void TitleUI::RenderMainMenu(ID3D11DeviceContext* dc)
{
	float startY = 300.0f; //���j���[�̊J�nY���W
	float spacing = 120.0f;//���j���[�Ԃ̃X�y�[�X

	std::vector<float> posXList, posYList;	//���j���[��X���W��Y���W���i�[���郊�X�g
	for (size_t i = 0; i < m_MainMenu.size(); ++i)
	{
		float x = m_MainMenu[i].position.x;	//�A�j���[�V�����t����X���W
		float y = startY + i * spacing;	    //Y���W�͊J�n�ʒu����̃I�t�Z�b�g

		posXList.push_back(x);				//X���W�����X�g�ɒǉ�
		posYList.push_back(y);				//Y���W�����X�g�ɒǉ�
	}

	RenderMenu(dc, m_MainMenu, m_MainMenuSprites, posXList, posYList);//���j���[�̕`�揈�����Ăяo��
}

//��Փx�I�����j���[�̕`�揈��
void TitleUI::RenderDifficultyMenu(ID3D11DeviceContext* dc)
{
	float startY = 260.0f;  // ��Փx���j���[�̊J�nY���W
	float spacing = 100.0f; // ���j���[�Ԃ̃X�y�[�X

	DeviceManager* deviceM = DeviceManager::instance();
	float screenWidth = deviceM->getScreenWidth();

	std::vector<float> posXList, posYList; // �`����W���X�g
	for (size_t i = 0; i < m_DifficultyMenu.size(); ++i)
	{
		// ��Փx���j���[�͒���������X���W���v�Z
		float spriteWidth = m_DifficultyMenuSprites[i]->getTextureWidth();
		float x = (screenWidth - spriteWidth) * 0.5f;

		float y = startY + i * spacing;

		posXList.push_back(x);
		posYList.push_back(y);
	}

	RenderMenu(dc, m_DifficultyMenu, m_DifficultyMenuSprites, posXList, posYList);
}

//�ݒ胁�j���[�̕`�揈��
void TitleUI::RenderSettingsMenu(ID3D11DeviceContext* dc)
{
	float startY = 260.0f;  //�ݒ胁�j���[�̊J�nY���W
	float spacing = 100.0f; //���j���[�Ԃ̃X�y�[�X

	DeviceManager* deviceM = DeviceManager::instance();
	float screenWidth = deviceM->getScreenWidth();

	std::vector<float> posXList, posYList; //�`����W���X�g
	for (size_t i = 0; i < m_SettingsMenu.size(); ++i)
	{
		// �ݒ胁�j���[�͒�������
		float spriteWidth = m_SettingsMenuSprites[i]->getTextureWidth();
		float x = (screenWidth - spriteWidth) * 0.5f;

		float y = startY + i * spacing;

		posXList.push_back(x);
		posYList.push_back(y);
	}

	RenderMenu(dc, m_SettingsMenu, m_SettingsMenuSprites, posXList, posYList);
}

//�e���j���[�̕`�揈��
void TitleUI::RenderMenu(ID3D11DeviceContext* dc,
	const std::vector<Menu>& menu,
	const std::vector<std::unique_ptr<Sprite>>& sprites,
	const std::vector<float>& posXList,
	const std::vector<float>& posYList)
{
	for (size_t i = 0; i < menu.size(); ++i)
	{
		float r = 1.0f, g = 1.0f, b = 1.0f;
		if (static_cast<int>(i) == m_selectIndex)
		{
			r = 1.0f; g = 1.0f; b = 0.0f;  // �I�𒆂͉��F�ɂ����
		}

		// �{���̃e�N�X�`���T�C�Y���擾
		float texW = sprites[i]->getTextureWidth();
		float texH = sprites[i]->getTextureHeight();

		// �{���̃T�C�Y�ŕ`��
		sprites[i]->render(
			dc,
			posXList[i], posYList[i],    // �`��ʒu�i����j
			texW, texH,                  // �`��T�C�Y�i�摜�̌��̃T�C�Y�j
			r, g, b, 1.0f,               // �F�i��=1�j
			0.0f,                        // ��]�i�Ȃ��j
			0.0f, 0.0f,                  // �e�N�X�`���̍���iUV�j
			texW, texH                   // UV�T�C�Y�i�S�́j
		);
	}
}
//���j���[���X���C�h�C��������֐�
void TitleUI::StartMenuSlideIn(std::vector<Menu>& menuList,float startX, float targetX)
{
	for (auto& menu : menuList)
	{
		menu.position = DirectX::XMFLOAT2(startX, menu.position.y);
		menu.targetPosition = DirectX::XMFLOAT2(targetX, menu.position.y);
		menu.is_Animating = true; // �A�j���[�V�����J�n�t���O��ݒ�
	}
}

