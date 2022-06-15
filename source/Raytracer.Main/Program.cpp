#include "pch.h"
#include <iostream>
#include <fstream>

#define NOMINMAX

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>

#include "common.h"
#include "ThreadPool.h"
#include "vec3.h"
#include "helper.h"
#include "Ray.h"
#include "Camera.h"
#include "RenderObjectList.h"
#include "Sphere.h"
#include "Material_Lambertian.h"
#include "Material_Metal.h"
#include "Material_Dielectric.h"

using namespace Library;

const static std::size_t img_width = 1280;
const static std::size_t img_height = static_cast<std::size_t>(img_width / aspect_ratio);

// dx11
static ID3D11Device* g_pd3dDevice{ nullptr };
static ID3D11DeviceContext* g_pd3dDeviceContext{ nullptr };
static IDXGISwapChain* g_pSwapChain{ nullptr };
static ID3D11RenderTargetView* g_mainRenderTargetView{ nullptr };
// dx helper functions
bool CreateDeviceD3D(HWND hWnd, void* img_buffer, ID3D11ShaderResourceView*& img_buffer_srv);
void CleanupDeviceD3D(ID3D11ShaderResourceView*& img_buffer_srv);
bool CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


int main(int, char**)
{
	std::uint8_t* img_buffer = new std::uint8_t[img_width * img_height * 4];
	ImVec2 windowsize{ img_width, img_height };

	// RAYTRACING PROPERTIES
	const std::size_t samples_per_pixel = 32;
	const std::size_t bounce_limit = 8;

#pragma region INIT DX11 & IMGUI

	// Create application window
	//ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, _T("Ray Tracer"), nullptr };
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("RayTracer"), WS_OVERLAPPEDWINDOW, 100, 100, img_width, img_height, nullptr, nullptr, wc.hInstance, nullptr);

	ID3D11ShaderResourceView* img_buffer_srv{ nullptr };

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd, img_buffer, img_buffer_srv))
	{
		delete[] img_buffer;
		CleanupDeviceD3D(img_buffer_srv);
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImVec4 clear_color = ImVec4(0, 0, 0, 1);
	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

#pragma endregion

#pragma region RayTracing

	// WORLD
	RenderObjectList world;

	// ------------------------------------------------------------ generate random world ------------------------------------------------------
	std::shared_ptr<Material_Lambertian> ground_material = std::make_shared<Material_Lambertian>(color(0.5, 0.5, 0.5));
	world.add(std::make_shared<Sphere>(pos3(0, -1000, 0), 1000, ground_material));

	for (std::int32_t i = -11; i < 11; ++i)
	{
		for (std::int32_t ii = -11; ii < 11; ++ii)
		{
			precision choose_mat = get_random01();
			pos3 center(i + 0.9 * get_random01(), 0.2, ii + 0.9 * get_random01());

			if ((center - pos3(4, 0.2, 0)).length() > 0.9) {
				std::shared_ptr<Material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = std::make_shared<Material_Lambertian>(albedo);
					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = get_random(0, 0.5);
					sphere_material = std::make_shared<Material_Metal>(albedo, fuzz);
					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = std::make_shared<Material_Dielectric>(1.5);
					world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	std::shared_ptr<Material_Dielectric> material1 = std::make_shared<Material_Dielectric>(1.5);
	world.add(std::make_shared<Sphere>(pos3(0, 1, 0), 1.0, material1));

	std::shared_ptr<Material_Lambertian> material2 = std::make_shared<Material_Lambertian>(color(0.4, 0.2, 0.1));
	world.add(std::make_shared<Sphere>(pos3(-4, 1, 0), 1.0, material2));

	std::shared_ptr<Material_Metal> material3 = std::make_shared<Material_Metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(std::make_shared<Sphere>(pos3(4, 1, 0), 1.0, material3));
	// // --------------------------------------------------------------------------------------------------------------------------------------

	// RENDER IMAGE
	Camera camera{ pos3 {13,2,3}, pos3{0,0,0}, 20.0, vec3{0,1,0}, 0.1, 10.0 };

	ThreadPool threadpool{ std::max(std::thread::hardware_concurrency(),2u) - 1u };

	bool renderstarted = false;
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);

#pragma endregion

	// Main loop
	bool done = false;
	while (!done)
	{
#pragma region Handle Messages

		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done) break;

#pragma endregion

#pragma region imgui UI init
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
#pragma endregion

		// render stuff START

		{
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(windowsize, ImGuiCond_Always);
			ImGui::Begin("output image", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);

			ID3D11Resource* img_buffer_resource;
			img_buffer_srv->GetResource(&img_buffer_resource);

			D3D11_MAPPED_SUBRESOURCE mapped_resource;
			g_pd3dDeviceContext->Map(img_buffer_resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
			memcpy(mapped_resource.pData, img_buffer, img_width * img_height * 4 * sizeof(uint8_t));
			g_pd3dDeviceContext->Unmap(img_buffer_resource, 0);

			ImGui::Image(img_buffer_srv, windowsize);

			ImGui::End();
		}

		// render stuff END

#pragma region imgui UIs

		{
			ImGui::Begin("thou scene.");

			if (ImGui::Button("let us begin.") && !renderstarted)
			{
				renderstarted = true;

				for (std::size_t i = 0; i < img_height; ++i)
				{
					for (std::size_t ii = 0; ii < img_width; ++ii)
					{
						threadpool.EnqueueTask([&, i, ii]
							{
								color pixel_color{ 0,0,0 };

								for (std::size_t sample = 0; sample < samples_per_pixel; ++sample)
								{
									precision u = (ii + get_random01()) / (img_width - 1);
									precision v = ((img_height - i - 1) + get_random01()) / (img_height - 1);
									pixel_color += ray_color(camera.get_ray(u, v), world, bounce_limit);
								}

								write_color(&img_buffer[(i * img_width + ii) * 4], pixel_color, samples_per_pixel);
							});
					}
				}
			}

			ImGui::Text("%.3f ms / %.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			ImGui::End();
		}

#pragma endregion

		// render imgui
		ImGui::Render();
		//const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		//g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync
	}

	// Cleanup
	threadpool.Stop();
	delete[] img_buffer;
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D(img_buffer_srv);
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd, void* img_buffer, ID3D11ShaderResourceView*& img_buffer_srv)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));

	// back buffer init
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;

	sd.Flags = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;

	// multisampling off
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;

	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &featureLevel, 1, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, nullptr, &g_pd3dDeviceContext) != S_OK)
		return false;

	// create render target
	if (!(CreateRenderTarget())) return false;

	// texture description
	D3D11_TEXTURE2D_DESC tex_description;
	ZeroMemory(&tex_description, sizeof(tex_description));
	tex_description.Width = static_cast<UINT>(img_width);
	tex_description.Height = static_cast<UINT>(img_height);
	tex_description.MipLevels = 1;
	tex_description.ArraySize = 1;
	tex_description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex_description.SampleDesc.Count = 1;
	tex_description.Usage = D3D11_USAGE_DYNAMIC;
	tex_description.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex_description.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// bind image buffer to texture sub-resource
	D3D11_SUBRESOURCE_DATA tex_resource;
	tex_resource.pSysMem = img_buffer;
	tex_resource.SysMemPitch = tex_description.Width * 4;
	tex_resource.SysMemSlicePitch = 0;

	// texture view description
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	ZeroMemory(&srv_desc, sizeof(srv_desc));
	srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1;
	srv_desc.Texture2D.MostDetailedMip = 0;

	// create the texture view
	ID3D11Texture2D* texture{ nullptr };
	g_pd3dDevice->CreateTexture2D(&tex_description, &tex_resource, &texture);
	if (texture == nullptr) return false;
	g_pd3dDevice->CreateShaderResourceView(texture, &srv_desc, &img_buffer_srv);
	texture->Release();

	return true;
}

bool CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer{ nullptr };
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer == nullptr) return false;
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();

	return true;
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView != nullptr) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}


void CleanupDeviceD3D(ID3D11ShaderResourceView*& img_buffer_srv)
{
	if (img_buffer_srv != nullptr) { img_buffer_srv->Release(); img_buffer_srv = nullptr; }
	CleanupRenderTarget();
	if (g_pSwapChain != nullptr) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext != nullptr) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice != nullptr) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}