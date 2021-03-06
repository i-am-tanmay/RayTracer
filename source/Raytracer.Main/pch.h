#pragma once

#include <iostream>
#include <fstream>
#include <assert.h>
#include <cmath>

#define NOMINMAX

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <d3d11.h>
#include <directxmath.h>
#include <tchar.h>

#pragma warning (push, 0)
#pragma warning(disable : 26439 26812)
#include <oidn\oidn.hpp>
#pragma warning (pop)