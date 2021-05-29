#pragma once
#include "Entity.h"
class Meteor :
	public CEntity
{
public:
	Meteor(int _resolution, float _radius, XMFLOAT3 _pos = XMFLOAT3(0, 0, 0));

public:
	virtual void Update(float _deltaTime) override;


	inline float GetRadius() { return m_radius; }
	inline XMFLOAT3 GetCenter() { return m_position; }
	inline void SetPos(XMFLOAT3 _pos) { m_position = _pos; }

protected:
	float m_radius;

	XMFLOAT3 forward;
	XMFLOAT3 right;
	XMFLOAT3 up;
};

