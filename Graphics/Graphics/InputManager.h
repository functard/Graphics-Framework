#pragma once

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include <dinput.h>

class CInputManager
{
public:
	int InitDirectInput(HINSTANCE _hInstance);
	void DetectInput();

	bool GetKeyDown(int _key) const;
	bool GetKeyUp(int _key) const;
	bool GetKey(int _key) const;

	bool GetMouseButtonDown(int _key) const;
	bool GetMouseButtonUp(int _key) const;
	bool GetMouseButton(int _key) const;

	float GetMouseWheel() const;
	XMFLOAT2 GetMouseMovement() const;
	inline XMFLOAT2 GetMousePosition() const { return m_mousePosition; }
private:
	LPDIRECTINPUT8 m_directInput;

	IDirectInputDevice8* m_mouse;
	IDirectInputDevice8* m_keyboard;

	DIMOUSESTATE m_currentMouseState;
	DIMOUSESTATE m_lastMouseState;

	byte m_currentKeyboardState[256];
	byte m_lastKeyboardState[256];

	XMFLOAT2 m_mousePosition;
};

