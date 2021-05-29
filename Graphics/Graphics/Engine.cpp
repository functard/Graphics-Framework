#include "PCH.h"
#include "ReadData.h"
#include "Engine.h"
#include "WindowSettings.h"
#include "DirectXSettings.h"
#include "ContentManager.h"
#include "Helpers.h"
#include "Triangle.h"
#include "Cube.h"
#include "Camera.h"
#include "Circle.h"
#include "Sphere.h"
#include "Dice.h"
#include "Quad.h"
#include "Meteor.h"
#include "Plane.h"
#include "ObjManager.h"
#include "WaterPlane.h"
#include "Image2D.h"
#include "Cursor2D.h"
#include "Button2D.h"
#include "Text2D.h"
#include "FPSDisplay2D.h"
#include "Collision.h"
#include "BlackHole.h"
#include "TexturedSphere.h"
#include "Cuboid.h"
#include "FastNoise.h"
#include "FastNoise.cpp"

#include <random>
void QuitButton(Button2D* _button)
{
	CEngine::Get()->Shutdown();
}

void RetryButton(Button2D* _button)
{
	CEngine::Get()->Retry(_button);
}

void NextLevelButton(Button2D* _button)
{
	CEngine::Get()->LoadNextLevel(_button);
}



LRESULT CALLBACK WndProc(HWND _hwnd, UINT _message, WPARAM _wParam, LPARAM _lParam)
{
	if (DXS.m_IsInitialized)
	{
		PAINTSTRUCT ps;
		HDC hdc;
		switch (_message)
		{
			case WM_PAINT:
			{
				hdc = BeginPaint(_hwnd, &ps);
				EndPaint(_hwnd, &ps);
				break;
			}
			case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}
		}
	}
	return ::DefWindowProc(_hwnd, _message, _wParam, _lParam);
}

CEngine* CEngine::s_engine = nullptr;

CEngine* CEngine::Get()
{
	//static CEngine* instance = new CEngine();

	if (nullptr == s_engine)
	{
		s_engine = new CEngine();
	}
	return s_engine;
}

CEngine::CEngine()
{
	m_windowSettings = new SWindowSettings();
	m_directXSettings = new SDirectXSettings();
	m_contentManager = new CContentManager();
	m_inputManager = new CInputManager();

	m_directXSettings->m_LightConstantBufferData = SLightConstantBuffer();
	m_directXSettings->m_WaterConstantBufferData = SWaterConstantBuffer();

	m_camera = new CCamera();
}

int CEngine::Init(HINSTANCE _hInstance)
{
	int value;

	if (!XMVerifyCPUSupport())
	{
		MessageBox(nullptr, L"Failed to load DirectXMath library support!", L"Error", MB_OK);
		return -1;
	}

	value = InitApplication(_hInstance);
	if (FAILED(value))
	{
		MessageBox(nullptr, L"Failed to init application!", L"Error", MB_OK);
		return value;
	}

	value = InitDirectX();
	if (FAILED(value))
	{
		MessageBox(nullptr, L"Failed to init DirectX!", L"Error", MB_OK);
		return value;
	}

	value = InitDirectXToolKit();
	if (FAILED(value))
	{
		MessageBox(nullptr, L"Failed to init DirectX ToolKit!", L"Error", MB_OK);
		return value;
	}


	value = m_inputManager->InitDirectInput(_hInstance);
	if (FAILED(value))
	{
		MessageBox(nullptr, L"Failed to init direct input", L"Error", MB_OK);
		return value;
	}

	m_directXSettings->m_Allocators[m_directXSettings->m_BackbufferIndex]->Reset();
	m_directXSettings->m_DirectCommandList
		->Reset(m_directXSettings->m_Allocators[m_directXSettings->m_BackbufferIndex].Get(), nullptr);

	value = CreateSimpleShader();
	if (FAILED(value))
	{
		return value;
	}

	value = CreateCelShadingShader();
	if (FAILED(value))
	{
		return value;
	}

	value = CreateTexturedShader();
	if (FAILED(value))
	{
		return value;
	}

	value = CreateHeightBlendingShader();
	if (FAILED(value))
	{
		return value;
	}

	value = CreateTerrainShader();
	if (FAILED(value))
	{
		return value;
	}

	value = CreateWaterShader();
	if (FAILED(value))
	{
		return value;
	}

	value = CreateLightConstantBuffer();
	if (FAILED(value))
	{
		return value;
	}

	value = CreateWaterConstantBuffer();
	if (FAILED(value))
	{
		return value;
	}

	LoadContent();

	m_directXSettings->m_DirectCommandList->Close();
	ID3D12CommandList* const commandLists[] =
	{
		m_directXSettings->m_DirectCommandList.Get()
	};

	m_directXSettings->m_DirectCommandQueue->ExecuteCommandLists(1, commandLists);
	UINT64 fenceValue = ++m_directXSettings->m_FenceValue;
	value = m_directXSettings->m_DirectCommandQueue->Signal(m_directXSettings->m_Fence.Get(), fenceValue);
	if (FAILED(value))
	{
		throw std::exception("Failed to signal command queue!");
	}
	m_directXSettings->m_FenceValue = fenceValue;
	CHelpers::WaitForFenceValue(m_directXSettings->m_Fence, fenceValue, m_directXSettings->m_FrameEvent);

	/************************************************************************/
	/* Before starting the engine, we have to make sure that everything		*/
	/* is initialized. On the CPU side and and the GPU side as well.		*/
	/************************************************************************/
	::ShowWindow(m_windowSettings->m_WindowHandle, SW_SHOW);
	m_isRunning = true;

	return 0;
}

int CEngine::Run()
{
	MSG msg = { 0 };
	m_previousTime = std::chrono::system_clock::now();
	float maxTimeStep;
	while (msg.message != WM_QUIT && m_isRunning)
	{
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			maxTimeStep = 1.0f / m_targetFrameRate;

			auto currentTime = std::chrono::system_clock::now();

			std::chrono::duration<float> duration = currentTime - m_previousTime;

			float deltaTime = std::min<float>(maxTimeStep, duration.count());
			m_previousTime = currentTime;

			Update(deltaTime);
			Render();
		}
	}
	return 0;
}

void CEngine::Finish()
{
}

void CEngine::Shutdown()
{
	m_isRunning = false;
}

void CEngine::Update(float _deltaTime)
{

#pragma region MyRegion
	if (m_gameWon)
	{
		m_winScreen->SetPos(XMFLOAT2(0, 0));
		m_retryButton->SetPos(XMFLOAT2(m_windowSettings->m_WindowWidth / 2, m_windowSettings->m_WindowHeigth / 2 + 180));
		m_nextLevelButton->SetPos(XMFLOAT2(m_windowSettings->m_WindowWidth / 2, m_windowSettings->m_WindowHeigth / 2  + 100));
		m_quitButton->SetPos(XMFLOAT2(m_windowSettings->m_WindowWidth / 2, m_windowSettings->m_WindowHeigth / 2 + 245));
	}
	
	if (m_gameLost)
	{
		if (!m_nextLevelStarted)
		{
			m_LoseScreen->SetPos(XMFLOAT2(0, 0));
			m_retryButton->SetPos(XMFLOAT2(m_windowSettings->m_WindowWidth / 2, m_windowSettings->m_WindowHeigth / 2 - 15));
			m_nextLevelButton->SetPos(XMFLOAT2(m_windowSettings->m_WindowWidth / 2, m_windowSettings->m_WindowHeigth / 2 - 85));
			m_quitButton->SetPos(XMFLOAT2(m_windowSettings->m_WindowWidth / 2, m_windowSettings->m_WindowHeigth / 2  + 55));
		}
	}
	
	if (m_player->GetCenter().x < -130 || m_player->GetCenter().x > 60 || m_player->GetCenter().y < -60 || m_player->GetCenter().y > 60)
		m_warningText->SetPos(XMFLOAT2(m_windowSettings->m_WindowWidth / 2, m_windowSettings->m_WindowHeigth / 2));
	else
		m_warningText->SetPos(XMFLOAT2(10000, 10000));
	
	if (m_player->GetCenter().x < -160 || m_player->GetCenter().x > 100  || m_player->GetCenter().y < -90 || m_player->GetCenter().y > 90)
	{
		m_gameLost = true;
		m_warningText->SetPos(XMFLOAT2(10000, 10000));
	}
	
	
	m_inputManager->DetectInput();
	
	if (m_inputManager->GetKeyDown(DIK_ESCAPE))
	{
		m_isRunning = false;
	}
		
	m_camera->Update(_deltaTime,m_player->GetCenter(), !m_nextLevelStarted);
	m_playerCollider->SetPos(m_player->GetCenter());
	
	m_contentManager->Update(_deltaTime);
	
	for (Meteor* c : m_meteors)
	{
		if (Collision::MeteorInCircle(*c, *m_playerCollider))
		{
			m_gameLost = true;
		}
	}
	
	 // Vacum
	if (Collision::BlackHoleInCircle(*m_blackHole, *m_playerCollider))
	{
		m_player->inBlackHole = true;
		float suckSpeed = 30 / Magnitude(m_blackHole->GetCenter() - m_player->GetCenter());
		float suckRotationSpeed = 100 / Magnitude(m_blackHole->GetCenter() - m_player->GetCenter());
		
		float multiplier = 4.5;
		
		if (suckSpeed <= 0)
			suckSpeed = 1;
		else if (suckSpeed >= 50)
			suckSpeed = 50;
	
		if (suckRotationSpeed <= 0)
			suckRotationSpeed = 1;
		else if (suckRotationSpeed >= 50)
			suckRotationSpeed = 50;
		
		m_player->AddToRotation(XMFLOAT3(0, 0, 1 * suckRotationSpeed * _deltaTime * multiplier));
		
		m_player->SetPos(m_player->MoveTowards(m_player->GetCenter(), m_blackHole->GetCenter(), suckSpeed * _deltaTime));
		
		// Inside Black hole
		if (Magnitude(m_player->GetCenter() - m_blackHole->GetCenter()) <= 0.001)
			m_gameLost = true;
	}
	else
		m_player->inBlackHole = false;
	
	if (Collision::PlanetInCircle(*m_goalPlanet, *m_playerCollider))
	{
		m_gameWon = true;
	}
	
	#pragma endregion
}

void CEngine::Render()
{
	/************************************************************************/
	/* In order to display a new frame, we have the get the next free		*/
	/* backbuffer.															*/
	/************************************************************************/
	UINT currentBackbufferIndex = m_directXSettings->m_BackbufferIndex;
	auto backbuffer = m_directXSettings->m_BackbufferArray[currentBackbufferIndex];
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv
	{
		m_directXSettings->m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		(int)currentBackbufferIndex,
		m_directXSettings->m_DescriptorSize
	};

	auto dsv = m_directXSettings->m_StencilHeap->GetCPUDescriptorHandleForHeapStart();

	m_directXSettings->m_Allocators[currentBackbufferIndex]->Reset();
	m_directXSettings->m_DirectCommandList->Reset(m_directXSettings->m_Allocators[currentBackbufferIndex].Get(), nullptr);

	ID3D12DescriptorHeap* heaps[] = { m_directXSettings->m_ResourceDescriptors.Get() };
	m_directXSettings->m_DirectCommandList->SetDescriptorHeaps(_countof(heaps), heaps);

	/************************************************************************/
	/* Clearing of a render target is only allowed, when it is in the 		*/
	/* render target state. Therefore we have to move the back buffer from  */
	/* the present state into the render target state.						*/
	/************************************************************************/
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		backbuffer.Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	m_directXSettings->m_DirectCommandList->ResourceBarrier(1, &barrier);
	//auto clearColor = m_directXSettings->m_BackbufferColors[currentBackbufferIndex];

	m_directXSettings->m_DirectCommandList->ClearRenderTargetView(rtv, m_directXSettings->m_ClearColor, 0, nullptr);
	m_directXSettings->m_DirectCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	XMMATRIX rotation = XMMatrixRotationRollPitchYaw
	(
		XMConvertToRadians(m_camera->GetRotation().x),
		XMConvertToRadians(m_camera->GetRotation().y),
		XMConvertToRadians(m_camera->GetRotation().z)
	);
	XMMATRIX position = XMMatrixTranslation
	(
		m_camera->GetPosition().x,
		m_camera->GetPosition().y,
		m_camera->GetPosition().z
	);

	m_directXSettings->m_ViewMatrix = XMMatrixInverse
	(
		nullptr,
		XMMatrixMultiply(rotation, position)
	);

	float aspectRatio = m_windowSettings->m_WindowWidth / (float)m_windowSettings->m_WindowHeigth;
	m_directXSettings->m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_camera->GetFOV()), aspectRatio, 0.1f, 100);

	m_directXSettings->m_LightConstantBufferData.m_CameraPosition = m_camera->GetPosition();
	memcpy(m_directXSettings->m_LightConstantBufferDataBegin, &m_directXSettings->m_LightConstantBufferData,
		sizeof(m_directXSettings->m_LightConstantBufferData));

	// 2D
	m_directXSettings->m_SpriteBatch->SetViewport(m_directXSettings->m_Viewport);
	m_directXSettings->m_SpriteBatch->Begin(m_directXSettings->m_DirectCommandList.Get());

	// Render
	m_contentManager->Render();

	// 2D
	m_directXSettings->m_SpriteBatch->End();

	/**************************************************************************/
	/* The swap chain can only present an image which is in the present state */
	/* Therefore we have to reverse our transition. The transition is now	  */
	/* render target to the present state.									  */
	/**************************************************************************/
	barrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		backbuffer.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	m_directXSettings->m_DirectCommandList->ResourceBarrier(1, &barrier);

	/************************************************************************/
	/* After all commands have been recorded, the command list has to be	*/
	/* closed and then executed on the command queue.						*/
	/************************************************************************/
	m_directXSettings->m_DirectCommandList->Close();

	ID3D12CommandList* const commandLists[] = { m_directXSettings->m_DirectCommandList.Get() };

	m_directXSettings->m_DirectCommandQueue->ExecuteCommandLists(1, commandLists);

	/************************************************************************/
	/* In order to know, when the commands have finished executing, we add	*/
	/* a fence with a specific value to the end. Once this fence is reached,*/
	/* we will get notified on the CPU and can respond.						*/
	/************************************************************************/
	UINT64 fenceValue = ++m_directXSettings->m_FenceValue;
	int value = m_directXSettings->m_DirectCommandQueue->Signal(m_directXSettings->m_Fence.Get(), fenceValue);
	if (FAILED(value))
	{
		throw std::exception("Failed to signal command queue!");
	}
	m_directXSettings->m_FenceValue = fenceValue;
	m_directXSettings->m_FrameFenceValue[currentBackbufferIndex] = fenceValue;

	// 1 = VSync On; 0 = VSync Off
	m_directXSettings->m_SwapChain->Present(1, 0);

	/************************************************************************/
	/* We need to know, which buffer is the next one we can render on.		*/
	/* Then we have to wait for its fence value, to know when it is			*/
	/* usable again.														*/
	/************************************************************************/
	m_directXSettings->m_BackbufferIndex = m_directXSettings->m_SwapChain->GetCurrentBackBufferIndex();

	value = CHelpers::WaitForFenceValue(m_directXSettings->m_Fence,
		m_directXSettings->m_FrameFenceValue[m_directXSettings->m_BackbufferIndex], m_directXSettings->m_FrameEvent);
	if (FAILED(value))
	{
		throw std::exception("Failed to wait for fence!");
	}

	m_directXSettings->m_GraphicsMemory->Commit(m_directXSettings->m_DirectCommandQueue.Get());
}

int CEngine::InitApplication(HINSTANCE _hInstance)
{
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	int value;
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debugInterface;
	value = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
	if (FAILED(value))
	{
		return -10;
	}
	debugInterface->EnableDebugLayer();
#endif

	/************************************************************************/
	/* The windows class contains all necessary information.				*/
	/* After creation it must be registered.								*/
	/************************************************************************/
	WNDCLASSEXW windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = &WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = _hInstance;
	windowClass.hIcon = ::LoadIcon(_hInstance, nullptr);
	windowClass.hCursor = ::LoadCursor(_hInstance, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = m_windowSettings->m_WindowClassName;
	windowClass.hIconSm = ::LoadIcon(_hInstance, NULL);

	/************************************************************************/
	/* This method needs to be called, before the window can be created.    */
	/************************************************************************/
	value = ::RegisterClassExW(&windowClass);
	if (value == 0)
	{
		return -11;
	}

	int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	RECT windowRect;
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = m_windowSettings->m_WindowWidth;
	windowRect.bottom = m_windowSettings->m_WindowHeigth;

	::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);
	int windowWidth = windowRect.right - windowRect.left;
	int windowHeigth = windowRect.bottom - windowRect.top;

	int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
	int windowY = std::max<int>(0, (screenHeight - windowHeigth) / 2);

	HWND handle = ::CreateWindowExW
	(
		NULL,
		m_windowSettings->m_WindowClassName,
		m_windowSettings->m_WindowName,
		WS_OVERLAPPEDWINDOW,
		windowX,
		windowY,
		windowWidth,
		windowHeigth,
		nullptr,
		nullptr,
		_hInstance,
		nullptr
	);
	if (nullptr == handle)
	{
		return -12;
	}

	m_windowSettings->m_WindowHandle = handle;
	::GetWindowRect(handle, &m_windowSettings->m_WindowRect);

	return 0;
}

int CEngine::InitDirectX()
{
	int value;

	ComPtr<IDXGIFactory4> dxgiFactory;
	int createFactoryFlags = 0;
#if (_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
	value = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(value))
	{
		return -20;
	}

	/************************************************************************/
	/* Because most devices contain multiple devices, which can be used		*/
	/* as a GPU, we look for the one with the most vram.					*/
	/************************************************************************/
	ComPtr<IDXGIAdapter1> adapter1;
	ComPtr<IDXGIAdapter4> adapter4;
	int maxDedicatedVideoMemory = 0;
	for (int i = 0; dxgiFactory->EnumAdapters1(i, &adapter1)
		!= DXGI_ERROR_NOT_FOUND; i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		adapter1->GetDesc1(&dxgiAdapterDesc);
		if ((dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0
			&& SUCCEEDED(D3D12CreateDevice(adapter1.Get(), D3D_FEATURE_LEVEL_12_1,
				__uuidof(ID3D12Device), nullptr))
			&& dxgiAdapterDesc.DedicatedVideoMemory > maxDedicatedVideoMemory)
		{
			maxDedicatedVideoMemory = dxgiAdapterDesc.DedicatedVideoMemory;
			value = adapter1.As(&adapter4);
			if (FAILED(value))
			{
				return -22;
			}
		}
	}

	value = D3D12CreateDevice(adapter4.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_directXSettings->m_Device));
	//value = D3D12CreateDevice(adapter4.Get(), D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), &m_directXSettings->m_Device);
	if (FAILED(value))
	{
		return -23;
	}

#if defined(_DEBUG)
	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(m_directXSettings->m_Device.As(&infoQueue)))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	}
#endif

	/************************************************************************/
	/* There are six different types of command lists/command queues:		*/
	/*		-	D3D12_COMMAND_LIST_TYPE_DIRECT								*/
	/*		-	D3D12_COMMAND_LIST_TYPE_BUNDLE								*/
	/*		-	D3D12_COMMAND_LIST_TYPE_COMPUTE								*/
	/*		-	D3D12_COMMAND_LIST_TYPE_COPY								*/
	/*		-	D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE						*/
	/*		-	D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS						*/
	/************************************************************************/
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;

	value = m_directXSettings->m_Device
		->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_directXSettings->m_DirectCommandQueue));
	if (FAILED(value))
	{
		return -24;
	}

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { };
	swapChainDesc.Width = m_windowSettings->m_WindowWidth;
	swapChainDesc.Height = m_windowSettings->m_WindowHeigth;
	// 32-bit unsigned-normalized-integer format that supports 8 bits per channel including alpha.
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = false;
	// The default sampler mode, with no anti-aliasing, has a count of 1 and a quality level of 0.
	// First value = count; Second value = quality
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = m_directXSettings->m_BackbufferAmount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	ComPtr<IDXGISwapChain1> swapChain;

	value = dxgiFactory->CreateSwapChainForHwnd
	(
		m_directXSettings->m_DirectCommandQueue.Get(),
		m_windowSettings->m_WindowHandle,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	);
	if (FAILED(value))
	{
		return -25;
	}
	value = swapChain.As(&m_directXSettings->m_SwapChain);
	if (FAILED(value))
	{
		return -26;
	}

	m_directXSettings->m_BackbufferIndex = m_directXSettings->m_SwapChain->GetCurrentBackBufferIndex();

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = { };
	heapDesc.NumDescriptors = (int)EResourceType::MAX;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	value = m_directXSettings->m_Device->CreateDescriptorHeap(&heapDesc,
		IID_PPV_ARGS(&m_directXSettings->m_ResourceDescriptors));
	if (FAILED(value))
	{
		return -27;
	}

	m_directXSettings->m_CBVDescriptorSize
		= m_directXSettings->m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	heapDesc = { };
	heapDesc.NumDescriptors = m_directXSettings->m_BackbufferAmount;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	value = m_directXSettings->m_Device->CreateDescriptorHeap(&heapDesc,
		IID_PPV_ARGS(&m_directXSettings->m_DescriptorHeap));
	if (FAILED(value))
	{
		return -28;
	}

	m_directXSettings->m_DescriptorSize
		= m_directXSettings->m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle
		= CD3DX12_CPU_DESCRIPTOR_HANDLE(m_directXSettings->m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (int i = 0; i < m_directXSettings->m_BackbufferAmount; i++)
	{
		ComPtr<ID3D12Resource> backBuffer;
		value = m_directXSettings->m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
		if (FAILED(value))
		{
			return -28;
		}

		m_directXSettings->m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
		m_directXSettings->m_BackbufferArray[i] = backBuffer;

		rtvHandle.Offset(m_directXSettings->m_DescriptorSize);
	}

	for (int i = 0; i < m_directXSettings->m_BackbufferAmount; i++)
	{
		value = m_directXSettings->m_Device->CreateCommandAllocator
		(
			D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_directXSettings->m_Allocators[i])
		);
		if (FAILED(value))
		{
			return -29;
		}
	}

	value = m_directXSettings->m_Device->CreateCommandList
	(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_directXSettings->m_Allocators[m_directXSettings->m_BackbufferIndex].Get(),
		nullptr,
		IID_PPV_ARGS(&m_directXSettings->m_DirectCommandList)
	);
	if (FAILED(value))
	{
		return -30;
	}
	value = m_directXSettings->m_DirectCommandList->Close();
	if (FAILED(value))
	{
		return -31;
	}

	value = m_directXSettings->m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_directXSettings->m_Fence));
	if (FAILED(value))
	{
		return -32;
	}
	m_directXSettings->m_FrameEvent = ::CreateEvent(nullptr, false, false, nullptr);

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	value = m_directXSettings->m_Device->CreateDescriptorHeap(&dsvHeapDesc,
		IID_PPV_ARGS(&m_directXSettings->m_StencilHeap));
	if (FAILED(value))
	{
		return -33;
	}

	D3D12_CLEAR_VALUE optimizedClearValue;
	optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	optimizedClearValue.DepthStencil = { 1.0f, 0 };

	value = m_directXSettings->m_Device->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_windowSettings->m_WindowWidth,
			m_windowSettings->m_WindowHeigth, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&optimizedClearValue,
		IID_PPV_ARGS(&m_directXSettings->m_DepthBuffer)
	);
	if (FAILED(value))
	{
		return -34;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC dsv;
	dsv.Format = DXGI_FORMAT_D32_FLOAT;
	dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsv.Texture2D.MipSlice = 0;
	dsv.Flags = D3D12_DSV_FLAG_NONE;

	m_directXSettings->m_Device->CreateDepthStencilView
	(
		m_directXSettings->m_DepthBuffer.Get(), &dsv, m_directXSettings->m_StencilHeap->GetCPUDescriptorHandleForHeapStart()
	);

	m_directXSettings->m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f,
		(float)m_windowSettings->m_WindowWidth, (float)m_windowSettings->m_WindowHeigth);
	m_directXSettings->m_ScissorRect = CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX);

	ResourceUploadBatch resourceUploadBatch(m_directXSettings->m_Device.Get());
	resourceUploadBatch.Begin();

	RenderTargetState renderState = RenderTargetState(swapChainDesc.Format, dsv.Format);
	SpriteBatchPipelineStateDescription spriteBatchDesc = SpriteBatchPipelineStateDescription(renderState,
		&CommonStates::NonPremultiplied);
	m_directXSettings->m_SpriteBatch = std::make_unique<SpriteBatch>(m_directXSettings->m_Device.Get(),
		resourceUploadBatch, spriteBatchDesc);
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle =
	{
		m_directXSettings->m_ResourceDescriptors->GetCPUDescriptorHandleForHeapStart(),
		(int)EResourceType::SPRITEFONT,
		m_directXSettings->m_CBVDescriptorSize
	};

	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle =
	{
		m_directXSettings->m_ResourceDescriptors->GetGPUDescriptorHandleForHeapStart(),
		(int)EResourceType::SPRITEFONT,
		m_directXSettings->m_CBVDescriptorSize
	};

	m_directXSettings->m_SpriteFont = std::make_unique<SpriteFont>
		(
			m_directXSettings->m_Device.Get(),
			resourceUploadBatch,
			L"Fonts/why.spritefont",
			cpuHandle,
			gpuHandle
		);

	auto uploadFinish = resourceUploadBatch.End(m_directXSettings->m_DirectCommandQueue.Get());
	uploadFinish.wait();

	m_directXSettings->m_GraphicsMemory = std::make_unique<GraphicsMemory>(m_directXSettings->m_Device.Get());

	m_directXSettings->m_IsInitialized = true;

	return 0;
}

int CEngine::InitDirectXToolKit()
{
	Wrappers::RoInitializeWrapper initalize(RO_INIT_MULTITHREADED);
	if (FAILED(initalize))
		return -401;

	int value = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(value))
		return -402;

	return 0;
}

void CEngine::LoadContent()
	#pragma region FirstLevel
{
	m_player = CObjManager::LoadObj("Objects/cobramkii.obj");
	m_player->SetPos(XMFLOAT3(-120, 40, 0));
	m_player->SetRot(XMFLOAT3(0,0,90));
	m_player->Scale(XMFLOAT3(0.025, 0.025, 0.025));
	m_contentManager->AddEntity(m_player);
	
	static std::random_device rd;
	static std::mt19937 gen(rd());
	
	std::uniform_int_distribution<> dis(-50, 50);
	for (int i = 1; i < 100; i++)
	{
		int rndnum = dis(gen);
		m_meteors.push_back(new Meteor(36, 2.25, XMFLOAT3(rndnum, i * 30, 0)));
	}
	
	for (Meteor* c : m_meteors)
	{ 
		m_contentManager->AddEntity(c);
	}

	m_planetArrived = new Sphere(36, 17, XMFLOAT3(10000, 10000, 10000));
	m_contentManager->AddEntity((m_planetArrived));

	m_goalPlanet = new Planet(36, 7, XMFLOAT3(0, 0, 0));
	m_contentManager->AddEntity(m_goalPlanet);
	
	m_playerCollider = new Circle(36, 3.5, XMFLOAT3(-5,15,0));
	
	m_blackHole = new BlackHole(36, 14, XMFLOAT3(-45, 30, 0));
	m_contentManager->AddEntity(m_blackHole);

	m_menuScreen = new Image2D(L"Textures/T_MenuScreen.png", EResourceType::MENU_IMAGE, m_windowSettings->m_WindowWidth, m_windowSettings->m_WindowHeigth, XMFLOAT2(0, 0));
	m_startButton = new Button2D(L"Textures/T_StartButton.png", EResourceType::START_BUTTON, RetryButton, XMFLOAT2(120, 60),
		XMFLOAT2(m_windowSettings->m_WindowWidth / 2, m_windowSettings->m_WindowHeigth / 2));
	m_winScreen = new Image2D(L"Textures/T_WinScreen.jpg", EResourceType::WIN_IMAGE, m_windowSettings->m_WindowWidth, m_windowSettings->m_WindowHeigth,
		XMFLOAT2(1000,1000));
	m_LoseScreen = new Image2D(L"Textures/T_LoseScreen.png", EResourceType::LOSE_IMAGE, m_windowSettings->m_WindowWidth, m_windowSettings->m_WindowHeigth,
		XMFLOAT2(1000, 1000));
	m_retryButton = new Button2D(L"Textures/T_RetryButotn.png", EResourceType::RETRY_BUTTON, RetryButton,XMFLOAT2(120,60), XMFLOAT2(10000, 10000));
	m_nextLevelButton = new Button2D(L"Textures/T_NextLevelBB.png", EResourceType::NEXT_LEVEL_BUTTON, NextLevelButton, XMFLOAT2(120, 60), XMFLOAT2(10000, 10000));
	m_quitButton = new Button2D(L"Textures/T_QuitButton.png", EResourceType::QUIT_BUTTON, QuitButton, XMFLOAT2(120, 60), XMFLOAT2(10000, 10000));
	m_warningText = new Text2D("WRONG WAY, TURN BACK");
	m_tutorialText = new Text2D("LAND TO THE PLANET", XMFLOAT2(m_windowSettings->m_WindowWidth / 2, m_windowSettings->m_WindowHeigth / 2 - 125));

	m_contentManager->AddEntity(m_menuScreen);
	m_contentManager->AddEntity(m_startButton);
	m_contentManager->AddEntity(m_tutorialText);
	m_contentManager->AddEntity(m_winScreen);
	m_contentManager->AddEntity(m_LoseScreen);
	m_contentManager->AddEntity(m_retryButton);
	m_contentManager->AddEntity(m_nextLevelButton);
	m_contentManager->AddEntity(m_quitButton);
	m_contentManager->AddEntity(m_warningText);
	m_contentManager->AddEntity(new Cursor2D(L"Textures/Gandalf_the_Grey.png", XMFLOAT2(120,120)));
#pragma endregion

	#pragma region SecondLevel
	FastNoise myNoise;
	float seed = rand() % 100;
	int buildingFootPrint = 7;

	XMFLOAT3 offset = XMFLOAT3(1000, 950, 1000);
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			XMFLOAT3 pos = XMFLOAT3(j * buildingFootPrint, 7, i * buildingFootPrint);
			XMFLOAT3 grassPos = XMFLOAT3(j * buildingFootPrint, 8.5f, i * buildingFootPrint);

			int result = myNoise.GetPerlin(j / 10.0f + seed, i / 10.0f + seed) * 1000;
			m_grasses.push_back(new Quad(L"Textures/T_Grass.png", EResourceType::QUAD_TEXTURE, 3.5f, 3.5f, grassPos + offset, XMFLOAT3(90, 0, 0)));

			if (result < 0)
			{
				pos.y += 5;
				m_buildings.push_back(new Cuboid(L"Textures/T_BlueBuilding.jpg", EResourceType::ATLAS_TEXTURE, 2, 5, 2, pos + offset));
			}

			else if (result < 2)
			{
				pos.y += 4;
				m_buildings.push_back(new Cuboid(L"Textures/T_BrickBuilding.jpg", EResourceType::BRICK_BUILDING, 2, 4, 2, pos + offset));
			}

			else if (result < 4)
			{
				pos.y += 6;
				m_buildings.push_back(new Cuboid(L"Textures/T_BrownBuilding.jpg", EResourceType::BROWN_BUILDING, 2, 6, 2, pos + offset));
			}

			else if (result < 8)
			{
				pos.y += 6;
				m_buildings.push_back(new Cuboid(L"Textures/T_GlassBuilding.jfif", EResourceType::GLASS_BUILDING, 2, 7, 2, pos + offset));
			}

			else if (result < 12)
			{
				pos.y += 4;
				m_buildings.push_back(new Cuboid(L"Textures/T_GrayBuilding.jpg", EResourceType::GRAY_BUILDING, 2, 4, 2, pos + offset));
			}

			else if (result < 18)
			{
				pos.y += 3;
				m_buildings.push_back(new Cuboid(L"Textures/T_LightBrownBuilding.jpg", EResourceType::LIGHT_BROWN_BUILDING, 2, 3, 2, pos + offset));
			}

			else if (result < 20)
			{
				pos.y += 5;
				m_buildings.push_back(new Cuboid(L"Textures/T_ModernBuilding.png", EResourceType::MODERN_BUILDING, 2, 7, 2, pos + offset));
			}

			else if (result < 25)
			{
				pos.y += 4;
				m_buildings.push_back(new Cuboid(L"Textures/T_OldBuilding.jfif", EResourceType::OLD_BUILDING, 2, 4, 2, pos + offset));
			}
		}
	}
	

	m_waters.push_back(new WaterPlane(20, XMFLOAT3(100, 1, 160), XMFLOAT3(-55, 0, 65) + offset));
	m_waters.push_back(new WaterPlane(20, XMFLOAT3(100, 1, 160), XMFLOAT3(185, 0, 65) + offset));
	m_waters.push_back(new WaterPlane(20, XMFLOAT3(205, 1, 160), XMFLOAT3(55, 0, -72) + offset));
	m_waters.push_back(new WaterPlane(20, XMFLOAT3(205, 1, 160), XMFLOAT3(55, 0, 205) + offset));
	

	for (WaterPlane* w : m_waters)
	{
		m_contentManager->AddEntity(w);
	}

	for (Cuboid* c: m_buildings)
		m_contentManager->AddEntity(c);
	 

	for (Quad* q : m_grasses)
		m_contentManager->AddEntity(q);
#pragma endregion
}

int CEngine::CreateSimpleShader()
{
	int value;
	LPCWSTR fileName;
#if defined (_DEBUG)
	fileName = L"SimpleVertexShader_d.cso";
#else
	fileName = L"SimpleVertexShader.cso";
#endif

	auto vertexShaderBlob = DX::ReadData(fileName);
	D3D12_SHADER_BYTECODE vertexShaderByteCode =
	{
		vertexShaderBlob.data(),
		vertexShaderBlob.size()
	};

#if defined (_DEBUG)
	fileName = L"SimplePixelShader_d.cso";
#else	
	fileName = L"SimplePixelShader.cso";
#endif

	auto pixelShaderBlob = DX::ReadData(fileName);
	D3D12_SHADER_BYTECODE pixelShaderByteCode =
	{
		pixelShaderBlob.data(),
		pixelShaderBlob.size()
	};

	D3D12_INPUT_ELEMENT_DESC inputLayout[]
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"POSITION",
			1,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
	};

	D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignature;
	rootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	value = m_directXSettings->m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE,
		&rootSignature, sizeof(rootSignature));
	if (FAILED(value))
	{
		rootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

	CD3DX12_ROOT_PARAMETER1 rootParameters[3];
	rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
	rootParameters[1].InitAsConstants(sizeof(XMMATRIX) / 4, 1, 0, D3D12_SHADER_VISIBILITY_VERTEX);

	CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

	rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
	rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

	ComPtr<ID3DBlob> rootSignatureBlob;
	ComPtr<ID3DBlob> errorBlob;

	value = D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
		rootSignature.HighestVersion, &rootSignatureBlob, &errorBlob);
	if (FAILED(value))
	{
		return -40;
	}

	value = m_directXSettings->m_Device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
		rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_directXSettings->m_SimpleShaderRootSignature));
	if (FAILED(value))
	{
		return -41;
	}

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	CD3DX12_RASTERIZER_DESC rasterizerDsc
	{
		D3D12_FILL_MODE_SOLID,
		D3D12_CULL_MODE_BACK,
		false,
		D3D12_DEFAULT_DEPTH_BIAS,
		D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
		true,
		false,
		false,
		0,
		D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};

	//CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC;
	CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER rasterizer =
		CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER(rasterizerDsc);

	CD3DX12_PIPELINE_STATE_STREAM pipelineStateStream;
	pipelineStateStream.pRootSignature = m_directXSettings->m_SimpleShaderRootSignature.Get();
	pipelineStateStream.VS = vertexShaderByteCode;
	pipelineStateStream.PS = pixelShaderByteCode;
	pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.RTVFormats = rtvFormats;
	pipelineStateStream.RasterizerState = rasterizer;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc =
	{
		sizeof(pipelineStateStream),
		&pipelineStateStream
	};

	value = m_directXSettings->m_Device->CreatePipelineState(&pipelineStateStreamDesc,
		IID_PPV_ARGS(&m_directXSettings->m_SimpleShaderPipelineState));
	if (FAILED(value))
	{
		return -42;
	}
	return 0;
}

int CEngine::CreateCelShadingShader()
{
	int value;
	LPCWSTR fileName;
#if defined (_DEBUG)
	fileName = L"CelShadingVertexShader_d.cso";
#else
	fileName = L"CelShadingVertexShader.cso";
#endif

	auto vertexShaderBlob = DX::ReadData(fileName);
	D3D12_SHADER_BYTECODE vertexShaderByteCode =
	{
		vertexShaderBlob.data(),
		vertexShaderBlob.size()
	};

#if defined (_DEBUG)
	fileName = L"CelShadingPixelShader_d.cso";
#else	
	fileName = L"CelShadingPixelShader.cso";
#endif

	auto pixelShaderBlob = DX::ReadData(fileName);
	D3D12_SHADER_BYTECODE pixelShaderByteCode =
	{
		pixelShaderBlob.data(),
		pixelShaderBlob.size()
	};

	D3D12_INPUT_ELEMENT_DESC inputLayout[]
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};

	D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignature;
	rootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	value = m_directXSettings->m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE,
		&rootSignature, sizeof(rootSignature));
	if (FAILED(value))
	{
		rootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

	CD3DX12_ROOT_PARAMETER1 rootParameters[3];
	rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
	rootParameters[1].InitAsConstants(sizeof(XMMATRIX) / 4, 1, 0, D3D12_SHADER_VISIBILITY_VERTEX);

	CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

	rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
	rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

	ComPtr<ID3DBlob> rootSignatureBlob;
	ComPtr<ID3DBlob> errorBlob;

	value = D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
		rootSignature.HighestVersion, &rootSignatureBlob, &errorBlob);
	if (FAILED(value))
	{
		return -50;
	}

	value = m_directXSettings->m_Device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
		rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_directXSettings->m_CellShadingRootSignature));
	if (FAILED(value))
	{
		return -51;
	}

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	CD3DX12_RASTERIZER_DESC rasterizerDsc
	{
		D3D12_FILL_MODE_SOLID,
		D3D12_CULL_MODE_BACK,
		false,
		D3D12_DEFAULT_DEPTH_BIAS,
		D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
		true,
		false,
		false,
		0,
		D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};

		//CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC;
	CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER rasterizer =
		CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER(rasterizerDsc);

	CD3DX12_PIPELINE_STATE_STREAM pipelineStateStream;
	pipelineStateStream.pRootSignature = m_directXSettings->m_CellShadingRootSignature.Get();
	pipelineStateStream.VS = vertexShaderByteCode;
	pipelineStateStream.PS = pixelShaderByteCode;
	pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.RTVFormats = rtvFormats;
	pipelineStateStream.RasterizerState = rasterizer;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc =
	{
		sizeof(pipelineStateStream),
		&pipelineStateStream
	};

	value = m_directXSettings->m_Device->CreatePipelineState(&pipelineStateStreamDesc,
		IID_PPV_ARGS(&m_directXSettings->m_CellShadingPipelineState));
	if (FAILED(value))
	{
		return -52;
	}
	return 0;
}

int CEngine::CreateTexturedShader()
{
	int value;
	LPCWSTR fileName;
#if defined (_DEBUG)
	fileName = L"TexturedVertexShader_d.cso";
#else
	fileName = L"TexturedVertexShader.cso";
#endif

	auto vertexShaderBlob = DX::ReadData(fileName);
	D3D12_SHADER_BYTECODE vertexShaderByteCode =
	{
		vertexShaderBlob.data(),
		vertexShaderBlob.size()
	};

#if defined (_DEBUG)
	fileName = L"TexturedPixelShader_d.cso";
#else	
	fileName = L"TexturedPixelShader.cso";
#endif

	auto pixelShaderBlob = DX::ReadData(fileName);
	D3D12_SHADER_BYTECODE pixelShaderByteCode =
	{
		pixelShaderBlob.data(),
		pixelShaderBlob.size()
	};

	D3D12_INPUT_ELEMENT_DESC inputLayout[]
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"POSITION",
			1,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},

	};

	D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignature;
	rootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	value = m_directXSettings->m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE,
		&rootSignature, sizeof(rootSignature));
	if (FAILED(value))
	{
		rootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

	CD3DX12_ROOT_PARAMETER1 rootParameters[4];
	rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
	rootParameters[1].InitAsConstants(sizeof(XMMATRIX) / 4, 1, 0, D3D12_SHADER_VISIBILITY_VERTEX);

	CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

	rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_STATIC_SAMPLER_DESC staticSamplerDesc = {};
	staticSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	staticSamplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
	staticSamplerDesc.MaxAnisotropy = 0;
	staticSamplerDesc.MinLOD = 0;
	staticSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplerDesc.MipLODBias = 0;
	staticSamplerDesc.ShaderRegister = 0;
	staticSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	staticSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;


	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
	rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 1, &staticSamplerDesc, rootSignatureFlags);

	ComPtr<ID3DBlob> rootSignatureBlob;
	ComPtr<ID3DBlob> errorBlob;

	value = D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
		rootSignature.HighestVersion, &rootSignatureBlob, &errorBlob);
	if (FAILED(value))
	{
		return -60;
	}

	value = m_directXSettings->m_Device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
		rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_directXSettings->m_TexturedShaderRootSignature));
	if (FAILED(value))
	{
		return -61;
	}

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	CD3DX12_RASTERIZER_DESC rasterizerDsc
	{
		D3D12_FILL_MODE_SOLID,
		D3D12_CULL_MODE_BACK,
		false,
		D3D12_DEFAULT_DEPTH_BIAS,
		D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
		true,
		false,
		false,
		0,
		D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};

	//CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC;
	CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER rasterizer =
		CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER(rasterizerDsc);

	CD3DX12_PIPELINE_STATE_STREAM pipelineStateStream;
	pipelineStateStream.pRootSignature = m_directXSettings->m_TexturedShaderRootSignature.Get();
	pipelineStateStream.VS = vertexShaderByteCode;
	pipelineStateStream.PS = pixelShaderByteCode;
	pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.RTVFormats = rtvFormats;
	pipelineStateStream.RasterizerState = rasterizer;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc =
	{
		sizeof(pipelineStateStream),
		&pipelineStateStream
	};

	value = m_directXSettings->m_Device->CreatePipelineState(&pipelineStateStreamDesc,
		IID_PPV_ARGS(&m_directXSettings->m_TexturedShaderPipelineState));
	if (FAILED(value))
	{
		return -62;
	}
	return 0;
}

int CEngine::CreateHeightBlendingShader()
{
	int value;
	LPCWSTR fileName;
#if defined (_DEBUG)
	fileName = L"HeigthBlendingVertexShader_d.cso";
#else
	fileName = L"HeigthBlendingVertexShader.cso";
#endif

	auto vertexShaderBlob = DX::ReadData(fileName);
	D3D12_SHADER_BYTECODE vertexShaderByteCode =
	{
		vertexShaderBlob.data(),
		vertexShaderBlob.size()
	};

#if defined (_DEBUG)
	fileName = L"HeigthBlendingPixelShader_d.cso";
#else	
	fileName = L"HeigthBlendingPixelShader.cso";
#endif

	auto pixelShaderBlob = DX::ReadData(fileName);
	D3D12_SHADER_BYTECODE pixelShaderByteCode =
	{
		pixelShaderBlob.data(),
		pixelShaderBlob.size()
	};

	D3D12_INPUT_ELEMENT_DESC inputLayout[]
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};

	D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignature;
	rootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	value = m_directXSettings->m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE,
		&rootSignature, sizeof(rootSignature));
	if (FAILED(value))
	{
		rootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

	CD3DX12_ROOT_PARAMETER1 rootParameters[6];
	rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
	rootParameters[1].InitAsConstants(sizeof(XMMATRIX) / 4, 1, 0, D3D12_SHADER_VISIBILITY_VERTEX);

	CD3DX12_DESCRIPTOR_RANGE1 ranges[4];
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

	rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[4].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[5].InitAsDescriptorTable(1, &ranges[3], D3D12_SHADER_VISIBILITY_PIXEL);

	D3D12_STATIC_SAMPLER_DESC staticSamplerDesc = {};
	staticSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	staticSamplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
	staticSamplerDesc.MaxAnisotropy = 0;
	staticSamplerDesc.MinLOD = 0;
	staticSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplerDesc.MipLODBias = 0;
	staticSamplerDesc.ShaderRegister = 0;
	staticSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	staticSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;


	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
	rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 1, &staticSamplerDesc, rootSignatureFlags);

	ComPtr<ID3DBlob> rootSignatureBlob;
	ComPtr<ID3DBlob> errorBlob;

	value = D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
		rootSignature.HighestVersion, &rootSignatureBlob, &errorBlob);
	if (FAILED(value))
	{
		return -70;
	}

	value = m_directXSettings->m_Device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
		rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_directXSettings->m_HeightBlendingRootSignature));
	if (FAILED(value))
	{
		return -71;
	}

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	CD3DX12_RASTERIZER_DESC rasterizerDsc
	{
		D3D12_FILL_MODE_SOLID,
		D3D12_CULL_MODE_BACK,
		false,
		D3D12_DEFAULT_DEPTH_BIAS,
		D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
		true,
		false,
		false,
		0,
		D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};

	//CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC;
	CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER rasterizer =
		CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER(rasterizerDsc);

	CD3DX12_PIPELINE_STATE_STREAM pipelineStateStream;
	pipelineStateStream.pRootSignature = m_directXSettings->m_HeightBlendingRootSignature.Get();
	pipelineStateStream.VS = vertexShaderByteCode;
	pipelineStateStream.PS = pixelShaderByteCode;
	pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.RTVFormats = rtvFormats;
	pipelineStateStream.RasterizerState = rasterizer;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc =
	{
		sizeof(pipelineStateStream),
		&pipelineStateStream
	};

	value = m_directXSettings->m_Device->CreatePipelineState(&pipelineStateStreamDesc,
		IID_PPV_ARGS(&m_directXSettings->m_HeightBlendingPipelineState));
	if (FAILED(value))
	{
		return -72;
	}
	return 0;
}

int CEngine::CreateTerrainShader()
{
	int value;
	LPCWSTR fileName;
#if defined (_DEBUG)
	fileName = L"TerrainVertexShader_d.cso";
#else
	fileName = L"TerrainVertexShader.cso";
#endif

	auto vertexShaderBlob = DX::ReadData(fileName);
	D3D12_SHADER_BYTECODE vertexShaderByteCode =
	{
		vertexShaderBlob.data(),
		vertexShaderBlob.size()
	};

#if defined (_DEBUG)
	fileName = L"TerrainPixelShader_d.cso";
#else	
	fileName = L"TerrainPixelShader.cso";
#endif

	auto pixelShaderBlob = DX::ReadData(fileName);
	D3D12_SHADER_BYTECODE pixelShaderByteCode =
	{
		pixelShaderBlob.data(),
		pixelShaderBlob.size()
	};

	D3D12_INPUT_ELEMENT_DESC inputLayout[]
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"POSITION",
			1,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};

	D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignature;
	rootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	value = m_directXSettings->m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE,
		&rootSignature, sizeof(rootSignature));
	if (FAILED(value))
	{
		rootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

	CD3DX12_ROOT_PARAMETER1 rootParameters[6];
	rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
	rootParameters[1].InitAsConstants(sizeof(XMMATRIX) / 4, 1, 0, D3D12_SHADER_VISIBILITY_VERTEX);
	rootParameters[2].InitAsConstants(sizeof(float), 0, 0, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_DESCRIPTOR_RANGE1 ranges[3];
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

	rootParameters[3].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[4].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[5].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_VERTEX);

	D3D12_STATIC_SAMPLER_DESC staticSamplerDesc[3] = {};
	staticSamplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	staticSamplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	staticSamplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	staticSamplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	staticSamplerDesc[0].Filter = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
	staticSamplerDesc[0].MaxAnisotropy = 0;
	staticSamplerDesc[0].MinLOD = 0;
	staticSamplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplerDesc[0].MipLODBias = 0;
	staticSamplerDesc[0].ShaderRegister = 0;
	staticSamplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	staticSamplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	staticSamplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc[1].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	staticSamplerDesc[1].Filter = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
	staticSamplerDesc[1].MaxAnisotropy = 0;
	staticSamplerDesc[1].MinLOD = 0;
	staticSamplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplerDesc[1].MipLODBias = 0;
	staticSamplerDesc[1].ShaderRegister = 1;
	staticSamplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	staticSamplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	staticSamplerDesc[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc[2].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	staticSamplerDesc[2].Filter = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
	staticSamplerDesc[2].MaxAnisotropy = 0;
	staticSamplerDesc[2].MinLOD = 0;
	staticSamplerDesc[2].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplerDesc[2].MipLODBias = 0;
	staticSamplerDesc[2].ShaderRegister = 0;
	staticSamplerDesc[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	staticSamplerDesc[2].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
	rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 3, staticSamplerDesc, rootSignatureFlags);

	ComPtr<ID3DBlob> rootSignatureBlob;
	ComPtr<ID3DBlob> errorBlob;

	value = D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
		rootSignature.HighestVersion, &rootSignatureBlob, &errorBlob);
	if (FAILED(value))
	{
		return -70;
	}

	value = m_directXSettings->m_Device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
		rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_directXSettings->m_TerrainShaderRootSignature));
	if (FAILED(value))
	{
		return -71;
	}

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	CD3DX12_RASTERIZER_DESC rasterizerDsc
	{
		D3D12_FILL_MODE_SOLID,
		D3D12_CULL_MODE_BACK,
		false,
		D3D12_DEFAULT_DEPTH_BIAS,
		D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
		true,
		false,
		false,
		0,
		D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};

	//CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC;
	CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER rasterizer =
		CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER(rasterizerDsc);

	CD3DX12_PIPELINE_STATE_STREAM pipelineStateStream;
	pipelineStateStream.pRootSignature = m_directXSettings->m_TerrainShaderRootSignature.Get();
	pipelineStateStream.VS = vertexShaderByteCode;
	pipelineStateStream.PS = pixelShaderByteCode;
	pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.RTVFormats = rtvFormats;
	pipelineStateStream.RasterizerState = rasterizer;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc =
	{
		sizeof(pipelineStateStream),
		&pipelineStateStream
	};

	value = m_directXSettings->m_Device->CreatePipelineState(&pipelineStateStreamDesc,
		IID_PPV_ARGS(&m_directXSettings->m_TerrainShaderPipelineState));
	if (FAILED(value))
	{
		return -72;
	}
	return 0;
}

int CEngine::CreateWaterShader()
{
	int value;
	LPCWSTR fileName;
#if defined (_DEBUG)
	fileName = L"WaterVertexShader_d.cso";
#else
	fileName = L"WaterVertexShader.cso";
#endif

	auto vertexShaderBlob = DX::ReadData(fileName);
	D3D12_SHADER_BYTECODE vertexShaderByteCode =
	{
		vertexShaderBlob.data(),
		vertexShaderBlob.size()
	};

#if defined (_DEBUG)
	fileName = L"WaterPixelShader_d.cso";
#else	
	fileName = L"WaterPixelShader.cso";
#endif

	auto pixelShaderBlob = DX::ReadData(fileName);
	D3D12_SHADER_BYTECODE pixelShaderByteCode =
	{
		pixelShaderBlob.data(),
		pixelShaderBlob.size()
	};

	D3D12_INPUT_ELEMENT_DESC inputLayout[]
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		{
			"POSITION",
			1,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};

	D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSignature;
	rootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	value = m_directXSettings->m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE,
		&rootSignature, sizeof(rootSignature));
	if (FAILED(value))
	{
		rootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;

	CD3DX12_ROOT_PARAMETER1 rootParameters[5];
	rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
	rootParameters[1].InitAsConstants(sizeof(XMMATRIX) / 4, 1, 0, D3D12_SHADER_VISIBILITY_VERTEX);

	CD3DX12_DESCRIPTOR_RANGE1 ranges[3];
	ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

	rootParameters[2].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);
	rootParameters[3].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[4].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_VERTEX);

	D3D12_STATIC_SAMPLER_DESC staticSamplerDesc[2] = {};
	staticSamplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	staticSamplerDesc[0].Filter = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
	staticSamplerDesc[0].MaxAnisotropy = 0;
	staticSamplerDesc[0].MinLOD = 0;
	staticSamplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplerDesc[0].MipLODBias = 0;
	staticSamplerDesc[0].ShaderRegister = 0;
	staticSamplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	staticSamplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	staticSamplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplerDesc[1].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	staticSamplerDesc[1].Filter = D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
	staticSamplerDesc[1].MaxAnisotropy = 0;
	staticSamplerDesc[1].MinLOD = 0;
	staticSamplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplerDesc[1].MipLODBias = 0;
	staticSamplerDesc[1].ShaderRegister = 0;
	staticSamplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	staticSamplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
	rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 2, staticSamplerDesc, rootSignatureFlags);

	ComPtr<ID3DBlob> rootSignatureBlob;
	ComPtr<ID3DBlob> errorBlob;

	value = D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
		rootSignature.HighestVersion, &rootSignatureBlob, &errorBlob);
	if (FAILED(value))
	{
		return -70;
	}

	value = m_directXSettings->m_Device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
		rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_directXSettings->m_WaterShaderRootSignature));
	if (FAILED(value))
	{
		return -71;
	}

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 1;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

	CD3DX12_RASTERIZER_DESC rasterizerDsc
	{
		D3D12_FILL_MODE_SOLID,
		D3D12_CULL_MODE_BACK,
		false,
		D3D12_DEFAULT_DEPTH_BIAS,
		D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
		true,
		false,
		false,
		0,
		D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF
	};

	// SrcColor * SrcAlpha + DestColor * (1 - SrcAlpha)
	CD3DX12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC();
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_SUBTRACT;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC;
	CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER rasterizer =
		CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER(rasterizerDsc);

	CD3DX12_PIPELINE_STATE_STREAM pipelineStateStream;
	pipelineStateStream.pRootSignature = m_directXSettings->m_WaterShaderRootSignature.Get();
	pipelineStateStream.VS = vertexShaderByteCode;
	pipelineStateStream.PS = pixelShaderByteCode;
	pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateStream.RTVFormats = rtvFormats;
	pipelineStateStream.RasterizerState = rasterizer;
	pipelineStateStream.BlendState = blendDesc;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc =
	{
		sizeof(pipelineStateStream),
		&pipelineStateStream
	};

	value = m_directXSettings->m_Device->CreatePipelineState(&pipelineStateStreamDesc,
		IID_PPV_ARGS(&m_directXSettings->m_WaterShaderPipelineState));
	if (FAILED(value))
	{
		return -72;
	}
	return 0;
}

int CEngine::CreateLightConstantBuffer()
{
	int value;

	value = m_directXSettings->m_Device->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_directXSettings->m_LightConstantBufferResource)
	);
	if (FAILED(value))
	{
		return -101;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
	desc.BufferLocation = m_directXSettings->m_LightConstantBufferResource->GetGPUVirtualAddress();
	desc.SizeInBytes = (sizeof(m_directXSettings->m_LightConstantBufferResource) + 255) & ~255;

	CD3DX12_CPU_DESCRIPTOR_HANDLE handle =
	{
		m_directXSettings->m_ResourceDescriptors->GetCPUDescriptorHandleForHeapStart(),
		(int)EResourceType::LIGHT_CONSTANT_BUFFER,
		m_directXSettings->m_CBVDescriptorSize
	};

	m_directXSettings->m_Device->CreateConstantBufferView(&desc, handle);

	CD3DX12_RANGE readRange(0, 0);
	value = m_directXSettings->m_LightConstantBufferResource->Map(0, &readRange,
		reinterpret_cast<void**>(&m_directXSettings->m_LightConstantBufferDataBegin));
	if (FAILED(value))
		return -102;

	memcpy(m_directXSettings->m_LightConstantBufferDataBegin, &m_directXSettings->m_LightConstantBufferData,
		sizeof(m_directXSettings->m_LightConstantBufferData));

	return 0;
}

int CEngine::CreateWaterConstantBuffer()
{
	int value;

	value = m_directXSettings->m_Device->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_directXSettings->m_WaterConstantBufferResource)
	);
	if (FAILED(value))
	{
		return -201;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
	desc.BufferLocation = m_directXSettings->m_WaterConstantBufferResource->GetGPUVirtualAddress();
	desc.SizeInBytes = (sizeof(m_directXSettings->m_WaterConstantBufferResource) + 255) & ~255;

	CD3DX12_CPU_DESCRIPTOR_HANDLE handle =
	{
		m_directXSettings->m_ResourceDescriptors->GetCPUDescriptorHandleForHeapStart(),
		(int)EResourceType::WATER_CONSTANT_BUFFER,
		m_directXSettings->m_CBVDescriptorSize
	};

	m_directXSettings->m_Device->CreateConstantBufferView(&desc, handle);

	CD3DX12_RANGE readRange(0, 0);
	value = m_directXSettings->m_WaterConstantBufferResource->Map(0, &readRange,
		reinterpret_cast<void**>(&m_directXSettings->m_WaterConstantBufferDataBegin));
	if (FAILED(value))
		return -202;

	memcpy(m_directXSettings->m_WaterConstantBufferDataBegin, &m_directXSettings->m_WaterConstantBufferData,
		sizeof(m_directXSettings->m_WaterConstantBufferData));

	return 0;
}

void CEngine::Retry(Button2D* _button)
{
	m_gameLost = false;
	m_gameWon = false;
	m_player->inBlackHole = false;
	m_player->SetPos(XMFLOAT3 (-120, 40, 0));
	m_player->SetRot(XMFLOAT3(0, 0, 0));
	//if(!m_nextLevelStarted)

	m_LoseScreen->SetPos(XMFLOAT2(10000, 10000));

	m_winScreen->SetPos(XMFLOAT2(10000, 10000));
	m_retryButton->SetPos(XMFLOAT2(10000, 10000));
	m_nextLevelButton->SetPos(XMFLOAT2(10000, 10000));
	m_quitButton->SetPos(XMFLOAT2(10000, 10000));
	m_menuScreen->SetPos(XMFLOAT2(10000, 10000));
	m_startButton->SetPos(XMFLOAT2(10000, 10000));
	m_tutorialText->SetPos(XMFLOAT2(10000, 10000));
	
	static std::random_device rd;
	static std::mt19937 gen(rd());

	std::uniform_int_distribution<> dis(-50, 50);

	int i = 1;
	for (Meteor* c : m_meteors)
	{
		int rndnum = dis(gen);
		c->SetPos(XMFLOAT3(rndnum, i * 30, 0));
		i++;
	}

}

void CEngine::LoadNextLevel(Button2D* _button)
{
	m_camera->SetPos(XMFLOAT3(0, -30, -10));
	m_nextLevelStarted = true;
	m_gameLost = false;
	m_gameWon = false;
	m_player->inBlackHole = false;

	m_player->SetPos(XMFLOAT3(1000, 10000, 10000));

	for (Meteor* m : m_meteors)
	{
		m->SetPos(XMFLOAT3(10000, 10000, 10000));
	}
	m_blackHole->SetPos(XMFLOAT3(10000, 10000, 10000));
	m_goalPlanet->SetPos(XMFLOAT3(10000, 10000, 10000));

	m_LoseScreen->SetPos(XMFLOAT2(10000, 10000));
	m_winScreen->SetPos(XMFLOAT2(10000, 10000));
	m_retryButton->SetPos(XMFLOAT2(10000, 10000));
	m_nextLevelButton->SetPos(XMFLOAT2(10000, 10000));
	m_quitButton->SetPos(XMFLOAT2(10000, 10000));
	m_menuScreen->SetPos(XMFLOAT2(10000, 10000));
	m_startButton->SetPos(XMFLOAT2(10000, 10000));
	m_tutorialText->SetPos(XMFLOAT2(10000, 10000));

	XMFLOAT3 offset = XMFLOAT3(-1000, -1000, -1000);

	for (Cuboid* c : m_buildings)
	{
		c->AddToPos(offset);
	}

	for (Quad* q : m_grasses)
	{
		q->AddToPos(offset);
	}

	for (WaterPlane* w : m_waters)
	{
		w->AddToPos(offset);
	}

	m_planetArrived->SetPos(XMFLOAT3(0, 0, 0));
}



