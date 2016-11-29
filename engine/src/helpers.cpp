#include "helpers.h"

#include "basic.h"
#include "platform.h"

// This file is just all of the implementations for helpers in Super Alchemist.
// Not gonna lie, got most of these from stack overflow.

std::vector<std::string> splitString(std::string in, char c, char ignoreBetween, unsigned int expectation) {
	std::stringstream test(in);
	std::string segment;
	std::vector<std::string> seglist;

	while (std::getline(test, segment, c))
	{
		seglist.push_back(segment);
	}

	// ignore between
	if (ignoreBetween != ' ') {
		// go through each character of each segment
		for (int i = 0; i < (int)seglist.size(); i++) {
			bool combineWithNext = false;
			for (unsigned int j = 0; j < seglist[i].size(); j++) {
				if (seglist[i][j] == ignoreBetween) {
					combineWithNext = !combineWithNext;
				}
			}

			if (combineWithNext && i + 1 < (int)seglist.size()) {
				seglist[i] = seglist[i] + c + seglist[i + 1];
				seglist.erase(seglist.begin() + i + 1);

				i--;
			}
		}
	}

	if (seglist.size() < expectation) {
		p::fatal("splitString returned too few values (found " + std::to_string(seglist.size()) + ", expected " + std::to_string(expectation) + ")");
	}

	return seglist;
}

MAGEDLL std::vector<std::string> splitStringAtFirst(std::string in, char c, char ignoreBetween)
{
	auto seglist = splitString(in, c, ignoreBetween);

	if (seglist.size() > 2) {
		std::string sData;
		std::string charString({ c });

		for (unsigned int i = 1; i < seglist.size(); i++) {
			sData += (i != 1 ? charString : "") + seglist[i];
		}

		seglist = { seglist[0], sData };
	}

	return seglist;
}

std::string strUpper(std::string in) {
	std::locale loc;

	for (unsigned int i = 0; i < in.size(); i++) {
		in[i] = std::toupper(in[i], loc);
	}

	return in;
}

std::string strLower(std::string in) {
	std::locale loc;

	for (unsigned int i = 0; i < in.size(); i++) {
		in[i] = std::tolower(in[i], loc);
	}

	return in;
}

std::string strReplace(std::string in, char replace, char with) {
	std::replace(in.begin(), in.end(), replace, with);
	return in;
}

std::string strFilter(std::string in, char replace)
{
	for (int i = 0; i < (int)in.size(); i++) {
		if (in[i] == replace) {
			in.erase(in.begin() + i);
			i--;
		}
	}

	return in;
}

std::string strSub(std::string in, unsigned int start, unsigned int finish)
{
	std::string output;

	for (unsigned int i = start; i < finish, i < in.size(); i++) {
		output.push_back(in[i]);
	}

	// output.push_back('\0'); is unneded in std::string apparently

	return output;
}

void removeNewline(std::string &str) {
#ifndef PLATFORM_WINDOWS
    if(str[str.size() - 1] == '\n' || str[str.size() - 1] == '\r') {
        str.erase(str.size()-1);
    }
#endif
}

std::string fileExtension(std::string path)
{
	auto sp = splitString(path, '.');
	return sp[sp.size() - 1];
}

std::string containerDir(std::string path)
{
	auto split = splitString(path, '/');

	if (split.size() <= 1) {
		// try splitting with windows '\' instead
		split = splitString(path, '\\');
	}

	std::stringstream s;

	for (unsigned int i = 0; i < split.size() - 1; i++) {
		s << split[i] << "/";
	}

	return s.str();
}

std::string fileName(std::string path)
{
	auto split = splitString(path, '/');

	return split[split.size() - 1];
}

int interpolate(int startValue, int endValue, int stepNumber, int lastStepNumber)
{
	return (endValue - startValue) * stepNumber / lastStepNumber + startValue;
}

float lerp(float a, float b, float f)
{
	return (a * (1.0f - f)) + (b * f);
}

sf::Color interpolateColors(sf::Color a, sf::Color b, int stepNumber, int lastStepNumber) {
	sf::Color finalc;

	finalc.r = interpolate(a.r, b.r, stepNumber, lastStepNumber);
	finalc.g = interpolate(a.g, b.g, stepNumber, lastStepNumber);
	finalc.b = interpolate(a.b, b.b, stepNumber, lastStepNumber);
	finalc.a = a.a;

	return finalc;
}

int randomInt(int min, int max) //range : [min, max)
{
	if (!(max - min)) {
		return min;
	}

	static bool first = true;
	if ( first )
	{
		srand((unsigned int)time(NULL)); //seeding for the first time only!
		first = false;
	}
	return min + rand() % (max - min);
}

float randomFloat(float low, float high)
{
	float r3 = low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
	return r3;
}

bool isMultiple(float testNum, float of) {
	return (testNum / of) == floor(testNum / of);
}

float vectorToAngle(sf::Vector2f vec)
{
	return atan2(vec.y, vec.x);
}

float vectorLength(sf::Vector2f vec)
{
	return sqrt(vec.y*vec.y + vec.x*vec.x); // this is just pythag again isnt it
}

float distanceBetweenVectors(sf::Vector2f v1, sf::Vector2f v2)
{
	// pythagoras method
	float xDist = fabs(v1.x - v2.x);
	float yDist = fabs(v1.y - v2.y);

	return sqrt(pow(xDist, 2) + pow(yDist, 2));
}

float directionToVector(sf::Vector2f v1, sf::Vector2f v2)
{
	sf::Vector2f v3(v1 - v2);

	return vectorToAngle(v3);
}

sf::Vector2f generateVector(float angle, float speed)
{
	sf::Vector2f vector;

	vector.x = cos(angle);
	vector.y = sin(angle);

	vector *= speed;

	return vector;
}

float dot(sf::Vector2f v1, sf::Vector2f v2)
{
	return (v1.x*v2.x + v1.y*v2.y);
}

float clamp(float in, float min, float max)
{
	if (in < min) in = min;
	if (in > max) in = max;

	return in;
}

sf::Vector2f vectorFloor(sf::Vector2f vec)
{
	return sf::Vector2f(floor(vec.x), floor(vec.y));
}

sf::Vector2f vectorCeil(sf::Vector2f vec)
{
	return sf::Vector2f(ceil(vec.x), ceil(vec.y));
}

float distanceBetweenObjects(objBasic * i, objBasic * j)
{
	return distanceBetweenVectors(i->getCenter(), j->getCenter());
}

float randomPitch()
{
	return 1.f + ((float)randomInt(-2, 2) / 10.f);
}

#include "scriptingEngine.h"
#line 40000
DeclareScriptingCustom(fun(&splitString), "splitString");
DeclareScriptingCustom(fun(&strUpper), "strUpper");
DeclareScriptingCustom(fun(&strLower), "strLower");
DeclareScriptingCustom(fun(&strReplace), "strReplace");
DeclareScriptingCustom(fun(&strFilter), "strFilter");
DeclareScriptingCustom(fun(&strSub), "strSub");
DeclareScriptingCustom(fun(&fileExtension), "fileExtension");

DeclareScriptingCustom(fun(&interpolateColors), "interpolateColors");
DeclareScriptingCustom(fun(&lerp), "lerp");
DeclareScriptingCustom(fun(&interpolate), "interpolate");
DeclareScriptingCustom(fun(&randomInt), "randomInt");
DeclareScriptingCustom(fun(&randomFloat), "randomFloat");
DeclareScriptingCustom(fun(&isMultiple), "isMultiple");
DeclareScriptingCustom(fun(&vectorToAngle), "vectorToAngle");
DeclareScriptingCustom(fun(&vectorLength), "vectorLength");
DeclareScriptingCustom(fun(&distanceBetweenVectors), "distanceBetweenVectors");
DeclareScriptingCustom(fun(&directionToVector), "directionToVector");
DeclareScriptingCustom(fun(&generateVector), "generateVector");
DeclareScriptingCustom(fun(&dot), "dot");
DeclareScriptingCustom(fun(&clamp), "clamp");

DeclareScriptingCustom(fun(&highestOf<float>), "highestOf");
DeclareScriptingCustom(fun(&lowestOf<float>), "lowestOf");

DeclareScriptingCustom(fun(&distanceBetweenObjects), "distanceBetweenObjects");