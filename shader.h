#pragma once
#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;
class Shader {
	public:
	static Shader load(const wchar_t* vs_path,const wchar_t * ps_path,D3D11_INPUT_ELEMENT_DESC * desc,int desc_num);
	static Shader create(const char* vs_code,size_t vs_code_length,const char* ps_code,size_t ps_code_length,D3D11_INPUT_ELEMENT_DESC * desc,int desc_num);
	static std::pair<ComPtr<ID3D11VertexShader>,ComPtr<ID3D11InputLayout>> load_vertex_shader(const wchar_t* vs_path,D3D11_INPUT_ELEMENT_DESC * desc,int desc_num);
	static std::pair<ComPtr<ID3D11VertexShader>,ComPtr<ID3D11InputLayout>> load_vertex_shader_from_memory(const char* vs_code,size_t code_length,D3D11_INPUT_ELEMENT_DESC * desc,int desc_num);
	static ComPtr<ID3D11PixelShader> load_pixel_shader(const wchar_t* ps_path);
	static ComPtr<ID3D11PixelShader> load_pixel_shader_from_memory(const char* ps_code,size_t code_length);
	Shader(const wchar_t * vs_path,const wchar_t * ps_path,D3D11_INPUT_ELEMENT_DESC * desc,int desc_num);
	Shader(ComPtr<ID3D11VertexShader> vs,ComPtr<ID3D11PixelShader> ps,ComPtr<ID3D11InputLayout> il);
	
	ComPtr<ID3D11PixelShader> getPS();
	ComPtr<ID3D11VertexShader> getVS();
	ComPtr<ID3D11InputLayout> getIL();
	private:
	ComPtr<ID3D11VertexShader> vs;
	ComPtr<ID3D11PixelShader> ps;
	ComPtr<ID3D11InputLayout> il;
};