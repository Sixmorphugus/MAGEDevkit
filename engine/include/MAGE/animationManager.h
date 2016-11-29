#pragma once
#include "StdAfx.h"
#include "SfmlAfx.h"

// basic rewrite of thor's animator class.

class basic;
class sfTextureResource;

class MAGEDLL animator
{
public:
	struct animation
	{
		sf::Time time;
		std::vector<unsigned int> frames;
	};

public:
	animator(std::shared_ptr<sfTextureResource> obj);

	void update(sf::Sprite &spr, sf::Time elapsed);

	bool isPlaying();
	std::string getPlaying();

	void add(std::string name, std::vector<unsigned int> frames, sf::Time duration);

	void play(std::string name, bool loop);
	void stop();

	bool isAnimation(std::string name);

	void setFrame(sf::Sprite &spr, unsigned int frame, bool interrupt = true);
	unsigned int getFrame(sf::Sprite &spr) const;

	unsigned int getFrameFromRect(sf::IntRect fr) const;

	unsigned int framesToEnd();

	void generateFrameRects(unsigned int spriteWidth, unsigned int spriteHeight);
	std::vector<unsigned int> getFrameList();

public:
	std::shared_ptr<sfTextureResource> textureUsed;
	std::vector<sf::IntRect> frameRects;

private:
	void resetFrameEndTimer();

private:
	std::unordered_map<std::string, animation> animations;

	std::string curAnimationName;
	unsigned int curAnimationIteratorIndex;
	bool playing;
	bool autoStop;

	unsigned int animationProgress;
	sf::Time toFrameEnd;
};

