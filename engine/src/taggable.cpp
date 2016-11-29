#include "taggable.h"

taggable::taggable()
{
}

bool taggable::hasTag(std::string tag)
{
	for (unsigned int i = 0; i < tags.size(); i++) {
		if (tags[i] == tag)
			return true;
	}

	return false;
}

// scripting
#include "scriptingEngine.h"

DeclareScriptingType(taggable);
DeclareScriptingConstructor(taggable(), "taggable");
DeclareScriptingFunction(&taggable::hasTag, "hasTag");
DeclareScriptingFunction(&taggable::tags, "tags");