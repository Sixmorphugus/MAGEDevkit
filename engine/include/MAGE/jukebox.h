#pragma once
#include "StdAfx.h"
#include "SfmlAfx.h"

// simple af class for loading music sounds from the game and playing them in one place.
class Game;
class sfMusicResource;

class MAGEDLL jukebox
{
public:
	jukebox();

	void playSong(std::shared_ptr<sfMusicResource> songName);
	void stopPlaying(bool fadeOut = false);

	void pause();
	void resume();

	void fadeToSong(std::shared_ptr<sfMusicResource> songName);

	void update();

	std::shared_ptr<sfMusicResource> getLastPlayed() { return lp.lock(); }
public:
	float volume; // between 0 and 100
	float pitchShift; // 1.f is normal
	float pitchShiftMultiplier;

	std::weak_ptr<sfMusicResource> lp; // public because there's actually situations where this needs to be changed manually (though we'll have the SE pretend it's private)
private:
	// this is how they know they're in use
	std::shared_ptr<sfMusicResource> player;
	std::shared_ptr<sfMusicResource> altPlayer;

	bool usingAltPlayer;
	bool playing;
	float pitchShiftPitch;
};

