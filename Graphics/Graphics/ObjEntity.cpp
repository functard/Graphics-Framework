#include "PCH.h"
#include "ObjEntity.h"

ObjEntity::ObjEntity(std::vector<SVertexPosColor>* _vertices, std::vector<WORD>* _indices,
	std::wstring _path, EResourceType _type, XMFLOAT3 _pos)
	: CTexturedEntity(_path, _type, _pos)
{
	for (SVertexPosColor vertex : *_vertices)
	{
		m_vertices.push_back(vertex);
	}
	for (WORD index : *_indices)
	{
		m_indices.push_back(index);
	}
}
