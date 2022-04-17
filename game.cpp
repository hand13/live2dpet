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
void FinishedMotion(Csm::ACubismMotion* self) {
}
void Game::calcNDCCoord(int screenx,int screeny,float& x,float& y) {
	UINT width,height;
	getWindowSize(width,height);
	x = static_cast<float>(screenx)*2.f/static_cast<float>(width) - 1.f;
	y = 1.f - static_cast<float>(screeny)*2.f/static_cast<float>(height);
}
static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static bool touched = false;
	static POINT pos;
	Game * instance = Game::getInstance();
	if (instance->handleEvent(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
	case WM_LBUTTONDOWN:{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		if(instance->tryRedirect(x,y,hWnd,msg,wParam,lParam)) {
			break;
		}

		touched = true;
		RECT rect;
		GetWindowRect(hWnd,&rect);
		pos.x = rect.left + x;
		pos.y = rect.top + y;

		float nx = 0.f,ny=0.f;
		instance->calcNDCCoord(x,y,nx,ny);
		LAppModel * model = instance->getSprite()->getModel();
		if(model->isHit(nx,ny)) {
			model->StartRandomMotion("TapBody",2,FinishedMotion);
		}
		break;
	}
	case WM_LBUTTONUP: {
		touched = false;
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		if(instance->tryRedirect(x,y,hWnd,msg,wParam,lParam)) {
			break;
		}
		break;
	}
	case WM_MOUSEMOVE:{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		if(instance->tryRedirect(x,y,hWnd,msg,wParam,lParam)) {
			break;
		}
		if((MK_LBUTTON == wParam) && touched) {
			RECT rect;
			GetWindowRect(hWnd,&rect);
			POINT tmp;
			tmp.x = rect.left + x;
			tmp.y = rect.top + y;
			int dx = tmp.x - pos.x;
			int dy = tmp.y - pos.y;
			pos = tmp;
			MoveWindow(hWnd,rect.left + dx,rect.top + dy,800,800,false);
		}
		break;
	}
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
	 WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL)
	 , NULL, NULL, NULL, NULL, TEXT("showMe"), NULL };
	 wc.hCursor = LoadCursor(NULL,IDC_ARROW);
    ::RegisterClassEx(&wc);
	int ws = 0;
    hWnd = ::CreateWindowEx(WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOPMOST,wc.lpszClassName
	, TEXT("such a title"), WS_POPUP, 800, 200, 800, 800, NULL, NULL, wc.hInstance, NULL);
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
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
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

	hr = dxFactory->CreateSwapChainForComposition(dxgiDevice.Get(),
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

	CD3D11_TEXTURE2D_DESC ttd(DXGI_FORMAT_D24_UNORM_S8_UINT,td.Width,td.Height);
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
	cp_rtv = nullptr;
	HRESULT hr = cp_swap_chain->ResizeBuffers(2,width,heigth,DXGI_FORMAT_UNKNOWN,0);
	createRenderTargetView();
	// setViewPort();
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
	setViewPort();
	sprite->render();
}

bool Game::isTransparent(int x,int y) {
	bool result = true;
	ComPtr<ID3D11Resource> res;
	D3D11_MAPPED_SUBRESOURCE sub;
	ZeroMemory(&sub,sizeof(sub));
	cp_rtv->GetResource(res.GetAddressOf());
	ComPtr<ID3D11Texture2D> pTexture;
	HRESULT hr = res->QueryInterface(IID_PPV_ARGS(pTexture.GetAddressOf()));
	ComPtr<ID3D11Texture2D> pStaging;
	D3D11_TEXTURE2D_DESC desc;
	pTexture->GetDesc(&desc);
	desc.BindFlags = 0;
	// desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	cp_device->CreateTexture2D(&desc, nullptr, pStaging.ReleaseAndGetAddressOf());
	cp_device_context->CopyResource(pStaging.Get(), res.Get());
	hr =  cp_device_context->Map(pStaging.Get(),0,D3D11_MAP_READ,0,&sub);
	unsigned char * mem = reinterpret_cast<unsigned char*>(sub.pData);
	UINT index = sub.RowPitch * y + x * 4 + 3;
	if(index < sub.DepthPitch) {
		unsigned char alpha = mem[sub.RowPitch * y + x*4 + 3];
		result = alpha == 0;
	}
	cp_device_context->Unmap(pStaging.Get(),0);
	return result;
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


HWND Game::findNearWindow(int x,int y) {
	RECT rect;
	POINT point;
	GetWindowRect(hWnd,&rect);
	point.x = rect.left + x;
	point.y = rect.top + y;
	HWND result = NULL;
	HWND dh = hWnd;
	wchar_t buffer[512];
	ZeroMemory(buffer,sizeof(buffer));
	while(dh != NULL) {
		if(dh != hWnd && IsWindowVisible(dh)) {
			RECT tr;
			GetWindowRect(dh,&tr);
			if(point.x >tr.left && point.x < tr.right && point.y > tr.top && point.y < tr.bottom) {
				int length = GetWindowTextW(dh,buffer,512);
				if(length >= 0) {
					if(wcscmp(L"QQ",buffer) != 0) {
						result = dh;
						break;
					}
				}else {
					result = dh;
					break;
				}
			}
		}
		dh = GetNextWindow(dh,GW_HWNDNEXT);
	}
	return result;
}

bool Game::tryRedirect(int x,int y,HWND,UINT msg,WPARAM wParam,LPARAM lParam){
	if(isTransparent(x,y)) {
		HWND next =findNearWindow(x,y);
		if(next != NULL) {
			RECT nr,tt;
			GetWindowRect(next,&nr);
			GetWindowRect(hWnd,&tt);
			UINT nx = tt.left + x - nr.left;
			UINT ny = tt.top + y - nr.top;
			SendMessage(next,msg,wParam,MAKELPARAM(nx,ny));
		}
		return true;
	}
	return false;
}