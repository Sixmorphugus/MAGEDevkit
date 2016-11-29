#pragma once
#include "StdAfx.h"

/*
Tweens need an easing function. The tween easing systems are:
- Linear (doesn't do anything with its easeMode)
- Sine
- Quad
- Cubic
- Circ
- Back
- Elastic
- Bounce

An easing function is given in the following format:
float easingFunction(int step, int numSteps, easeMode m)
These return smoothly from 0.0 to 1.0 depending on the first two arguments.
*/

typedef std::function<float(unsigned int, unsigned int)> easingFunction;

namespace ease {
	float linear(unsigned int step, unsigned int numSteps);

	namespace sine {
		float in(unsigned int step, unsigned int numSteps);
		float out(unsigned int step, unsigned int numSteps);
	}

	namespace quad {
		float in(unsigned int step, unsigned int numSteps);
		float out(unsigned int step, unsigned int numSteps);
	}

	namespace circ {
		float in(unsigned int step, unsigned int numSteps);
		float out(unsigned int step, unsigned int numSteps);
	}

	namespace back {
		float in(unsigned int step, unsigned int numSteps);
		float out(unsigned int step, unsigned int numSteps);
	}

	namespace elastic {
		float in(unsigned int step, unsigned int numSteps);
		float out(unsigned int step, unsigned int numSteps);
	}

	namespace bounce {
		float in(unsigned int step, unsigned int numSteps);
		float out(unsigned int step, unsigned int numSteps);
	}
}