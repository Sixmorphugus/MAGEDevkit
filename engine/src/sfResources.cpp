#include "sfResources.h"

#include "Game.h"

sfTextureResource::sfTextureResource()
{}

// texture resource
sfTextureResource::sfTextureResource(std::string filepath)
	: filepathResource(filepath)
{}

bool sfTextureResource::load()
{
	auto t = filepathResource::load();

	data = std::make_shared<sf::Texture>();

	bool success = data->loadFromFile(filepath);

	if (success) {
		return t;
	}

	return false;
}

void sfTextureResource::unload()
{
	data = std::shared_ptr<sf::Texture>();
	filepathResource::unload();
}

std::shared_ptr<sf::Texture> sfTextureResource::get()
{
	return data;
}

// sound resource
sfSoundBufferResource::sfSoundBufferResource()
{}

sfSoundBufferResource::sfSoundBufferResource(std::string filepath)
	: filepathResource(filepath)
{}

bool sfSoundBufferResource::load()
{
	auto t = filepathResource::load();

	data = std::make_shared<sf::SoundBuffer>();

	bool success = data->loadFromFile(filepath);

	if (success) {
		return t;
	}

	return false;
}

void sfSoundBufferResource::unload()
{
	data = std::shared_ptr<sf::SoundBuffer>();
	filepathResource::unload();
}

std::shared_ptr<sf::SoundBuffer> sfSoundBufferResource::get()
{
	return data;
}

// font resource
sfFontResource::sfFontResource()
{}

sfFontResource::sfFontResource(std::string filepath)
	: filepathResource(filepath)
{}

bool sfFontResource::load()
{
	auto t = filepathResource::load();

	data = std::make_shared<sf::Font>();

	bool success = data->loadFromFile(filepath);

	if (success) {
		return t;
	}

	return false;
}

void sfFontResource::unload()
{
	data = std::shared_ptr<sf::Font>();
	filepathResource::unload();
}

std::shared_ptr<sf::Font> sfFontResource::get()
{
	return data;
}

// shader resource
sfShaderResource::sfShaderResource()
	: shShaderError(false)
{
}

sfShaderResource::sfShaderResource(std::string singleShader, sf::Shader::Type type)
	: shShaderError(false)
{
	setTypePath(type, singleShader);
}

sfShaderResource::sfShaderResource(std::string vfComboV, std::string vfComboF)
	: shShaderError(false)
{
	setTypePath(sf::Shader::Vertex, vfComboV);
	setTypePath(sf::Shader::Fragment, vfComboF);
}

sfShaderResource::sfShaderResource(std::string vgfComboV, std::string vgfComboG, std::string vgfComboF)
	: shShaderError(false)
{
	setTypePath(sf::Shader::Vertex, vgfComboV);
	setTypePath(sf::Shader::Geometry, vgfComboF);
	setTypePath(sf::Shader::Fragment, vgfComboF);
}

bool sfShaderResource::load()
{
	data = std::make_shared<sf::Shader>();

	bool anythingFailed = false;

	if (isType(sf::Shader::Vertex)) {
		p::log("\tVertex Shader: " + vertexPath);
		if (!data->loadFromFile(vertexPath, sf::Shader::Vertex)) anythingFailed = true;
	}

	if (isType(sf::Shader::Geometry)) {
		p::log("\tGeometry Shader: " + geometryPath);
		if (!data->loadFromFile(geometryPath, sf::Shader::Geometry)) anythingFailed = true;
	}

	if (isType(sf::Shader::Fragment)) {
		p::log("\tFragment Shader: " + fragmentPath);
		if (!data->loadFromFile(fragmentPath, sf::Shader::Fragment)) anythingFailed = true;
	}

	if(anythingFailed)
		return false;

	return resource::load();
}

void sfShaderResource::unload()
{
	data = std::shared_ptr<sf::Shader>();

	resource::unload();
}

std::shared_ptr<sf::Shader> sfShaderResource::get()
{
	return data;
}

bool sfShaderResource::isType(sf::Shader::Type t)
{
	switch (t) {
	case sf::Shader::Fragment:
		return fragmentPath != "";
	case sf::Shader::Geometry:
		return geometryPath != "";
	case sf::Shader::Vertex:
		return vertexPath != "";
	}

	return false;
}

void sfShaderResource::setTypePath(sf::Shader::Type t, std::string path)
{
	auto rpath = p::realPath(path);

	if (!sf::Shader::isAvailable()) {
		showShaderErrorOnce("This machine does not support shaders!");
	}

	switch (t) {
	case sf::Shader::Fragment:
		fragmentPath = rpath;
		break;
	case sf::Shader::Geometry:
		geometryPath = rpath;
		
		if (!sf::Shader::isGeometryAvailable()) {
			showShaderErrorOnce("This machine does not support geometry shaders.");
		}

		break;
	case sf::Shader::Vertex:
		vertexPath = rpath;
		break;
	}
}

void sfShaderResource::showShaderErrorOnce(std::string err)
{
	if (!shShaderError) {
		p::warn(err);
		shShaderError = true;
	}
}

// volatile music resource
sfMusicResource::sfMusicResource()
{}

sfMusicResource::sfMusicResource(std::string filepath)
	: filepathResource(filepath)
{}

bool sfMusicResource::load()
{
	auto t = filepathResource::load();

	data = std::make_shared<sf::Music>();

	if (!data->openFromFile(filepath)) return false;

	data->setLoop(true);

	return t;
}

void sfMusicResource::unload()
{
	data = std::shared_ptr<sf::Music>();

	resource::unload();
}

bool sfMusicResource::isNeeded()
{
	// if this music track isn't referenced anywhere, unload it to save memory.
	// note that this SHOULD be safe - it's impossible to "get" an unloaded resource.
	return (numReferences() > 0);
}

std::shared_ptr<sf::Music> sfMusicResource::get()
{
	return data;
}

// SE Binding
#line 20000

// sfTextureResource
DeclareScriptingFilepathResource(sfTextureResource);
DeclareScriptingFunction(&sfTextureResource::get, "get");

// sfSoundBufferResource
DeclareScriptingFilepathResource(sfSoundBufferResource);
DeclareScriptingFunction(&sfSoundBufferResource::get, "get");

// sfFontResource
DeclareScriptingFilepathResource(sfFontResource);
DeclareScriptingFunction(&sfFontResource::get, "get");

// sfShaderResource
DeclareScriptingResource(sfShaderResource);
DeclareScriptingConstructor(sfShaderResource(std::string, sf::Shader::Type), "sfShaderResource");
DeclareScriptingConstructor(sfShaderResource(std::string, std::string), "sfShaderResource");
DeclareScriptingConstructor(sfShaderResource(std::string, std::string, std::string), "sfShaderResource");
DeclareScriptingFunction(&sfShaderResource::get, "get");
DeclareScriptingFunction(&sfShaderResource::vertexPath, "vertexPath");
DeclareScriptingFunction(&sfShaderResource::geometryPath, "geometryPath");
DeclareScriptingFunction(&sfShaderResource::fragmentPath, "fragmentPath");
DeclareScriptingFunction(&sfShaderResource::setTypePath, "setTypePath");

// sfMusicResource
DeclareScriptingFilepathResource(sfMusicResource);
DeclareScriptingFunction(&sfMusicResource::get, "get");