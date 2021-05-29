#include "PCH.h"
#include "Cursor2D.h"
#include "Engine.h"

Cursor2D::Cursor2D(std::wstring _path, XMFLOAT2 _size)
	: Image2D(_path, EResourceType::CURSOR,_size.x, _size.y)
{
	m_rotation = 180;
}

bool Cursor2D::Init()
{
	if (Image2D::Init())
	{
		m_origin = XMFLOAT2(m_size.x * 0.5f, m_size.y);
		m_scale = 0.1f;
		return true;
	}
	return false;
}

void Cursor2D::Update(float _deltaTime)
{
	m_position = IPM.GetMousePosition();
}
