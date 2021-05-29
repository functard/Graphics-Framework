#include "PCH.h"
#include "Entity.h"
#include "Helpers.h"
#include "Engine.h"

CEntity::CEntity(XMFLOAT3 _pos)
{
	m_position = _pos;
}

bool CEntity::Init()
{
	CHelpers::UploadBufferResource(DXS.m_DirectCommandList, &m_vertexBuffer, &m_intermediateVertexBuffer,
		m_vertices.size(), sizeof(SVertexPosColor), m_vertices.data());

	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = m_vertices.size() * sizeof(SVertexPosColor);
	m_vertexBufferView.StrideInBytes = sizeof(SVertexPosColor);

	CHelpers::UploadBufferResource(DXS.m_DirectCommandList, &m_indexBuffer, &m_intermediateIndexBuffer,
		m_indices.size(), sizeof(WORD), m_indices.data());

	m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.SizeInBytes = m_indices.size() * sizeof(WORD);
	m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;

	return true;
}

void CEntity::Update(float _deltaTime)
{
	if (IPM.GetKey(DIK_SPACE))
	{
		m_position = m_position + (right * _deltaTime * 24);
	}

	if (m_rotation.z >= 360)
		m_rotation.z -= 360;

	if (!inBlackHole)
		m_position = m_position - (right * _deltaTime * 4.5);

	if (IPM.GetKey(DIK_A))
		m_rotation.z += _deltaTime * 40;

	if (IPM.GetKey(DIK_D))
		m_rotation.z -= _deltaTime * 40;


	XMVECTOR x = { 1, 0, 0, 0 };
	XMVECTOR y = { 0, 1, 0, 0 };
	XMVECTOR z = { 0, 0, 1, 0 };

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw
	(
		XMConvertToRadians(m_rotation.x),
		XMConvertToRadians(m_rotation.y),
		XMConvertToRadians(m_rotation.z)
	);

	x = XMVector3Transform(x, rotationMatrix);
	y = XMVector3Transform(y, rotationMatrix);
	z = XMVector3Transform(z, rotationMatrix);

	forward = XMFLOAT3(z.m128_f32[0], z.m128_f32[1], z.m128_f32[2]);
	right = XMFLOAT3(x.m128_f32[0], x.m128_f32[1], x.m128_f32[2]);
	up = XMFLOAT3(y.m128_f32[0], y.m128_f32[1], y.m128_f32[2]);
}



void CEntity::Render()
{
	XMMATRIX modelMatrix;
	XMMATRIX scale = XMMatrixScaling(m_scaling.x, m_scaling.y, m_scaling.z);
	XMMATRIX rotation = XMMatrixRotationRollPitchYaw
	(
		XMConvertToRadians(m_rotation.x),
		XMConvertToRadians(m_rotation.y),
		XMConvertToRadians(m_rotation.z)
	);
	XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	modelMatrix = scale * rotation * translation;
	XMMATRIX view = DXS.m_ViewMatrix;
	XMMATRIX proj = DXS.m_ProjectionMatrix;
	XMMATRIX mvpMatrix = XMMatrixMultiply(modelMatrix, view);
	mvpMatrix = XMMatrixMultiply(mvpMatrix, proj);

	DXS.m_DirectCommandList->SetPipelineState(DXS.m_SimpleShaderPipelineState.Get());
	DXS.m_DirectCommandList->SetGraphicsRootSignature(DXS.m_SimpleShaderRootSignature.Get());
	DXS.m_DirectCommandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvpMatrix, 0);
	DXS.m_DirectCommandList->SetGraphicsRoot32BitConstants(1, sizeof(XMMATRIX) / 4, &modelMatrix, 0);

	CD3DX12_GPU_DESCRIPTOR_HANDLE handle =
	{
		DXS.m_ResourceDescriptors->GetGPUDescriptorHandleForHeapStart(),
		(int)EResourceType::LIGHT_CONSTANT_BUFFER,
		DXS.m_DescriptorSize
	};
	DXS.m_DirectCommandList->SetGraphicsRootDescriptorTable(2, handle);

	DXS.m_DirectCommandList->IASetIndexBuffer(&m_indexBufferView);
	DXS.m_DirectCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	DXS.m_DirectCommandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DXS.m_DirectCommandList->RSSetScissorRects(1, &DXS.m_ScissorRect);
	DXS.m_DirectCommandList->RSSetViewports(1, &DXS.m_Viewport);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv
	{
		DXS.m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		DXS.m_BackbufferIndex,
		DXS.m_CBVDescriptorSize
	};

	DXS.m_DirectCommandList->OMSetRenderTargets(1, &rtv, false, &DXS.m_StencilHeap->GetCPUDescriptorHandleForHeapStart());

	DXS.m_DirectCommandList->DrawIndexedInstanced(m_indices.size(), 1, 0, 0, 0);
}

XMFLOAT3 CEntity::MoveTowards(XMFLOAT3 _current, XMFLOAT3 _target, float _maxDist)
{
	XMFLOAT3 a = _target - _current;
	float magnitude = Magnitude(a);
	if (magnitude <= _maxDist || magnitude == 0)
	{
		return _target;
	}
	return _current + a / magnitude * _maxDist;
}
