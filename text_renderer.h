#pragma once
#include <memory>
#include <DirectXTK/SpriteFont.h>
#include <string>
using DirectX::SpriteBatch;
using DirectX::SpriteFont;
using DirectX::XMFLOAT2;
class TextRenderer {
    private:
    std::unique_ptr<SpriteBatch> spriteBatch;
    std::unique_ptr<SpriteFont> spriteFont;
    public:
    TextRenderer() = default;
    bool init(ID3D11Device * device,ID3D11DeviceContext * context,const wchar_t * font_path);
    void drawString(const char* str,const XMFLOAT2& pos);
    void drawString(const wchar_t* str,const XMFLOAT2& pos);
};
class TextToast {
	private:
	std::wstring text;
	int life;
	XMFLOAT2 pos;
	public:
	inline TextToast(const wchar_t* text,const XMFLOAT2& pos,int life = -1):text(text),life(life),pos(pos){}
	virtual void render(TextRenderer * tr);
	inline int& getLife(){return life;}
	~TextToast() = default;
};

class ToastManager {
	private:
	std::shared_ptr<TextToast> top;
	public:
	ToastManager(){}
	void toast(const wchar_t* text,const XMFLOAT2& pos,int life = -1);
	void update(int delta);
	void render(TextRenderer * tr);
};