#include "pch.h"
#include <iostream>
#include <filesystem>
#include <fstream>

#define NOMINMAX

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <d3d11.h>
#include <directxmath.h>
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

static ID3D11VertexShader* simplequad_vs{ nullptr };
static ID3D11PixelShader* simplequad_ps{ nullptr };
static ID3D11InputLayout* simplequad_inputlayout{ nullptr };
static ID3D11Buffer* simplequad_vertexbuffer{ nullptr };
static ID3D11Buffer* simplequad_indexbuffer{ nullptr };
static ID3D11RasterizerState* simplequad_rasterizerstate{ nullptr };
static ID3D11SamplerState* simplequad_samplerstate{ nullptr };
static ID3D11ShaderResourceView* img_buffer_srv{ nullptr };

// dx helper functions
bool CreateDeviceD3D(HWND hWnd, void* img_buffer);
void CleanupDeviceD3D();
bool CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ThrowIfFailed(HRESULT hr, const char* const message = "");

// RayTracing
void StartRendering(ThreadPool& threadpool, std::size_t samples_per_pixel, std::size_t bounce_limit, Camera& camera, IRenderObject& world, std::uint8_t* img_buffer);

int main(int, char**)
{
	std::uint8_t* img_buffer = new std::uint8_t[img_width * img_height * 4];

#pragma region INIT DX11 & IMGUI

	// Create application window
	//ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, _T("Ray Tracer"), nullptr };
	::RegisterClassEx(&wc);
	HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("RayTracer"), WS_OVERLAPPEDWINDOW, 100, 100, img_width, img_height, nullptr, nullptr, wc.hInstance, nullptr);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd, img_buffer))
	{
		delete[] img_buffer;
		CleanupDeviceD3D();
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
	////io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	////io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	//// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	D3D11_MAPPED_SUBRESOURCE mapped_resource;
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
#pragma endregion

	bool renderstarted = false;
	std::unique_ptr<ThreadPool> threadpool = std::make_unique<ThreadPool>(std::max(std::thread::hardware_concurrency(), 2u) - 1u);

	int gui_samplesperpixel = 32;
	int gui_bouncelimit = 8;

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

		g_pd3dDeviceContext->DrawIndexed(6, 0, 0);

		{
			ImGui::SetNextWindowPos(ImVec2{ 10,10 });

			ImGui::Begin("thou scene.", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);

			if (!renderstarted)
			{
				const float clear_color[4] = { 0, 0, 0, 1 };
				g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);

				if (ImGui::Button("let us begin."))
				{
					renderstarted = true;
					StartRendering(*threadpool, static_cast<std::size_t>(gui_samplesperpixel), static_cast<std::size_t>(gui_bouncelimit), camera, world, img_buffer);
				}

				ImGui::InputInt("Samples per Pixel", &gui_samplesperpixel);
				ImGui::InputInt("Max Bounces", &gui_bouncelimit);
			}
			else
			{
				if (ImGui::Button("thats enough."))
				{
					renderstarted = false;
					threadpool.reset(new ThreadPool{ std::max(std::thread::hardware_concurrency(), 2u) - 1u });
				}

				ID3D11Resource* img_buffer_resource;
				img_buffer_srv->GetResource(&img_buffer_resource);

				g_pd3dDeviceContext->Map(img_buffer_resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
				memcpy(mapped_resource.pData, img_buffer, img_width * img_height * 4 * sizeof(uint8_t));
				g_pd3dDeviceContext->Unmap(img_buffer_resource, 0);
			}

			ImGui::Text("%.3f ms / %.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			ImGui::End();
		}
		
		// render imgui
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync
	}

	// Cleanup
	delete threadpool.release();
	delete[] img_buffer;
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}

// RayTracing

void StartRendering(ThreadPool& threadpool, std::size_t samples_per_pixel, std::size_t bounce_limit, Camera& camera, IRenderObject& world, std::uint8_t* img_buffer)
{
	std::memset(img_buffer, 255, img_width * img_height * 4 * sizeof(std::uint8_t));

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

// Helper functions

bool CreateDeviceD3D(HWND hWnd, void* img_buffer)
{
#pragma region Swap Chain
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
#pragma endregion

	// create render target
	if (!(CreateRenderTarget())) return false;

#pragma region VertexShader
	std::vector<char> compiledVertexShader;
	std::ifstream vsfile("simplequad_vs.cso", std::ios::binary);
	if (!vsfile.good()) throw std::runtime_error("Could not open vertex shader");

	vsfile.seekg(0, std::ios::end);
	std::uint32_t size = (std::uint32_t)vsfile.tellg();

	if (size > 0)
	{
		compiledVertexShader.resize(size);
		vsfile.seekg(0, std::ios::beg);
		vsfile.read(&compiledVertexShader.front(), size);
	}

	vsfile.close();
	ThrowIfFailed(g_pd3dDevice->CreateVertexShader(&compiledVertexShader[0], compiledVertexShader.size(), nullptr, &simplequad_vs), "CreateVertexShader Failed");
#pragma endregion

#pragma region PixelShader
	std::vector<char> compiledPixelShader;
	std::ifstream psfile("simplequad_ps.cso", std::ios::binary);
	if (!psfile.good()) throw std::runtime_error("Could not open pixel shader");

	psfile.seekg(0, std::ios::end);
	size = (std::uint32_t)psfile.tellg();

	if (size > 0)
	{
		compiledPixelShader.resize(size);
		psfile.seekg(0, std::ios::beg);
		psfile.read(&compiledPixelShader.front(), size);
	}

	psfile.close();
	ThrowIfFailed(g_pd3dDevice->CreatePixelShader(&compiledPixelShader[0], compiledPixelShader.size(), nullptr, &simplequad_ps), "CreatePixelShader Failed");
#pragma endregion

#pragma region Input Layout
	const D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ThrowIfFailed(g_pd3dDevice->CreateInputLayout(inputElementDescriptions, sizeof(inputElementDescriptions) / sizeof(inputElementDescriptions[0]), compiledVertexShader.data(), compiledVertexShader.size(), &simplequad_inputlayout), "CreateInputLayout Failed");
#pragma endregion

	struct vertexpostex final
	{
		DirectX::XMFLOAT4 _pos;
		DirectX::XMFLOAT2 _texuv;
	};

#pragma region Vertex Buffer
	const vertexpostex vertex_positiontexture[] =
	{
		DirectX::XMFLOAT4(-1, 1, 0, 1),		// Top Left
		DirectX::XMFLOAT2(0, 0),

		DirectX::XMFLOAT4(1, 1, 0, 1),		// Top Right
		DirectX::XMFLOAT2(1, 0),

		DirectX::XMFLOAT4(-1, -1, 0, 1),	// Bot Left
		DirectX::XMFLOAT2(0, 1),

		DirectX::XMFLOAT4(1, -1, 0, 1),		// Bot Right
		DirectX::XMFLOAT2(1, 1)
	};

	D3D11_BUFFER_DESC vertexbuffer_desc{ 0 };
	vertexbuffer_desc.ByteWidth = sizeof(vertexpostex) * 4;
	vertexbuffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexbuffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertex_subresource{ 0 };
	vertex_subresource.pSysMem = vertex_positiontexture;
	ThrowIfFailed(g_pd3dDevice->CreateBuffer(&vertexbuffer_desc, &vertex_subresource, &simplequad_vertexbuffer), "CreateVertexBuffer Failed");
#pragma endregion

#pragma region Index Buffer
	const unsigned long indices[] =
	{
		0,1,2,
		2,1,3
	};

	D3D11_BUFFER_DESC indexbuffer_desc{ 0 };
	indexbuffer_desc.ByteWidth = sizeof(unsigned long) * 6;
	indexbuffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	indexbuffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA index_subresource{ 0 };
	index_subresource.pSysMem = indices;
	ThrowIfFailed(g_pd3dDevice->CreateBuffer(&indexbuffer_desc, &index_subresource, &simplequad_indexbuffer), "CreateIndexBuffer Failed");
#pragma endregion

	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);

#pragma region Rasterizer State
	D3D11_RASTERIZER_DESC rasterizer_desc;
	rasterizer_desc.AntialiasedLineEnable = false;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0.0f;
	rasterizer_desc.DepthClipEnable = true;
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.FrontCounterClockwise = false;
	rasterizer_desc.MultisampleEnable = false;
	rasterizer_desc.ScissorEnable = false;
	rasterizer_desc.SlopeScaledDepthBias = 0.0f;

	ThrowIfFailed(g_pd3dDevice->CreateRasterizerState(&rasterizer_desc, &simplequad_rasterizerstate), "CreateRasterizerState Failed");
	g_pd3dDeviceContext->RSSetState(simplequad_rasterizerstate);
#pragma endregion

#pragma region Screen Viewport
	D3D11_VIEWPORT screen_viewport;
	screen_viewport.Width = static_cast<float>(img_width);
	screen_viewport.Height = static_cast<float>(img_height);
	screen_viewport.MinDepth = 0.0f;
	screen_viewport.MaxDepth = 1.0f;
	screen_viewport.TopLeftX = 0.0f;
	screen_viewport.TopLeftY = 0.0f;

	g_pd3dDeviceContext->RSSetViewports(1, &screen_viewport);
#pragma endregion

#pragma region Sampler State
	D3D11_SAMPLER_DESC texsampler_description;
	texsampler_description.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	texsampler_description.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	texsampler_description.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	texsampler_description.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	texsampler_description.MipLODBias = 0.0f;
	texsampler_description.MaxAnisotropy = 1;
	texsampler_description.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	texsampler_description.BorderColor[0] = 0;
	texsampler_description.BorderColor[1] = 0;
	texsampler_description.BorderColor[2] = 0;
	texsampler_description.BorderColor[3] = 0;
	texsampler_description.MinLOD = 0;
	texsampler_description.MaxLOD = D3D11_FLOAT32_MAX;

	ThrowIfFailed(g_pd3dDevice->CreateSamplerState(&texsampler_description, &simplequad_samplerstate), "CreateSamplerState Failed");
#pragma endregion

#pragma region Shader Resource View for Texture
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
	ThrowIfFailed(g_pd3dDevice->CreateTexture2D(&tex_description, &tex_resource, &texture), "CreateTexture2D Failed");
	if (texture == nullptr) return false;
	ThrowIfFailed(g_pd3dDevice->CreateShaderResourceView(texture, &srv_desc, &img_buffer_srv), "CreateShaderResourceView Failed");
	texture->Release();
#pragma endregion

	g_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_pd3dDeviceContext->IASetInputLayout(simplequad_inputlayout);

	const uint32_t stride = sizeof(vertexpostex);
	const uint32_t offset = 0;
	g_pd3dDeviceContext->IASetVertexBuffers(0, 1, &simplequad_vertexbuffer, &stride, &offset);
	g_pd3dDeviceContext->IASetIndexBuffer(simplequad_indexbuffer, DXGI_FORMAT_R32_UINT, 0);

	g_pd3dDeviceContext->VSSetShader(simplequad_vs, nullptr, 0);
	g_pd3dDeviceContext->PSSetShader(simplequad_ps, nullptr, 0);

	g_pd3dDeviceContext->PSSetSamplers(0, 1, &simplequad_samplerstate);
	g_pd3dDeviceContext->PSSetShaderResources(0, 1, &img_buffer_srv);

	return true;
}

bool CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer{ nullptr };
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer == nullptr) return false;
	ThrowIfFailed(g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView), "CreateRenderTargetView Failed");
	pBackBuffer->Release();

	return true;
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView != nullptr) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}


void CleanupDeviceD3D()
{
	if (img_buffer_srv != nullptr) { img_buffer_srv->Release(); img_buffer_srv = nullptr; }
	CleanupRenderTarget();
	if (simplequad_vs != nullptr) { simplequad_vs->Release(); simplequad_vs = nullptr; }
	if (simplequad_ps != nullptr) { simplequad_ps->Release(); simplequad_ps = nullptr; }
	if (simplequad_inputlayout != nullptr) { simplequad_inputlayout->Release(); simplequad_inputlayout = nullptr; }
	if (simplequad_vertexbuffer != nullptr) { simplequad_vertexbuffer->Release(); simplequad_vertexbuffer = nullptr; }
	if (simplequad_indexbuffer != nullptr) { simplequad_indexbuffer->Release(); simplequad_indexbuffer = nullptr; }
	if (simplequad_rasterizerstate != nullptr) { simplequad_rasterizerstate->Release(); simplequad_rasterizerstate = nullptr; }
	if (simplequad_samplerstate != nullptr) { simplequad_samplerstate->Release(); simplequad_samplerstate = nullptr; }
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
			//CleanupRenderTarget();
			//g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			//CreateRenderTarget();
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

inline void ThrowIfFailed(HRESULT hr, const char* const message)
{
	if (FAILED(hr))
	{
		throw std::runtime_error(message);
	}
}