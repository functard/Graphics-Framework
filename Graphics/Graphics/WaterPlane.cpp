#include "PCH.h"
#include "WaterPlane.h"

WaterPlane::WaterPlane(int _resolution, XMFLOAT3 _scale, XMFLOAT3 _pos)
	: CTexturedEntity(L"", EResourceType::MAX, _pos)
{
	m_scaling = _scale;
	m_path = L"Textures/TestTexture.png";
	m_type = EResourceType::QUAD_TEXTURE;

	if (_resolution < 1)
	{
		_resolution = 1;
	}

	XMFLOAT4 color = XMFLOAT4(1, 1, 1, 1);
	XMFLOAT3 normal = XMFLOAT3(0, 1, 0);
	std::vector<int> rowStarts;
	float sizePerResolution = 1.0f / _resolution;

	for (int z = 0; z < _resolution + 1; z++)
	{
		rowStarts.push_back(m_vertices.size());
		for (int x = 0; x < _resolution + 1; x++)
		{
			m_vertices.push_back({ XMFLOAT3(-0.5f + sizePerResolution * x, 0, 0.5f - sizePerResolution * z),
				color, normal, XMFLOAT2(sizePerResolution * x, sizePerResolution * z) });
		}
	}

	for (int row = 0; row < _resolution; row++)
	{
		for (int column = 0; column < _resolution; column++)
		{
			m_indices.push_back(rowStarts[row] + column);
			m_indices.push_back(rowStarts[row] + column + 1);
			m_indices.push_back(rowStarts[row + 1] + column);

			m_indices.push_back(rowStarts[row + 1] + column);
			m_indices.push_back(rowStarts[row] + column + 1);
			m_indices.push_back(rowStarts[row + 1] + column + 1);
		}
	}
}

bool WaterPlane::Init()
{
	if (CEntity::Init())
	{
		ResourceUploadBatch resourceUploadBatch(DXS.m_Device.Get());
		resourceUploadBatch.Begin();

		int value = ::CreateWICTextureFromFile(DXS.m_Device.Get(), resourceUploadBatch, L"Textures/T_Water_H.jpg",
			&m_heightMap);
		if (FAILED(value))
		{
			return false;
		}

		CD3DX12_CPU_DESCRIPTOR_HANDLE handle =
		{
			DXS.m_ResourceDescriptors->GetCPUDescriptorHandleForHeapStart(),
			(int)EResourceType::WATER_HEIGTH,
			DXS.m_CBVDescriptorSize
		};

		::CreateShaderResourceView(DXS.m_Device.Get(), m_heightMap.Get(), handle);

		value = ::CreateWICTextureFromFile(DXS.m_Device.Get(), resourceUploadBatch, L"Textures/T_Water_H.jpg",
			&m_normalMap);
		if (FAILED(value))
		{
			return false;
		}

		handle =
		{
			DXS.m_ResourceDescriptors->GetCPUDescriptorHandleForHeapStart(),
			(int)EResourceType::WATER_NORMAL,
			DXS.m_CBVDescriptorSize
		};

		::CreateShaderResourceView(DXS.m_Device.Get(), m_normalMap.Get(), handle);

		::TransitionResource(DXS.m_DirectCommandList.Get(), m_heightMap.Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		auto uploadFinish = resourceUploadBatch.End(DXS.m_DirectCommandQueue.Get());
		uploadFinish.wait();

		return true;
	}
	return false;
}

void WaterPlane::Update(float _deltaTime)
{
	DXS.m_WaterConstantBufferData.m_Time += _deltaTime;
}

void WaterPlane::Render()
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

	memcpy(DXS.m_WaterConstantBufferDataBegin, &DXS.m_WaterConstantBufferData, sizeof(DXS.m_WaterConstantBufferData));

	DXS.m_DirectCommandList->SetPipelineState(DXS.m_WaterShaderPipelineState.Get());
	DXS.m_DirectCommandList->SetGraphicsRootSignature(DXS.m_WaterShaderRootSignature.Get());
	DXS.m_DirectCommandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvpMatrix, 0);
	DXS.m_DirectCommandList->SetGraphicsRoot32BitConstants(1, sizeof(XMMATRIX) / 4, &modelMatrix, 0);

	CD3DX12_GPU_DESCRIPTOR_HANDLE handle =
	{
		DXS.m_ResourceDescriptors->GetGPUDescriptorHandleForHeapStart(),
		(int)EResourceType::WATER_HEIGTH,
		DXS.m_CBVDescriptorSize
	};
	DXS.m_DirectCommandList->SetGraphicsRootDescriptorTable(2, handle);

	handle =
	{
		DXS.m_ResourceDescriptors->GetGPUDescriptorHandleForHeapStart(),
		(int)EResourceType::WATER_NORMAL,
		DXS.m_CBVDescriptorSize
	};
	DXS.m_DirectCommandList->SetGraphicsRootDescriptorTable(3, handle);

	handle =
	{
		DXS.m_ResourceDescriptors->GetGPUDescriptorHandleForHeapStart(),
		(int)EResourceType::WATER_CONSTANT_BUFFER,
		DXS.m_CBVDescriptorSize
	};
	DXS.m_DirectCommandList->SetGraphicsRootDescriptorTable(4, handle);

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
