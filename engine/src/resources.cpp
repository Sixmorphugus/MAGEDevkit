#include "resources.h"

#include "Game.h"
#include "helpers.h"
#include "scriptingEngine.h"

// RESOURCE
resource::resource() {
	loaded = false;
}

resource::~resource()
{
	unload();
}

bool resource::load()
{
	loaded = true;

	return true;
}

void resource::unload()
{
	if (numReferences() > 0) {
		p::warn("Unloading a resource with references!");
	}

	loaded = false;
}

unsigned int resource::numReferences()
{
	auto selfRefShp = selfRef.lock();

	if (selfRefShp) {
		return selfRefShp.use_count() - 3; // one for this, one for the resource manager's
	}

	return 0;
}

bool resource::isLoaded()
{
	return loaded;
}

bool resource::isNeeded()
{
	return true; // by default always true. Only relevant for deferred resources.
	// if this function returns false the resource manager will unload the resource completely until it returns true again
	// most things should always be loaded - we're a big computer, we have the memory to do it. However, 
	// some stuff (i.e. gamestates, shaders) can be a pain to hold in the memory.
	// these things won't load unless they're being used.

	// note that the resourcemanager WILL load these things in if they're requested and not already loaded - this function generally tells it when to UNLOAD them.
}

std::string resource::name()
{
	return theGame()->resources->nameOf(selfRef.lock());
}

void resource::setInternalRef(std::shared_ptr<resource> res)
{
	selfRef = res;
}

// FILEPATH RESOURCE
filepathResource::filepathResource()
{}

filepathResource::filepathResource(std::string fp)
{
	filepath = p::realPath(fp);
}

bool filepathResource::load()
{
	p::log("\tPath: " + filepath);
	return resource::load();
}

// RESOURCE MANAGER
resourceMngr::resourceMngr()
{
}

void resourceMngr::add(std::string name, std::shared_ptr<resource> rc, bool immediate)
{
	if (exists(name)) {
		p::warn("Resource already exists with name " + name + ", overwriting...");
	}

	resourceCache[name] = rc;
	rc->setInternalRef(rc);

	if (immediate) {
		p::info("Loading resource \"" + name + "\"."); // s
		resourceCache[name]->load();
	}
}

void resourceMngr::update()
{
	// find what we need and don't need
	std::vector<std::shared_ptr<resource>> nList;
	std::vector<std::shared_ptr<resource>> uList;

	for (auto i = resourceCache.begin(); i != resourceCache.end(); i++) {
		auto r = i->second;

		if (r->isNeeded() && !r->isLoaded()) {
			nList.push_back(r);
		}
		else if (!r->isNeeded() && r->isLoaded()) {
			uList.push_back(r);
		}
	}

	// load what's needed (display loading screen if possible)
	for (unsigned int i = 0; i < nList.size(); i++) {
		p::info("Loading resource \"" + nameOf(nList[i]) + "\"."); // s

		std::string lString = "Loading " + std::to_string(i) + " of " + std::to_string(nList.size());
		theGame()->drawLoadingScreenFrame(lString, i, (int)nList.size());

		bool s = nList[i]->load(); // a resource can of course draw its own loading screen

		if (!s) {
			p::fatal("Resource \"" + nameOf(nList[i]) + "\" failed to load.");
		}
	}

	// unload what isn't (again, display loading screen)
	for (unsigned int i = 0; i < uList.size(); i++) {
		std::string lString = "Loading " + std::to_string(i) + " of " + std::to_string(uList.size());
		theGame()->drawLoadingScreenFrame(lString, i, (int)uList.size());

		uList[i]->unload();
	}

	// this update function should probably be called where it's most likley to make a loading screen on the tail-end of another, but before
	// main game startup.
}

void resourceMngr::flush()
{
	for (auto i = resourceCache.begin(); i != resourceCache.end(); i++) {
		if (i->second->isLoaded()) {
			i->second->unload();
		}
	}
}

unsigned int resourceMngr::getCount()
{
	return resourceCache.size();
}

std::shared_ptr<resource> resourceMngr::get(std::string name, bool safe)
{
	if (exists(name)) {
		if (!resourceCache[name]->isLoaded() && safe) {
			p::info("Quickly loading resource \"" + name + "\".");
			resourceCache[name]->load(); // failsafe
		}

		return resourceCache[name];
	}

	p::warn("No such resource as \"" + name + "\"");
	return nullptr;
}

std::shared_ptr<resource> resourceMngr::getByIndex(unsigned int index)
{
	auto res = resourceCache.begin();

	for (unsigned int i = 0; i < index; i++)
		res++;

	return res->second;
}

std::vector<std::shared_ptr<resource>> resourceMngr::list(std::string tag)
{
	std::vector<std::shared_ptr<resource>> rcList;

	for (auto i = resourceCache.begin(); i != resourceCache.end(); i++) {
		if (i->second->hasTag(tag))
			rcList.push_back(i->second);
	}

	return rcList;
}

std::string resourceMngr::nameOf(std::shared_ptr<resource> in)
{
	for (auto i = resourceCache.begin(); i != resourceCache.end(); i++) {
		if (i->second == in) {
			return i->first;
		}
	}

	return "";
}

bool resourceMngr::exists(std::string name)
{
	return (resourceCache.count(name) != 0);
}

// SE Binding
#include "scriptingEngine.h"

// resource
DeclareScriptingType(resource);
DeclareScriptingBaseClass(taggable, resource);
DeclareScriptingConstructor(resource(), "resource");
DeclareScriptingFunction(&resource::isLoaded, "isLoaded");
DeclareScriptingFunction(&resource::isNeeded, "isNeeded");
DeclareScriptingFunction(&resource::load, "load");
DeclareScriptingFunction(&resource::unload, "unload");
DeclareScriptingFunction(&resource::name, "name");
DeclareScriptingCopyOperator(resource);

// filepathResource
DeclareScriptingType(filepathResource);
DeclareScriptingBaseClass(taggable, filepathResource);
DeclareScriptingBaseClass(resource, filepathResource);
DeclareScriptingConstructor(filepathResource(), "filepathResource");
DeclareScriptingConstructor(filepathResource(std::string), "filepathResource");
DeclareScriptingFunction(&filepathResource::filepath, "filepath");
DeclareScriptingCopyOperator(filepathResource);

DeclareScriptingFunction(&resourceMngr::getAs<filepathResource>, "getFilepathResource");
DeclareScriptingFunction(&resourceMngr::listType<filepathResource>, "listFilepathResources");
DeclareScriptingFunction(&resourceMngr::addFolder<filepathResource>, "addFolderOfFilepathResources");

// resourceMngr
DeclareScriptingType(resourceMngr);
DeclareScriptingFunction(&resourceMngr::add, "add");
DeclareScriptingFunction([](resourceMngr &m, std::string n, std::shared_ptr<resource> r) { m.add(n, r); }, "add"); // simplified add
DeclareScriptingFunction(&resourceMngr::exists, "exists");
DeclareScriptingFunction(&resourceMngr::get, "get");
DeclareScriptingFunction([](resourceMngr &m, std::string g) { return m.get(g); }, "get"); // simplified get
DeclareScriptingFunction(&resourceMngr::getByIndex, "getByIndex");
DeclareScriptingFunction(&resourceMngr::getCount, "getCount");
DeclareScriptingFunction(&resourceMngr::list, "list");
DeclareScriptingFunction([](resourceMngr &m) { return m.list(); }, "list");
DeclareScriptingFunction(&resourceMngr::nameOf, "nameOf");
DeclareScriptingFunction(&resourceMngr::flush, "flush");

// list of resources
DeclareScriptingListableNamed(std::shared_ptr<resource>, "resourceSharedPtrList");