#include "jukebox.h"
#include "Game.h"
#include "helpers.h"

#include "sfResources.h"

jukebox::jukebox()
{
	volume = 100.f;
	pitchShiftPitch = 1.f;
	pitchShift = 1.f;
	pitchShiftMultiplier = 0.1f;

	playing = false;
}

void jukebox::playSong(std::shared_ptr<sfMusicResource> song)
{
	if (!song) {
		stopPlaying();
		return;
	}

	// check the resource is nontype
	p::info("playing " + theGame()->resources->nameOf(song));

	stopPlaying();

	lp = song;

	player = song;
	altPlayer.reset();

	usingAltPlayer = false;

	player->get()->setVolume(volume);

	// play
	resume();
}

void jukebox::stopPlaying(bool fadeOut)
{
	lp.reset();

	if (fadeOut) {
		if (usingAltPlayer) {
			if (player) player->get()->stop();
		}
		else {
			if (altPlayer) altPlayer->get()->stop();
		}

		usingAltPlayer = !usingAltPlayer; // fade to a player that is no longer playing
	}
	else {
		usingAltPlayer = false;
		playing = false;

		if(player) player->get()->stop();
		if(altPlayer) altPlayer->get()->stop();
	}
}

void jukebox::pause()
{
	if (player) player->get()->pause();
	if (altPlayer) altPlayer->get()->pause();

	playing = false;
}

void jukebox::resume()
{
	if (player) player->get()->play();
	if (altPlayer) altPlayer->get()->play();

	playing = true;
}

void jukebox::fadeToSong(std::shared_ptr<sfMusicResource> song)
{
	if (!song) {
		stopPlaying(true);
		return;
	}

	p::info("fading to " + theGame()->resources->nameOf(song));

	if (lp.lock() == song) {
		p::info("already playing!");
		return;
	}

	lp = song;

	if (usingAltPlayer) {
		player = song;

		if(altPlayer) altPlayer->get()->setVolume(volume);
		player->get()->setVolume(0.f);
		player->get()->play();
	}
	else {
		altPlayer = song;

		if (player) player->get()->setVolume(volume);
		altPlayer->get()->setVolume(0.f);
		altPlayer->get()->play();
	}

	usingAltPlayer = !usingAltPlayer;
}

void jukebox::update()
{
	if (player) {
		if (player.unique() || !player->isLoaded()) {
			p::warn("Music resource(s) unloaded while playing!");
			stopPlaying();
		}
	}

	if (altPlayer) {
		if (altPlayer.unique() || !altPlayer->isLoaded()) {
			p::warn("Music resource(s) unloaded while playing!");
			stopPlaying();
		}
	}

	pitchShift = clamp(pitchShift, 0, 100);
	pitchShiftMultiplier = clamp(pitchShiftMultiplier, 0, 1);

	pitchShiftPitch += ((pitchShift - pitchShiftPitch) * pitchShiftMultiplier);

	// fade between the two players based on the boolean.
	float firstV = 0.f;
	if(player) firstV = player->get()->getVolume();

	float secondV = 0.f;
	if(altPlayer) secondV = altPlayer->get()->getVolume();

	float maxV = volume;
	float incV = 1.f;

	if (usingAltPlayer) {
		if (altPlayer) altPlayer->get()->setPitch(pitchShiftPitch / theGame()->tickScale);

		//p::info("equalizing second player");

		// decrease the first volume if it's above 0
		if (firstV > 0.f && player) {
			float nV = firstV - incV < 0.f ? 0.f : firstV - incV;
			player->get()->setVolume(nV);
		}

		// increase the second volume if it's below maxV
		if (secondV < maxV && altPlayer) {
			float nV = secondV + incV > maxV ? maxV : secondV + incV;
			altPlayer->get()->setVolume(nV);
		}
		else if(altPlayer) {
			// set second volume to maxV
			altPlayer->get()->setVolume(maxV);
		}
	}
	else {
		if (player) player->get()->setPitch(pitchShiftPitch / theGame()->tickScale);

		//p::info("equalizing first player");

		if (secondV > 0.f && altPlayer) {
			float nV = secondV - incV < 0.f ? 0.f : secondV - incV;
			altPlayer->get()->setVolume(nV);
		}

		if (firstV < maxV && player) {
			float nV = firstV + incV > maxV ? maxV : firstV + incV;
			player->get()->setVolume(nV);
		}
		else if(player) {
			// set second volume to maxV
			player->get()->setVolume(maxV);
		}
	}
}

// SE
DeclareScriptingCustom(user_type<jukebox>(), "jukebox");
DeclareScriptingCustom(constructor<jukebox()>(), "jukebox");
DeclareScriptingCustom(fun(&jukebox::fadeToSong), "fadeToSong");
DeclareScriptingCustom(fun(&jukebox::getLastPlayed), "getLastPlayed");
DeclareScriptingCustom(fun(&jukebox::pause), "pause");
DeclareScriptingCustom(fun(&jukebox::pitchShift), "pitchShift");
DeclareScriptingCustom(fun(&jukebox::pitchShiftMultiplier), "pitchShiftMultiplier");
DeclareScriptingCustom(fun(&jukebox::playSong), "playSong");
DeclareScriptingCustom(fun(&jukebox::resume), "resume");
DeclareScriptingCustom(fun(&jukebox::stopPlaying), "stopPlaying");
DeclareScriptingCustom(fun(&jukebox::update), "update");
DeclareScriptingCustom(fun(&jukebox::volume), "volume");