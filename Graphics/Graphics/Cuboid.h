#pragma once
#include "TexturedEntity.h"
class Cuboid :
	public CTexturedEntity
{
public:
	Cuboid(std::wstring _path, EResourceType _type, float _width, float _heigth, float _depth, XMFLOAT3 _pos = XMFLOAT3(0, 0, 0));

	void Update(float _deltaTime) override;
	
};

