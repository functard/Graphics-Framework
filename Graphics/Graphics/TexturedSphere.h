#pragma once
#include "TexturedEntity.h"
class TexturedSphere :
	public CTexturedEntity
{
public:
	TexturedSphere(std::wstring _path, EResourceType _type, int _resolution, float _radius, XMFLOAT3 _pos = XMFLOAT3(0, 0, 0));

	void Render() override;

protected:
	float m_radius;
};

