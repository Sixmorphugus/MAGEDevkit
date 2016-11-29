#include "animationManager.h"
#include "platform.h"

#include "Game.h"

#include "sfResources.h"

animator::animator(std::shared_ptr<sfTextureResource> obj)
{
	textureUsed = obj;

	playing = false;
	autoStop = false;

	if(!textureUsed)
		p::warn("No texture was passed to the animator.")
}

void animator::update(sf::Sprite &spr, sf::Time elapsed)
{
	if (playing) {
		if (!textureUsed->isLoaded()) {
			p::fatal("Texture resource used by animator was unloaded without warning");
		}

		auto curAnimation = &animations[curAnimationName];

		toFrameEnd -= elapsed;

		// go to the next frame yet?
		if (toFrameEnd.asSeconds() <= 0.f) {
			animationProgress++;
			resetFrameEndTimer();

			// is the animation over?
			if (animationProgress >= curAnimation->frames.size()) {
				animationProgress = curAnimation->frames.size()-1;

				if (autoStop) {
					stop();
				}
				else {
					animationProgress = 0;
				}
			}
		}

		// set basic's frame to the current animation frame
		setFrame(spr, curAnimation->frames[animationProgress], false);
	}
}

bool animator::isPlaying()
{
	return playing;
}

std::string animator::getPlaying()
{
	return curAnimationName;
}

void animator::add(std::string name, std::vector<unsigned int> frames, sf::Time duration)
{
	if (isAnimation(name)) {
		//p::info("Animation already exists with name " + name + ", replacing");
	}

	if (frames.size() == 0) {
		p::warn("Unable to register animation " + name + "; no frames");
		return;
	}

	animation a;
	a.frames = frames;
	a.time = duration;

	animations[name] = a;
}

void animator::play(std::string name, bool loop)
{
	if (!isAnimation(name)) {
		p::warn("No such animation as " + name);
		return;
	}

	if (curAnimationName == name) {
		return;
	}

	curAnimationName = name;
	playing = true;
	autoStop = !loop;
	animationProgress = 0;
	resetFrameEndTimer();
}

void animator::stop()
{
	curAnimationName = "";
	playing = false;
}

bool animator::isAnimation(std::string name)
{
	return animations.count(name) > 0;
}

void animator::resetFrameEndTimer()
{
	if (!playing) {
		return;
	}

	// calculate how long it takes for a single frame to pass in a total animation time
	animation* curAnimation = &animations[curAnimationName];

	sf::Time total = curAnimation->time;
	sf::Time one = (total / (float)curAnimation->frames.size()) * theGame()->tickScale;

	toFrameEnd = one;
}

void animator::setFrame(sf::Sprite &spr, unsigned int frame, bool interrupt) {
	if (frame >= frameRects.size()) {
		//p::warn("Bad animation frame " + std::to_string(frame));
		return;
	}

	spr.setTextureRect(frameRects[frame]);

	if (interrupt)
		stop();
}

unsigned int animator::getFrame(sf::Sprite & spr) const
{
	return getFrameFromRect(spr.getTextureRect());
}

unsigned int animator::getFrameFromRect(sf::IntRect fr) const
{
	for (unsigned int i = 0; i < frameRects.size(); i++) {
		if (fr == frameRects[i]) {
			return i;
		}
	}

	return 0;
}

unsigned int animator::framesToEnd()
{
	if (!isAnimation(curAnimationName))
		return 0;

	animation* curAnimation = &animations[curAnimationName];
	return curAnimation->frames.size() - animationProgress;
}

void animator::generateFrameRects(unsigned int spriteWidth, unsigned int spriteHeight)
{
	if (!textureUsed)
		return;

	stop();

	frameRects.clear();

	auto texture = textureUsed->get();

	unsigned int spriteColumns = texture->getSize().x / spriteWidth;
	unsigned int spriteRows = texture->getSize().y / spriteHeight;

	for (unsigned int j = 0; j < spriteRows; j++) {
		for (unsigned int i = 0; i < spriteColumns; i++) {
			frameRects.push_back(sf::IntRect(i * spriteWidth, j * spriteHeight, spriteWidth, spriteHeight));
		}
	}
}

std::vector<unsigned int> animator::getFrameList()
{
	std::vector<unsigned int> v;

	for (unsigned int i = 0; i < frameRects.size(); i++) {
		v.push_back(i);
	}

	return v;
}

// SE
#include "scriptingEngine.h"

DeclareScriptingCustom(user_type<animator::animation>(), "animation");
DeclareScriptingCustom(constructor<animator::animation()>(), "animation");
DeclareScriptingCustom(constructor<animator::animation(const animator::animation&)>(), "animation");
DeclareScriptingCustom(fun(&animator::animation::frames), "frames");
DeclareScriptingCustom(fun(&animator::animation::time), "time");
DeclareScriptingCopyOperator(animator::animation);

DeclareScriptingCustom(user_type<animator>(), "animator");
DeclareScriptingCustom(constructor<animator(std::shared_ptr<sfTextureResource> in)>(), "animator");
DeclareScriptingCustom(constructor<animator(const animator&)>(), "animator");
DeclareScriptingCustom(fun(&animator::add), "add");
DeclareScriptingCustom(fun(&animator::textureUsed), "textureUsed");
DeclareScriptingCustom(fun(&animator::frameRects), "frameRects");
DeclareScriptingCustom(fun(&animator::generateFrameRects), "generateFrameRects");
DeclareScriptingCustom(fun(&animator::getFrameList), "getFrameList");
DeclareScriptingCustom(fun(&animator::getPlaying), "getPlaying");
DeclareScriptingCustom(fun(&animator::isAnimation), "isAnimation");
DeclareScriptingCustom(fun(&animator::isPlaying), "isPlaying");
DeclareScriptingCustom(fun(&animator::play), "play");
DeclareScriptingCustom(fun(&animator::setFrame), "setFrame");
DeclareScriptingCustom(fun(&animator::stop), "stop");
DeclareScriptingCustom(fun(&animator::update), "update");
DeclareScriptingCopyOperator(animator);