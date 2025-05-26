#include "GameScene.h"
#include "DeviceManager.h"
#include "Graphics/GraphicsManager.h"
#include"Graphics/ImGuiRenderer.h"
#include "Camera.h"
#include "EnemyPlantune.h"
#include "EnemyManager.h"
#include "EffectManager.h"
#include "StageManager.h"
#include "Input/InputManager.h"

#include "Graphics/Texture.h"
#include "Mathf.h"
#include "ObjManager.h"

#include "Utilities.h"

// 初期化
void GameScene::initialize()
{
	DeviceManager* deviceMgr = DeviceManager::instance();

	// シーン定数バッファの作成
	createBuffer<GameScene::SceneConstants>(DeviceManager::instance()->getDevice(), buffer.GetAddressOf());

	//ステージの作成
	stage = std::make_unique<Stage>();
	StageManager::instance()->regist(stage.get());

	//エフェクトの初期化
	EffectManager::instance()->initialize();

	//プレイヤーの作成
	player = std::make_unique<Player>();

	//カメラ操作の初期化
	cameraCtrl = std::make_unique<CameraController>();

	//敵の作成と初期化
		EnemyPlantune* plantune = new EnemyPlantune();
		plantune->setPosition({ 2.0f,0.0f,5.0f });

		//敵管理クラスに取り付け
		EnemyManager* eneMgr = EnemyManager::instance();
		eneMgr->regist(plantune);
	//ゲージ用スプライト
	gauge = std::make_unique<Sprite>(deviceMgr->getDevice(), nullptr);

	
	//カメラ初期設定
	Camera* camera = Camera::instance();
	camera->setLookAt(
		DirectX::XMFLOAT3(0, 10, -10),// カメラの視点（位置）
		DirectX::XMFLOAT3(0, 0, 0),	  // カメラの注視点（ターゲット）
		DirectX::XMFLOAT3(0, 1, 0)    // カメラの上方向
	);

	camera->setPerspectiveFov(
		DirectX::XMConvertToRadians(45),	// 視野角
		deviceMgr->getScreenWidth() / deviceMgr->getScreenHeight(),	// 画面縦横比率
		0.1f,		// カメラが映し出すの最近距離
		1000.0f		// カメラが映し出すの最遠距離
	);

	skymap = std::make_unique<Skymap>(deviceMgr->getDevice(), L"Resources/Image/incskies_043_16k.png");
	hp = std::make_unique<Sprite>(deviceMgr->getDevice(), L"Resources/Image/HPgage1.png");
	hp->SetShaderResourceView(hp->GetShaderResourceView(),hp->getTextureWidth(),hp->getTextureHeight());
	
	//	爆発アニメーション付きパーティクル
	{
		D3D11_TEXTURE2D_DESC texture2d_desc;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;

		//	パーティクル用画像ロード
		TextureManager::instance()->loadTextureFromFile(deviceMgr->getDevice(), L"Resources/Image/Explosion01_5x5.png", shader_resource_view.GetAddressOf(), &texture2d_desc);

		//	パーティクルシステム生成
		particle_bomb = std::make_unique<ParticleSystem>(deviceMgr->getDevice(), shader_resource_view, 5, 5);
	}

	//新しい描画ターゲットの生成
	{
		renderTarget = std::make_unique<RenderTarget>(
			static_cast<UINT>(deviceMgr->getScreenWidth()),
			static_cast<UINT>(deviceMgr->getScreenHeight()),
			DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	//ポストプロセス描画クラス生成
	{
		//postprosessingRenderer = std::make_unique<PostprosessingRenderer>();
		////シーンテクスチャを設定しておく
		//ShaderResourceViewData srvData;
		//srvData.srv = renderTarget->GetShaderResorceView();
		//srvData.width = renderTarget->GetWidth();
		//srvData.height = renderTarget->GetHeight();
		//postprosessingRenderer->SetSceneData(srvData);
	}

}

// 終了処理
void GameScene::finalize()
{
	//エネミー終了化
	EnemyManager::instance()->clear();
	EffectManager::instance()->finalize();
}

// 更新処理
void GameScene::update(float elapsedTime)
{
	//カメラ操作の更新
	DirectX::XMFLOAT3 target = *(player->getPosition());
	target.y += options.x; //プレイヤーの腰あたりに注視点設定
	
	cameraCtrl->setTarget(target);

	cameraCtrl->update(elapsedTime, player->getAngle()->y);

	//ステージ更新処理
	stage->update(elapsedTime);

	//プレイヤー更新処理
	player->update(elapsedTime);

	//タッチによる敵の出現
	enemyPlacementByTouch(DeviceManager::instance()->getDeviceContext(), elapsedTime);

	//敵の更新処理
	EnemyManager::instance()->update(elapsedTime);

	//エフェクトの更新
	EffectManager::instance()->update(elapsedTime);

	glitchData.time += elapsedTime;
	if (glitchData.time > 1.0f)
	{
		glitchData.time = 0.1f;
	}
}


// 描画処理
void GameScene::render()
{
	DeviceManager* mgr = DeviceManager::instance();
	GraphicsManager* graphics = GraphicsManager::instance();

	ID3D11DeviceContext* dc = mgr->getDeviceContext();
	ID3D11RenderTargetView* rtv = mgr->getRenderTargetView();
	ID3D11DepthStencilView* dsv = mgr->getDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT4X4* view = camera->getView();
	const DirectX::XMFLOAT4X4* proj = camera->getProjection();

	// 3D モデルの描画に必要な情報
	SceneConstants sc;

	//ビュー行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(view);

	//プロジェクション行列
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(proj);

	DirectX::XMStoreFloat4x4(&sc.viewProjection, View * Projection);

	// ライト方向（下方向）
	sc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };

	// 3D 描画に利用する定数バッファの更新と設定
	bindBuffer(dc, 1, buffer.GetAddressOf(), &sc);

	//スカイマップ
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// サンプラーステートの設定（アニソトロピック）
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
		// ブレンドステートの設定（アルファ）
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// 深度ステンシルステートの設定（深度テストオン、深度書き込みオフ）
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_OFF)].Get(), 0);
		// ラスタライザステートの設定（ソリッド、裏面表示オフ）
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
		});
	{
		DirectX::XMFLOAT4X4 inverse_view_projection;
		//カメラの逆行列
		DirectX::XMStoreFloat4x4(&inverse_view_projection, DirectX::XMMatrixInverse(nullptr, View * Projection));
		skymap->Render(dc, camera->getEye(), inverse_view_projection);
	}

	// 3D 描画設定
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// サンプラーステートの設定（アニソトロピック）
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
		// ブレンドステートの設定（アルファ）
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// 深度ステンシルステートの設定（深度テストオン、深度書き込みオン）
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_ON)].Get(), 0);
		// ラスタライザステートの設定（ソリッド、裏面表示オフ）
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
		});

	// 3D 描画
	{
		//ステージの描画
		stage->render(dc);

		//プレイヤーの描画
		player->render(dc);

		//敵描画処理
		EnemyManager::instance()->render(dc);
	}

	//3Dエフェクト描画
	{
		EffectManager::instance()->render(view, proj);
	}


	//パーティクル描画
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// サンプラーステートの設定（アニソトロピック）
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
		// ブレンドステートの設定（アルファ）
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// 深度ステンシルステートの設定（深度テストオン、深度書き込みオフ）
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_OFF)].Get(), 0);
		// ラスタライザステートの設定（ソリッド、裏面表示オフ）
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
		});
	{
		if (particle_bomb)particle_bomb->Render(dc, *view, *proj);
	}

	// 3Dデバッグ描画
	{
		// ラインレンダラ描画実行
		graphics->getLineRenderer()->render(dc, *view, *proj);

		// デバッグレンダラ描画実行
		graphics->getDebugRenderer()->render(dc, *view, *proj);

		player->drawDrawPrimitive();

		EnemyManager::instance()->drawDebugPrimitive();
	}

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
		RenderEnemyGauge(dc, *view, *proj);
		RenderContext rc;

		SpriteShader* gaussianShader = graphics->GetShader(SpriteShaderID::GaussianBlur);
		SpriteShader* glitchShader = graphics->GetShader(SpriteShaderID::Glitch);
		SpriteShader* luminanceShader = graphics->GetShader(SpriteShaderID::LuminanceExtraction);

		glitchShader->Begin(dc);
		rc.glitchData = glitchData;
		glitchShader->Draw(dc, rc, hp.get());
		glitchShader->End(dc);

	/*	rc.gaussianFilterData = gaussianFilterData;
		rc.gaussianFilterData.textureSize.x = hp->getTextureWidth();
		rc.gaussianFilterData.textureSize.y = hp->getTextureHeight();
		rc.luminanceExtractionData = lumianceExtractionData;
		luminanceShader->Begin(dc);
		luminanceShader->Draw(dc,rc, hp.get());
		luminanceShader->End(dc);*/

		/*rc.gaussianFilterData = gaussianFilterData;
		rc.gaussianFilterData.textureSize.x = hp->getTextureWidth();
		rc.gaussianFilterData.textureSize.y = hp->getTextureHeight();
		gaussianShader->Begin(dc);
		gaussianShader->Draw(dc, rc, hp.get());
		gaussianShader->End(dc);*/
		
	}

	if (ImGui::Begin("Debug"))
	{
		if (ImGui::BeginTabBar("DebugTabs"))
		{
			// === 2D Debug ===
			if (ImGui::BeginTabItem("2D Debug"))
			{
				if (ImGui::TreeNode("Glitch Shader"))
				{
					if (ImGui::CollapsingHeader("Param", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::InputFloat("Time", &glitchData.time);
						ImGui::SliderFloat("Density", &glitchData.density, 0.0f, 1.0f);
						ImGui::SliderFloat2("Shift Range", &glitchData.shift, 0.0f, 1.0f);
						ImGui::SliderFloat("Line Impact", &glitchData.line_impact, 0.0f, 100.0f);
						ImGui::SliderFloat("X Chromatic Shift", &glitchData.x_shifting, 0.0f, 1.0f);
						ImGui::SliderFloat("Y Chromatic Shift", &glitchData.y_shifting, 0.0f, 1.0f);
						ImGui::SliderFloat("HP(gage)", &glitchData.gage, 0.0f, 1.0f);
						ImGui::SliderFloat("Brightness", &glitchData.brightness, 0.0f, 1.0f);
						ImGui::SliderInt("Sampling Count", &glitchData.glitch_sampling_count, 0, 10);
					}

					if (ImGui::CollapsingHeader("Rectangular Noise", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::SliderFloat("Rect Noise Width", &glitchData.rect_noise_width, 0.0f, 1.0f);
						ImGui::SliderFloat("Rect Noise Height", &glitchData.rect_noise_height, 0.0f, 1.0f);
						ImGui::SliderFloat("Rect Noise Strength", &glitchData.rect_noise_strength, 0.0f, 1.0f);
					}

				

					// === Save / Load Buttons ===
					const std::string path = "glitch_settings.json";
					if (ImGui::Button("Save Glitch Settings")) {
						SaveSettings(glitchData, path);
					}
					ImGui::SameLine();
					if (ImGui::Button("Load Glitch Settings")) {
						LoadSettings(glitchData, path);
					}
					ImGui::SameLine();
					if (ImGui::Button("Reset All")) {
						// すべてのパラメータを 0 に初期化
						glitchData.time = 0.0f;
						glitchData.density = 0.0f;
						glitchData.shift = 0.0f;
						glitchData.line_impact = 0.0f;
						glitchData.x_shifting = 0.0f;
						glitchData.y_shifting = 0.0f;
						glitchData.brightness = 0.0f;
						glitchData.glitch_sampling_count = 0;
						glitchData.rect_noise_width = 0.0f;
						glitchData.rect_noise_height = 0.0f;
						glitchData.rect_noise_strength = 0.0f;
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Gaussian Filter"))
				{
					if (ImGui::CollapsingHeader("Param", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::SliderInt("kernelSize", &gaussianFilterData.kernelSize, 1, MaxKernelSize - 1);
						ImGui::SliderFloat("deviation", &gaussianFilterData.deviation, 1.0f, 10.0f);
					}
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Bloom"))
				{
					ImGui::SliderFloat("threshold", &lumianceExtractionData.threshold, 0.0f, 1.0f);
					ImGui::SliderFloat("intensity", &lumianceExtractionData.intensity, 0.0f, 10.0f);
					ImGui::TreePop();
				}

				ImGui::EndTabItem();
			}


			// === 3D Debug ===
			if (ImGui::BeginTabItem("3D Debug"))
			{
				if (ImGui::TreeNode("Player3D"))
				{
					DirectX::XMFLOAT3 eye = *(camera->getEye());
					player->drawDebugGui();
					ImGui::SliderFloat3("options", &options.x, -5.0f, 5.0f);
					ImGui::InputFloat3("camera position", &eye.x);
					ImGui::SliderFloat("damp", &damp, 2.0f, 5.0f);
					ImGui::TreePop();
				}
				ImGui::EndTabItem();
			}

			// === Map Debug ===
			if (ImGui::BeginTabItem("Map Debug"))
			{
				ImGui::Text("Map Debug ここに追加");
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
}



//敵ライフゲージの描画
void GameScene::RenderEnemyGauge(
	ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection)
{
	// ビューポート取得
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	// マウス入力
	Mouse* mouse = InputManager::instance()->getMouse();
	static Enemy* selectedEnemy = nullptr;
	static bool isDragging = false;

	// 敵マネージャー取得
	EnemyManager* enemyMgr = EnemyManager::instance();
	int enemyCount = enemyMgr->getEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyMgr->getEnemy(i);

		// エネミーの頭上のワールド座標
		DirectX::XMFLOAT3 worldPosition = *enemy->getPosition();
		worldPosition.y += enemy->getHeight();

		DirectX::XMVECTOR worldPositionVec = DirectX::XMLoadFloat3(&worldPosition);

		// ワールド座標からスクリーン座標へ変換
		DirectX::XMVECTOR screenPositionVec = DirectX::XMVector3Project(
			worldPositionVec,
			viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height,
			viewport.MinDepth, viewport.MaxDepth,
			projMat, viewMat, worldMat
		);

		// スクリーン座標取得
		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMStoreFloat2(&screenPosition, screenPositionVec);

		// ゲージの長さ
		const float gaugeWidth = 30.0f;
		const float gaugeHeight = 5.0f;

		float healthRate = enemy->getHealth() / static_cast<float>(enemy->getMaxHealth());


		float& gaugeOffsetX = gaugeOffsets[enemy].x;
		float& gaugeOffsetY = gaugeOffsets[enemy].y;

		if (gauge_b)
		{
			// ゲージ描画
			gauge->render(
				dc,
				gaugeOffsetX, gaugeOffsetY,
				gaugeWidth * healthRate, gaugeHeight,
				1.0f, 0.0f, 0.0f, 1.0f,
				0.0f,
				0, 0,
				static_cast<float>(gauge->getTextureWidth()),
				static_cast<float>(gauge->getTextureHeight())
			);
		}
	}
}

//タッチによる敵の配置
void GameScene::enemyPlacementByTouch(ID3D11DeviceContext* dc, float elapsedTime)
{
	static Enemy* selectedEnemy = nullptr;
	static bool isDragging = false;

	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT4X4* view = camera->getView();
	const DirectX::XMFLOAT4X4* proj = camera->getProjection();

	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(view);
	DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(proj);
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	Mouse* mouse = InputManager::instance()->getMouse();
	/*const float doubleClickThreshold = 0.3f;
	if (click) lastClickTime += elapsedTime;
	if (lastClickTime > doubleClickThreshold)
	{
		click = !click;
		lastClickTime = 0.0f;
	}*/

	// --- 敵選択処理 ---
	if (mouse->getButtonDown() & Mouse::BTN_LEFT)
	{
		/*click = !click;
		if (click) return;
		if (lastClickTime > doubleClickThreshold)
		{
			lastClickTime = 0.0f;
			return;
		}
		lastClickTime = 0.0f;*/
		DirectX::XMFLOAT3 screenPosition{
			static_cast<float>(mouse->getPositionX()),
			static_cast<float>(mouse->getPositionY()),
			0.0f
		};

		DirectX::XMVECTOR screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		DirectX::XMVECTOR worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec, viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth,
			projMat, viewMat, worldMat
		);

		DirectX::XMFLOAT3 rayStart;
		DirectX::XMStoreFloat3(&rayStart, worldPositionVec);

		screenPosition.z = 1.0f;
		screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec, viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth,
			projMat, viewMat, worldMat
		);

		DirectX::XMFLOAT3 rayEnd;
		DirectX::XMStoreFloat3(&rayEnd, worldPositionVec);

		if (!isDragging)
		{
			selectedEnemy = EnemyManager::instance()->getEnemyByRay(rayStart, rayEnd);
			if (selectedEnemy)
			{
				isDragging = true;
			}
		}
	}

	// --- ドラッグ中の敵の位置を更新 ---
	if (isDragging && selectedEnemy && mouse->getButton() & Mouse::BTN_LEFT)
	{
		DirectX::XMFLOAT3 screenPosition{
			static_cast<float>(mouse->getPositionX()),
			static_cast<float>(mouse->getPositionY()),
			0.0f
		};

		DirectX::XMVECTOR screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		DirectX::XMVECTOR worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec, viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth,
			projMat, viewMat, worldMat
		);

		DirectX::XMFLOAT3 rayStart;
		DirectX::XMStoreFloat3(&rayStart, worldPositionVec);

		screenPosition.z = 1.0f;
		screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec, viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth,
			projMat, viewMat, worldMat
		);

		DirectX::XMFLOAT3 rayEnd;
		DirectX::XMStoreFloat3(&rayEnd, worldPositionVec);

		/*HitResult hit;
		if (stage->raycast(rayStart, rayEnd, hit))
		{
			selectedEnemy->setPosition(hit.position);
		}*/
	}

	// --- ドロップ処理 ---
	if (isDragging && selectedEnemy && !(mouse->getButton() & Mouse::BTN_LEFT))
	{
		isDragging = false;

		selectedEnemy = nullptr;
	}
}