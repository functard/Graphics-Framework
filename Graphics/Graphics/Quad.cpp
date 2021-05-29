#include "PCH.h"
#include "Quad.h"


Quad::Quad(std::wstring _path, EResourceType _type, float _heigth, float _width, XMFLOAT3 _pos, XMFLOAT3 _rotation)
	:CTexturedEntity(_path,_type,_pos)
{
	m_rotation = _rotation;

	m_vertices.push_back({ XMFLOAT3(-_width, _heigth, 0.0f), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(0, 0, -1), XMFLOAT2(-2, -2) });
	m_vertices.push_back({ XMFLOAT3(_width, _heigth, 0.0f), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(0, 0, -1), XMFLOAT2(2, -2) });
	m_vertices.push_back({ XMFLOAT3(_width,-_heigth, 0.0f), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(0, 0, -1), XMFLOAT2(2, 2) });
	m_vertices.push_back({ XMFLOAT3(-_width,-_heigth, 0.0f), XMFLOAT4(1, 1, 1, 1), XMFLOAT3(0, 0, -1), XMFLOAT2(-2, 2) });

	m_indices.push_back(0);
	m_indices.push_back(1);
	m_indices.push_back(2);

	m_indices.push_back(0);
	m_indices.push_back(2);
	m_indices.push_back(3);
}

void Quad::Update(float _deltaTime)
{
}
