#include "PCH.h"
#include "Triangle.h"

Triangle::Triangle(XMFLOAT3 _pos) : CEntity(_pos)
{
	m_vertices.push_back({ XMFLOAT3(0, 1.0f, 0), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(0, 0, -1)});
	m_vertices.push_back({ XMFLOAT3(1.0f, 0, 0), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT3(0, 0, -1)});
	m_vertices.push_back({ XMFLOAT3(-1.0f, 0, 0), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT3(0, 0, -1)});

	m_indices.push_back(0);
	m_indices.push_back(1);
	m_indices.push_back(2);
}

void Triangle::Update(float _deltaTime)
{
	m_rotation.x += 2;
}
