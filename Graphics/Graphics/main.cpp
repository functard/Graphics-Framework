#include "PCH.h"
#include "Engine.h"

int CALLBACK wWinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, PWSTR _cmdLineArgs, int _cmdShow)
{
	int value = CEngine::Get()->Init(_hInstance);
	if (FAILED(value))
	{
		return value;
	}
	value = CEngine::Get()->Run();
	CEngine::Get()->Finish();

	return value;
}