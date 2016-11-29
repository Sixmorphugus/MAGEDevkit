#include "mixer.h"

#include "helpers.h"
#include "Game.h"

#include "sfResources.h"

mixer::mixer()
{
	volume = 100.f;
	pitchShift = 1.f;
}

mixer::~mixer()
{
	silence();
}

void mixer::play(std::shared_ptr<sfSoundBufferResource> sound, sf::Vector2f pan, bool replaceSame, bool generatePitch)
{
	if (!sound)
		return;
	
	// add new sound to channels list
	channel* ch = new channel();

	//p::info(soundName);

	// configure
	ch->source = sound;

	if (!ch->source->isLoaded()) {
		p::warn("An unloaded sound was passed to the mixer. Be careful.");
	}

	ch->player = sf::Sound(*ch->source->get());

	ch->player.setVolume(volume);
	ch->player.setPosition(pan.x, pan.y, 0);
	ch->player.setPitch(pitchShift * theGame()->tickScale);

	if (generatePitch) {
		ch->player.setPitch((pitchShift / theGame()->tickScale) + randomPitch());
	}

	if (replaceSame) {
		stop(sound);
	}

	// play
	channels.push_back(ch);
	ch->player.play();
}

void mixer::stop(std::shared_ptr<sfSoundBufferResource> sound)
{
	// erase channels that match the soundName
	for (unsigned int i = 0; i < channels.size(); i++) {
		if (channels[i]->source == sound) {
			channels[i]->player.stop();
		}
	}
}

void mixer::silence()
{
	for (unsigned int i = 0; i < channels.size(); i++) {
		delete channels[i];
	}

	channels.clear();
}

void mixer::update()
{
	// find and delete channels that are done playing
	for (int i = 0; i < (int)channels.size(); i++) {
		if (channels[i]->player.getStatus() == sf::Sound::Stopped || !channels[i]->source->isLoaded()) {
			delete channels[i];
			channels.erase(channels.begin() + i);
			i--;
		}
	}
}

// SE
DeclareScriptingCustom(user_type<mixer>(), "mixer");
DeclareScriptingCustom(constructor<mixer()>(), "mixer");
DeclareScriptingCustom(constructor<mixer(const mixer&)>(), "mixer");
DeclareScriptingCustom(fun(&mixer::play), "play");
DeclareScriptingCustom(fun(&mixer::silence), "silence");
DeclareScriptingCustom(fun(&mixer::stop), "stop");
DeclareScriptingCustom(fun(&mixer::update), "update");
DeclareScriptingCustom(fun(&mixer::volume), "volume");
DeclareScriptingCustom(fun(&mixer::pitchShift), "pitchShift");