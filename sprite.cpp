#include "sprite.h"
#include <DirectXTK/WICTextureLoader.h>
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


SimpleSprite::SimpleSprite() {
}

bool SimpleSprite::init(const wchar_t * texture_path,const Matrix& model_matrix) {

	// ComPtr<ID3D11Resource> resource;
	// if(FAILED(DirectX::CreateWICTextureFromFile(Game::getInstance()->getDevice().Get(),texture_path
	// ,resource.GetAddressOf(),cp_srv.GetAddressOf()))) {
	// 	std::cout<<"load texture failed"<<std::endl;
	// 	return false;
	// }
	cubismState.init(Game::getInstance()->getDevice().Get());
	model = std::make_unique<LAppModel>(&cubismState);
	model->LoadAssets("resource/Rice/","Rice.model3.json");
	model->GetRenderBuffer().CreateOffscreenFrame(Game::getInstance()->getDevice().Get(),
                    1600, 1600);

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

	// mvp.model = model_matrix;
	// mvp.view = Game::getInstance()->getCamera().getViewMatrix();
	// mvp.perspective = Game::getInstance()->getPerspectiveMatrix();
	mvp.model = Matrix::Identity;
	mvp.view = Matrix::Identity;
	mvp.perspective = Matrix::Identity;
	return true;
}


void SimpleSprite::render() {
	static Timer timer;
	Game* game = Game::getInstance();
	auto device = game->getDevice();
	auto context = game->getDeviceContext();
	UINT a = 20;
	UINT offset = 0;
	Csm::CubismMatrix44 m;
	m.SetMatrix((float*)Matrix::Identity.m);
	Csm::Rendering::CubismRenderer_D3D11::StartFrame (device.Get(), context.Get(), 1600, 1600);
	model->GetRenderBuffer().BeginDraw(context.Get());
	model->GetRenderBuffer().Clear(context.Get(),0,0,0,0);
	model->Update(static_cast<float>(timer.delta())/1000.f);
	model->Draw(m);
	model->GetRenderBuffer().EndDraw(context.Get());

	float bc[] = {0.f,0.0f,0.f,0.f};
	context->ClearRenderTargetView(game->getRTV().Get(),bc);
	// context->ClearDepthStencilView(cp_dsv.Get(),D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,1.f,0);
	context->OMSetRenderTargets(1,game->getRTV().GetAddressOf(),NULL);
	// context->OMSetDepthStencilState(cp_dss.Get(),0);

	context->UpdateSubresource(constant_buffer.Get(),NULL,NULL,&mvp,0,0);
	context->IASetVertexBuffers(0,1,vertices_buffer.GetAddressOf(),&a,&offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(shader->getIL().Get());
	context->VSSetShader(shader->getVS().Get(),nullptr,0);
	context->PSSetShader(shader->getPS().Get(),nullptr,0);
	ID3D11ShaderResourceView * r = model->GetRenderBuffer().GetTextureView();
	context->PSSetShaderResources(0,1,&r);
	context->PSSetSamplers(0,1,cp_sampler_state.GetAddressOf());
	context->VSSetConstantBuffers(0,1,constant_buffer.GetAddressOf());

	context->Draw(6,0);

}

void SimpleSprite::setModelMatrix(const Matrix& matrix) {
	model_matrix = matrix;
}