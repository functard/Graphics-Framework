#pragma once

#include <Windows.h>
#include <shellapi.h>

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

#include <wrl.h>
using namespace Microsoft::WRL;

// DirectX 12
#include <d3d12.h>	
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "d3dx12.h"

using namespace DirectX;

#include "WICTextureLoader.h"
#include "ResourceUploadBatch.h"
#include "CommonStates.h"
#include "DirectXHelpers.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"

// STD
#include <algorithm>
#include <chrono>