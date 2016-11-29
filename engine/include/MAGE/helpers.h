#pragma once
#include "StdAfx.h"
#include "SfmlAfx.h"

// Helpers! Making life bearable.

// STRINGS
// ---------------------------------------------------------------------
MAGEDLL std::vector<std::string> splitString(std::string in, char c = ' ', char ignoreBetween = ' ', unsigned int expectation = 0);
MAGEDLL std::vector<std::string> splitStringAtFirst(std::string in, char c = ' ', char ignoreBetween = ' ');
MAGEDLL std::string strUpper(std::string in);
MAGEDLL std::string strLower(std::string in);
MAGEDLL std::string strReplace(std::string in, char replace, char with);
MAGEDLL std::string strFilter(std::string in, char replace);
MAGEDLL std::string strSub(std::string in, unsigned int start, unsigned int finish);
MAGEDLL void removeNewline(std::string &str);
MAGEDLL std::string fileExtension(std::string path);
MAGEDLL std::string containerDir(std::string path);
MAGEDLL std::string fileName(std::string path);

// INTERPOLATION
// ---------------------------------------------------------------------
MAGEDLL sf::Color interpolateColors(sf::Color a, sf::Color b, int stepNumber, int lastStepNumber);

// MATHS
// ---------------------------------------------------------------------
MAGEDLL float lerp(float a, float b, float f);
MAGEDLL int interpolate(int startValue, int endValue, int stepNumber, int lastStepNumber);
MAGEDLL int randomInt(int low, int high);
MAGEDLL float randomFloat(float low, float high);
MAGEDLL bool isMultiple(float testNum, float of);
MAGEDLL float vectorToAngle(sf::Vector2f vec);
MAGEDLL float vectorLength(sf::Vector2f vec);
MAGEDLL float distanceBetweenVectors(sf::Vector2f v1, sf::Vector2f v2);
MAGEDLL float directionToVector(sf::Vector2f v1, sf::Vector2f v2);
MAGEDLL sf::Vector2f generateVector(float angle, float speed);
MAGEDLL float dot(sf::Vector2f v1, sf::Vector2f v2);
MAGEDLL float clamp(float in, float min, float max);
MAGEDLL sf::Vector2f vectorFloor(sf::Vector2f vec);
MAGEDLL sf::Vector2f vectorCeil(sf::Vector2f vec);

// CONDITIONAL
// ---------------------------------------------------------------------
template<typename T> unsigned int highestOf(std::vector<T> inVecs) {
	unsigned int match = 0;
	T held = inVecs[0];

	for (unsigned int i = 1; i < inVecs.size(); i++) {
		if (inVecs[i] > held) {
			match = i;
			held = inVecs[i];
		}
	}

	return match;
}

template<typename T> unsigned int lowestOf(std::vector<T> inVecs) {
	unsigned int match = 0;
	T held = inVecs[0];

	for (unsigned int i = 1; i < inVecs.size(); i++) {
		if (inVecs[i] < held) {
			match = i;
			held = inVecs[i];
		}
	}

	return match;
}

// ENGINE
// ---------------------------------------------------------------------
class objBasic;
class uiBasic;

MAGEDLL float distanceBetweenObjects(objBasic* i, objBasic* j);
MAGEDLL float randomPitch();