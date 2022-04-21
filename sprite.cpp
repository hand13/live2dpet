#include "sprite.h"
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/SpriteFont.h>
#include <Rendering/D3D11/CubismRenderer_D3D11.hpp>
#include <iostream>


static const float vertices[] = {
	1.f,1.f,0,1,0
	,1.f,-1.f,0	,1,1
	,-1.f,-1.f,0	,0,1
	,1.f,1.f,0	,1,0
	,-1.f,-1.f,0	,0,1
	,-1.f,1.f,0	,0,0
};

static D3D11_INPUT_ELEMENT_DESC ie_desc[] = {
	{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
	};


bool Sprite::init() {
	CD3D11_BUFFER_DESC buffer_desc(sizeof(vertices),D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA sub;
	ZeroMemory(&sub,sizeof(D3D11_SUBRESOURCE_DATA));
	sub.pSysMem = vertices;
	sub.SysMemPitch = 0;
	sub.SysMemSlicePitch = 0;
	Game::getInstance()->getDevice()->CreateBuffer(&buffer_desc,&sub,vertices_buffer.GetAddressOf());

	CD3D11_BUFFER_DESC constant_buffer_desc(sizeof(MVP),D3D11_BIND_CONSTANT_BUFFER);
	Game::getInstance()->getDevice()->CreateBuffer(&constant_buffer_desc,NULL,constant_buffer.GetAddressOf());

	CD3D11_DEFAULT tmp;
	CD3D11_SAMPLER_DESC sampler_desc(tmp);
	Game::getInstance()->getDevice()->CreateSamplerState(&sampler_desc,cp_sampler_state.GetAddressOf());


	shader = std::make_shared<Shader>( L"resource\\shaders\\basic_shader_vert.hlsl"
	,L"resource\\shaders\\basic_shader_pixel.hlsl",ie_desc,2);

	this->model_matrix = model_matrix;
	mvp.model = model_matrix;
	mvp.view = Game::getInstance()->getCamera().getViewMatrix();
	mvp.perspective = Game::getInstance()->getPerspectiveMatrix();
	return true;
}

void Sprite::render() {
	Game * game =Game::getInstance();
	auto device = game->getDevice();
	auto context = game->getDeviceContext();
	UINT a = 20;
	UINT offset = 0;
	context->UpdateSubresource(constant_buffer.Get(),NULL,NULL,&mvp,0,0);
	context->IASetVertexBuffers(0,1,vertices_buffer.GetAddressOf(),&a,&offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(shader->getIL().Get());
	context->VSSetShader(shader->getVS().Get(),nullptr,0);
	context->PSSetShader(shader->getPS().Get(),nullptr,0);
	ID3D11ShaderResourceView * srv = getTexture();
	context->PSSetShaderResources(0,1,&srv);
	context->PSSetSamplers(0,1,cp_sampler_state.GetAddressOf());
	context->VSSetConstantBuffers(0,1,constant_buffer.GetAddressOf());
	context->Draw(6,0);

}

void Sprite::setModelMatrix(const Matrix& matrix) {
	model_matrix = matrix;
}

SimpleSprite::SimpleSprite() {
}

bool SimpleSprite::init(const char * resourceDir,const char * jsonName,const Matrix& model_matrix) {
	Sprite::init();
	cubismState.init(Game::getInstance()->getDevice().Get());
	model = std::make_unique<LAppModel>(&cubismState);
	model->LoadAssets(resourceDir,jsonName);
	model->GetRenderBuffer().CreateOffscreenFrame(Game::getInstance()->getDevice().Get(),
                    1600, 1600);

	mvp.model = Matrix::Identity;
	mvp.view = Matrix::Identity;
	mvp.perspective = Matrix::Identity;
	return true;
}

void SimpleSprite::update(int delta) {
	Game* game = Game::getInstance();
	float eyeX = 0.f,eyeY = 0.f;
	model->getEyePos(eyeX,eyeY);
	POINT cp;
	GetCursorPos(&cp);
	RECT rect;
	GetWindowRect(game->getHWND(),&rect);
	cp.x = cp.x - rect.left;
	if(cp.x < rect.left) {
		cp.x = 0;
	}else if(cp.x >rect.right) {
		cp.y = rect.right;
	}
	cp.y = cp.y - rect.top;
	if(cp.y < rect.top) {
		cp.y = rect.top;
	}else if(cp.y > rect.bottom) {
		cp.y = rect.bottom;
	}
	float drx,dry;
	game->calcNDCCoord(cp.x,cp.y,drx,dry);
	model->SetDragging(drx,dry);

	auto device = game->getDevice();
	auto context = game->getDeviceContext();
	Csm::CubismMatrix44 m;
	m.SetMatrix((float*)Matrix::Identity.m);

	Csm::Rendering::CubismRenderer_D3D11::StartFrame (device.Get(), context.Get(), 1600, 1600);
	model->GetRenderBuffer().BeginDraw(context.Get());
	model->GetRenderBuffer().Clear(context.Get(),0,0,0,0);
	model->Update(static_cast<float>(delta)/1000.f);
	model->Draw(m);
	model->GetRenderBuffer().EndDraw(context.Get());

}
ID3D11ShaderResourceView * SimpleSprite::getTexture() {
	return model->GetRenderBuffer().GetTextureView();
}