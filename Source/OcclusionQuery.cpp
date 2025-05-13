#include "OcclusionQuery.h"
#include "DeviceManager.h"
#include "imgui.h"

#include "TitleScene.h"
#include "SceneManager.h"
#include "LoadingScene.h"

// ピクセル測定を開始
HRESULT OcclusionQuery::BeginQuery(Enemy* enemy, ID3D11DeviceContext* dc)
{
    std::lock_guard<std::mutex> lock(mutex);

    ID3D11Device* device = DeviceManager::instance()->getDevice();
    auto& data = queries[enemy];

    // クエリが完了していない場合はスキップ
    if (data.ready.load(std::memory_order_acquire)) return S_OK;

    // クエリが既に開始されている場合はスキップ
    if (data.started.load(std::memory_order_acquire)) return S_OK;

    // クエリが未作成の場合は作成
    if (!data.query) {
        D3D11_QUERY_DESC desc = { D3D11_QUERY_OCCLUSION, 0 };
        HRESULT hr = device->CreateQuery(&desc, data.query.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return hr;
    }

    // クエリ開始
    dc->Begin(data.query.Get());
    data.started.store(true, std::memory_order_release);
    return S_OK;
}

//Enemyのピクセル測定を終了する関数
HRESULT OcclusionQuery::EndQuery(Enemy* enemy, ID3D11DeviceContext* context) 
{
    std::lock_guard<std::mutex> lock(mutex);
    //Enemyに対応するクエリデータを検索
    auto it = queries.find(enemy);

    //該当するクエリが無い場合かBeginクエリが呼ばれていない場合は失敗扱い
    if (it == queries.end() || !it->second.started.load(std::memory_order_acquire))return E_FAIL;

    //クエリの終了処理をコンテキストに通知
    context->End(it->second.query.Get());

    //このクエリが終了した事をマークした後結果取得が可能になる
    it->second.started.store(false, std::memory_order_release);
    it->second.ready.store(true, std::memory_order_release);
    return S_OK;
}

//GPU側で描画が終わっているか確認してピクセル数の取得と状態更新を行う
void OcclusionQuery::CheckPixelCount(ID3D11DeviceContext* dc) 
{
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& [enemy, data] : queries) {
      //クエリが未完了またはクエリオブジェクトが無ければスキップ
        if (!data.ready.load(std::memory_order_acquire) || !data.query)continue;

        //GPUからピクセル数の取得を試みる
        UINT64 count = 0;
        HRESULT hr = dc->GetData(         //dcのデータ取得
            data.query.Get(),             //対象クエリ
            &count,                       //ピクセル数の格納先  
            sizeof(count),                //サイズ
            D3D11_ASYNC_GETDATA_DONOTFLUSH//非ブロッキングで取得
        );

        //ピクセル数が取得出来たら
        if (hr == S_OK) {
            data.pixelCount.store(count, std::memory_order_release); //ピクセル数を保存
            data.is_hide.store(count > 0, std::memory_order_release);//可視状態を記憶
            recentPixels[enemy] = count;                             //可視確認用に最近値を保存
            data.ready.store(false, std::memory_order_release);      //begin実行可
        }
    }
}

//pixelcountに基づくタイムカウンター
void OcclusionQuery::CheckCountTimer(float elapsedTime)
{
	std::lock_guard<std::mutex> lock(mutex);
	for (auto& [enemy, data] : queries) {
        //ピクセル数が取得出来ているか確認
        if (data.pixelCount.load(std::memory_order_acquire) > 0){
            data.hideTime.store(0.0f, std::memory_order_release);//hideTimeをリセット
        }
        else{
            //ピクセル数が取得出来ていない場合、hideTimeを加算
			float currentHideTime = data.hideTime.load(std::memory_order_acquire);
			currentHideTime += elapsedTime;
			data.hideTime.store(currentHideTime, std::memory_order_release);

			//任意の時間以上経過した場合、シーンを変更
            if (currentHideTime >= data.hideTimeThereshold){
                //現在はシーンを変えるだけだが、後に応じて変更する予定
				SceneManager::instance()->changeScene(new LoadingScene(new TitleScene));
            }
        }
	}
}
// ImGuiでデバッグ情報を表示
void OcclusionQuery::DrawImGui() {
    if (recentPixels.empty()) {
        ImGui::Text("No pixel data available.");
        return;
    }

    for (const auto& [enemy, count] : recentPixels) {
        ImGui::Text("Enemy: %p", static_cast<void*>(enemy));
        ImGui::Text("Pixel Count: %llu", count);

        //queriesマップからデータを取得
        auto it = queries.find(enemy);
        if (it != queries.end()) {
            const auto& data = it->second;
            float hideTime = data.hideTime.load(std::memory_order_acquire);
            float hideTimeThreshold = data.hideTimeThereshold.load(std::memory_order_acquire);

            ImGui::Text("Hide Time: %.2f seconds", hideTime);
            ImGui::Text("Threshold: %.2f seconds", hideTimeThreshold);

            if (hideTime >= hideTimeThreshold) {
                ImGui::TextColored({ 1, 0.5f, 0, 1 }, "Threshold Exceeded!");
            }
        }
        else {
            ImGui::Text("Hide Time: N/A");
            ImGui::Text("Threshold: N/A");
        }

        ImGui::TextColored(
            count > 0 ? ImVec4{ 0, 1, 0, 1 } : ImVec4{ 1, 0, 0, 1 },
            count > 0 ? "Visible" : "Hide"
        );

        ImGui::Separator();
    }
}