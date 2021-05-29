#pragma once

#include <vector>

class CEntity;
class CEntity2D;

class CContentManager
{
public:
	void AddEntity(CEntity* _entity);
	void AddEntity(CEntity2D* _entity);

	void Update(float _deltaTime);
	void Render();

private:
	std::vector<CEntity*> m_entities;
	std::vector<CEntity2D*> m_entities2D;
};

