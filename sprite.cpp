#include "sprite.h"
#include <DirectXTK/WICTextureLoader.h>
#include <iostream>

static const float vertices[] = {
	1,1,1,1,1
	,-1,-1,1,0,0
	,1,-1,1,1,0
	,1,1,1,1,1
	-1,1,1,0,1
	-1,-1,1,0,0
};

static D3D11_INPUT_ELEMENT_DESC ie_desc[] = {
	{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
	};


SimpleSprite::SimpleSprite() {
}

bool SimpleSprite::init(const wchar_t * texture_path,const Matrix& model_matrix) {
	ComPtr<ID3D11Resource> resource;
	if(FAILED(DirectX::CreateWICTextureFromFile(Game::getInstance()->getDevice().Get(),texture_path
	,resource.GetAddressOf(),cp_srv.GetAddressOf()))) {
		std::cout<<"load texture failed"<<std::endl;
		return false;
	}

	CD3D11_BUFFER_DESC buffer_desc(sizeof(vertices),D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA sub;
	ZeroMemory(&sub,sizeof(D3D11_SUBRESOURCE_DATA));
	sub.pSysMem = vertices;
	sub.SysMemPitch = 0;
	sub.SysMemSlicePitch = 0;
	Game::getInstance()->getDevice()->CreateBuffer(&buffer_desc,&sub,vertices_buffer.GetAddressOf());

	CD3D11_BUFFER_DESC constant_buffer_desc(sizeof(MVP),D3D11_BIND_CONSTANT_BUFFER);
	Game::getInstance()->getDevice()->CreateBuffer(&constant_buffer_desc,NULL,constant_buffer.GetAddressOf());


	// D3D11_SAMPLER_DESC sampler_desc;
	// ZeroMemory(&sampler_desc,sizeof(D3D11_SAMPLER_DESC));
	// sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;

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


void SimpleSprite::render() {
	Game* game = Game::getInstance();
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
	context->PSSetSamplers(0,1,cp_sampler_state.GetAddressOf());
	context->VSSetConstantBuffers(0,1,constant_buffer.GetAddressOf());
	context->Draw(6,0);
}

void SimpleSprite::setModelMatrix(const Matrix& matrix) {
	model_matrix = matrix;
}