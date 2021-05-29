#include "PCH.h"
#include "Meteor.h"
#include "Helpers.h"

Meteor::Meteor(int _resolution, float _radius, XMFLOAT3 _pos)
	:CEntity(_pos)
{
	m_radius = _radius;
	if (_resolution < 3)
	{
		_resolution = 3;
	}

	if (_radius <= 0)
	{
		_radius = 1.0f;
	}


	XMFLOAT4 color = XMFLOAT4(1, 0, 1, 0);
	XMFLOAT3 normal = XMFLOAT3(0, 0, -1);

	m_vertices.push_back({ XMFLOAT3(0, 0, 0), color, normal });

	float radiansPerSlice = (2 * XM_PI) / _resolution;

	float x;
	float y;

	for (int i = 0; i < _resolution; i++)
	{
		x = ::sinf(radiansPerSlice * i);
		y = ::cosf(radiansPerSlice * i);

		m_vertices.push_back({ XMFLOAT3(x * _radius, y * _radius, 0), color, normal });
	}

	for (int i = 0; i < _resolution - 1; i++)
	{
		m_indices.push_back(0);
		m_indices.push_back(1 + i);
		m_indices.push_back(2 + i);
	}

	m_indices.push_back(0);
	m_indices.push_back(_resolution);
	m_indices.push_back(1);
}

void Meteor::Update(float _deltaTime)
{
	m_position.y -= 6 * _deltaTime;
}
