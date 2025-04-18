#pragma once

#include "GamePad.h"
#include "Mouse.h"
#include <memory>

// �C���v�b�g
class InputManager
{
private:
	InputManager() {}
	~InputManager() {}

public:
	// �C���X�^���X�擾
	static InputManager* instance()
	{
		static InputManager inst;
		return &inst;
	}

	InputManager* initialize(HWND hwnd);

	// �X�V����
	void update();

	// �Q�[���p�b�h�擾
	GamePad* getGamePad() { return gamePad.get(); }

	// �}�E�X�擾
	Mouse* getMouse() { return mouse.get(); }

private:
	static InputManager*	inst;

	std::unique_ptr<GamePad>	gamePad;
	std::unique_ptr<Mouse>		mouse;
};
