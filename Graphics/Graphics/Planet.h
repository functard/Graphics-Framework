#pragma once
#include "Entity.h"
class Planet :
	public CEntity
{
public:
	Planet(int _resolution, float _radius, XMFLOAT3 _pos = XMFLOAT3(0,0,0));

	virtual void Update(float _deltaTime) override;

	inline const XMFLOAT3 GetCenter() { return m_position; }
	inline const float GetRadius() { return m_radius; }
private:
	float m_radius;
};

