#include "SceneTitleUI.h"

#include "Input/InputManager.h"
#include "Input/GamePad.h"
#include "SceneManager.h"
#include "LoadingScene.h"
#include "GameScene.h"

void TitleUI::Initialize()
{
	//デバイス取得
	auto device = DeviceManager::instance()->getDevice();
	//タイトルロゴの読み込み
	m_TitleLogo = std::make_unique<Sprite>(device, L"Resources\\Image\\TItle_Images\\Title.png");
	//メニュー初期化
	SetupMenus();
	
	m_CurrentMenu = m_MainMenu; //初期メニューはメインメニュー
	m_selectIndex = 0;          //初期選択インデックスを0に設定

	//メインメニューを左からスライドインさせる準備
	for (auto& Menu : m_CurrentMenu)
	{
		Menu.position.x= -500.0f; //初期位置を画面外左に設定
	}
	StartMenuSlideIn(m_CurrentMenu,-500.0f,100.0f);//画面外左から画面内左にスライドインするように設定
}

void TitleUI::Update(float elapsedTime)
{
	GamePad* gamepad = InputManager::instance()->getGamePad();
		
	 // アニメーション処理
    for (auto& menu : m_CurrentMenu)
    {
        if (menu.is_Animating)
        {
            float dx = menu.targetPosition.x - menu.position.x; // 目標位置と現在位置xの差分
            float dy = menu.targetPosition.y - menu.position.y; // 目標位置と現在位置yの差分
            float dist = std::sqrtf(dx * dx + dy * dy);         // 目標位置までの距離
            float step = menu.animationSpeed * elapsedTime;

            if (dist <= step)
            {
                menu.position.x = menu.targetPosition.x; // 目標位置xに到達
                menu.position.y = menu.targetPosition.y; // 目標位置yに到達
                menu.is_Animating = false;                // アニメーション終了フラグをオフ
            }
            else
            {
                // 目標位置に向かって移動
                menu.position.x += (dx / dist) * step;
                menu.position.y += (dy / dist) * step;
            }
        }
    }

	//下ボタンで次のメニューへ、上ボタンで前のメニューへ移動
	if (gamepad->getButtonDown() & GamePad::BTN_DOWN)
	{
		m_selectIndex = (m_selectIndex + 1) % m_CurrentMenu.size(); //下キーで次のメニューへ
	}
	if (gamepad->getButtonDown() & GamePad::BTN_UP)
	{
		m_selectIndex = (m_selectIndex - 1 + m_CurrentMenu.size()) % m_CurrentMenu.size(); //上キーで前のメニューへ
	}

	//決定ボタン(A)で選択されたメニューを実行
	if (gamepad->getButtonDown() & GamePad::BTN_A)
	{
		if (m_selectIndex >= 0 && m_selectIndex < m_CurrentMenu.size())
		{
			m_CurrentMenu[m_selectIndex].SelectCallback();
		}
	}

	//キャンセルボタン(B)でメインメニューに戻る
	if (gamepad->getButtonDown() & GamePad::BTN_B)
	{
		if (m_State != MenuType::MAIN_MENU) //メインメニュー以外の状態なら
		{
			m_State = MenuType::MAIN_MENU; //メインメニュー状態に遷移
			m_CurrentMenu = m_MainMenu;      //メインメニューに切り替え
			m_selectIndex = 0;               //選択インデックスを0にリセット
			// m_CurrentMenu に対して呼ぶ
			for (auto& menu : m_CurrentMenu)
			{
				menu.position.x = -500.0f; // スライド開始位置にリセット
			}
			StartMenuSlideIn(m_CurrentMenu, -500.0f, 100.0f);
		}
	}
}

void TitleUI::Render(ID3D11DeviceContext* dc)
{
	RenderTitleLogo(dc);          //タイトルロゴ描画
	
	switch (m_State)
	{
	case MenuType::MAIN_MENU:
		RenderMainMenu(dc);      //メインメニュー描画
		break;
	case MenuType::SELECT_DIFFICULTY:
		RenderDifficultyMenu(dc);//難易度選択メニュー描画
		break;
	case MenuType::SETTINGS:
		RenderSettingsMenu(dc); //設定メニュー描画
		break;
	case MenuType::EXIT:
		break;
	}
}

//メインメニューの作成(Start,settings,Exit)
void TitleUI::SetupMainMenuCallbacks()
{
	m_MainMenu.clear();
	m_MainMenuSprites.clear();
	auto device = DeviceManager::instance()->getDevice();

	// Start メニュー
	{
		Menu startMenu;
		startMenu.SelectCallback = [this]()
			{
				m_State = MenuType::SELECT_DIFFICULTY;
				m_selectIndex = 0;
				StartMenuSlideIn(m_DifficultyMenu, -500.0f, 100.0f);
			};
		//位置等の設定
		startMenu.position.x = 100.0f;
		startMenu.position.y = 200.0f;
		startMenu.targetPosition.x = 100.0f;
		startMenu.targetPosition.y = 200.0f;

		m_MainMenu.push_back(startMenu);
		m_MainMenuSprites.push_back(std::make_unique<Sprite>(device, L"Resources\\Image\\TItle_Images\\MainMenu_Images\\Start.png"));
	}

	// Settings メニュー
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

	// Exit メニュー
	{
		Menu exitMenu;
		exitMenu.SelectCallback = []()
			{
				// 終了処理
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

//難易度選択メニューの作成(Easy、Normal、Hard、戻る)
void TitleUI::BuildDifficultyMenu()
{
	m_DifficultyMenu.clear();
	m_DifficultyMenuSprites.clear();
	auto device = DeviceManager::instance()->getDevice();
	
	// Easy メニュー
	{
		Menu easyMenu;
		easyMenu.SelectCallback = [this]()
			{
				SceneManager::instance()->changeScene(new LoadingScene(new GameScene));
				m_State = MenuType::MAIN_MENU;  // メインメニューに戻す例
			};
		easyMenu.position.x = 100.0f;
		easyMenu.position.y = 200.0f;
		easyMenu.targetPosition.x = 100.0f;
		easyMenu.targetPosition.y = 200.0f;

		m_DifficultyMenu.push_back(easyMenu);
		m_DifficultyMenuSprites.push_back(std::make_unique<Sprite>(device, L"Resources\\Image\\TItle_Images\\Difficult_Images\\Easy.png"));
	}

	// Normal メニュー
	{
		Menu normalMenu;
		normalMenu.SelectCallback = [this]()
			{
				// 難易度Normal選択時の処理
				m_State = MenuType::MAIN_MENU;
			};
		normalMenu.position.x = 100.0f;
		normalMenu.position.y = 300.0f;
		normalMenu.targetPosition.x = 100.0f;
		normalMenu.targetPosition.y = 300.0f;

		m_DifficultyMenu.push_back(normalMenu);
		m_DifficultyMenuSprites.push_back(std::make_unique<Sprite>(device, L"Resources\\Image\\TItle_Images\\Difficult_Images\\Normal.png"));
	}


	// Hard メニュー
	{
		Menu hardMenu;
		hardMenu.SelectCallback = [this]()
			{
				// 難易度Hard選択時の処理
				m_State = MenuType::MAIN_MENU;
			};
		hardMenu.position.x= 100.0f;
		hardMenu.position.y = 400.0f;
		hardMenu.targetPosition.x = 100.0f;
		hardMenu.targetPosition.y = 400.0f;

		m_DifficultyMenu.push_back(hardMenu);
		m_DifficultyMenuSprites.push_back(std::make_unique<Sprite>(device, L"Resources\\Image\\TItle_Images\\Difficult_Images\\Hard.png"));
	}

	// Back メニュー（メインメニューに戻る）
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

//設定メニュー
void TitleUI::BuildSettingsMenu()
{
	auto device = DeviceManager::instance()->getDevice();

	// 既存の設定メニューとスプライトをクリア
	m_SettingsMenu.clear();
	m_SettingsMenuSprites.clear();

	// 例: 音量設定メニュー
	{
		Menu volumeMenu;
		volumeMenu.SelectCallback = []()
			{
				// 音量設定画面へ遷移や設定処理など
			};
		volumeMenu.position.x = 100.0f;
		volumeMenu.position.y = 200.0f;
		volumeMenu.targetPosition.x = 100.0f;
		volumeMenu.targetPosition.y = 200.0f;

		m_SettingsMenu.push_back(volumeMenu);
		m_SettingsMenuSprites.push_back(std::make_unique<Sprite>(device, L"Resources\\Image\\Title_Images\\Settings_Images\\Volume.png"));
	}

	// Back メニュー（メインメニューに戻る）
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
//各メニューの初期設定処理
void TitleUI::SetupMenus()
{
	SetupMainMenuCallbacks();   //メインメニューのコールバック登録
	BuildDifficultyMenu();      //難易度メニューの作成
	BuildSettingsMenu();        //設定メニューの作成
}

//タイトルロゴの描画処理
void TitleUI::RenderTitleLogo(ID3D11DeviceContext* dc)
{
	DeviceManager* deviceM = DeviceManager::instance();
	float scrW = deviceM->getScreenWidth();
	float scrH = deviceM->getScreenHeight();
	float sprW = m_TitleLogo->getTextureWidth();
	float sprH = m_TitleLogo->getTextureHeight();

	//タイトルロゴの描画位置を画面中央に設定
	m_TitleLogo->render(
		dc,
		0, 0, scrW, scrH,
		1, 1, 1, 1,
		0,
		0, 0, sprW, sprH
	);
}

//メニューの描画処理
void TitleUI::RenderMainMenu(ID3D11DeviceContext* dc)
{
	float startY = 300.0f; //メニューの開始Y座標
	float spacing = 120.0f;//メニュー間のスペース

	std::vector<float> posXList, posYList;	//メニューのX座標とY座標を格納するリスト
	for (size_t i = 0; i < m_MainMenu.size(); ++i)
	{
		float x = m_MainMenu[i].position.x;	//アニメーション付きのX座標
		float y = startY + i * spacing;	    //Y座標は開始位置からのオフセット

		posXList.push_back(x);				//X座標をリストに追加
		posYList.push_back(y);				//Y座標をリストに追加
	}

	RenderMenu(dc, m_MainMenu, m_MainMenuSprites, posXList, posYList);//メニューの描画処理を呼び出す
}

//難易度選択メニューの描画処理
void TitleUI::RenderDifficultyMenu(ID3D11DeviceContext* dc)
{
	float startY = 260.0f;  // 難易度メニューの開始Y座標
	float spacing = 100.0f; // メニュー間のスペース

	DeviceManager* deviceM = DeviceManager::instance();
	float screenWidth = deviceM->getScreenWidth();

	std::vector<float> posXList, posYList; // 描画座標リスト
	for (size_t i = 0; i < m_DifficultyMenu.size(); ++i)
	{
		// 難易度メニューは中央揃えでX座標を計算
		float spriteWidth = m_DifficultyMenuSprites[i]->getTextureWidth();
		float x = (screenWidth - spriteWidth) * 0.5f;

		float y = startY + i * spacing;

		posXList.push_back(x);
		posYList.push_back(y);
	}

	RenderMenu(dc, m_DifficultyMenu, m_DifficultyMenuSprites, posXList, posYList);
}

//設定メニューの描画処理
void TitleUI::RenderSettingsMenu(ID3D11DeviceContext* dc)
{
	float startY = 260.0f;  //設定メニューの開始Y座標
	float spacing = 100.0f; //メニュー間のスペース

	DeviceManager* deviceM = DeviceManager::instance();
	float screenWidth = deviceM->getScreenWidth();

	std::vector<float> posXList, posYList; //描画座標リスト
	for (size_t i = 0; i < m_SettingsMenu.size(); ++i)
	{
		// 設定メニューは中央揃え
		float spriteWidth = m_SettingsMenuSprites[i]->getTextureWidth();
		float x = (screenWidth - spriteWidth) * 0.5f;

		float y = startY + i * spacing;

		posXList.push_back(x);
		posYList.push_back(y);
	}

	RenderMenu(dc, m_SettingsMenu, m_SettingsMenuSprites, posXList, posYList);
}

//各メニューの描画処理
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
			r = 1.0f; g = 1.0f; b = 0.0f;  // 選択中は黄色にする例
		}

		// 本来のテクスチャサイズを取得
		float texW = sprites[i]->getTextureWidth();
		float texH = sprites[i]->getTextureHeight();

		// 本来のサイズで描画
		sprites[i]->render(
			dc,
			posXList[i], posYList[i],    // 描画位置（左上）
			texW, texH,                  // 描画サイズ（画像の元のサイズ）
			r, g, b, 1.0f,               // 色（α=1）
			0.0f,                        // 回転（なし）
			0.0f, 0.0f,                  // テクスチャの左上（UV）
			texW, texH                   // UVサイズ（全体）
		);
	}
}
//メニューをスライドインさせる関数
void TitleUI::StartMenuSlideIn(std::vector<Menu>& menuList,float startX, float targetX)
{
	for (auto& menu : menuList)
	{
		menu.position = DirectX::XMFLOAT2(startX, menu.position.y);
		menu.targetPosition = DirectX::XMFLOAT2(targetX, menu.position.y);
		menu.is_Animating = true; // アニメーション開始フラグを設定
	}
}

