#pragma once
#include <memory>
#include "shader.h"
#include "game.h"
#include <memory>
#include "LAppModel.hpp"
#include "text_renderer.h"
#include <map>

class TextRenderer;
class Sprite {
	protected:
	Matrix model_matrix;
	MVP mvp;
	private:
	ComPtr<ID3D11SamplerState> cp_sampler_state;
	ComPtr<ID3D11Buffer> vertices_buffer;
	ComPtr<ID3D11Buffer> constant_buffer;
	std::shared_ptr<Shader> shader;
	public:
	Sprite() = default;
	bool init();
	virtual void update(int delta) = 0;
	virtual ID3D11ShaderResourceView * getTexture() = 0;
	virtual void render();
	void setModelMatrix(const Matrix& matrix);
	virtual ~Sprite() = default;
};

class SimpleSprite :public Sprite{
	private:
	CubismState cubismState;
	std::unique_ptr<LAppModel> model;
	public:
	SimpleSprite();
	bool init(const char * resourceDir,const char * jsonName,const Matrix& matrix);
	virtual void update(int delta)override;
	inline LAppModel * getModel(){
		return model.get();
	}
	virtual ID3D11ShaderResourceView * getTexture()override;
	virtual ~SimpleSprite() = default;
};
