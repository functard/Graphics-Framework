#include "PCH.h"
#include "InputManager.h"
#include "Engine.h"
#include "Helpers.h"

int CInputManager::InitDirectInput(HINSTANCE _hInstance)
{
	int value;

	value = DirectInput8Create(_hInstance, DIRECTINPUT_VERSION,
		IID_IDirectInput8, (LPVOID*)&m_directInput, nullptr);
	if (FAILED(value))
		return -10001;

	value = m_directInput->CreateDevice(GUID_SysKeyboard,
		&m_keyboard, nullptr);
	if (FAILED(value))
	{
		return -10002;
	}
	value = m_directInput->CreateDevice(GUID_SysMouse,
		&m_mouse, nullptr);
	if (FAILED(value))
	{
		return -10003;
	}

	value = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(value))
		return -10004;

	value = m_keyboard->SetCooperativeLevel
	(
		WDS.m_WindowHandle,
		DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND
	);
	if (FAILED(value))
		return -10005;

	value = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(value))
		return -10006;

	value = m_mouse->SetCooperativeLevel
	(
		WDS.m_WindowHandle,
		DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND
	);

	m_mousePosition = XMFLOAT2(WDS.m_WindowWidth * 0.5f, WDS.m_WindowHeigth * 0.5f);

	ZeroMemory(m_currentKeyboardState, sizeof(m_currentKeyboardState));
	ZeroMemory(&m_currentMouseState, sizeof(m_currentMouseState));

	ZeroMemory(m_lastKeyboardState, sizeof(m_lastKeyboardState));
	ZeroMemory(&m_currentKeyboardState, sizeof(m_currentKeyboardState));

	return 0;
}

void CInputManager::DetectInput()
{
	m_lastMouseState = m_currentMouseState;
	memcpy(m_lastKeyboardState, m_currentKeyboardState, sizeof(m_currentKeyboardState));

	m_keyboard->Acquire();
	m_mouse->Acquire();

	m_keyboard->GetDeviceState(sizeof(m_currentKeyboardState), m_currentKeyboardState);
	m_mouse->GetDeviceState(sizeof(m_currentMouseState), &m_currentMouseState);

	m_mousePosition = m_mousePosition + GetMouseMovement();
	m_mousePosition = ::Clamp(m_mousePosition, XMFLOAT2(0, 0), XMFLOAT2(WDS.m_WindowWidth, WDS.m_WindowHeigth));
}

bool CInputManager::GetKeyDown(int _key) const
{
	return m_currentKeyboardState[_key] & 0x80
		&& !(m_lastKeyboardState[_key] & 0x80);
}

bool CInputManager::GetKeyUp(int _key) const
{
	return !(m_currentKeyboardState[_key] & 0x80)
		&& m_lastKeyboardState[_key] & 0x80;
}

bool CInputManager::GetKey(int _key) const
{
	return m_currentKeyboardState[_key] & 0x80
		&& m_lastKeyboardState[_key] & 0x80;
}

bool CInputManager::GetMouseButtonDown(int _key) const
{
	return !(m_currentMouseState.rgbButtons[_key] & 0x80)
		&& m_lastMouseState.rgbButtons[_key] & 0x80;
}

bool CInputManager::GetMouseButtonUp(int _key) const
{
	return m_currentMouseState.rgbButtons[_key] & 0x80
		&& !(m_lastMouseState.rgbButtons[_key] & 0x80);
}

bool CInputManager::GetMouseButton(int _key) const
{
	return m_currentMouseState.rgbButtons[_key] & 0x80
		&& m_lastMouseState.rgbButtons[_key] & 0x80;
}

float CInputManager::GetMouseWheel() const
{
	return m_currentMouseState.lZ;
}

XMFLOAT2 CInputManager::GetMouseMovement() const
{
	return XMFLOAT2(m_currentMouseState.lX, m_currentMouseState.lY);
}
