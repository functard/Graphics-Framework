#include "PCH.h"
#include "ContentManager.h"
#include "Entity.h"
#include "Entity2D.h"

void CContentManager::AddEntity(CEntity* _entity)
{
	if (nullptr == _entity)
		return;

	if (_entity->Init())
	{
		m_entities.push_back(_entity);
	}
	else
	{
		delete _entity;
	}
}

void CContentManager::AddEntity(CEntity2D* _entity)
{
	if (nullptr == _entity)
		return;

	if (_entity->Init())
	{
		m_entities2D.push_back(_entity);
	}
	else
	{
		delete _entity;
	}
}

void CContentManager::Update(float _deltaTime)
{
	for (CEntity* entity : m_entities)
	{
		entity->Update(_deltaTime);
	}

	for (CEntity2D* entity : m_entities2D)
	{
		entity->Update(_deltaTime);
	}
}

void CContentManager::Render()
{
	for (CEntity* entity : m_entities)
	{
		entity->Render();
	}

	for (CEntity2D* entity : m_entities2D)
	{
		entity->Render();
	}
}
