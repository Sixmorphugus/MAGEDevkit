#pragma once
#include "basic.h"
#include "taggable.h"

// class for managing a single saved basic which is used as a template.
class MAGEDLL prefab : public taggable, public serializable {
public:
	prefab(std::shared_ptr<basic> clonable, std::vector<std::string> tags = {});

	// "big three" (with default constructor instead of destructor)
	prefab();
	prefab(const prefab& p);
	prefab& operator=(const prefab& p);

	void setTemplate(std::shared_ptr<basic> clonable);

	std::shared_ptr<basic> copyTemplate();
	template<typename T> bool castsTo();

	std::string name();

public:
	std::string knownName;

private:
	std::shared_ptr<basic> templateObject;
};

// class for managing all the saved basic template objects.
class MAGEDLL prefabMngr {
public:
	prefabMngr();

	void add(std::string name, std::shared_ptr<prefab> input);
	bool exists(std::string name);

	std::string nameOf(std::shared_ptr<prefab> input);

	std::shared_ptr<prefab> get(std::string name);
	std::shared_ptr<prefab> getByIndex(unsigned int index);

	unsigned int getCount();

	std::vector<std::shared_ptr<prefab>> list(std::string tag = "");
	template<typename T> std::vector<std::shared_ptr<prefab>> listType();

	std::shared_ptr<basic> newInstance(std::string name, Group* attachTo = nullptr);

private:
	std::map<std::string, std::shared_ptr<prefab>> prefabMap;
};

template<typename T>
inline bool prefab::castsTo()
{
	return std::dynamic_pointer_cast<T>(templateObject) != nullptr; // just try casting
}

template<typename T>
inline std::vector<std::shared_ptr<prefab>> prefabMngr::listType()
{
	std::vector<std::shared_ptr<prefab>> results;

	// now we must iterate
	for (auto i = prefabMap.begin(); i != prefabMap.end(); i++) {
		if (i->second->castsTo<T>()) {
			results.push_back(i->second);
		}
	}

	return results;
}
