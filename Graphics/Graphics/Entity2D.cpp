#include "PCH.h"
#include "Entity2D.h"

CEntity2D::CEntity2D(XMFLOAT2 _pos)
{
	m_position = _pos;
}

bool CEntity2D::Init()
{
	return true;
}

void CEntity2D::Update(float _deltaTime)
{
}
