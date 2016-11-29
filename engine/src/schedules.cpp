#include "schedules.h"
#include "Game.h"

schedule::schedule()
{
	running = false;
	startTime = sf::seconds(0.f);

	conditionFunction = nullptr;
}

schedule::schedule(std::function<bool()> stopCond)
{
	running = false;
	startTime = sf::seconds(0.f);

	conditionFunction = stopCond;
}

void schedule::start()
{
	if (!running) {
		running = true;
		startTime = theGame()->getSimTime();

		onStart.notify(this);
	}
}

void schedule::end()
{
	if (running) {
		running = false;
		onEnd.notify(this);
	}
}

void schedule::run()
{
	if (conditionFunction) {
		if (conditionFunction()) {
			end();
		}
	}

	onRun.notify(this);
}

void schedule::update()
{
	if (running) {
		run();
	}
}

bool schedule::isRunning()
{
	return running;
}

sf::Time schedule::getStartTime()
{
	return startTime;
}

waitSchedule::waitSchedule(sf::Time toWait, std::function<void()> callee)
{
	length = toWait;
	onWaited.registerObserver(callee);
}

waitSchedule::waitSchedule(sf::Time toWait, std::function<void()> callee, std::function<bool()> stopCond)
	: schedule(stopCond)
{
	length = toWait;
	onWaited.registerObserver(callee);
}

void waitSchedule::run()
{
	// find difference between start time and now
	sf::Time dif = theGame()->getSimTime() - getStartTime();

	if (dif >= length) {
		onWaited.notify();
		end();
	}

	schedule::run();
}

tweenSchedule::tweenSchedule(sf::Time toTake, unsigned int phases, easingFunction ef, std::function<void(float)> callee)
{
	length = toTake;
	numFrames = phases;
	easing = ef;
	skipOnEnd = true;
	tLast = -1.f;

	onTween.registerObserver(callee);
}

tweenSchedule::tweenSchedule(sf::Time toTake, unsigned int phases, easingFunction ef, std::function<void(float)> callee, std::function<bool()> stopCond)
	: schedule(stopCond)
{
	length = toTake;
	numFrames = phases;
	easing = ef;
	skipOnEnd = true;
	tLast = -1.f;

	onTween.registerObserver(callee);
}

void tweenSchedule::start()
{
	schedule::start();
	notifyOnTween(0.f);
}

void tweenSchedule::run()
{
	// sanity
	if (!easing) {
		p::fatal("Invalid tweenSchedule easing function");
	}

	unsigned int fr = getFrame();

	if (fr >= numFrames) {
		end();
	}
	else {
		notifyOnTween(easing(fr, numFrames));
	}

	schedule::run();
}

void tweenSchedule::end()
{
	if (skipOnEnd) {
		notifyOnTween(1.f);
	}

	schedule::end();
}

unsigned int tweenSchedule::getFrame()
{
	// find difference between start time and now + frame length
	sf::Time dif = theGame()->getSimTime() - getStartTime();
	sf::Time frameLength = getFrameLength();

	// use the dif to find how many "frames" have elapsed
	float fr = floor(dif / frameLength);

	return (unsigned int)fr;
}

sf::Time tweenSchedule::getFrameLength()
{
	return sf::seconds(length.asSeconds() / numFrames);
}

void tweenSchedule::notifyOnTween(float val)
{
	if (val != tLast) {
		onTween.notify(val);
	}

	tLast = val;
}

tickSchedule::tickSchedule(sf::Time toWait, std::function<void(int)> callee)
{
	length = toWait;
	onWaited.registerObserver(callee);
	lastTickTime = theGame()->getSimTime();
}

tickSchedule::tickSchedule(sf::Time toWait, std::function<void(int)> callee, std::function<bool()> stopCond)
	: schedule(stopCond)
{
	length = toWait;
	onWaited.registerObserver(callee);
	lastTickTime = theGame()->getSimTime();
}

void tickSchedule::start()
{
	schedule::start();
	lastTickTime = getStartTime();
}

void tickSchedule::run()
{
	// find difference between start time and now
	sf::Time dif = getTimeSinceLastTick();

	if (dif >= length) {
		timesLooped++;
		lastTickTime = theGame()->getSimTime();

		onWaited.notify(timesLooped);
	}

	schedule::run();
}

sf::Time tickSchedule::getLastTickTime()
{
	return lastTickTime;
}

sf::Time tickSchedule::getTimeSinceLastTick()
{
	return theGame()->getSimTime() - lastTickTime;
}

unsigned int tickSchedule::getLoopCount()
{
	return timesLooped;
}

scheduleMngr::scheduleMngr()
{
}

std::shared_ptr<schedule> scheduleMngr::add(std::shared_ptr<schedule> input)
{
	tasks.push_back(input);
	input->start();

	return input;
}

int scheduleMngr::indexOf(std::shared_ptr<schedule> in)
{
	for (unsigned int i = 0; i < tasks.size(); i++) {
		if (tasks[i] == in) {
			return i;
		}
	}

	return -1;
}

std::shared_ptr<schedule> scheduleMngr::get(unsigned int index)
{
	if (index >= tasks.size()) {
		return nullptr;
	}

	return tasks[index];
}

unsigned int scheduleMngr::getCount()
{
	return tasks.size();
}

void scheduleMngr::endAll()
{
	for (unsigned int i = 0; i < tasks.size(); i++) {
		tasks[i]->end();
	}
}

void scheduleMngr::update(sf::Time elapsed)
{
	// update tasks that are running, forget about tasks that finished
	for (int i = 0; i < (int)tasks.size(); i++) {
		if (tasks[i]->isRunning()) {
			tasks[i]->update();
		}
		else {
			tasks.erase(tasks.begin() + i);
			i--;
		}
	}
}

std::shared_ptr<waitSchedule> scheduleMngr::wait(sf::Time toWait, std::function<void()> callee)
{
	return wait(toWait, callee, nullptr);
}

std::shared_ptr<waitSchedule> scheduleMngr::wait(sf::Time toWait, std::function<void()> callee, std::function<bool()> stopCond)
{
	auto s = std::make_shared<waitSchedule>(toWait, callee, stopCond);
	add(s);

	return s;
}

std::shared_ptr<tweenSchedule> scheduleMngr::tween(sf::Time toTake, unsigned int phases, easingFunction ef, std::function<void(float)> callee)
{
	return tween(toTake, phases, ef, callee, nullptr);
}

std::shared_ptr<tweenSchedule> scheduleMngr::tween(sf::Time toTake, unsigned int phases, easingFunction ef, std::function<void(float)> callee, std::function<bool()> stopCond)
{
	auto s = std::make_shared<tweenSchedule>(toTake, phases, ef, callee, stopCond);
	add(s);

	return s;
}

std::shared_ptr<tickSchedule> scheduleMngr::tick(sf::Time toWait, std::function<void(unsigned int)> callee)
{
	return tick(toWait, callee, nullptr);
}

std::shared_ptr<tickSchedule> scheduleMngr::tick(sf::Time toWait, std::function<void(unsigned int)> callee, std::function<bool()> stopCond)
{
	auto s = std::make_shared<tickSchedule>(toWait, callee, stopCond);
	add(s);

	return s;
}
