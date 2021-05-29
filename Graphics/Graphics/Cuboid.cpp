#include "PCH.h"
#include "Cuboid.h"

Cuboid::Cuboid(std::wstring _path, EResourceType _type, float _width, float _heigth, float _depth, XMFLOAT3 _pos)
	:CTexturedEntity(_path,_type, _pos)
{
	/*
		2 -- 3   3 -- 2
		|    |   |    |
		0 -- 1   1 -- 0
	*/

	XMFLOAT4 color = XMFLOAT4(1, 0, 1, 0);

	// vorne
	m_vertices.push_back({ XMFLOAT3(-_width / 2, -_heigth / 2, -_depth / 2), color, XMFLOAT3(0, 0, -1), XMFLOAT2(0.5f,  0.66f) });
	m_vertices.push_back({ XMFLOAT3(_width / 2, -_heigth / 2, -_depth / 2), color, XMFLOAT3(0, 0, -1), XMFLOAT2(0.75f, 0.66f) });
	m_vertices.push_back({ XMFLOAT3(-_width / 2,  _heigth / 2, -_depth / 2), color, XMFLOAT3(0, 0, -1), XMFLOAT2(0.5f,  0.33f) });
	m_vertices.push_back({ XMFLOAT3(_width / 2,  _heigth / 2, -_depth / 2), color, XMFLOAT3(0, 0, -1), XMFLOAT2(0.75f, 0.33f) });

	// rechts						 
	m_vertices.push_back({ XMFLOAT3(_width / 2, -_heigth / 2, -_depth / 2), color, XMFLOAT3(1,  0, 0), XMFLOAT2(0.75f, 0.66f) });
	m_vertices.push_back({ XMFLOAT3(_width / 2, -_heigth / 2,  _depth / 2), color, XMFLOAT3(1,  0, 0), XMFLOAT2(1.0f,  0.66f) });
	m_vertices.push_back({ XMFLOAT3(_width / 2,  _heigth / 2, -_depth / 2), color, XMFLOAT3(1,  0, 0), XMFLOAT2(0.75f, 0.33f) });
	m_vertices.push_back({ XMFLOAT3(_width / 2,  _heigth / 2,  _depth / 2), color, XMFLOAT3(1,  0, 0), XMFLOAT2(1.0f,  0.33f) });

	// hinten						 															 
	m_vertices.push_back({ XMFLOAT3(_width / 2, -_heigth / 2,  _depth / 2), color, XMFLOAT3(0,  0, 1), XMFLOAT2(0.0f,  0.66f) });
	m_vertices.push_back({ XMFLOAT3(-_width / 2, -_heigth / 2,  _depth / 2), color, XMFLOAT3(0,  0, 1), XMFLOAT2(0.25f, 0.66f) });
	m_vertices.push_back({ XMFLOAT3(_width / 2,  _heigth / 2,  _depth / 2), color, XMFLOAT3(0,  0, 1), XMFLOAT2(0.0f,  0.33f) });
	m_vertices.push_back({ XMFLOAT3(-_width / 2,  _heigth / 2,  _depth / 2), color, XMFLOAT3(0,  0, 1), XMFLOAT2(0.25f, 0.33f) });

	// links						 
	m_vertices.push_back({ XMFLOAT3(-_width / 2, -_heigth / 2,  _depth / 2), color, XMFLOAT3(-1, 0, 0), XMFLOAT2(0.25f, 0.66f) });
	m_vertices.push_back({ XMFLOAT3(-_width / 2, -_heigth / 2, -_depth / 2), color, XMFLOAT3(-1, 0, 0), XMFLOAT2(0.5f,  0.66f) });
	m_vertices.push_back({ XMFLOAT3(-_width / 2,  _heigth / 2,  _depth / 2), color, XMFLOAT3(-1, 0, 0), XMFLOAT2(0.25f, 0.33f) });
	m_vertices.push_back({ XMFLOAT3(-_width / 2,  _heigth / 2, -_depth / 2), color, XMFLOAT3(-1, 0, 0), XMFLOAT2(0.5f,  0.33f) });

	// oben							 
	m_vertices.push_back({ XMFLOAT3(-_width / 2,  _heigth / 2, -_depth / 2), color, XMFLOAT3(0,  1, 0), XMFLOAT2(0.5f,  0.33f) });
	m_vertices.push_back({ XMFLOAT3(_width / 2,  _heigth / 2, -_depth / 2), color, XMFLOAT3(0,  1, 0) ,XMFLOAT2(0.75f, 0.33f) });
	m_vertices.push_back({ XMFLOAT3(-_width / 2,  _heigth / 2,  _depth / 2), color, XMFLOAT3(0,  1, 0), XMFLOAT2(0.5f,  0.0f) });
	m_vertices.push_back({ XMFLOAT3(_width / 2,  _heigth / 2,  _depth / 2), color, XMFLOAT3(0,  1, 0) ,XMFLOAT2(0.75f, 0.0f) });

	// unten						 
	m_vertices.push_back({ XMFLOAT3(_width / 2, -_heigth / 2, -_depth / 2), color, XMFLOAT3(0, -1, 0) ,XMFLOAT2(0.5f,  1.0f) });
	m_vertices.push_back({ XMFLOAT3(-_width / 2, -_heigth / 2, -_depth / 2), color, XMFLOAT3(0, -1, 0), XMFLOAT2(0.75f, 1.0f) });
	m_vertices.push_back({ XMFLOAT3(_width / 2, -_heigth / 2,  _depth / 2), color, XMFLOAT3(0, -1, 0) ,XMFLOAT2(0.5f,  0.66f) });
	m_vertices.push_back({ XMFLOAT3(-_width / 2, -_heigth / 2,  _depth / 2), color, XMFLOAT3(0, -1, 0), XMFLOAT2(0.75f, 0.66f) });

	/*
		6 -- 7
		|    |
		4 -- 5
	*/

	for (int i = 0; i < 6; i++)
	{
		m_indices.push_back(0 + i * 4);
		m_indices.push_back(2 + i * 4);
		m_indices.push_back(3 + i * 4);

		m_indices.push_back(3 + i * 4);
		m_indices.push_back(1 + i * 4);
		m_indices.push_back(0 + i * 4);
	}
}

void Cuboid::Update(float _deltaTime)
{
}
