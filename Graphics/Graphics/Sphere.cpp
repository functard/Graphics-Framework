#include "PCH.h"
#include "Sphere.h"
#include "Helpers.h"
#include "Engine.h"

Sphere::Sphere(int _resolution, float _radius, XMFLOAT3 _pos)
	: CEntity(_pos)
{
	m_radius = _radius;
	if (_resolution < 3)
	{
		_resolution = 3;
	}
	if (_radius <= 0)
	{
		_radius = 1;
	}

	float anglePerFrontSlice = XM_PI / _resolution;
	float anglePerTopSlice = (2 * XM_PI) / _resolution;

	XMFLOAT4 color = XMFLOAT4(1, 0, 0, 1);

	m_vertices.push_back({ XMFLOAT3(0, _radius, 0), color, XMFLOAT3(0, 1, 0)});
	std::vector<int> startIndices;

	float ringRadius;
	float y;
	float x;
	float z;
	for (int ringIndex = 1; ringIndex < _resolution; ringIndex++)
	{
		startIndices.push_back(m_vertices.size());
		ringRadius = ::sinf(ringIndex * anglePerFrontSlice);
		y = ::cosf(ringIndex * anglePerFrontSlice);

		color = XMFLOAT4(ringRadius, y, 0, 1);

		for (int i = 0; i < _resolution; i++)
		{
			x = ::sinf(i * anglePerTopSlice);
			z = ::cosf(i * anglePerTopSlice);

			m_vertices.push_back({ XMFLOAT3(x * ringRadius, y, z * ringRadius) * _radius, color, XMFLOAT3(x * ringRadius, y, z * ringRadius) * _radius });
		}
	}

	color = XMFLOAT4(0, 0, 1, 1);
	m_vertices.push_back({ XMFLOAT3(0, -_radius, 0), color, XMFLOAT3(0, -1, 0)});

	// TOP
	for (int i = 0; i < _resolution - 1; i++)
	{
		m_indices.push_back(0);
		m_indices.push_back(1 + i);
		m_indices.push_back(2 + i);
	}

	m_indices.push_back(0);
	m_indices.push_back(startIndices[1] - 1);
	m_indices.push_back(1);

	// BOTTOM
	for (int i = 0; i < _resolution - 1; i++)
	{
		m_indices.push_back(startIndices[startIndices.size() - 1] + i);
		m_indices.push_back(m_vertices.size() - 1);
		m_indices.push_back(startIndices[startIndices.size() - 1] + 1 + i);
	}

	m_indices.push_back(startIndices[startIndices.size() - 1]);
	m_indices.push_back(m_vertices.size() - 2);
	m_indices.push_back(m_vertices.size() - 1);

	// SIDES
	for (int ring = 0; ring < _resolution - 2; ring++)
	{
		for (int i = 0; i < _resolution - 1; i++)
		{
			m_indices.push_back(i + startIndices[0 + ring]);
			m_indices.push_back(i + startIndices[1 + ring]);
			m_indices.push_back(i + startIndices[1 + ring] + 1);

			m_indices.push_back(i + startIndices[0 + ring]);
			m_indices.push_back(i + startIndices[1 + ring] + 1);
			m_indices.push_back(i + startIndices[0 + ring] + 1);
		}

		m_indices.push_back(startIndices[0 + ring] + _resolution - 1);
		m_indices.push_back(startIndices[1 + ring] + _resolution - 1);
		m_indices.push_back(startIndices[1 + ring]);

		m_indices.push_back(startIndices[0 + ring] + _resolution - 1);
		m_indices.push_back(startIndices[1 + ring]);
		m_indices.push_back(startIndices[0 + ring]);
	}
}

void Sphere::Update(float _deltaTime)
{
}

