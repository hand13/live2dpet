#include <Windows.h>
static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
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
int main() {
    HWND hWnd;
	 WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, TEXT("show"), NULL };
	 wc.hCursor = LoadCursor(NULL,IDC_ARROW);
    ::RegisterClassEx(&wc);
    hWnd = ::CreateWindow(wc.lpszClassName, TEXT("such"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
	ShowWindow(hWnd,SW_SHOW);
	UpdateWindow(hWnd);
	bool done = false;
	while(!done) {
		MSG msg;
		while(::PeekMessage(&msg,0,0U,0U,PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if(msg.message == WM_QUIT) {
				done = true;
			}
		}
		if(done) {
			break;
		}
    }
    return 0;
}