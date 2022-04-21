#include "text_renderer.h"
bool TextRenderer::init(ID3D11Device * device,ID3D11DeviceContext * context,const wchar_t * font_path) {
    spriteBatch = std::make_unique<SpriteBatch>(context);
    spriteFont = std::make_unique<SpriteFont>(device,font_path);
    return spriteBatch != nullptr && spriteFont != nullptr;
}
void TextRenderer::drawString(const wchar_t* str,const XMFLOAT2& pos) {
    spriteBatch->Begin();
    spriteFont->DrawString(spriteBatch.get(),str,pos);
    spriteBatch->End();
}
void TextRenderer::drawString(const char* str,const XMFLOAT2& pos) {
    spriteBatch->Begin();
    spriteFont->DrawString(spriteBatch.get(),str,pos);
    spriteBatch->End();
}
void TextToast::render(TextRenderer * tr) {
	tr->drawString(text.c_str(),pos);
}
void ToastManager::toast(const wchar_t* text,const XMFLOAT2& pos,int life) {
	top = std::make_shared<TextToast>(text,pos,life);
}
void ToastManager::update(int delta) {
	if(top == nullptr || top->getLife() == -1){
		return;
	}
	int newLife = top->getLife() - delta;
	if(newLife < 0){
		top = nullptr;
	}else {
		top->getLife() = newLife;
	}
}
void ToastManager::render(TextRenderer * renderer){
	if(top == nullptr){
		return;
	}
	top->render(renderer);
}