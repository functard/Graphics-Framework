#pragma once
class CEntity2D
{
public:
	CEntity2D(XMFLOAT2 _pos = XMFLOAT2(0, 0));

public:
	virtual bool Init();
	virtual void Update(float _deltaTime);
	virtual void Render() = 0;

protected:
	XMFLOAT2 m_position;
	XMUINT2 m_size;
	float m_rotation = 0;
	float m_scale = 1;
};

