#pragma once
#include "Entity.h"
#include "Circle.h"
class BlackHole :
	public CEntity
{
public:
	BlackHole(int _resolution, float _radius, XMFLOAT3 _pos = XMFLOAT3(0, 0, 0));

    virtual void Update(float _deltaTime) override;
	virtual void Render() override;

 	inline const XMFLOAT3 GetCenter() { return m_position; }
	inline const float GetRadius() { return m_radius; }
	inline void SetPos(XMFLOAT3 _pos) { m_position = _pos; }

private:
	float m_radius;
};

