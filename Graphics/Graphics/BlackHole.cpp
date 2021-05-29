#include "PCH.h"
#include "Helpers.h"
#include "Engine.h"
#include "BlackHole.h"

BlackHole::BlackHole(int _resolution, float _radius, XMFLOAT3 _pos)
	:CEntity(_pos)
{
	m_radius = _radius;
	if (_resolution < 3)
	{
		_resolution = 3;
	}

	if (_radius <= 0)
	{
		_radius = 1.0f;
	}

	XMFLOAT4 color = XMFLOAT4(1, 0, 1, 0);
	XMFLOAT3 normal = XMFLOAT3(0, 0, -1);

	m_vertices.push_back({ XMFLOAT3(0, 0, 0), color, normal });

	float radiansPerSlice = (2 * XM_PI) / _resolution;

	float x;
	float y;

	for (int i = 0; i < _resolution; i++)
	{
		x = ::sinf(radiansPerSlice * i);
		y = ::cosf(radiansPerSlice * i);

		m_vertices.push_back({ XMFLOAT3(x * _radius, y * _radius, 0), color, normal });
	}

	for (int i = 0; i < _resolution - 1; i++)
	{
		m_indices.push_back(0);
		m_indices.push_back(1 + i);
		m_indices.push_back(2 + i);
	}

	m_indices.push_back(0);
	m_indices.push_back(_resolution);
	m_indices.push_back(1);
}
void BlackHole::Update(float _deltaTime)
{
	m_rotation.z += 5 * _deltaTime;
}

void BlackHole::Render()
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

	DXS.m_DirectCommandList->SetPipelineState(DXS.m_CellShadingPipelineState.Get());
	DXS.m_DirectCommandList->SetGraphicsRootSignature(DXS.m_CellShadingRootSignature.Get());
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

	DXS.m_DirectCommandList->RSSetScissorRects(2, &DXS.m_ScissorRect);
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
