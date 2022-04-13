#include "game.h"
#include <iostream>
#include <dcomp.h>
#include <uuids.h>
Game* Game::INSTANCE = nullptr;
Game* Game::getInstance() {
	if(Game::INSTANCE == nullptr) {
		Game::INSTANCE = new Game();
	}
	return Game::INSTANCE;
}
void Game::releaseInstance() {
	if(INSTANCE != nullptr){
		delete INSTANCE;
	}
}

Game::Game() {
}


bool Game::init() {
	if(!createWindow()){
		return false;
	}
	if(!createDevice()){
		return false;
	}

	default_camera = Camera::defaultCamera();
	createPerspectiveMatrix();
	sprite = std::make_shared<SimpleSprite>();
	sprite->init(L"resource/img/background.jpg",Matrix::Identity);

	return true;
}

static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	Game * instance = Game::getInstance();
	if (instance->handleEvent(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (instance->getDevice() != NULL && wParam != SIZE_MINIMIZED)
        {
			instance->resize( (UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
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
bool Game::createWindow() {
	 WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, TEXT("showMe"), NULL };
    ::RegisterClassEx(&wc);
    hWnd = ::CreateWindowEx(WS_EX_NOREDIRECTIONBITMAP,wc.lpszClassName, TEXT("such a title"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
	return true;
}

bool Game::createDevice() {
	HRESULT hr;
	DXGI_SWAP_CHAIN_DESC1 description={};
	description.Format           = DXGI_FORMAT_B8G8R8A8_UNORM;     
	description.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	description.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	description.BufferCount      = 2;                              
	description.SampleDesc.Count = 1;                              
	description.AlphaMode        = DXGI_ALPHA_MODE_PREMULTIPLIED;
	RECT rect = {};
	GetClientRect(hWnd, &rect);
	description.Width  = rect.right - rect.left;  
	description.Height = rect.bottom - rect.top;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION,cp_device.GetAddressOf(), &featureLevel, cp_device_context.GetAddressOf()) != S_OK)
        return false;
	
	ComPtr<IDXGIDevice> dxgiDevice;
	hr = cp_device.As(&dxgiDevice);
	ComPtr<IDXGIFactory2>  dxFactory;
	CreateDXGIFactory2( DXGI_CREATE_FACTORY_DEBUG, __uuidof(dxFactory), reinterpret_cast<void **>(dxFactory.GetAddressOf()));
	hr = DCompositionCreateDevice(dxgiDevice.Get(),__uuidof(cp_dcd),reinterpret_cast<void**>(cp_dcd.GetAddressOf()));
	hr = cp_dcd->CreateTargetForHwnd(hWnd,true,cp_dct.GetAddressOf());
	hr = cp_dcd->CreateVisual(cp_dcv.GetAddressOf());

	dxFactory->CreateSwapChainForComposition(dxgiDevice.Get(),
                                            &description,
                                            nullptr, // Don’t restrict
                                            cp_swap_chain.GetAddressOf());


	return createRenderTargetView();
}

bool Game::createRenderTargetView() {

	HRESULT hr;
	ID3D11Texture2D* pBackBuffer;
	cp_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    cp_device->CreateRenderTargetView(pBackBuffer, NULL,cp_rtv.ReleaseAndGetAddressOf());

	ZeroMemory(&view_port, sizeof(D3D11_VIEWPORT));
	D3D11_TEXTURE2D_DESC td;
	pBackBuffer->GetDesc(&td);
	view_port.Height = (float)(td.Height);
	view_port.Width = (float) (td.Width);
	view_port.MinDepth = 0;
	view_port.MaxDepth = 1;

	CD3D11_DEFAULT cd;
	CD3D11_DEPTH_STENCIL_DESC sd(cd);
	sd.DepthEnable = true;
	hr = cp_device->CreateDepthStencilState(&sd,cp_dss.ReleaseAndGetAddressOf());

	CD3D11_TEXTURE2D_DESC ttd(DXGI_FORMAT_D24_UNORM_S8_UINT,(float)(td.Width),(float)(td.Height));
	ttd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ComPtr<ID3D11Texture2D> dt;
	hr = cp_device->CreateTexture2D(&ttd,NULL,dt.GetAddressOf());

	CD3D11_DEPTH_STENCIL_VIEW_DESC svd(D3D11_DSV_DIMENSION_TEXTURE2D);
	hr = cp_device->CreateDepthStencilView(dt.Get(),&svd,cp_dsv.ReleaseAndGetAddressOf());

	pBackBuffer->Release();
	return true;
}

ComPtr<ID3D11Device> Game::getDevice() {
	return cp_device;
}
ComPtr<ID3D11DeviceContext> Game::getDeviceContext() {
	return cp_device_context;
}
ComPtr<ID3D11RenderTargetView> Game::getRTV() {
	return cp_rtv;
}
ComPtr<IDXGISwapChain1> Game::getSwapChain() {
	return cp_swap_chain;
}

void Game::show() {
	ShowWindow(hWnd,SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
}

void Game::resize(UINT width,UINT heigth) {
	cp_swap_chain->ResizeBuffers(0,width,heigth,DXGI_FORMAT_UNKNOWN,0);
	createRenderTargetView();
	setViewPort();
}
void Game::loop() {
	bool done = false;
	while(!done) {
		MSG msg;
		while(PeekMessage(&msg,NULL,0U,0U,PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if(msg.message == WM_QUIT) {
				done = true;
			}
		}
		if(done) {
			break;
		}
		render();
		cp_swap_chain->Present(0,0);
		cp_dcv->SetContent(cp_swap_chain.Get());
		cp_dct->SetRoot(cp_dcv.Get());
		cp_dcd->Commit();
	}
}

void Game::render(){
	float bc[] = {0.f,0.0f,0.f,0.f};
	cp_device_context->ClearRenderTargetView(cp_rtv.Get(),bc);
	cp_device_context->ClearDepthStencilView(cp_dsv.Get(),D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,1.f,0);
	cp_device_context->OMSetRenderTargets(1,cp_rtv.GetAddressOf(),cp_dsv.Get());
	cp_device_context->OMSetDepthStencilState(cp_dss.Get(),0);
	sprite->render();
}

bool Game::handleEvent(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam){
	//todo
	return false;
}

void Game::getWindowSize(UINT& width,UINT & height) {
	RECT rect;
	GetClientRect(hWnd,&rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
}

void Game::createPerspectiveMatrix() {
	UINT x,y;
	getViewSize(x,y);
	default_persperctive_matrix = Matrix::CreatePerspectiveFieldOfView(3.1415f * 0.2f
	,static_cast<float>(y)/static_cast<float>(x),0.1f,1000.f);
}

Camera& Game::getCamera() {
	return default_camera;
}
Matrix Game::getPerspectiveMatrix() {
	return default_persperctive_matrix;
}

void Game::setViewPort(){
	cp_device_context->RSSetViewports(
	1,
	&view_port
	);
}

void Game::getViewSize(UINT& width,UINT& height) {
	ID3D11Texture2D* pBackBuffer;
	cp_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	D3D11_TEXTURE2D_DESC td;
	pBackBuffer->GetDesc(&td);
	width = td.Width;
	height = td.Height;
	pBackBuffer->Release();
}


