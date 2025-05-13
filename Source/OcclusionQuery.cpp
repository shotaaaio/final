#include "OcclusionQuery.h"
#include "DeviceManager.h"
#include "imgui.h"

#include "TitleScene.h"
#include "SceneManager.h"
#include "LoadingScene.h"

// �s�N�Z��������J�n
HRESULT OcclusionQuery::BeginQuery(Enemy* enemy, ID3D11DeviceContext* dc)
{
    std::lock_guard<std::mutex> lock(mutex);

    ID3D11Device* device = DeviceManager::instance()->getDevice();
    auto& data = queries[enemy];

    // �N�G�����������Ă��Ȃ��ꍇ�̓X�L�b�v
    if (data.ready.load(std::memory_order_acquire)) return S_OK;

    // �N�G�������ɊJ�n����Ă���ꍇ�̓X�L�b�v
    if (data.started.load(std::memory_order_acquire)) return S_OK;

    // �N�G�������쐬�̏ꍇ�͍쐬
    if (!data.query) {
        D3D11_QUERY_DESC desc = { D3D11_QUERY_OCCLUSION, 0 };
        HRESULT hr = device->CreateQuery(&desc, data.query.ReleaseAndGetAddressOf());
        if (FAILED(hr)) return hr;
    }

    // �N�G���J�n
    dc->Begin(data.query.Get());
    data.started.store(true, std::memory_order_release);
    return S_OK;
}

//Enemy�̃s�N�Z��������I������֐�
HRESULT OcclusionQuery::EndQuery(Enemy* enemy, ID3D11DeviceContext* context) 
{
    std::lock_guard<std::mutex> lock(mutex);
    //Enemy�ɑΉ�����N�G���f�[�^������
    auto it = queries.find(enemy);

    //�Y������N�G���������ꍇ��Begin�N�G�����Ă΂�Ă��Ȃ��ꍇ�͎��s����
    if (it == queries.end() || !it->second.started.load(std::memory_order_acquire))return E_FAIL;

    //�N�G���̏I���������R���e�L�X�g�ɒʒm
    context->End(it->second.query.Get());

    //���̃N�G�����I�����������}�[�N�����㌋�ʎ擾���\�ɂȂ�
    it->second.started.store(false, std::memory_order_release);
    it->second.ready.store(true, std::memory_order_release);
    return S_OK;
}

//GPU���ŕ`�悪�I����Ă��邩�m�F���ăs�N�Z�����̎擾�Ə�ԍX�V���s��
void OcclusionQuery::CheckPixelCount(ID3D11DeviceContext* dc) 
{
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& [enemy, data] : queries) {
      //�N�G�����������܂��̓N�G���I�u�W�F�N�g��������΃X�L�b�v
        if (!data.ready.load(std::memory_order_acquire) || !data.query)continue;

        //GPU����s�N�Z�����̎擾�����݂�
        UINT64 count = 0;
        HRESULT hr = dc->GetData(         //dc�̃f�[�^�擾
            data.query.Get(),             //�ΏۃN�G��
            &count,                       //�s�N�Z�����̊i�[��  
            sizeof(count),                //�T�C�Y
            D3D11_ASYNC_GETDATA_DONOTFLUSH//��u���b�L���O�Ŏ擾
        );

        //�s�N�Z�������擾�o������
        if (hr == S_OK) {
            data.pixelCount.store(count, std::memory_order_release); //�s�N�Z������ۑ�
            data.is_hide.store(count > 0, std::memory_order_release);//����Ԃ��L��
            recentPixels[enemy] = count;                             //���m�F�p�ɍŋߒl��ۑ�
            data.ready.store(false, std::memory_order_release);      //begin���s��
        }
    }
}

//pixelcount�Ɋ�Â��^�C���J�E���^�[
void OcclusionQuery::CheckCountTimer(float elapsedTime)
{
	std::lock_guard<std::mutex> lock(mutex);
	for (auto& [enemy, data] : queries) {
        //�s�N�Z�������擾�o���Ă��邩�m�F
        if (data.pixelCount.load(std::memory_order_acquire) > 0){
            data.hideTime.store(0.0f, std::memory_order_release);//hideTime�����Z�b�g
        }
        else{
            //�s�N�Z�������擾�o���Ă��Ȃ��ꍇ�AhideTime�����Z
			float currentHideTime = data.hideTime.load(std::memory_order_acquire);
			currentHideTime += elapsedTime;
			data.hideTime.store(currentHideTime, std::memory_order_release);

			//�C�ӂ̎��Ԉȏ�o�߂����ꍇ�A�V�[����ύX
            if (currentHideTime >= data.hideTimeThereshold){
                //���݂̓V�[����ς��邾�������A��ɉ����ĕύX����\��
				SceneManager::instance()->changeScene(new LoadingScene(new TitleScene));
            }
        }
	}
}
// ImGui�Ńf�o�b�O����\��
void OcclusionQuery::DrawImGui() {
    if (recentPixels.empty()) {
        ImGui::Text("No pixel data available.");
        return;
    }

    for (const auto& [enemy, count] : recentPixels) {
        ImGui::Text("Enemy: %p", static_cast<void*>(enemy));
        ImGui::Text("Pixel Count: %llu", count);

        //queries�}�b�v����f�[�^���擾
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