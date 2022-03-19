#include "game.h"
#include <iostream>
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
    hWnd = ::CreateWindow(wc.lpszClassName, TEXT("such a title"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
	return true;
}

bool Game::createDevice() {
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, cp_swap_chain.GetAddressOf(),cp_device.GetAddressOf(), &featureLevel, cp_device_context.GetAddressOf()) != S_OK)
        return false;
	return createRenderTargetView();
}

bool Game::createRenderTargetView() {
	ID3D11Texture2D* pBackBuffer;
	cp_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    cp_device->CreateRenderTargetView(pBackBuffer, NULL,cp_rtv.ReleaseAndGetAddressOf());
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
ComPtr<IDXGISwapChain> Game::getSwapChain() {
	return cp_swap_chain;
}

void Game::show() {
	ShowWindow(hWnd,SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
}

void Game::resize(UINT width,UINT heigth) {
	cp_swap_chain->ResizeBuffers(0,width,heigth,DXGI_FORMAT_UNKNOWN,0);
	createRenderTargetView();
	std::cout<<"width is "<<width<<std::endl;
	std::cout<<"height is "<<heigth<<std::endl;
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
		cp_swap_chain->Present(1,0);
	}
}

void Game::render(){
	cp_device_context->OMSetRenderTargets(1,cp_rtv.GetAddressOf(),nullptr);
	FLOAT color[]={1.f,0.f,1.f,1.f};
	cp_device_context->ClearRenderTargetView(cp_rtv.Get(),color);
	//todo
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
	getWindowSize(x,y);
	default_persperctive_matrix = Matrix::CreatePerspectiveFieldOfView(3.1415f * 0.2f
	,static_cast<float>(y)/static_cast<float>(x),0.1f,1000.f);
}

Camera& Game::getCamera() {
	return default_camera;
}
Matrix& Game::getPerspectiveMatrix() {
	return default_persperctive_matrix;
}
