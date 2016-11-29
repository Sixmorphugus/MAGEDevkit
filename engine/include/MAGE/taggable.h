#pragma once
#include "platform.h"

// basic taggable object
class MAGEDLL taggable {
public:
	taggable();
	bool hasTag(std::string tag);

public:
	std::vector<std::string> tags;
};