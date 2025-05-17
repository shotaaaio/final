#include"TitleScene.h"
#include"DeviceManager.h"
#include"Graphics/GraphicsManager.h"
#include"Input/InputManager.h"
#include"SceneManager.h"
#include"GameScene.h"
#include"LoadingScene.h"
#include "imgui.h"


//初期化処理
void TitleScene::initialize()
{
	//タイトル画像読み込み
	titleImage = std::make_unique<Sprite>(DeviceManager::instance()->getDevice(), L"Resources\\Image\\Title.png");
}

//終了化
void TitleScene::finalize()
{

}

//更新処理
void TitleScene::update(float elapsedTime)
{
	GamePad* gamePad = InputManager::instance()->getGamePad();

	//ボタンを押したらゲームシーンに遷移
	if (gamePad->getButtonDown() & GamePad::BTN_A)
	{
		SceneManager::instance()->changeScene(new LoadingScene(new GameScene));
	}
}

//描画処理
void TitleScene::render()
{
	DeviceManager* mgr = DeviceManager::instance();
	GraphicsManager* graphics = GraphicsManager::instance();

	ID3D11DeviceContext* dc = mgr->getDeviceContext();
	ID3D11RenderTargetView* rtv = mgr->getRenderTargetView();
	ID3D11DepthStencilView* dsv = mgr->getDepthStencilView();

	//画面クリア＆レンダーターゲットビュー設定
	FLOAT color[] = { 0.0f,0.0f,0.5f,1.0f };//RGBA(0.0f~1.0f)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);
	
	// 2D 描画設定
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// サンプラーステートの設定（リニア）
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
		// ブレンドステートの設定（アルファ）
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// 深度ステンシルステートの設定（深度テストオフ、深度書き込みオフ）
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::OFF_OFF)].Get(), 0);
		// ラスタライザステートの設定（ソリッド、裏面表示オフ）
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
	});

	// 2D 描画
	{
		titleImage->render(dc,
			0, 0, mgr->getScreenWidth(), mgr->getScreenHeight(),
			1, 1, 1, 1,
			0,
			0, 0, 1280, 720
		);
	}

	// サーバーと接続
	if (ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::Button(u8"Stage:1"))
		{
			SceneManager::instance()->changeScene(new LoadingScene(new GameScene));
		}
	}
	ImGui::End();
}