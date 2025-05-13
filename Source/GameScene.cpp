
#include "GameScene.h"
#include "DeviceManager.h"
#include "Graphics/GraphicsManager.h"
#include"Graphics/ImGuiRenderer.h"
#include "Camera.h"
#include "EnemySlime.h"
#include "EnemyManager.h"
#include "EffectManager.h"
#include "StageManager.h"
#include "Input/InputManager.h"

#include "Graphics/Texture.h"
#include "Mathf.h"

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

	//	爆発アニメーション付きパーティクル
	{
		D3D11_TEXTURE2D_DESC texture2d_desc;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;

		//	パーティクル用画像ロード
		TextureManager::instance()->loadTextureFromFile(deviceMgr->getDevice(), L"Resources/Image/Explosion01_5x5.png", shader_resource_view.GetAddressOf(), &texture2d_desc);

		//	パーティクルシステム生成
		particle_bomb = std::make_unique<ParticleSystem>(deviceMgr->getDevice(), shader_resource_view, 5, 5);
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
	//DirectX::XMFLOAT3 vel = player->getVelocity();

	//DirectX::XMFLOAT3 dir = Mathf::GetForwardDirection(player->getAngle()->y);
	//target.x += options.y * dir.x;
	//target.z += options.y * dir.z;
	cameraCtrl->setTarget(target);
	//Camera* camera = Camera::instance();

	//float damping = 2.0f * sqrt(damp);
	//idealPos = *camera->getEye();
	//DirectX::XMFLOAT3 actualpos = cameraCtrl->computeEye();

	//DirectX::XMFLOAT3 diff;
	//DirectX::XMStoreFloat3(&diff,
	//	DirectX::XMVector3Normalize(
	//	DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&idealPos), DirectX::XMLoadFloat3(&actualpos)))
	//);
	//

	//DirectX::XMVECTOR accel =
	//	DirectX::XMVectorSubtract(
	//		DirectX::XMVectorScale(DirectX::XMLoadFloat3(&vel), damping),
	//		DirectX::XMVectorScale(DirectX::XMLoadFloat3(&diff), -damp)
	//	);

	//DirectX::XMFLOAT3 eye;
	//DirectX::XMStoreFloat3(&eye,
	//DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&idealPos), DirectX::XMVectorScale(accel, elapsedTime)));

	//cameraCtrl->rotateCameraAngle(elapsedTime);
	//camera->setLookAt(eye, target, {0,1,0});
	cameraCtrl->update(elapsedTime, player->getAngle()->y);

	//ステージ更新処理
	stage->update(elapsedTime);

	//プレイヤー更新処理
	player->update(elapsedTime);

	//タッチによる敵の出現
	//enemyPlacementByTouch(DeviceManager::instance()->getDeviceContext());

	//敵の更新処理
	EnemyManager::instance()->update(elapsedTime);

	//エフェクトの更新
	EffectManager::instance()->update(elapsedTime);


	////	パーティクルシステム更新
	//if (particle_bomb)
	//{
	//	//	爆発演出
	//	if (::GetAsyncKeyState('R') & 0x8000)
	//	{
	//		particle_bomb->Set(
	//			0, 1.0f,
	//			*player.get()->getPosition(),
	//			DirectX::XMFLOAT3(
	//				(rand() % 200 - 100) * 0.001f,
	//				(rand() % 100) * 0.005f,
	//				0),
	//			DirectX::XMFLOAT3(0, 0.5f, 0),
	//			DirectX::XMFLOAT2(3.0f, 3.0f),
	//			true,
	//			24.0f
	//		);
	//	}
	//	particle_bomb->Update(elapsedTime);
	//}
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
		skymap->Render(dc,camera->getEye(),inverse_view_projection);
	}

	// 3D 描画設定
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc){
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
		graphics->getLineRenderer()->render(dc, *view,*proj);

		// デバッグレンダラ描画実行
		graphics->getDebugRenderer()->render(dc, *view, *proj);

		player->drawDrawPrimitive();

		EnemyManager::instance()->drawDebugPrimitive();
		EnemyManager::instance()->drawImgui();
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
		//RenderEnemyGauge(dc, *view, *proj);
	}

	// 2DデバッグGUI描画
	{
		DirectX::XMFLOAT3 eye = *(camera->getEye());
		//プレイヤーのデバックの描画
		player->drawDebugGui();
		ImGui::Begin("game");
		ImGui::SliderFloat3("options", &options.x, -5.0f, 5.0f);
		ImGui::InputFloat3("camera position", &eye.x);
		ImGui::SliderFloat("damp", &damp, 2.0f, 5.0f);
		ImGui::End();
	}
}


//敵ライフゲージの描画
void GameScene::RenderEnemyGauge(
	ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection
)
{
	//ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	//変換行列
	DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	//全ての敵の頭上にライフゲージを表示
	EnemyManager* enemyMgr = EnemyManager::instance();
	int enemyCount = enemyMgr->getEnemyCount();

	for (int i = 0;i < enemyCount;++i)
	{
		Enemy* enemy = enemyMgr->getEnemy(i);
		//エネミーの頭上のワールド座標
		DirectX::XMFLOAT3 worldPosition = *enemy->getPosition();
		worldPosition.y += enemy->getHeight();

		DirectX::XMVECTOR worldPositionVec = DirectX::XMLoadFloat3(&worldPosition);

		//ワールド座標からスクリーン座標へ変換
		DirectX::XMVECTOR screenPositionVec = DirectX::XMVector3Project(
			worldPositionVec,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			projMat,
			viewMat,
			worldMat
		);

		//スクリーン座標
		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMStoreFloat2(&screenPosition, screenPositionVec);

		//ゲージの長さ
		const float gaugeWidth = 30.0f;
		const float gaugeHeight = 5.0f;

		float healthRate = enemy->getHealth() / static_cast<float>(enemy->getMaxHealth());

		//ゲージ描画
		gauge->render(
			dc,
			screenPosition.x - gaugeWidth * 0.5f,
			screenPosition.y - gaugeHeight,
			gaugeWidth * healthRate,
			gaugeHeight,
			1.0f, 0.0f, 0.0f, 1.0f,
			0.0f,
			0, 0,
			static_cast<float>(gauge->getTextureWidth()),
			static_cast<float>(gauge->getTextureHeight())
		);
	}
}

//タッチによる敵の配置
void GameScene::enemyPlacementByTouch(ID3D11DeviceContext* dc)
{
	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT4X4* view = camera->getView();
	const DirectX::XMFLOAT4X4* proj = camera->getProjection();

	//ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	//変換行列
	DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(view);
	DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(proj);
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	//敵配置処理
	Mouse* mouse = InputManager::instance()->getMouse();
	if (mouse->getButtonDown() & Mouse::BTN_LEFT)
	{
		//マウスカーソル座標を取得
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<float>(mouse->getPositionX());
		screenPosition.y = static_cast<float>(mouse->getPositionY());

		DirectX::XMVECTOR screenPositionVec, worldPositionVec;

		//レイの始点を計算
		screenPosition.z = 0.0f; //カメラに一番近い位置
		screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			projMat,
			viewMat,
			worldMat
		);

		DirectX::XMFLOAT3 rayStart;
		DirectX::XMStoreFloat3(&rayStart, worldPositionVec);

		//レイの終点を計算
		screenPosition.z = 1.0f;	//カメラに一番遠い位置
		screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			projMat,
			viewMat,
			worldMat
		);
		DirectX::XMFLOAT3 rayEnd;
		DirectX::XMStoreFloat3(&rayEnd, worldPositionVec);

		//レイキャスト
		HitResult hit;
		if (stage->raycast(rayStart, rayEnd, hit))
		{
			//敵を配置
			EnemySlime* slime = new EnemySlime();
			slime->setPosition(hit.position);
			EnemyManager::instance()->regist(slime);
		}


	}
}