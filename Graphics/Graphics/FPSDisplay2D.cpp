#include "PCH.h"
#include "FPSDisplay2D.h"

void FPSDisplay2D::Update(float _deltaTime)
{
	m_text = std::to_string(1.0f / _deltaTime);
}

FPSDisplay2D::FPSDisplay2D(XMFLOAT2 _pos)
	: Text2D("", _pos)
{
}
