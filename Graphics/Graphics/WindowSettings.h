#pragma once

struct SWindowSettings
{
	LPCWSTR m_WindowName = L"Graphics System";
	int m_WindowWidth = 1280;
	int m_WindowHeigth = 720;
	LPCWSTR m_WindowClassName = L"Graphics Class";
	HWND m_WindowHandle;
	RECT m_WindowRect;
};