#pragma once
#include "resources.h"

// SFML Texture Resource
class MAGEDLL sfTextureResource : public filepathResource
{
public:
	sfTextureResource();
	sfTextureResource(std::string filepath);

	bool load();
	void unload();

	std::shared_ptr<sf::Texture> get();

private:
	std::shared_ptr<sf::Texture> data;
};

// SFML Sound Resource
class MAGEDLL sfSoundBufferResource : public filepathResource
{
public:
	sfSoundBufferResource();
	sfSoundBufferResource(std::string filepath);

	bool load();
	void unload();

	std::shared_ptr<sf::SoundBuffer> get();

private:
	std::shared_ptr<sf::SoundBuffer> data;
};

// SFML Font Resource
class MAGEDLL sfFontResource : public filepathResource
{
public:
	sfFontResource();
	sfFontResource(std::string filepath);

	bool load();
	void unload();

	std::shared_ptr<sf::Font> get();

private:
	std::shared_ptr<sf::Font> data;
};

// SFML Shader Resource
class MAGEDLL sfShaderResource : public resource
{
public:
	sfShaderResource();
	sfShaderResource(std::string singleShader, sf::Shader::Type type);
	sfShaderResource(std::string vfComboV, std::string vfComboF);
	sfShaderResource(std::string vgfComboV, std::string vgfComboG, std::string vgfComboF);

	bool load();
	void unload();

	std::shared_ptr<sf::Shader> get();

	bool isType(sf::Shader::Type t);
	void setTypePath(sf::Shader::Type t, std::string path);

public:
	std::string vertexPath;
	std::string geometryPath;
	std::string fragmentPath;

private:
	void showShaderErrorOnce(std::string err);

	std::shared_ptr<sf::Shader> data;
	bool shShaderError;
};

// SFML Music Resource
// The only one that can unload when unused
class MAGEDLL sfMusicResource : public filepathResource
{
public:
	sfMusicResource();
	sfMusicResource(std::string filepath);

	bool load();
	void unload();

	bool isNeeded();

	std::shared_ptr<sf::Music> get();

private:
	std::shared_ptr<sf::Music> data;
};