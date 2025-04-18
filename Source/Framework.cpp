#include <memory>
#include <sstream>
#include <d3d11.h>

#include "Framework.h"
#include "Graphics/GraphicsManager.h"
#include "TitleScene.h"
#include "SceneManager.h"

// �R���X�g���N�^
Framework::Framework(HWND hwnd) : hwnd(hwnd)
{
	// �f�o�C�X�Ǘ��̏�����
	deviceMgr = DeviceManager::instance()->initialize(hwnd);

	// ���͊Ǘ��̏�����
	inputMgr = InputManager::instance()->initialize(hwnd);

	// �I�[�f�B�I�Ǘ��̏�����
	audioMgr = AudioManager::instance()->initialize();

	// �O���t�B�b�N�X�Ǘ��̏�����
	GraphicsManager::instance()->initialize(deviceMgr->getDevice(), deviceMgr->getDeviceContext());

	// imgui �̍쐬
	imguiRenderer = std::make_unique<ImGuiRenderer>(hwnd, deviceMgr->getDevice());

	//�V�[���̐����Ə�����
	SceneManager::instance()->changeScene(new TitleScene);
}

// �f�X�g���N�^
Framework::~Framework()
{
	SceneManager::instance()->clear();
}

// �X�V����
void Framework::update(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// ���͍X�V����
	inputMgr->update();

	// �V�[���X�V����
	SceneManager::instance()->update(elapsedTime);
}

// �`�揈��
void Framework::render(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// �r������
	std::lock_guard<std::mutex> lock(DeviceManager::instance()->getMutex());

	ID3D11DeviceContext* dc = deviceMgr->getDeviceContext();

	// IMGUI�t���[���J�n����
	imguiRenderer->newFrame();

	// �V�[���`�揈��
	SceneManager::instance()->render();

	// demo
	//ImGui::ShowDemoWindow();

	// IMGUI�`��
	imguiRenderer->render(dc);

	// �o�b�N�o�b�t�@�ɕ`�悵�������ʂɕ\������B
	deviceMgr->getSwapChain()->Present(syncInterval, 0);
}

// �t���[�����[�g�v�Z
void Framework::calculateFrameStats()
{
	if (++frames, (timer.timeStamp() - elapsed_time) >= 1.0f)
	{
		float fps = static_cast<float>(frames);
		std::ostringstream outs;
		outs.precision(6);
		outs << " : FPS : " << fps << " / " << "Frame Time : " << 1000.0f / fps << " (ms)";
		SetWindowTextA(hwnd, outs.str().c_str());

		frames = 0;
		elapsed_time += 1.0f;
	}
}

// �A�v���P�[�V�������[�v
int Framework::run()
{
	MSG msg = {};

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.tick();
			calculateFrameStats();

			float elapsedTime = syncInterval == 0 ? timer.timeInterval() : syncInterval / 60.0f;
			
			update(elapsedTime);
			render(elapsedTime);
		}
	}
	return static_cast<int>(msg.wParam);
}

// ���b�Z�[�W�n���h��
LRESULT CALLBACK Framework::handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (imguiRenderer->handleMessage(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		timer.stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		timer.start();
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
