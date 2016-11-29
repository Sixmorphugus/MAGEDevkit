#include "prefabs.h"
#include "group.h"

prefab::prefab()
{
	templateObject = nullptr;
}

prefab::prefab(std::shared_ptr<basic> clonable, std::vector<std::string> t)
{
	if (!clonable->isCloneSafe())
		p::warn("Prefab failed clone test! Expect some odd behavior.");

	setTemplate(clonable);
	tags = t;
}

prefab::prefab(const prefab & p)
{
	setTemplate(p.templateObject);
	tags = p.tags;
}

prefab& prefab::operator=(const prefab& p) {
	setTemplate(p.templateObject);
	tags = p.tags;

	return *this;
}

void prefab::setTemplate(std::shared_ptr<basic> clonable)
{
	templateObject = clonable;
}

std::shared_ptr<basic> prefab::copyTemplate()
{
	if (templateObject)
		return std::shared_ptr<basic>(templateObject->clone());
	else {
		p::warn("newInstance was called on a prefab that is uninitialized!")
		return nullptr;
	}
}

std::string prefab::name()
{
	return knownName;
}

// prefab manager
prefabMngr::prefabMngr()
{
	// ...
}

void prefabMngr::add(std::string name, std::shared_ptr<prefab> input)
{
	if (exists(name)) {
		p::warn("Prefab already exists with name \"" + name + "\" - replacing");
	}

	prefabMap[name] = input;
	prefabMap[name]->knownName = name;

	p::info("Registered prefab \"" + name + "\"");
}

bool prefabMngr::exists(std::string name)
{
	return prefabMap.count(name) != 0;
}

std::string prefabMngr::nameOf(std::shared_ptr<prefab> input)
{
	// now we must iterate
	for (auto i = prefabMap.begin(); i != prefabMap.end(); i++) {
		if (i->second == input) {
			return i->first;
		}
	}

	return "";
}

std::shared_ptr<prefab> prefabMngr::get(std::string name)
{
	if (exists(name))
		return prefabMap[name];
	else
		return nullptr;
}

std::shared_ptr<prefab> prefabMngr::getByIndex(unsigned int index)
{
	auto it = prefabMap.begin();

	for (unsigned int i = 0; i < index; i++) {
		it++;
	}

	return it->second;
}

unsigned int prefabMngr::getCount()
{
	return prefabMap.size();
}

std::vector<std::shared_ptr<prefab>> prefabMngr::list(std::string tag)
{
	std::vector<std::shared_ptr<prefab>> results;

	// now we must iterate
	for (auto i = prefabMap.begin(); i != prefabMap.end(); i++) {
		if (i->second->hasTag(tag)) {
			results.push_back(i->second);
		}
	}

	return results;
}

std::shared_ptr<basic> prefabMngr::newInstance(std::string name, Group* attachTo)
{
	if (exists(name)) {
		std::shared_ptr<basic> ni = prefabMap[name]->copyTemplate();
		ni->prefabSource = prefabMap[name];

		if (attachTo)
			attachTo->attach(ni);

		return ni;
	}

	return nullptr;
}

// SE Binding
#include "scriptingEngine.h"

// for prefab
DeclareScriptingType(prefab);
DeclareScriptingBaseClass(taggable, prefab);
DeclareScriptingBaseClass(serializable, prefab);
DeclareScriptingConstructor(prefab(std::shared_ptr<basic> refersTo, std::vector<std::string> strList), "prefab");
DeclareScriptingCustom(fun([](std::shared_ptr<basic> refersTo) { return prefab(refersTo); }), "prefab"); // simplified constructor
DeclareScriptingConstructor(prefab(), "prefab"); // very simplified constructor
DeclareScriptingFunction(&prefab::copyTemplate, "copyTemplate");
DeclareScriptingFunction(&prefab::setTemplate, "setTemplate");
DeclareScriptingFunction(&prefab::name, "name");

// for prefabMngr
DeclareScriptingType(prefabMngr);
DeclareScriptingFunction(&prefabMngr::add, "add");
DeclareScriptingFunction(&prefabMngr::exists, "exists");
DeclareScriptingFunction(&prefabMngr::get, "get");
DeclareScriptingFunction(&prefabMngr::getByIndex, "getByIndex");
DeclareScriptingFunction(&prefabMngr::getCount, "getCount");
DeclareScriptingFunction(&prefabMngr::list, "list");
DeclareScriptingFunction(&prefabMngr::nameOf, "nameOf");
DeclareScriptingFunction(&prefabMngr::newInstance, "newInstance");
DeclareScriptingFunction([](prefabMngr &pm, std::string instName) { return pm.newInstance(instName); }, "newInstance");