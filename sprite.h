#pragma once
#include <memory>
#include "shader.h"
#include "game.h"
#include <memory>
#include "LAppModel.hpp"

class SimpleSprite {
	private:
	CubismState cubismState;
	std::unique_ptr<LAppModel> model;
	// ComPtr<ID3D11ShaderResourceView> cp_srv;
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
	inline LAppModel * getModel(){
		return model.get();
	}
};