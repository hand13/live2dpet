#include "game.h"
#include "shader.h"
#include <DirectXTK/Effects.h>
#include "util.h"
#include <d3dcompiler.h>
#include <iostream>
Shader Shader::load(const wchar_t* vs_path,const wchar_t * ps_path,D3D11_INPUT_ELEMENT_DESC * desc,int desc_num) {
	return Shader(vs_path,ps_path,desc,desc_num);
}

Shader Shader::create(const char* vs_code,size_t vs_code_length,const char* ps_code,size_t ps_code_length,D3D11_INPUT_ELEMENT_DESC * desc,int desc_num) {
	ComPtr<ID3D11VertexShader> vs;
	ComPtr<ID3D11PixelShader> ps;
	ComPtr<ID3D11InputLayout> il;
	if(vs_code != nullptr){
		auto tmp = Shader::load_vertex_shader_from_memory(vs_code,vs_code_length,desc,desc_num);
		vs = tmp.first;
		il = tmp.second;
	}
	if(ps_code != nullptr){
		ps = Shader::load_pixel_shader_from_memory(ps_code,ps_code_length);
	}
	return Shader(vs,ps,il);
}

Shader::Shader(ComPtr<ID3D11VertexShader> vs,ComPtr<ID3D11PixelShader> ps,ComPtr<ID3D11InputLayout> il) :vs(vs),ps(ps),il(il){
}


Shader::Shader(const wchar_t * vs_path,const wchar_t * ps_path,D3D11_INPUT_ELEMENT_DESC * desc,int desc_num) {
	if(vs_path != nullptr){
		auto tmp = Shader::load_vertex_shader(vs_path,desc,desc_num);
		vs = tmp.first;
		il = tmp.second;
	}
	if(ps_path != nullptr){
		ps = Shader::load_pixel_shader(ps_path);
	}
}

std::pair<ComPtr<ID3D11VertexShader>,ComPtr<ID3D11InputLayout>> Shader::load_vertex_shader(const wchar_t* vs_path,D3D11_INPUT_ELEMENT_DESC * desc,int desc_num) {
	Buffer buffer = readFileToBuffer(vs_path);
	return load_vertex_shader_from_memory(static_cast<char*>(buffer.getData()),buffer.getDataLength(),desc,desc_num);
}

std::pair<ComPtr<ID3D11VertexShader>,ComPtr<ID3D11InputLayout>> Shader::load_vertex_shader_from_memory(const char* vs_code,size_t code_length,D3D11_INPUT_ELEMENT_DESC * desc,int desc_num) {
	ComPtr<ID3DBlob> bytecode,error;
	D3DCompile(vs_code,code_length,NULL,
	NULL,NULL,"main","vs_5_0",0,0,&bytecode,&error);
	if(error != nullptr) {
		return std::pair<ComPtr<ID3D11VertexShader>,ComPtr<ID3D11InputLayout>>(nullptr,nullptr);
	}
	ComPtr<ID3D11VertexShader> vs;
	ComPtr<ID3D11InputLayout> il;
	Game::getInstance()->getDevice()->CreateVertexShader(bytecode->GetBufferPointer(),
	bytecode->GetBufferSize(),NULL,vs.GetAddressOf());

	HRESULT hr = Game::getInstance()->getDevice()->CreateInputLayout(desc,desc_num,bytecode->GetBufferPointer(),bytecode->GetBufferSize(),il.GetAddressOf());
	return std::pair<ComPtr<ID3D11VertexShader>,ComPtr<ID3D11InputLayout>>(vs,il);
}

ComPtr<ID3D11PixelShader> Shader::load_pixel_shader(const wchar_t* ps_path) {
	Buffer buffer = readFileToBuffer(ps_path);
	return load_pixel_shader_from_memory(static_cast<char*>(buffer.getData()),buffer.getDataLength());
}

ComPtr<ID3D11PixelShader> Shader::load_pixel_shader_from_memory(const char* ps_code,size_t code_length) {
	ID3DBlob *bytecode,*error;
	D3DCompile(ps_code,code_length,NULL,
	NULL,NULL,"main","ps_5_0",0,0,&bytecode,&error);
	if(error != nullptr) {
		return nullptr;
	}
	ComPtr<ID3D11PixelShader> ps;
	Game::getInstance()->getDevice()->CreatePixelShader(bytecode->GetBufferPointer(),bytecode->GetBufferSize(),NULL,ps.GetAddressOf());
	return ps;
}

ComPtr<ID3D11PixelShader> Shader::getPS(){
	return ps;
}
ComPtr<ID3D11VertexShader> Shader::getVS() {
	return vs;
}
ComPtr<ID3D11InputLayout> Shader::getIL() {
	return il;
}
