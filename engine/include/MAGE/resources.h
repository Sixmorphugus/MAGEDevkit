#pragma once
#include "StdAfx.h"
#include "SfmlAfx.h"

#include "taggable.h"

#include "platform.h"

class resourceMngr;

class loadingScreen;

class MAGEDLL resource
: public taggable {
public:
	resource();
	~resource();

	virtual bool load();
	virtual void unload();

	unsigned int numReferences();

	bool isLoaded();
	virtual bool isNeeded();

	std::string name();

	void setInternalRef(std::shared_ptr<resource> res);

private:
	bool loaded = false;
	std::weak_ptr<resource> selfRef;
};

// basic class for a resource using ONE file.
class MAGEDLL filepathResource : public resource
{
public:
	filepathResource();
	filepathResource(std::string filepath);

	virtual bool load();
public:
	std::string filepath;
};

class MAGEDLL resourceMngr {
public:
	resourceMngr();

	void add(std::string name, std::shared_ptr<resource> rc, bool immediate = false);
	template<typename T> void addFolder(std::string preName, std::string path);

	void update(); // requires __fontDef and __splash
	void flush(); // not a function you want to be calling in scripts. Reloads every resource.

	unsigned int getCount();

	std::shared_ptr<resource> get(std::string name, bool safe = true);
	template<typename T> std::shared_ptr<T> getAs(std::string name);
	std::shared_ptr<resource> getByIndex(unsigned int index);

	std::vector<std::shared_ptr<resource>> list(std::string tag = "");
	template<typename T> std::vector<std::shared_ptr<T>> listType(std::string tag = "");

	std::string nameOf(std::shared_ptr<resource> in);

	bool exists(std::string name);

private:
	// stuff
	std::unordered_map<std::string, std::shared_ptr<resource>> resourceCache;
};

template<typename T>
inline void resourceMngr::addFolder(std::string preName, std::string path)
{
	std::vector<std::string> fileList = p::dirList(path);
	std::vector<std::string> fileListNoExt;
	std::vector<std::string> fileExtList;

	for (unsigned int i = 0; i < fileList.size(); i++) {
		if (fileList[i].find(".") != std::string::npos) {
			std::string withExt = fileList[i];
			std::string withoutExt;

			for (unsigned int i = 0; i < withExt.size(); i++) {
				if (withExt[i] == '.') {
					break;
				}

				withoutExt += withExt[i];
			}

			fileExtList.push_back(withExt);
			fileListNoExt.push_back(withoutExt);
		}
	}

	auto prevWorkDir = p::getWorkDir();

	p::setWorkDir(path);
	for (unsigned int i = 0; i < fileListNoExt.size(); i++) {
		auto shared = std::make_shared<T>(fileExtList[i]);

		shared->tags.push_back(preName);

		add(preName + fileListNoExt[i], shared);
	}
	p::setWorkDir(prevWorkDir);
}

template<typename T>
inline std::shared_ptr<T> resourceMngr::getAs(std::string name)
{
	return std::dynamic_pointer_cast<T>(get(name));
}

template<typename T>
inline std::vector<std::shared_ptr<T>> resourceMngr::listType(std::string tag)
{
	std::vector<std::shared_ptr<resource>> tList = list(tag);
	std::vector<std::shared_ptr<T>> rcList;

	for (unsigned int i = 0; i < tList.size(); i++) {
		std::shared_ptr<T> converted = std::dynamic_pointer_cast<T>(tList[i]);

		if (converted) {
			rcList.push_back(converted);
		}
	}

	return rcList;
}

#define DeclareScriptingResource(name) \
DeclareScriptingType(name); \
DeclareScriptingBaseClass(resource, name);\
DeclareScriptingBaseClass(taggable, name);\
DeclareScriptingConstructor(name(), STRING(name));\
DeclareScriptingFunction(&resourceMngr::getAs<name>, "get_" STRING(name));\
DeclareScriptingFunction(&resourceMngr::listType<name>, "list_" STRING(name));\
DeclareScriptingListableNamed(std::shared_ptr<name>, STRING(name) "SharedPtrList");\
DeclareScriptingCastingFunction("to_" STRING(name), resource, name);

#define DeclareScriptingFilepathResource(name) \
DeclareScriptingResource(name) \
DeclareScriptingBaseClass(filepathResource, name);\
DeclareScriptingConstructor(name(std::string), STRING(name));\
DeclareScriptingFunction(&resourceMngr::addFolder<name>, "addFolderOf_" STRING(name));\
DeclareScriptingCopyOperator(name);