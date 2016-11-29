// REGISTRY.
// A file with a set of ordered values, interpretable only by the Game.
// These ALWAYS depend on their order!
#pragma once
#include "StdAfx.h"
#include "SfmlAfx.h"
#include "platform.h"

class MAGEDLL registry
{
public:
	// Same as the Basic Property class, but doesn't refer to a memory address.
	// This one stores its own value and a name, too.

	class prop {
	public:
		prop(std::string value);

		prop(std::string value, int in);
		prop(std::string value, float floa);
		prop(std::string value, std::string strin);
		prop(std::string value, char* strin);
		prop(std::string value, bool boo);
		prop(std::string value, sf::Uint8 col);

		enum TYPE {
			INTPROP,
			FLOATPROP,
			STRINGPROP,
			BOOLPROP,
			COLPROP,
			NA
		};

		void update(int up);
		void update(float up);
		void update(std::string up);

		template<typename T> T as();
		std::string read();

	public:
		std::string name;

	private:
		int i;
		float f;
		std::string s;
		bool b;
		sf::Uint8 c;

		TYPE typ;
	};

public:
	registry();
	registry(std::string defaultFileLocation, std::vector<prop> defaultProps = {}, bool labelFile = true, bool autoSave = true);
	~registry();

	void init(std::string defaultFileLocation, std::vector<prop> defaultProps = {}, bool labelFile = true, bool autoSave = true);

	void add(prop newProp);
	void remove(std::string name);
	prop* get(std::string name);

	bool load(); // DO NOT CALL until you've added all the defaults
	bool save();

public:
	bool labelFile; // when saved, adds "propertyname=" to each line which is ignored on load. Purely asthetic.
	bool autoSave; // save when the object is destroyed.
	std::string location;
	std::string header;

private:
	bool ready;
	unsigned int getIndex(std::string name);

private:
	std::vector<prop> values;
};