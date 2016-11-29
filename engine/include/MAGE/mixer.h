#pragma once
#include "StdAfx.h"
#include "SfmlAfx.h"

// creates/disposes of sf::Sound objects manually to layer sounds properly
class sfSoundBufferResource;

class MAGEDLL mixer
{
public:
	mixer();
	~mixer();

	void play(std::shared_ptr<sfSoundBufferResource> sound, sf::Vector2f pan = sf::Vector2f(), bool replaceSame = true, bool generatePitch = false);
	void stop(std::shared_ptr<sfSoundBufferResource> sound);
	void silence();

	void update();

public:
	float volume; // between 0 and 100
	float pitchShift; // 1 is base. Can't be less than 0 or more than 100.

private:
	struct channel {
		sf::Sound player;
		std::shared_ptr<sfSoundBufferResource> source;
	};

	std::vector<channel*> channels;
};

