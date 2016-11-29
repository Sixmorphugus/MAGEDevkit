#include "registry.h"

#include "helpers.h"

// REGISTRY PROPERTY CLASS
// ----------------------------------------------------------------------------
registry::prop::prop(std::string value) {
	name = value;

	typ = NA;
}

registry::prop::prop(std::string value, int in) {
	name = value;

	i = in;
	typ = INTPROP;
}

registry::prop::prop(std::string value, float floa) {
	name = value;

	f = floa;
	typ = FLOATPROP;
}

registry::prop::prop(std::string value, std::string strin) {
	name = value;
	
	s = strin;
	typ = STRINGPROP;
}

registry::prop::prop(std::string value, char * strin)
{
	name = value;

	s = strin;
	typ = STRINGPROP;
}

registry::prop::prop(std::string value, bool boo) {
	name = value;
	
	b = boo;
	typ = BOOLPROP;
}

registry::prop::prop(std::string value, sf::Uint8 col) {
	name = value;
	
	c = col;
	typ = COLPROP;
}

void registry::prop::update(int iUpdate) {
	if (typ == INTPROP) {
		// just use the int
		i = iUpdate;
	}
	else if (typ == FLOATPROP) {
		// int to float
		f = (float)iUpdate;
	}
	else if (typ == STRINGPROP) {
		// toString the int
		s = std::to_string(iUpdate);
	}
	else if (typ == BOOLPROP) {
		b = (iUpdate != 0);
	}
	else if (typ == COLPROP) {
		c = ((sf::Uint8) iUpdate);
	}
}

void registry::prop::update(float fUpdate) {
	if (typ == INTPROP) {
		// float to int (values were lost that day)
		i = (int)fUpdate;
	}
	else if (typ == FLOATPROP) {
		// float is float.
		f = fUpdate;
	}
	else if (typ == STRINGPROP) {
		// toString the int
		s = std::to_string(fUpdate);
	}
	else if (typ == BOOLPROP) {
		b = (fUpdate != 0.f);
	}
	else if (typ == COLPROP) {
		c = ((sf::Uint8) fUpdate);
	}
}

void registry::prop::update(std::string sUpdate) {
	if (typ == INTPROP) {
		// atoi() the string.
		i = atoi(sUpdate.c_str());
	}
	else if (typ == FLOATPROP) {
		// atof() the string.
		f = (float)atof(sUpdate.c_str());
	}
	else if (typ == STRINGPROP) {
		s = sUpdate;
		
		// failsafe: registries can't store the "=" sign.
		// remove this for now.
		s = strReplace(s, '=', ' ');
	}
	else if (typ == BOOLPROP) {
		b = (sUpdate == "1");
	}
	else if (typ == COLPROP) {
		c = (sf::Uint8)atoi(sUpdate.c_str());
	}
}

template<typename T> T registry::prop::as() {
	if (typ == INTPROP) {
		// int
		return (T)i;
	}
	else if (typ == FLOATPROP) {
		// float
		return (T)f;
	}
	else if (typ == STRINGPROP) {
		// string
		return (T)s;
	}
	else if (typ == BOOLPROP) {
		return (T)b;
	}
	else {
		return (T)c;
	}
}
// specializations for the above:
template<> bool registry::prop::as() {
	if (typ == INTPROP) {
		// int
		return i != 0;
	}
	else if (typ == FLOATPROP) {
		// float
		return f != 0;
	}
	else if (typ == STRINGPROP) {
		// string
		return false;
	}
	else if (typ == BOOLPROP) {
		return b;
	}
	else {
		return c != 0;
	}
}
template<> int registry::prop::as() {
	if (typ == INTPROP) {
		// int
		return i;
	}
	else if (typ == FLOATPROP) {
		// float
		return (int)floor(f);
	}
	else if (typ == STRINGPROP) {
		// string
		return 0;
	}
	else if (typ == BOOLPROP) {
		return (int)b;
	}
	else {
		return (int)c;
	}
}
template<> float registry::prop::as() {
	if (typ == INTPROP) {
		// int
		return (float)i;
	}
	else if (typ == FLOATPROP) {
		// float
		return f;
	}
	else if (typ == STRINGPROP) {
		// string
		return 0;
	}
	else if (typ == BOOLPROP) {
		return (float)b;
	}
	else {
		return (float)c;
	}
}
template<> std::string registry::prop::as() {
	if (typ == INTPROP) {
		// int
		return std::to_string(i);
	}
	else if (typ == FLOATPROP) {
		// float
		return std::to_string(f);
	}
	else if (typ == STRINGPROP) {
		// string
		return s;
	}
	else if (typ == BOOLPROP) {
		return b ? "1" : "0";
	}
	else {
		return std::to_string((int)c);
	}
}

std::string registry::prop::read()
{
	if (typ == INTPROP) {
		// int
		return std::to_string(i);
	}
	else if (typ == FLOATPROP) {
		// float
		return std::to_string(f);
	}
	else if (typ == STRINGPROP) {
		// string
		return s;
	}
	else if (typ == BOOLPROP) {
		return std::to_string(b);
	}
	else if (typ == COLPROP) {
		return std::to_string(c);
	}

	return "";
}

registry::registry()
{
	// you must call init() yourself this way
	ready = false;
	autoSave = false; // safety
}

// REGISTRY CLASS
// ----------------------------------------------------------------------------
registry::registry(std::string defaultLocation, std::vector<prop> defaultProps, bool lf, bool as)
{
	ready = false;
	init(defaultLocation, defaultProps, lf, as);
}

registry::~registry()
{
	if(autoSave)
		save();
}

void registry::init(std::string defaultLocation, std::vector<prop> defaultProps, bool lf, bool as) {
	location = defaultLocation;
	values = defaultProps;
	labelFile = lf;
	autoSave = as;
	header = "This is an MGE registry file. Make of that what you wish.";

	ready = true;
}

void registry::add(prop newProp)
{
	values.push_back(newProp);
}

void registry::remove(std::string name)
{
	unsigned int index = getIndex(name);
	values.erase(values.begin() + index);
}

registry::prop* registry::get(std::string name)
{
	unsigned int index = getIndex(name);
	return &values[index];
}

bool registry::load()
{
	if (!ready)
		p::fatal("registry not ready");

	std::ifstream saveFile(location);

	if (!saveFile.good()) {
		p::warn("Unable to read/find/comprehend registry file \"" + location + "\". Keeping default values...");
		return false;
	}

	// load
	std::string sLine;
	int count = 0;
	while (!saveFile.eof()) {
		int iCount = count - 1;
		getline(saveFile, sLine);
		removeNewline(sLine);

		if (count != 0) { // ignore the header
			// filter out any labels
			auto split = splitString(sLine, '=');

			if (split.size() != 1) {
				sLine = split[1]; // ignore everything up to and including the '=' sign.
			}

			// iCount is the property this relates to.
			if (iCount >= values.size()) {
				p::warn("Found more properties than expected in \"" + location + "\". Did you set the registry object up properly?");
				break;
			}

			prop* thisProp = &values[iCount];

			if (sLine == thisProp->name + "=") {
				sLine = "";
			}

			thisProp->update(sLine); // update this property with the new value
		}

		count++;
	}

	return true;
}

bool registry::save()
{
	if (!ready)
		p::fatal("registry not ready");

	//p::info("Saving \"" + location + "\"");

	std::ofstream saveFile(location);

	if (saveFile.bad()) {
		//p::warn("Unable to open registry file \"" + location + "\". Can't save!");
		return false;
	}

	// save
	saveFile << header << "\n";
	for (unsigned int i = 0; i < values.size(); i++) {
		prop* thisProp = &values[i];
		std::string pv = thisProp->read();
		std::string pn = thisProp->name;

		if (labelFile)
			saveFile << pn << "=" << pv;
		else
			saveFile << pv;

		if (i < values.size() - 1) {
			saveFile << "\n";
		}
	}

	return true;
}

unsigned int registry::getIndex(std::string name)
{
	if (!ready)
		p::fatal("registry not ready");

	for (unsigned int i = 0; i < values.size(); i++) {
		if (values[i].name == name)
			return i;
	}

	p::fatal("Registry lookup error: name \"" + name + "\" is invalid!"); // for now, this is a fatal error

	return 0;
}

// SE Bindings
#include "scriptingEngine.h"

DeclareScriptingCustom(user_type<registry::prop>(), "registryProp");
DeclareScriptingCustom(fun(&registry::prop::name), "name");
DeclareScriptingCustom(fun<void, registry::prop, std::string>(&registry::prop::update), "update");
DeclareScriptingCustom(fun(&registry::prop::read), "read");

DeclareScriptingCustom(user_type<registry>(), "registry");
DeclareScriptingCustom(constructor<registry()>(), "registry");
DeclareScriptingConstructor(registry(const registry&), "registry")
DeclareScriptingCopyOperator(registry);
DeclareScriptingCustom(fun(&registry::add), "add");
DeclareScriptingCustom(fun(&registry::autoSave), "autoSave");
DeclareScriptingCustom(fun(&registry::get), "get");
DeclareScriptingCustom(fun(&registry::header), "header");
DeclareScriptingCustom(fun(&registry::init), "init");
DeclareScriptingCustom(fun(&registry::labelFile), "labelFile");
DeclareScriptingCustom(fun(&registry::load), "load");
DeclareScriptingCustom(fun(&registry::location), "location");
DeclareScriptingCustom(fun(&registry::remove), "remove");
DeclareScriptingCustom(fun(&registry::save), "save");