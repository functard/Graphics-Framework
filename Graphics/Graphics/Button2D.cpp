#include "PCH.h"
#include "Button2D.h"
#include "Engine.h"

Button2D::Button2D(std::wstring _path, EResourceType _type, BTNFUNCTION _func,XMFLOAT2 _size, XMFLOAT2 _pos)
	: Image2D(_path, _type, _size.x, _size.y, _pos)
{
	m_function = _func;
}

void Button2D::Update(float _deltaTime)
{
	if (IPM.GetMouseButtonDown(0))
	{
		float minX = m_position.x - m_origin.x;
		float maxX = m_position.x + (m_size.x - m_origin.x);
		float minY = m_position.y - m_origin.y;
		float maxY = m_position.y + (m_size.y - m_origin.y);

		XMFLOAT2 mousePosition = IPM.GetMousePosition();

		if (mousePosition.x >= minX && mousePosition.x <= maxX
			&& mousePosition.y >= minY && mousePosition.y <= maxY)
		{
			m_function(this);
		}
	}
}
