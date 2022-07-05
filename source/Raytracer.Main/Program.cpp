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
#include "BVHNode.h"
#include "Sphere.h"
#include "Rect.h"
#include "Cuboid.h"
#include "ConstantDensityMedium.h"

#include "Translate.h"
#include "Rotate.h"

#include "Material_Lambertian.h"
#include "Material_Metal.h"
#include "Material_Dielectric.h"
#include "Material_Light_Diffuse.h"

#pragma warning (push, 0)
#pragma warning(disable : 6262 26451 26819)
#define STB_IMAGE_IMPLEMENTATION
#include "ImageTexture.h"
#pragma warning (pop)

#pragma warning(push)
#pragma warning(disable : 4996 26819 6385 6386 26451)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#pragma warning(pop)

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
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
bool CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ThrowIfFailed(HRESULT hr, const char* const message = "");

void GetWorld(std::vector<std::shared_ptr<IRenderObject>>& world, int)
{
	// cornell box

	std::shared_ptr<Material_Lambertian> red = std::make_shared<Material_Lambertian>(color(.65, .05, .05));
	std::shared_ptr<Material_Lambertian> white = std::make_shared<Material_Lambertian>(color(.73, .73, .73));
	std::shared_ptr<Material_Lambertian> green = std::make_shared<Material_Lambertian>(color(.12, .45, .15));
	std::shared_ptr<Material_Light_Diffuse> light = std::make_shared<Material_Light_Diffuse>(color(15, 15, 15));

	world.push_back(std::make_shared<Rect_YZ>(0, 555, 0, 555, 555, green));
	world.push_back(std::make_shared<Rect_YZ>(0, 555, 0, 555, 0, red));
	world.push_back(std::make_shared<Rect_XZ>(0, 555, 0, 555, 0, white));
	world.push_back(std::make_shared<Rect_XZ>(0, 555, 0, 555, 555, white));
	world.push_back(std::make_shared<Rect_XY>(0, 555, 0, 555, 555, white));

	std::shared_ptr<IRenderObject> cuboid_1 = std::make_shared<Cuboid>(vec3{ 0,0,0 }, 165, 165, 165, white);
	cuboid_1 = std::make_shared<Rotate_Y>(cuboid_1, -18);
	cuboid_1 = std::make_shared<Translate>(cuboid_1, vec3{ 212.5, 82.5, 147.5 });
	cuboid_1 = std::make_shared<ConstantDensityMedium>(cuboid_1, color{ 1,1,1 }, .01);
	world.push_back(cuboid_1);

	std::shared_ptr<IRenderObject> cuboid_2 = std::make_shared<Cuboid>(vec3{ 0,0,0 }, 165, 330, 165, white);
	cuboid_2 = std::make_shared<Rotate_Y>(cuboid_2, 15);
	cuboid_2 = std::make_shared<Translate>(cuboid_2, vec3{ 347.5, 165, 377.5 });
	cuboid_2 = std::make_shared<ConstantDensityMedium>(cuboid_2, color{ 0,0,0 }, .01);
	world.push_back(cuboid_2);

	world.push_back(std::make_shared<Rect_XZ>(213, 343, 227, 332, 554, light));
}

int main(int, char**)
{
	std::uint8_t* img_buffer = new std::uint8_t[img_width * img_height * 4];

#pragma region INIT DX11 & IMGUI

	// Create application window
	//ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, _T("Ray Tracer"), nullptr };
	::RegisterClassEx(&wc);
	RECT window_rect{ 0,0,img_width, img_height };
	AdjustWindowRect(&window_rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);
	HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("RayTracer"), WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 100, 100, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, nullptr, nullptr, wc.hInstance, nullptr);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
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

	ID3D11Resource* img_buffer_resource;
	img_buffer_srv->GetResource(&img_buffer_resource);
#pragma endregion

#pragma region RayTracing

	// WORLD
	Camera camera{ pos3 {278,278,-850}, pos3{278,278,0}, 40.0, vec3{0,1,0} };
	std::vector<std::shared_ptr<IRenderObject>> world;
	GetWorld(world, 0);

	// // --------------------------------------------------------------------------------------------------------------------------------------

	BVHNode world_bvh{ world };

	// RENDER IMAGE
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

		{
			ImGui::SetNextWindowPos(ImVec2{ 10,10 });

			ImGui::Begin("thou scene.", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);

			if (!renderstarted)
			{
				//const float clear_color[4] = { 0, 0, 0, 1 };
				//g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);

				if (ImGui::Button("let us begin."))
				{
					renderstarted = true;

					std::memset(img_buffer, 255, img_width * img_height * 4 * sizeof(std::uint8_t));

					precision samples_inverse = 1.0 / gui_samplesperpixel;

					for (std::size_t i = 0; i < img_height; ++i)
					{
						for (std::size_t ii = 0; ii < img_width; ++ii)
						{
							threadpool->EnqueueTask([&, i, ii, samples_inverse]
								{
									color pixel_color{ 0,0,0 };

									for (int sample = 0; sample < gui_samplesperpixel; ++sample)
									{
										precision u = (ii + get_random01()) / (img_width - 1);
										precision v = ((img_height - i - 1) + get_random01()) / (img_height - 1);
										pixel_color += ray_color(camera.get_ray(u, v), world_bvh, static_cast<std::size_t>(gui_bouncelimit));
									}

									write_color(&img_buffer[(i * img_width + ii) * 4], pixel_color, samples_inverse);
								});
						}
					}
				}

				if (ImGui::Button("wallpaper engine."))
				{
					std::cout << "\ntime to write to file xo\n";
					if (stbi_write_png("../../../../outputimage.png", img_width, img_height, 4, img_buffer, img_width * 4) == 0) std::cout << "couldn't write to PNG lolol";
					else std::cout << "fin.\n";
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

				g_pd3dDeviceContext->Map(img_buffer_resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
				std::memcpy(mapped_resource.pData, img_buffer, img_width * img_height * 4 * sizeof(std::uint8_t));
				g_pd3dDeviceContext->Unmap(img_buffer_resource, 0);
			}

			ImGui::Text("%.3f ms / %.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			ImGui::End();
		}

		g_pd3dDeviceContext->DrawIndexed(6, 0, 0);

		// render imgui
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync
	}

	// Cleanup
	img_buffer_resource->Release();
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

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
#pragma region Swap Chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));

	// back buffer init
	sd.BufferCount = 1;
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

	// texture view description
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	ZeroMemory(&srv_desc, sizeof(srv_desc));
	srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1;
	srv_desc.Texture2D.MostDetailedMip = 0;

	// create the texture view
	ID3D11Texture2D* texture{ nullptr };
	ThrowIfFailed(g_pd3dDevice->CreateTexture2D(&tex_description, nullptr, &texture), "CreateTexture2D Failed");
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
		std::cerr << message;
		throw std::runtime_error(message);
	}
}