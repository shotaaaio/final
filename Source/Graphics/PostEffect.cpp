#include "PostEffect.h"
#include "Buffer.h"
#include "GameScene.h"
#include "DeviceManager.h"
#include "Framework.h"

PostEffect::PostEffect(ID3D11Device* device)
{
	//それぞれのポストエフェクト定数バッファの生成
	createBuffer<PostEffectSceneConstants>(device, luminancebuffer.GetAddressOf());
	createBuffer<PostEffectSceneConstants>(device, vignettebuffer.GetAddressOf());
	createBuffer<PostEffectSceneConstants>(device, colorfilterbuffer.GetAddressOf());

	frameBuffers[0] = std::make_unique<FrameBuffer>(device, static_cast<uint32_t>(SCREEN_WIDTH), static_cast<uint32_t>(SCREEN_HEIGHT));
	frameBuffers[1] = std::make_unique<FrameBuffer>(device, static_cast<uint32_t>(SCREEN_WIDTH / 2), static_cast<uint32_t>(SCREEN_HEIGHT / 2));
	frameBuffers[2] = std::make_unique<FrameBuffer>(device, static_cast<uint32_t>(SCREEN_WIDTH), static_cast<uint32_t>(SCREEN_HEIGHT));
	frameBuffers[3] = std::make_unique<FrameBuffer>(device, static_cast<uint32_t>(SCREEN_WIDTH), static_cast<uint32_t>(SCREEN_HEIGHT));
	bitBlockTransfer = std::make_unique<FullScreenQuad>(device);

	ShaderManager::instance()->createPsFromCso(device, ".\\Shader\\LuminanceExtractPS.cso", pixel_shader[0].GetAddressOf());
	ShaderManager::instance()->createPsFromCso(device, ".\\Shader\\LuminanceBlurPS.cso", pixel_shader[1].GetAddressOf());
	ShaderManager::instance()->createPsFromCso(device, ".\\Shader\\VignettePS.cso", pixel_shader[2].GetAddressOf());
	ShaderManager::instance()->createPsFromCso(device, ".\\Shader\\ColorFilterPS.cso", pixel_shader[3].GetAddressOf());
}

void PostEffect::Update()
{
	//周辺減光の更新
	if (vignettedata.vignette_compute_flag)
	{
		vignette.vignette_color = vignettedata.vignette_color;
		vignette.vignette_center = vignettedata.vignette_center;
		vignette.vignette_intensity = vignettedata.vignette_intensity * 3.0f;
		vignette.vignette_smoothness = max(0.000001f, vignettedata.vignette_smoothness);
		vignette.vignette_rounded = vignettedata.vignette_rounded ? 1.0f : 0.0f;
		vignette.vignette_roundness = 6.0f * (1.0f - vignettedata.vignette_roundness);
	}
}

void PostEffect::UpdateImGui()
{
	ImGui::Begin("post effect");

	ImGui::SliderInt("post effect index", &postEffectIndex, 0, 3);

	if (ImGui::TreeNode("Bloom"))
	{
		ImGui::ColorEdit3("luminance color", &luminance.luminanceColor.x);
		ImGui::SliderFloat("luminance threshold", &luminance.threshold, 0.0f, 1.0f);
		ImGui::SliderFloat("luminanse intencity", &luminance.intensity, 0.0f, 10.0f);

		ImGui::SliderInt("luminance flag", &luminance.luminance_use_flag, 0, 1);
		ImGui::SliderInt("blur flag", &luminance.blur_use_flag, 0, 1);
		ImGui::TreePop();
	}
	ImGui::Separator();

	if (ImGui::TreeNode("Vignette"))
	{
		ImGui::ColorEdit3("vignette color", &vignettedata.vignette_color.x);
		ImGui::SliderFloat2("vignette center", &vignettedata.vignette_center.x, 0.0f, 1.0f);
		ImGui::SliderFloat("vignette intensity", &vignettedata.vignette_intensity, 0.0f, 1.0f);
		ImGui::SliderFloat("vignette smoothness", &vignettedata.vignette_smoothness, 0.0f, 1.0f);
		ImGui::Checkbox("vignette rounded", &vignettedata.vignette_rounded);
		ImGui::SliderFloat("vignette roundness", &vignettedata.vignette_roundness, 0.0f, 1.0f);
		ImGui::TreePop();
	}
	ImGui::Separator();

	if (ImGui::TreeNode("ColorFilter"))
	{
		ImGui::ColorEdit3("filter color", &colorfilter.filterColor.x);
		ImGui::SliderFloat("filter hueshift", &colorfilter.hueShift, 0.0f, 360.0f);
		ImGui::SliderFloat("filter saturate", &colorfilter.saturation, 0.0f, 2.0f);
		ImGui::SliderFloat("filter brightness", &colorfilter.brightness, 0.0f, 2.0f);
		ImGui::TreePop();
	}
	ImGui::Separator();

	ImGui::End();
}

void PostEffect::UpdateBuffer(ID3D11DeviceContext* dc)
{
	bindBuffer(dc, 5, luminancebuffer.GetAddressOf(), &luminance);
	bindBuffer(dc, 6, vignettebuffer.GetAddressOf(), &vignette);
	bindBuffer(dc, 7, colorfilterbuffer.GetAddressOf(), &colorfilter);
}

void PostEffect::Render(ID3D11DeviceContext* dc)
{
	if (postEffectIndex <= 0)
	{
		bitBlockTransfer->blit(dc, frameBuffers[0]->shader_resource_views[0].GetAddressOf(), 0, 1);
		return;//インデックス番号0番はオフスクリーンレンダリングなので描画だけしてスキップ
	}

	if (postEffectIndex == 2) { vignettedata.vignette_compute_flag = true; }
	else { vignettedata.vignette_compute_flag = false; }

	frameBuffers[postEffectIndex]->clear(dc);
	frameBuffers[postEffectIndex]->activate(dc);
	bitBlockTransfer->blit(dc, frameBuffers[0]->shader_resource_views[0].GetAddressOf(), 0, 1, pixel_shader[0].Get());
	frameBuffers[postEffectIndex]->deactivate(dc);
	ID3D11ShaderResourceView* shader_resourse_views[2]
	{ frameBuffers[0]->shader_resource_views[0].Get(),
		frameBuffers[postEffectIndex]->shader_resource_views[0].Get() };
	bitBlockTransfer->blit(dc, shader_resourse_views, 0, 2, pixel_shader[postEffectIndex].Get());
}

void PostEffect::OffScreenStart(ID3D11DeviceContext* dc)
{
	frameBuffers[0]->clear(dc);
	frameBuffers[0]->activate(dc);
}

void PostEffect::OffScrrenEnd(ID3D11DeviceContext* dc)
{
	frameBuffers[0]->deactivate(dc);
}
