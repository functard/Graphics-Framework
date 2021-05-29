#include "PCH.h"
#include "Cube.h"

Cube::Cube(XMFLOAT3 _pos) : CEntity(_pos)
{
	XMFLOAT4 color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);


	/*
		2 -- 3   3 -- 2
		|    |   |    |
		0 -- 1   1 -- 0
	*/

	// vorne
	m_vertices.push_back({ XMFLOAT3(-0.5f, -0.5f, -0.5f), color, XMFLOAT3(0, 0, -1) });
	m_vertices.push_back({ XMFLOAT3( 0.5f, -0.5f, -0.5f), color, XMFLOAT3(0, 0, -1) });
	m_vertices.push_back({ XMFLOAT3(-0.5f,  0.5f, -0.5f), color, XMFLOAT3(0, 0, -1) });
	m_vertices.push_back({ XMFLOAT3( 0.5f,  0.5f, -0.5f), color, XMFLOAT3(0, 0, -1) });

	// rechts
	m_vertices.push_back({ XMFLOAT3( 0.5f, -0.5f, -0.5f), color, XMFLOAT3(1, 0, 0) });
	m_vertices.push_back({ XMFLOAT3( 0.5f, -0.5f,  0.5f), color, XMFLOAT3(1, 0, 0) });
	m_vertices.push_back({ XMFLOAT3( 0.5f,  0.5f, -0.5f), color, XMFLOAT3(1, 0, 0) });
	m_vertices.push_back({ XMFLOAT3( 0.5f,  0.5f,  0.5f), color, XMFLOAT3(1, 0, 0) });

	// hinten
	m_vertices.push_back({ XMFLOAT3( 0.5f, -0.5f,  0.5f), color, XMFLOAT3(0, 0, 1) });
	m_vertices.push_back({ XMFLOAT3(-0.5f, -0.5f,  0.5f), color, XMFLOAT3(0, 0, 1) });
	m_vertices.push_back({ XMFLOAT3( 0.5f,  0.5f,  0.5f), color, XMFLOAT3(0, 0, 1) });
	m_vertices.push_back({ XMFLOAT3(-0.5f,  0.5f,  0.5f), color, XMFLOAT3(0, 0, 1) });

	// links
	m_vertices.push_back({ XMFLOAT3(-0.5f, -0.5f,  0.5f), color, XMFLOAT3(-1, 0, 0) });
	m_vertices.push_back({ XMFLOAT3(-0.5f, -0.5f, -0.5f), color, XMFLOAT3(-1, 0, 0) });
	m_vertices.push_back({ XMFLOAT3(-0.5f,  0.5f,  0.5f), color, XMFLOAT3(-1, 0, 0) });
	m_vertices.push_back({ XMFLOAT3(-0.5f,  0.5f, -0.5f), color, XMFLOAT3(-1, 0, 0) });

	// oben
	m_vertices.push_back({ XMFLOAT3(-0.5f, 0.5f, -0.5f), color, XMFLOAT3(0, 1, 0) });
	m_vertices.push_back({ XMFLOAT3( 0.5f, 0.5f, -0.5f), color, XMFLOAT3(0, 1, 0) });
	m_vertices.push_back({ XMFLOAT3(-0.5f, 0.5f,  0.5f), color, XMFLOAT3(0, 1, 0) });
	m_vertices.push_back({ XMFLOAT3( 0.5f, 0.5f,  0.5f), color, XMFLOAT3(0, 1, 0) });

	// unten
	m_vertices.push_back({ XMFLOAT3( 0.5f, -0.5f, -0.5f), color, XMFLOAT3(0, -1, 0) });
	m_vertices.push_back({ XMFLOAT3(-0.5f, -0.5f, -0.5f), color, XMFLOAT3(0, -1, 0) });
	m_vertices.push_back({ XMFLOAT3( 0.5f, -0.5f,  0.5f), color, XMFLOAT3(0, -1, 0) });
	m_vertices.push_back({ XMFLOAT3(-0.5f, -0.5f,  0.5f), color, XMFLOAT3(0, -1, 0) });

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

#pragma region ---Old---
	/*
	m_vertices.push_back({ XMFLOAT3(-0.5f, -0.5f, -0.5f), color });
	m_vertices.push_back({ XMFLOAT3(-0.5f, -0.5f,  0.5f), color });
	m_vertices.push_back({ XMFLOAT3( 0.5f, -0.5f, -0.5f), color });
	m_vertices.push_back({ XMFLOAT3( 0.5f, -0.5f,  0.5f), color });

	m_vertices.push_back({ XMFLOAT3(-0.5f,  0.5f, -0.5f), color });
	m_vertices.push_back({ XMFLOAT3(-0.5f,  0.5f,  0.5f), color });
	m_vertices.push_back({ XMFLOAT3( 0.5f,  0.5f, -0.5f), color });
	m_vertices.push_back({ XMFLOAT3( 0.5f,  0.5f,  0.5f), color });

	/*
	  5 -- 7
	 /|   /|
	4 -- 6 |
	| 1 -| 3
	|/	 |/
	0 -- 2 
	*/
	/*
	// vorne
	m_indices.push_back(0);
	m_indices.push_back(4);
	m_indices.push_back(2);

	m_indices.push_back(4);
	m_indices.push_back(6);
	m_indices.push_back(2);

	// rechts
	m_indices.push_back(2);
	m_indices.push_back(6);
	m_indices.push_back(3);

	m_indices.push_back(6);
	m_indices.push_back(7);
	m_indices.push_back(3);

	// hinten
	m_indices.push_back(7);
	m_indices.push_back(5);
	m_indices.push_back(3);

	m_indices.push_back(5);
	m_indices.push_back(1);
	m_indices.push_back(3);

	// links
	m_indices.push_back(1);
	m_indices.push_back(5);
	m_indices.push_back(4);

	m_indices.push_back(4);
	m_indices.push_back(0);
	m_indices.push_back(1);

	// oben
	m_indices.push_back(4);
	m_indices.push_back(5);
	m_indices.push_back(6);

	m_indices.push_back(7);
	m_indices.push_back(6);
	m_indices.push_back(5);

	// unten
	m_indices.push_back(0);
	m_indices.push_back(3);
	m_indices.push_back(1);

	m_indices.push_back(0);
	m_indices.push_back(2);
	m_indices.push_back(3);
	*/
#pragma endregion
}

void Cube::Update(float _deltaTime)
{
	//m_rotation.x += 10 * 0.016f;
}
