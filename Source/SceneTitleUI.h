#pragma once
#include "UIComponent.h"
#include <functional>

//画面状態
enum class MenuType
{
    MAIN_MENU,            //メインメニュー画面
    SELECT_DIFFICULTY,    //難易度選択画面
    SETTINGS,             //設定画面
    EXIT                  //終了処理
};
//メニュー項目を表す構造体
struct Menu
{
    std::function<void()>SelectCallback;//選択が決定された時処理用のコールバック関数

    //スプライト描画位置
	DirectX::XMFLOAT2 position;       //現在の位置
	DirectX::XMFLOAT2 targetPosition; //アニメーションの目標位置
    float animationSpeed;//px/秒
    bool is_Animating;//
};

//タイトル用UI
class TitleUI : public UIComponent
{
public:
    TitleUI() {};
    ~TitleUI() override {};

    void Initialize() override;
    void Update(float elapsedTime) override;
    void Render(ID3D11DeviceContext* dc) override;

private:
    //タイトルロゴ画像
    std::unique_ptr<Sprite> m_TitleLogo;
 
    MenuType m_State = MenuType::MAIN_MENU; //状態
    int m_selectIndex; //メニュー内で選択されているインデックス

    // メニュー用スプライト群
    std::vector<std::unique_ptr<Sprite>> m_MainMenuSprites;
    std::vector<std::unique_ptr<Sprite>> m_DifficultyMenuSprites;
    std::vector<std::unique_ptr<Sprite>> m_SettingsMenuSprites;

    std::vector<Menu> m_MainMenu;          // メインメニュー用
    std::vector<Menu> m_DifficultyMenu;   // 難易度メニュー用
    std::vector<Menu> m_SettingsMenu;     // 設定メニュー用
    std::vector<Menu> m_CurrentMenu;       // 現在表示中のメニュー

private:
    //初期化関数関係
    void SetupMainMenuCallbacks(); //メインメニューからの各選択シーンのコールバック関数
    void BuildDifficultyMenu(); //難易度選択メニュー
    void BuildSettingsMenu();  //設定メニュー
	void SetupMenus(); //メニューの初期化

    //レンダリング処理関係
    void RenderTitleLogo(ID3D11DeviceContext* dc);
	void RenderMainMenu(ID3D11DeviceContext* dc);      //メインメニューの描画
	void RenderDifficultyMenu(ID3D11DeviceContext* dc);//難易度メニューの描画
	void RenderSettingsMenu(ID3D11DeviceContext* dc);  //設定メニューの描画
	//メニューの描画
    void RenderMenu(ID3D11DeviceContext* dc,
        const std::vector<Menu>& menu,
        const std::vector<std::unique_ptr<Sprite>>& sprites,
        const std::vector<float>& posXList,
        const std::vector<float>& posYList);

    //メニューのアニメーション開始設定
    void  StartMenuSlideIn(std::vector<Menu>& menuList, float startX, float targetX);
};