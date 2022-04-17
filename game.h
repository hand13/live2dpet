#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dcomp.h>
#include <wrl/client.h>
#include <DirectXTK/SimpleMath.h>
#include "util.h"
#include "sprite.h"
using Microsoft::WRL::ComPtr;
using DirectX::SimpleMath::Matrix;


class SimpleSprite;

class Game {
	private:
	static Game * INSTANCE;
	HWND hWnd;
	ComPtr<ID3D11Device> cp_device;
	ComPtr<ID3D11DeviceContext> cp_device_context;
	ComPtr<ID3D11RenderTargetView> cp_rtv;
	ComPtr<ID3D11DepthStencilView> cp_dsv;
	ComPtr<ID3D11DepthStencilState> cp_dss;
	ComPtr<IDCompositionDevice> cp_dcd;
	ComPtr<IDCompositionTarget> cp_dct;
	ComPtr<IDCompositionVisual> cp_dcv;
	ComPtr<IDXGISwapChain1> cp_swap_chain;
	Camera default_camera;
	Matrix default_persperctive_matrix;
	std::shared_ptr<SimpleSprite> sprite;
	D3D11_VIEWPORT view_port;
	public:
	Game();
	static Game* getInstance();
	static void releaseInstance();
	ComPtr<ID3D11Device> getDevice();
	ComPtr<ID3D11DeviceContext> getDeviceContext();
	ComPtr<ID3D11RenderTargetView> getRTV();
	ComPtr<IDXGISwapChain1> getSwapChain();
	Camera& getCamera();
	Matrix getPerspectiveMatrix();
	bool init();
	void show();
	void resize(UINT width,UINT height);
	void loop();
	bool handleEvent(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void render();
	void getWindowSize(UINT& width,UINT & height);
	void getViewSize(UINT& width,UINT & height);
	void calcNDCCoord(int screenx,int screeny,float& x,float& y);
	inline SimpleSprite * getSprite(){
		return sprite.get();
	}
	bool isTransparent(int x,int y);
	bool tryRedirect(int x,int y,HWND,UINT msg,WPARAM wParam,LPARAM lParam);
	private:
	bool createWindow();
	bool createDevice();
	bool createRenderTargetView();
	void createPerspectiveMatrix();
	void setViewPort();
	HWND findNearWindow(int x,int y);
};