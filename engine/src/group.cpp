#include "group.h"

#include "helpers.h"
#include "Game.h"
#include "sfResources.h"

namespace groupPrivate {
	bool compareObjects(std::shared_ptr<basic> i, std::shared_ptr<basic> j) {
		auto woi = dynamic_cast<objBasic*>(i.get());
		auto woj = dynamic_cast<objBasic*>(j.get());

		if (woi && woj) {
			return woi->operator<(*woj); // two objBasics
		}

		p::fatal("A world object group was found to be malformed during a depth ordering operation.");
		return false; // if this is called, uiObjectStart is in the wrong place
	}

	prefabMngr * prefabs()
	{
		return theGame()->prefabs.get();
	}

	void drawLoadingScreenFrame(std::string text, int prog, int maxProg)
	{
		theGame()->drawLoadingScreenFrame(text, prog, maxProg);
	}
}

void groupBase::drawWorldObjects(sf::RenderTarget & target, sf::RenderStates states)
{
	for (unsigned int i = 0; i < getNumWorldObjects(); i++) {
		applyShader(states, getWorldObject(i)->getTexSizeF());
		target.draw(*getWorldObject(i), states);
	}

	if (theGame()->colBoxes) {
		for (unsigned int i = 0; i < getNumWorldObjects(); i++) {
			getWorldObject(i)->drawCollisionBoxes(target, states);
		}
	}
}

void groupBase::drawUiObjects(sf::RenderTarget & target, sf::RenderStates states)
{
	for (unsigned int i = 0; i < getNumUiObjects(); i++) {
		applyShader(states, getUiObject(i)->getTexSizeF());
		target.draw(*getUiObject(i), states);
	}
}


// binding
#include "scriptingEngine.h"

// groupBase
DeclareScriptingType(groupBase);
DeclareScriptingBaseClass(serializable, groupBase);
DeclareScriptingBaseClass(shadable, groupBase);
DeclareScriptingFunction(&groupBase::bringToFront, "bringToFront");
DeclareScriptingFunction(&groupBase::clearObjects, "clearObjects");
DeclareScriptingFunction(&groupBase::serialize, "serialize");
DeclareScriptingFunction(&groupBase::deserialize, "deserialize");
DeclareScriptingCustom(fun<void, groupBase, unsigned int>(&groupBase::detach), "detach");
DeclareScriptingCustom(fun<void, groupBase, basic*>(&groupBase::detach), "detach");
DeclareScriptingFunction(&groupBase::getNumObjects, "getNumObjects");
DeclareScriptingFunction(&groupBase::getNumUiObjects, "getNumUiObjects");
DeclareScriptingFunction(&groupBase::getNumWorldObjects, "getNumWorldObjects");
DeclareScriptingFunction(&groupBase::getObject, "getObject");
DeclareScriptingFunction(&groupBase::getUiObject, "getUiObject");
DeclareScriptingFunction(&groupBase::getWorldObject, "getWorldObject");
DeclareScriptingFunction(&groupBase::preUpdateObjects, "preUpdateObjects");
DeclareScriptingFunction(&groupBase::sortWorldObjects, "sortWorldObjects");
DeclareScriptingFunction(&groupBase::updateObjects, "updateObjects");
DeclareScriptingFunction(&groupBase::onPreUpdate, "onPreUpdate");
DeclareScriptingFunction(&groupBase::onUpdate, "onUpdate");
DeclareScriptingFunction(&groupBase::onAttach, "onAttach");
DeclareScriptingFunction(&groupBase::onDetach, "onDetach");
DeclareScriptingFunction(&groupBase::onSet, "onSet");
DeclareScriptingFunction(&groupBase::onCombined, "onCombined");
DeclareScriptingFunction(&groupBase::onIncorporated, "onIncorporated");

DeclareScriptingHook("groupBaseHook", groupBase*);
DeclareScriptingHook("groupBaseObjectHook", groupBase*, basic*);
DeclareScriptingHook("groupBaseIntegrateHook", const groupBase*, const groupBase*);

// group types
DeclareScriptingGroup(basic, "Group");
DeclareScriptingCustom(fun(&group<basic>::combine<basic, uiBasic>), "combineUi"); \
DeclareScriptingCustom(fun(&group<basic>::combine<basic, objBasic>), "combineWorld"); \

DeclareScriptingGroup(objBasic, "objGroup");
DeclareScriptingGroup(uiBasic, "uiGroup");