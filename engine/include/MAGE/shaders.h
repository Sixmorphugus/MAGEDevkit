#pragma once
#include "platform.h"

class sfShaderResource;

class MAGEDLL shadable {
public:
	shadable();

	void setShader(std::shared_ptr<sfShaderResource> shaderResource = nullptr);
	void clearShader();

	virtual std::shared_ptr<sfShaderResource> getShader() const;

	void shaderRestartEffect();
	void shaderUpdate(sf::Vector2f texSize) const;
	void shaderUpdateCustom(sf::Shader* cShader, sf::Vector2f texSize) const;

	void applyShader(sf::RenderStates& st, sf::Vector2f texSize = sf::Vector2f(0.f, 0.f)) const;

private:
	std::weak_ptr<sfShaderResource> shader;
};