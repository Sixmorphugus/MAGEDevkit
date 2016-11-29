#include "scriptingEngine.h"

using namespace chaiscript;

#define UINT_TYPE(type, name) \
chai->add(user_type<type>(), name); \
BIND_COPY_OPERATOR(type);
//chai->add(type_conversion<type, unsigned int>());

#define INT_TYPE(type, name) \
chai->add(user_type<type>(), name); \
BIND_COPY_OPERATOR(type);
//chai->add(type_conversion<type, int>());

#define bco(type) BIND_COPY_OPERATOR(type);

// chaiscript SFML Binding
void scriptingEngine::bindSFML()
{
	// Attach the LOW-level SFML library by Laurent Gomila

	// SFML classes
	UINT_TYPE(sf::Uint8, "Uint8");
	UINT_TYPE(sf::Uint16, "Uint16");
	UINT_TYPE(sf::Uint32, "Uint32");
	UINT_TYPE(sf::Uint64, "Uint64");

	INT_TYPE(sf::Int8, "Int8");
	INT_TYPE(sf::Int16, "Int16");
	INT_TYPE(sf::Int32, "Int32");
	INT_TYPE(sf::Int64, "Int64");

	chai->add(user_type<sf::String>(), "sfString");
	chai->add(constructor<sf::String()>(), "sfString");
	chai->add(constructor<sf::String(const std::string&)>(), "sfString");
	bco(sf::String);

	// vector classes
	chai->add(user_type<sf::Vector2f>(), "Vector2f");
	chai->add(constructor<sf::Vector2f()>(), "Vector2f");
	chai->add(constructor<sf::Vector2f(float, float)>(), "Vector2f");
	chai->add(constructor<sf::Vector2f(const sf::Vector2f&)>(), "Vector2f");
	chai->add(fun(&sf::Vector2f::x), "x");
	chai->add(fun(&sf::Vector2f::y), "y");
	bco(sf::Vector2f);

	chai->add(user_type<sf::Vector2i>(), "Vector2i");
	chai->add(constructor<sf::Vector2i()>(), "Vector2i");
	chai->add(constructor<sf::Vector2i(int, int)>(), "Vector2i");
	chai->add(constructor<sf::Vector2i(const sf::Vector2i&)>(), "Vector2i");
	chai->add(fun(&sf::Vector2i::x), "x");
	chai->add(fun(&sf::Vector2i::y), "y");
	bco(sf::Vector2i);

	chai->add(user_type<sf::Vector2u>(), "Vector2u");
	chai->add(constructor<sf::Vector2u()>(), "Vector2u");
	chai->add(constructor<sf::Vector2u(unsigned int, unsigned int)>(), "Vector2u");
	chai->add(constructor<sf::Vector2u(const sf::Vector2u&)>(), "Vector2u");
	chai->add(fun(&sf::Vector2u::x), "x");
	chai->add(fun(&sf::Vector2u::y), "y");
	bco(sf::Vector2u);

	// transform class
	chai->add(user_type<sf::Transform>(), "Transform");
	chai->add(constructor<sf::Transform()>(), "Transform");
	chai->add(constructor<sf::Transform(float, float, float, float, float, float, float, float, float)>(), "Transform");
	chai->add(fun(&sf::Transform::getInverse), "getInverse");
	chai->add(fun<sf::Vector2f, sf::Transform, float, float>(&sf::Transform::transformPoint), "transformPoint");
	chai->add(fun<sf::Vector2f, sf::Transform, const sf::Vector2f&>(&sf::Transform::transformPoint), "transformPoint");
	chai->add(fun(&sf::Transform::transformRect), "transformRect");
	chai->add(fun(&sf::Transform::combine), "combine");
	chai->add(fun<sf::Transform&, sf::Transform, float, float>(&sf::Transform::translate), "translate");
	chai->add(fun<sf::Transform&, sf::Transform, const sf::Vector2f&>(&sf::Transform::translate), "translate");
	chai->add(fun<sf::Transform&, sf::Transform, float>(&sf::Transform::rotate), "rotate");
	chai->add(fun<sf::Transform&, sf::Transform, float, float, float>(&sf::Transform::rotate), "rotate");
	chai->add(fun<sf::Transform&, sf::Transform, float, const sf::Vector2f&>(&sf::Transform::rotate), "rotate");
	chai->add(fun<sf::Transform&, sf::Transform, float, float>(&sf::Transform::scale), "scale");
	chai->add(fun<sf::Transform&, sf::Transform, float, float, float, float>(&sf::Transform::scale), "scale");
	chai->add(fun<sf::Transform&, sf::Transform, const sf::Vector2f&>(&sf::Transform::scale), "scale");
	chai->add(fun<sf::Transform&, sf::Transform, const sf::Vector2f&, const sf::Vector2f&>(&sf::Transform::scale), "scale");
	bco(sf::Transform);

	// Rectangle Classes
	chai->add(user_type<sf::FloatRect>(), "FloatRect");
	chai->add(constructor<sf::FloatRect()>(), "FloatRect");
	chai->add(constructor<sf::FloatRect(const sf::FloatRect&)>(), "FloatRect");
	chai->add(constructor<sf::FloatRect(float, float, float, float)>(), "FloatRect");
	chai->add(constructor<sf::FloatRect(sf::Vector2f, sf::Vector2f)>(), "FloatRect");
	chai->add(fun(&sf::FloatRect::left), "left");
	chai->add(fun(&sf::FloatRect::top), "top");
	chai->add(fun(&sf::FloatRect::width), "width");
	chai->add(fun(&sf::FloatRect::height), "height");
	chai->add(fun<bool, sf::FloatRect, const sf::Vector2f&>(&sf::FloatRect::contains), "contains");
	chai->add(fun<bool, sf::FloatRect, float, float>(&sf::FloatRect::contains), "contains");
	chai->add(fun<bool, sf::FloatRect, const sf::FloatRect&>(&sf::FloatRect::intersects), "intersects");
	bco(sf::FloatRect);

	chai->add(user_type<sf::IntRect>(), "IntRect");
	chai->add(constructor<sf::IntRect()>(), "IntRect");
	chai->add(constructor<sf::IntRect(const sf::IntRect&)>(), "IntRect");
	chai->add(constructor<sf::IntRect(int, int, int, int)>(), "IntRect");
	chai->add(constructor<sf::IntRect(sf::Vector2i, sf::Vector2i)>(), "IntRect");
	chai->add(fun(&sf::IntRect::left), "left");
	chai->add(fun(&sf::IntRect::top), "top");
	chai->add(fun(&sf::IntRect::width), "width");
	chai->add(fun(&sf::IntRect::height), "height");
	chai->add(fun<bool, sf::IntRect, const sf::Vector2i&>(&sf::IntRect::contains), "contains");
	chai->add(fun<bool, sf::IntRect, int, int>(&sf::IntRect::contains), "contains");
	chai->add(fun<bool, sf::IntRect, const sf::IntRect&>(&sf::IntRect::intersects), "intersects");
	bco(sf::IntRect);

	// color class
	chai->add(user_type<sf::Color>(), "Color");
	chai->add(constructor<sf::Color()>(), "Color");
	chai->add(constructor<sf::Color(sf::Uint8, sf::Uint8, sf::Uint8, sf::Uint8)>(), "Color");
	chai->add(constructor<sf::Color(const sf::Color&)>(), "Color");
	chai->add(fun(&sf::Color::r), "r");
	chai->add(fun(&sf::Color::g), "g");
	chai->add(fun(&sf::Color::b), "b");
	chai->add(fun(&sf::Color::a), "a");
	bco(sf::Color);

	chai->add(var(&sf::Color::Black), "color_Black");
	chai->add(var(&sf::Color::Blue), "color_Blue");
	chai->add(var(&sf::Color::Cyan), "color_Cyan");
	chai->add(var(&sf::Color::Green), "color_Green");
	chai->add(var(&sf::Color::Magenta), "color_Magenta");
	chai->add(var(&sf::Color::Red), "color_Red");
	chai->add(var(&sf::Color::Transparent), "color_Transparent");
	chai->add(var(&sf::Color::White), "color_White");
	chai->add(var(&sf::Color::Yellow), "color_Yellow");

	// sf time classes/functions
	chai->add(user_type<sf::Time>(), "Time");
	chai->add(constructor<sf::Time()>(), "Time");
	chai->add(constructor<sf::Time(const sf::Time&)>(), "Time");
	chai->add(fun(&sf::Time::asMicroseconds), "asMicroseconds");
	chai->add(fun(&sf::Time::asMilliseconds), "asMilliseconds");
	chai->add(fun(&sf::Time::asSeconds), "asSeconds");
	bco(sf::Time);

	chai->add(user_type<sf::Clock>(), "Clock");
	chai->add(constructor<sf::Clock()>(), "Clock");
	chai->add(constructor<sf::Clock(const sf::Clock&)>(), "Clock");
	chai->add(fun(&sf::Clock::getElapsedTime), "getElapsedTime");
	chai->add(fun(&sf::Clock::restart), "restart");
	bco(sf::Clock);

	chai->add(fun(sf::microseconds), "microseconds");
	chai->add(fun(sf::milliseconds), "milliseconds");
	chai->add(fun(sf::seconds), "seconds");

	// SFML Image Utility Class
	chai->add(user_type<sf::Image>(), "Image");
	chai->add(constructor<sf::Image()>(), "Image");
	chai->add(constructor<sf::Image(const sf::Image&)>(), "Image");
	chai->add(fun(&sf::Image::copy), "copy");
	chai->add(fun<void, sf::Image, unsigned int, unsigned int, const sf::Color&>(&sf::Image::create), "create");
	chai->add(fun(&sf::Image::createMaskFromColor), "createMaskFromColor");
	chai->add(fun(&sf::Image::flipHorizontally), "flipHorizontally");
	chai->add(fun(&sf::Image::flipVertically), "flipVertically");
	chai->add(fun(&sf::Image::getPixel), "getPixel");
	chai->add(fun(&sf::Image::getSize), "getSize");
	chai->add(fun(&sf::Image::loadFromFile), "loadFromFile");
	chai->add(fun(&sf::Image::saveToFile), "saveToFile");
	chai->add(fun(&sf::Image::setPixel), "setPixel");

	// SFML View Utility Class
	chai->add(user_type<sf::View>(), "View");
	chai->add(constructor<sf::View()>(), "View");
	chai->add(constructor<sf::View(const sf::View&)>(), "View");
	chai->add(constructor<sf::View(const sf::FloatRect&)>(), "View");
	chai->add(constructor<sf::View(const sf::Vector2f&, const sf::Vector2f&)>(), "View");
	chai->add(fun(&sf::View::getCenter), "getCenter");
	chai->add(fun(&sf::View::getInverseTransform), "getInverseTransform");
	chai->add(fun(&sf::View::getRotation), "getRotation");
	chai->add(fun(&sf::View::getSize), "getSize");
	chai->add(fun(&sf::View::getTransform), "getTransform");
	chai->add(fun(&sf::View::getViewport), "getViewport");
	chai->add(fun<void, sf::View, const sf::Vector2f&>(&sf::View::setCenter), "setCenter");
	chai->add(fun(&sf::View::setRotation), "setRotation");
	chai->add(fun<void, sf::View, const sf::Vector2f&>(&sf::View::setSize), "setSize");
	chai->add(fun(&sf::View::setViewport), "setViewport");
	chai->add(fun<void, sf::View, float, float>(&sf::View::move), "move");
	chai->add(fun<void, sf::View, const sf::Vector2f&>(&sf::View::move), "move");
	chai->add(fun(&sf::View::rotate), "rotate");
	chai->add(fun(&sf::View::zoom), "zoom");
	bco(sf::View);

	// SFML bases
	chai->add(user_type<sf::RenderTarget>(), "RenderTarget");
	chai->add(fun(&sf::RenderTarget::clear), "clear");
	chai->add(fun<void, sf::RenderTarget, const sf::Drawable&, const sf::RenderStates&>(&sf::RenderTarget::draw), "draw");
	chai->add(fun(&sf::RenderTarget::getDefaultView), "getDefaultView");
	chai->add(fun(&sf::RenderTarget::getSize), "getSize");
	chai->add(fun(&sf::RenderTarget::getView), "getView");
	chai->add(fun(&sf::RenderTarget::getViewport), "getViewport");
	chai->add(fun<sf::Vector2f, sf::RenderTarget, const sf::Vector2i&>(&sf::RenderTarget::mapPixelToCoords), "mapPixelToCoords");
	chai->add(fun<sf::Vector2f, sf::RenderTarget, const sf::Vector2i&, const sf::View&>(&sf::RenderTarget::mapPixelToCoords), "mapPixelToCoords");
	chai->add(fun<sf::Vector2i, sf::RenderTarget, const sf::Vector2f&>(&sf::RenderTarget::mapCoordsToPixel), "mapCoordsToPixel");
	chai->add(fun<sf::Vector2i, sf::RenderTarget, const sf::Vector2f&, const sf::View&>(&sf::RenderTarget::mapCoordsToPixel), "mapCoordsToPixel");

	chai->add(user_type<sf::Transformable>(), "Transformable");
	chai->add(fun<void, sf::Transformable, float, float>(&sf::Transformable::setPosition), "setPosition");
	chai->add(fun<void, sf::Transformable, const sf::Vector2f&>(&sf::Transformable::setPosition), "setPosition");
	chai->add(fun(&sf::Transformable::setRotation), "setRotation");
	chai->add(fun<void, sf::Transformable, float, float>(&sf::Transformable::setScale), "setScale");
	chai->add(fun<void, sf::Transformable, const sf::Vector2f&>(&sf::Transformable::setScale), "setScale");
	chai->add(fun<void, sf::Transformable, float, float>(&sf::Transformable::setOrigin), "setOrigin");
	chai->add(fun<void, sf::Transformable, const sf::Vector2f&>(&sf::Transformable::setOrigin), "setOrigin");
	chai->add(fun(&sf::Transformable::getPosition), "getPosition");
	chai->add(fun(&sf::Transformable::getRotation), "getRotation");
	chai->add(fun(&sf::Transformable::getScale), "getScale");
	chai->add(fun(&sf::Transformable::getOrigin), "getOrigin");
	chai->add(fun(&sf::Transformable::getTransform), "getTransform");
	chai->add(fun(&sf::Transformable::getInverseTransform), "getInverseTransform");
	chai->add(fun<void, sf::Transformable, float, float>(&sf::Transformable::move), "move");
	chai->add(fun<void, sf::Transformable, const sf::Vector2f&>(&sf::Transformable::move), "move");
	chai->add(fun(&sf::Transformable::rotate), "rotate");
	chai->add(fun<void, sf::Transformable, float, float>(&sf::Transformable::scale), "scale");
	chai->add(fun<void, sf::Transformable, const sf::Vector2f&>(&sf::Transformable::scale), "scale");

	chai->add(user_type<sf::Drawable>(), "Drawable");

	// SFML Texture
	chai->add(user_type<sf::Texture>(), "Texture");
	chai->add(constructor<sf::Texture()>(), "Texture");
	chai->add(constructor<sf::Texture(const sf::Texture &)>(), "Texture");
	chai->add(fun(&sf::Texture::copyToImage), "copyToImage");
	chai->add(fun(&sf::Texture::create), "create");
	chai->add(fun(&sf::Texture::getMaximumSize), "getMaximumSize");
	chai->add(fun(&sf::Texture::getSize), "getSize");
	chai->add(fun(&sf::Texture::isRepeated), "isRepeated");
	chai->add(fun(&sf::Texture::isSmooth), "isSmooth");
	chai->add(fun(&sf::Texture::loadFromFile), "loadFromFile");
	chai->add(fun(&sf::Texture::loadFromImage), "loadFromImage");
	chai->add(fun(&sf::Texture::setRepeated), "setRepeated");
	chai->add(fun(&sf::Texture::setSmooth), "setSmooth");

	bco(sf::Texture);

	// SFML RenderTexture
	chai->add(user_type<sf::RenderTexture>(), "RenderTexture");
	chai->add(base_class<sf::RenderTarget, sf::RenderTexture>());
	chai->add(constructor<sf::RenderTexture()>(), "RenderTexture");
	chai->add(fun(&sf::RenderTexture::setSmooth), "setSmooth");
	chai->add(fun(&sf::RenderTexture::setRepeated), "setRepeated");
	chai->add(fun(&sf::RenderTexture::setActive), "setActive");
	chai->add(fun(&sf::RenderTexture::setView), "setView");
	chai->add(fun(&sf::RenderTexture::isSmooth), "isSmooth");
	chai->add(fun(&sf::RenderTexture::isRepeated), "isRepeated");
	chai->add(fun(&sf::RenderTexture::create), "create");

	// SFML SoundBuffer
	chai->add(user_type<sf::SoundBuffer>(), "SoundBuffer");
	chai->add(constructor<sf::SoundBuffer()>(), "SoundBuffer");
	chai->add(constructor<sf::SoundBuffer(const sf::SoundBuffer&)>(), "SoundBuffer");
	chai->add(fun(&sf::SoundBuffer::loadFromFile), "loadFromFile");
	chai->add(fun(&sf::SoundBuffer::saveToFile), "saveToFile");
	chai->add(fun(&sf::SoundBuffer::getSampleCount), "getSampleCount");
	chai->add(fun(&sf::SoundBuffer::getChannelCount), "getChannelCount");
	chai->add(fun(&sf::SoundBuffer::getSampleRate), "getSampleRate");
	chai->add(fun(&sf::SoundBuffer::getDuration), "getDuration");

	bco(sf::SoundBuffer);

	// SFML Music
	chai->add(user_type<sf::Music>(), "Music");
	chai->add(constructor<sf::Music()>(), "Music");
	chai->add(fun(&sf::Music::openFromFile), "openFromFile");
	chai->add(fun(&sf::Music::getChannelCount), "getMusicChannelCount");
	chai->add(fun(&sf::Music::getSampleRate), "getMusicSampleRate");
	chai->add(fun(&sf::Music::getDuration), "getMusicDuration");
	chai->add(fun(&sf::Music::getAttenuation), "getMusicAttenuation");
	chai->add(fun(&sf::Music::getLoop), "doesMusicLoop");
	chai->add(fun(&sf::Music::getMinDistance), "getMusicMinDistance");
	chai->add(fun(&sf::Music::getPitch), "getMusicPitch");
	chai->add(fun(&sf::Music::getPlayingOffset), "getMusicPlayingOffset");
	chai->add(fun(&sf::Music::getPosition), "getMusicPosition");
	chai->add(fun(&sf::Music::getVolume), "getMusicVolume");
	chai->add(fun(&sf::Music::isRelativeToListener), "isMusicRelativeToListener");
	chai->add(fun(&sf::Music::setVolume), "setMusicVolume");
	chai->add(fun(&sf::Music::setAttenuation), "setMusicAttenuation");
	chai->add(fun(&sf::Music::setLoop), "setMusicLoop");
	chai->add(fun(&sf::Music::setMinDistance), "setMusicMinDistance");
	chai->add(fun(&sf::Music::setPitch), "setMusicPitch");
	chai->add(fun(&sf::Music::setPlayingOffset), "setMusicPlayingOffset");
	//chai->add(fun<void, sf::Music, float, float, float>(&sf::Music::setPosition), "setPosition");
	chai->add(fun(&sf::Music::setRelativeToListener), "setMusicRelativeToListener");
	chai->add(fun(&sf::Music::play), "playMusic");
	chai->add(fun(&sf::Music::pause), "pauseMusic");
	chai->add(fun(&sf::Music::stop), "stopMusic");

	// SFML Sprite
	chai->add(user_type<sf::Sprite>(), "Sprite");
	chai->add(base_class<sf::Transformable, sf::Sprite>());
	chai->add(base_class<sf::Drawable, sf::Sprite>());
	chai->add(constructor<sf::Sprite()>(), "Sprite");
	chai->add(constructor<sf::Sprite(const sf::Texture&)>(), "Sprite");
	chai->add(constructor<sf::Sprite(const sf::Sprite&)>(), "Sprite");
	chai->add(fun(&sf::Sprite::getColor), "getColor");
	chai->add(fun(&sf::Sprite::getGlobalBounds), "getGlobalBounds");
	chai->add(fun(&sf::Sprite::getLocalBounds), "getLocalBounds");
	chai->add(fun(&sf::Sprite::getTexture), "getTexture");
	chai->add(fun(&sf::Sprite::getTextureRect), "getTextureRect");
	chai->add(fun(&sf::Sprite::setColor), "setColor");
	chai->add(fun(&sf::Sprite::setTexture), "setTexture");
	chai->add(fun(&sf::Sprite::setTextureRect), "setTextureRect");

	bco(sf::Sprite);

	// SFML Sound
	chai->add(user_type<sf::Sound>(), "Sound");
	chai->add(constructor<sf::Sound()>(), "Sound");
	chai->add(constructor<sf::Sound(const sf::SoundBuffer&)>(), "Sound");
	chai->add(constructor<sf::Sound(const sf::Sound&)>(), "Sound");
	chai->add(fun(&sf::Sound::getAttenuation), "getAttenuation");
	chai->add(fun(&sf::Sound::getLoop), "doesLoop");
	chai->add(fun(&sf::Sound::getMinDistance), "getMinDistance");
	chai->add(fun(&sf::Sound::getPitch), "getPitch");
	chai->add(fun(&sf::Sound::getPlayingOffset), "getPlayingOffset");
	chai->add(fun(&sf::Sound::getPosition), "getPosition");
	chai->add(fun(&sf::Sound::getVolume), "getVolume");
	chai->add(fun(&sf::Sound::isRelativeToListener), "isRelativeToListener");
	chai->add(fun(&sf::Sound::setVolume), "setVolume");
	chai->add(fun(&sf::Sound::setAttenuation), "setAttenuation");
	chai->add(fun(&sf::Sound::setLoop), "setLoop");
	chai->add(fun(&sf::Sound::setMinDistance), "setMinDistance");
	chai->add(fun(&sf::Sound::setPitch), "setPitch");
	chai->add(fun(&sf::Sound::setPlayingOffset), "setPlayingOffset");
	//chai->add(fun<void, sf::Sound, float, float, float>(&sf::Sound::setPosition), "setPosition");
	chai->add(fun(&sf::Sound::setRelativeToListener), "setRelativeToListener");
	chai->add(fun(&sf::Sound::play), "play");
	chai->add(fun(&sf::Sound::pause), "pause");
	chai->add(fun(&sf::Sound::stop), "stop");
	chai->add(fun(&sf::Sound::setBuffer), "setBuffer");
	chai->add(fun(&sf::Sound::getBuffer), "getBuffer");

	bco(sf::Sound);

	// SFML Global Listener
	chai->add(fun(&sf::Listener::setGlobalVolume), "setGlobalAudioVolume");
	chai->add(fun(&sf::Listener::getGlobalVolume), "getGlobalAudioVolume");

	// SFML Window Prerequisites
	chai->add(user_type<sf::RenderStates>(), "RenderStates");
	chai->add(constructor<sf::RenderStates()>(), "RenderStates");

	chai->add(user_type<sf::VideoMode>(), "VideoMode");
	chai->add(constructor<sf::VideoMode()>(), "VideoMode");
	chai->add(constructor<sf::VideoMode(unsigned int, unsigned int, unsigned int)>(), "VideoMode");
	chai->add(fun([](unsigned int x, unsigned int y) { return sf::VideoMode(x, y); }), "VideoMode");
	chai->add(fun(&sf::VideoMode::width), "width");
	chai->add(fun(&sf::VideoMode::height), "height");
	chai->add(fun(&sf::VideoMode::isValid), "isValid");
	chai->add(fun(&sf::VideoMode::bitsPerPixel), "bitsPerPixel");
	chai->add(fun(&sf::VideoMode::getDesktopMode), "getDesktopVideoMode");
	chai->add(fun(&sf::VideoMode::getFullscreenModes), "getFullscreenVideoModes");

	bco(sf::VideoMode);

	chai->add(user_type<sf::ContextSettings>(), "ContextSettings");
	chai->add(constructor<sf::ContextSettings()>(), "ContextSettings");
	chai->add(constructor<sf::ContextSettings(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int)>(), "ContextSettings");

	bco(sf::ContextSettings);

	chai->add(user_type<sf::Event>(), "Event");
	chai->add(constructor<sf::Event()>(), "Event");

	bco(sf::Event);

	// SFML Window
	chai->add(user_type<sf::Window>(), "Window");
	chai->add(constructor<sf::Window()>(), "Window");
	chai->add(constructor<sf::Window(sf::VideoMode, const sf::String&, sf::Uint32, const sf::ContextSettings&)>(), "Window");
	chai->add(fun(&sf::Window::close), "close");
	chai->add(fun<void, sf::Window, sf::VideoMode, const sf::String&, sf::Uint32, const sf::ContextSettings&>(&sf::Window::create), "create");
	chai->add(fun(&sf::Window::display), "display");
	chai->add(fun(&sf::Window::getPosition), "getPosition");
	chai->add(fun(&sf::Window::getSize), "getSize");
	chai->add(fun(&sf::Window::hasFocus), "hasFocus");
	chai->add(fun(&sf::Window::isOpen), "isOpen");
	chai->add(fun(&sf::Window::requestFocus), "requestFocus");
	chai->add(fun(&sf::Window::setFramerateLimit), "setFramerateLimit");
	chai->add(fun(&sf::Window::setKeyRepeatEnabled), "setKeyRepeatEnabled");
	chai->add(fun(&sf::Window::setMouseCursorVisible), "setMouseCursorVisible");
	chai->add(fun(&sf::Window::setPosition), "setPosition");
	chai->add(fun(&sf::Window::setSize), "setSize");
	chai->add(fun(&sf::Window::setTitle), "setTitle");
	chai->add(fun(&sf::Window::setVerticalSyncEnabled), "setVerticalSyncEnabled");
	chai->add(fun(&sf::Window::setVisible), "setVisible");
	chai->add(fun(&sf::Window::waitEvent), "waitEvent");
	chai->add(fun(&sf::Window::pollEvent), "pollEvent");

	// SFML RenderWindow
	chai->add_global(var(sf::Style::Default), "style_Default");

	chai->add(user_type<sf::RenderWindow>(), "RenderWindow");
	chai->add(base_class<sf::RenderTarget, sf::RenderWindow>());
	chai->add(base_class<sf::Window, sf::RenderWindow>());
	chai->add(constructor<sf::RenderWindow()>(), "RenderWindow");
	chai->add(constructor<sf::RenderWindow(sf::VideoMode, const sf::String&, sf::Uint32, const sf::ContextSettings&)>(), "RenderWindow");

	chai->add_global(var(sf::Shader::Type::Vertex), "shader_Vertex");
	chai->add_global(var(sf::Shader::Type::Geometry), "shader_Geometry");
	chai->add_global(var(sf::Shader::Type::Fragment), "shader_Fragment");
}
