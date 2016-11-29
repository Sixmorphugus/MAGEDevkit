#pragma once
#include "StdAfx.h"
#include "SfmlAfx.h"

#include "easing.h"
#include "hook.h"

/*
The SCHEDULER is used for global animation tasks.
All tasks CAN take a function that has no arguments and returns a boolean, to verify whether they should keep running, but don't have to
Tasks take the form of different things:
- Wait Task: call a function (using a hook<>) after a set amount of time on update. Done using wait(), which returns a task.
			  (The simplest type of task)
- Tween Task: call a function (using a hook<float>) every update frame with a varying value. Done using tween(), which returns a task.
			  (These need an easing function, found in easing.h - if none (nullptr) is given, they will just use Linear)
			  (If cancelled, a tween will not stop in the middle but skip to the end unless skipOnCancel is set to false)
- Tick Task: call a function (using a hook<unsigned int>) every set amount of time forever. Done using tick(), which returns a task.
			  (other than the int argument, just a wait task that doesn't cancel after happening once)
*/

// you can instantiate the default task but it will go on forever and does nothing.
// if you want to do something interesting, however, you can use the HOOKS to make a custom task with its own behavior.
class MAGEDLL schedule {
public:
	schedule();
	schedule(std::function<bool()> stopCond);

	virtual void start(); // Starts the task, called when the task is given to a scheduler.
	virtual void end(); // Ends the task, called either manually to cancel the task or by update() when the task thinks it is finished.
	virtual void run(); // continues the task as it runs

	void update();

	bool isRunning(); // if this returns false to the scheduler the scheduler ususally deletes it.

	sf::Time getStartTime();

public:
	std::function<bool()> conditionFunction;

	hook<schedule*> onStart;
	hook<schedule*> onEnd;
	hook<schedule*> onRun;
private:
	bool running;
	sf::Time startTime;
};

// wait task.
class MAGEDLL waitSchedule : public schedule {
public:
	waitSchedule(sf::Time toWait, std::function<void()> callee);
	waitSchedule(sf::Time toWait, std::function<void()> callee, std::function<bool()> stopCond);

	void run(); // if it is time, call the hook and stop the task.

public:
	hook<> onWaited;
	sf::Time length;
};

// tween task.
class MAGEDLL tweenSchedule : public schedule {
public:
	tweenSchedule(sf::Time toTake, unsigned int phases, easingFunction ef, std::function<void(float)> callee);
	tweenSchedule(sf::Time toTake, unsigned int phases, easingFunction ef, std::function<void(float)> callee, std::function<bool()> stopCond);

	void start();
	void run(); // if it is time, call the hook and stop the task.
	void end(); // if ended, make sure the tween FINISHED before calling the original end() function.

	unsigned int getFrame();
	sf::Time getFrameLength();

private:
	void notifyOnTween(float val);

public:
	hook<float> onTween;

	easingFunction easing;

	sf::Time length;
	unsigned int numFrames;
	
	bool skipOnEnd;

	float tLast;
};

// tick task.
class MAGEDLL tickSchedule : public schedule {
public:
	tickSchedule(sf::Time toWait, std::function<void(int)> callee);
	tickSchedule(sf::Time toWait, std::function<void(int)> callee, std::function<bool()> stopCond);

	void start();
	void run(); // if it is time, call the hook and reset the startTime.

	sf::Time getLastTickTime();
	sf::Time getTimeSinceLastTick();

	unsigned int getLoopCount();
public:
	hook<int> onWaited;
	sf::Time length;

private:
	sf::Time lastTickTime;
	unsigned int timesLooped;
};

// the manager object
class MAGEDLL scheduleMngr {
public:
	scheduleMngr();

	std::shared_ptr<schedule> add(std::shared_ptr<schedule> input);

	int indexOf(std::shared_ptr<schedule> in);

	std::shared_ptr<schedule> get(unsigned int index);
	unsigned int getCount();

	void endAll();

	void update(sf::Time elapsed);

	// adder functions
	std::shared_ptr<waitSchedule> wait(sf::Time toWait, std::function<void()> callee);
	std::shared_ptr<waitSchedule> wait(sf::Time toWait, std::function<void()> callee, std::function<bool()> stopCond);

	std::shared_ptr<tweenSchedule> tween(sf::Time toTake, unsigned int phases, easingFunction ef, std::function<void(float)> callee);
	std::shared_ptr<tweenSchedule> tween(sf::Time toTake, unsigned int phases, easingFunction ef, std::function<void(float)> callee, std::function<bool()> stopCond);

	std::shared_ptr<tickSchedule> tick(sf::Time toWait, std::function<void(unsigned int)> callee);
	std::shared_ptr<tickSchedule> tick(sf::Time toWait, std::function<void(unsigned int)> callee, std::function<bool()> stopCond = nullptr);

private:
	std::vector<std::shared_ptr<schedule>> tasks;
};