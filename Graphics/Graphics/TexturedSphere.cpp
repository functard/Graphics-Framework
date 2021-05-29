#include "PCH.h"
#include "Engine.h"
#include "Helpers.h"
#include "TexturedSphere.h"

TexturedSphere::TexturedSphere(std::wstring _path, EResourceType _type, int _resolution, float _radius, XMFLOAT3 _pos)
	:CTexturedEntity(_path, _type, _pos)
{
	m_radius = _radius;
	if (_resolution < 3)
	{
		_resolution = 3;
	}
	if (_radius <= 0)
	{
		_radius = 1;
	}

	float anglePerFrontSlice = XM_PI / _resolution;
	float anglePerTopSlice = (2 * XM_PI) / _resolution;

	XMFLOAT4 color = XMFLOAT4(1, 0, 0, 1);

	m_vertices.push_back({ XMFLOAT3(0, _radius, 0), color, XMFLOAT3(0, 1, 0) });
	std::vector<int> startIndices;

	float ringRadius;
	float y;
	float x;
	float z;
	for (int ringIndex = 1; ringIndex < _resolution; ringIndex++)
	{
		startIndices.push_back(m_vertices.size());
		ringRadius = ::sinf(ringIndex * anglePerFrontSlice);
		y = ::cosf(ringIndex * anglePerFrontSlice);

		color = XMFLOAT4(ringRadius, y, 0, 1);

		for (int i = 0; i < _resolution; i++)
		{
			x = ::sinf(i * anglePerTopSlice);
			z = ::cosf(i * anglePerTopSlice);

			m_vertices.push_back({ XMFLOAT3(x * ringRadius, y, z * ringRadius) * _radius, color, XMFLOAT3(x * ringRadius, y, z * ringRadius) * _radius });
		}
	}

	color = XMFLOAT4(0, 0, 1, 1);
	m_vertices.push_back({ XMFLOAT3(0, -_radius, 0), color, XMFLOAT3(0, -1, 0) });

	// TOP
	for (int i = 0; i < _resolution - 1; i++)
	{
		m_indices.push_back(0);
		m_indices.push_back(1 + i);
		m_indices.push_back(2 + i);
	}

	m_indices.push_back(0);
	m_indices.push_back(startIndices[1] - 1);
	m_indices.push_back(1);

	// BOTTOM
	for (int i = 0; i < _resolution - 1; i++)
	{
		m_indices.push_back(startIndices[startIndices.size() - 1] + i);
		m_indices.push_back(m_vertices.size() - 1);
		m_indices.push_back(startIndices[startIndices.size() - 1] + 1 + i);
	}

	m_indices.push_back(startIndices[startIndices.size() - 1]);
	m_indices.push_back(m_vertices.size() - 2);
	m_indices.push_back(m_vertices.size() - 1);

	// SIDES
	for (int ring = 0; ring < _resolution - 2; ring++)
	{
		for (int i = 0; i < _resolution - 1; i++)
		{
			m_indices.push_back(i + startIndices[0 + ring]);
			m_indices.push_back(i + startIndices[1 + ring]);
			m_indices.push_back(i + startIndices[1 + ring] + 1);

			m_indices.push_back(i + startIndices[0 + ring]);
			m_indices.push_back(i + startIndices[1 + ring] + 1);
			m_indices.push_back(i + startIndices[0 + ring] + 1);
		}

		m_indices.push_back(startIndices[0 + ring] + _resolution - 1);
		m_indices.push_back(startIndices[1 + ring] + _resolution - 1);
		m_indices.push_back(startIndices[1 + ring]);

		m_indices.push_back(startIndices[0 + ring] + _resolution - 1);
		m_indices.push_back(startIndices[1 + ring]);
		m_indices.push_back(startIndices[0 + ring]);
	}
}

void TexturedSphere::Render()
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

	DXS.m_DirectCommandList->SetPipelineState(DXS.m_TexturedShaderPipelineState.Get());
	DXS.m_DirectCommandList->SetGraphicsRootSignature(DXS.m_TexturedShaderRootSignature.Get());
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
