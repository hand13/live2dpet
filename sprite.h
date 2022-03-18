#pragma once
#include <memory>
#include "shader.h"
#include "game.h"

class SimpleSprite {
	private:
	ComPtr<ID3D11ShaderResourceView> cp_srv;
	ComPtr<ID3D11SamplerState> cp_sampler_state;
	ComPtr<ID3D11Buffer> vertices_buffer;
	ComPtr<ID3D11Buffer> constant_buffer;
	std::shared_ptr<Shader> shader;
	Matrix model_matrix;
	MVP mvp;
	public:
	SimpleSprite();
	bool init(const wchar_t * texture_path,const Matrix& matrix);
	void render();
	void setModelMatrix(const Matrix& matrix);
};