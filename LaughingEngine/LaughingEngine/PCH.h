#pragma once

#include <winsdkver.h>
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>

#define NOMINMAX

#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

#define NOHELP

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <wrl/client.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include "d3dx12.h"

#ifdef _DEBUG
#include <dxgidebug.h>
#endif // _DEBUG

#define D3D12_GPU_VIRTUAL_ADDRESS_NULL      ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

#include <string>
#include <vector>
#include <queue>
#include <comdef.h>
#include <fstream>

#include "Utility.h"
#include "FileUtility.h"
#include "TSingleton.h"

#include "Common.h"