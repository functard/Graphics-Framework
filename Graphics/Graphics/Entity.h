#pragma once
#include "VertexPosColor.h"
#include "Helpers.h"

#include <vector>

class CEntity
{
public:
	CEntity(XMFLOAT3 _pos = XMFLOAT3(0, 0, 0));

	/**
	 * \brief	Buffer and Views will be created.
	 *			Uploads index and vertex buffer to the GPU and creates views for both buffers.
	 * \returns Was uploading and creating successful.
	 */
	virtual bool Init();
	/**
	 * \brief					Here updates to the object should happen.
	 *							Default implementation is empty.
	 */
	virtual void Update(float _deltaTime);

	void Move(XMFLOAT3 _dir, float _speed);
	/**
	 * \brief	Renders the object with the simple shader pipeline state.
	 *			Override this, if you want to use a different pipeline state.
	 */
	virtual void Render();

	inline const std::vector<SVertexPosColor>* GetVertices() const { return &m_vertices; }
	inline const std::vector<WORD>* GetIndices() const { return &m_indices; }
	//inline const XMFLOAT3 GetForward() const { return forward; }

	inline const XMFLOAT3 GetCenter() { return m_position; }
	inline void SetPos(XMFLOAT3 _pos) { m_position = _pos; }
	inline void SetRot(XMFLOAT3 _rot) { m_rotation = _rot; }
	inline void AddToRotation(XMFLOAT3 _value) { m_rotation = m_rotation + _value; }
	inline void AddToPos(XMFLOAT3 _value) { m_position = m_position + _value; }
	inline void Scale(XMFLOAT3 _scale) { m_scaling = _scale; }


	XMFLOAT3 MoveTowards(XMFLOAT3 _current, XMFLOAT3 _target, float _maxDist);

	bool inBlackHole;

protected:
	XMFLOAT3 m_position;
	XMFLOAT3 m_scaling = XMFLOAT3(1, 1, 1);
	XMFLOAT3 m_rotation;
	XMFLOAT3 forward;
	XMFLOAT3 right;
	XMFLOAT3 up;

	std::vector<SVertexPosColor> m_vertices;
	std::vector<WORD> m_indices;

	ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	ComPtr<ID3D12Resource> m_intermediateVertexBuffer;

	ComPtr<ID3D12Resource> m_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
	ComPtr<ID3D12Resource> m_intermediateIndexBuffer;
};

