#include "scriptingEngine.h"

using namespace chaiscript;

// Chaiscript Scripting Engine Objects and binding

#include "helpers.h"
#include "platform.h"
#include "Game.h"

#include <chaiscript_stdlib.hpp>

// -------------------------------------------------------------
// BINDING
// -------------------------------------------------------------

scriptingEngine::scriptingEngine()
{
	init = false;
}

scriptingEngine::~scriptingEngine()
{
	delete chai;
}

void scriptingEngine::initChai()
{
	init = true;

	p::info("Initializing scripting engine.");

	p::log("\tLoad precompiled chai libraries...");
	chai = new ChaiScript(Std_Lib::library()); // compiles in stdlib

	bind();
}

void scriptingEngine::update()
{
	initCheck();

	for (int i = 0; i < activeSchedules.size(); i++) {
		auto fTime = (activeSchedules[i].startTime + activeSchedules[i].time) - theGame()->getSimTime();

		if (fTime.asSeconds() < 0.f) {
			activeSchedules[i].fun();
			activeSchedules.erase(activeSchedules.begin() + i);
			i -= 1;
		}
	}
}

void scriptingEngine::bind()
{
	try {
		p::log("\tBind base...");
		chai->add(fun(&chaiPrint), "logInfo");
		chai->add(fun(&chaiCrash), "crashGame");
		chai->add(fun([&](std::string in) { return evalFile(in); } ), "evalFile");

		p::log("\tBind SFML...");
		bindSFML();

		p::log("\tBind Engine...");
		bindMAGE();

		p::log("\tBind Hooks...");
		bindHooks();

		p::log("\tBind custom datatypes...");
		bindLists();

		p::log("\tReplace some default functions...");
		chai->eval("global print = fun(x){ logInfo(to_string(x), \"unknown file\", \"unknown\", 0); }");
		chai->eval("global use = fun(x){ return evalFile(x); }");

		p::log("\tFinito!");
	}
	catch (std::runtime_error &e) {
		p::fatal("Unable to start chaiscript engine:\n" + std::string(e.what()));
	}

	//dump();
}

void scriptingEngine::initCheck()
{
	// if we didn't initialize do that now
	if (!init)
		initChai();
}

Boxed_Value scriptingEngine::evalFile(std::string filePath)
{
	initCheck();

	// this means that this script (and any hooks in it) will work from the folder of the script.
	std::string pWorkDir = p::getWorkDir();
	p::setWorkDir(containerDir(filePath));

	p::info("Executing " + filePath);

	Boxed_Value returnValue(NULL);

	try {
		returnValue = chai->eval_file(fileName(filePath));
	}
	catch (chaiscript::exception::eval_error& e) {
		handleEvalError(e);
	}
	catch (chaiscript::exception::file_not_found_error& e) {
		p::warn(std::string(e.what()));
	}
	catch (std::runtime_error &e) {
		p::warn(e.what());
	}
	catch (...) {
		p::warn("Unknown error. Please never do that again.");
	}

	p::setWorkDir(pWorkDir);

	return returnValue;
}

Boxed_Value scriptingEngine::eval(std::string in)
{
	return chai->eval(in);
}

void scriptingEngine::dump(chaiscript::Boxed_Value* in)
{
	initCheck();

	p::log("BEGIN SE DUMP --------------------------");

	auto s = chai->get_state().engine_state;

	if (in) {
		p::log("C++ Class: " + in->get_type_info().bare_name());
	}

	p::log("Registered functions and members:");
	for (auto m = s.m_functions.begin(); m != s.m_functions.end(); m++) {
		if (in) {
			auto ourType = in->get_type_info();

			bool displayIt = false;
			for (auto o = m->second.get()->begin(); o != m->second.get()->end(); o++) {
				if (o->get()->get_param_types().size() < 2)
					continue;

				auto funcType = o->get()->get_param_types()[1];

				if (ourType == funcType) {
					displayIt = true;
				}
			}

			if (!displayIt)
				continue;
		}

		p::log(m->first + " (" + std::to_string((m->second.get()->size())) + " overloads)");

		for (auto o = m->second.get()->begin(); o != m->second.get()->end(); o++) {
			auto f = o->get();

			std::string name = f->get_param_types()[0].bare_name();
			std::string n = "[C++] " + name;

			// find the chai name
			for (auto j = s.m_types.begin(); j != s.m_types.end(); j++) {
				if (j->second == f->get_param_types()[0]) {
					n = j->first;
				}
			}

			p::log("\t" + n + " " + m->first + "(", false);

			for (unsigned int i = 1; i < f->get_param_types().size(); i++) {
				std::string name = f->get_param_types()[i].bare_name();
				std::string n = "[C++] " + name;

				// find the chai name
				for (auto j = s.m_types.begin(); j != s.m_types.end(); j++) {
					if (j->second == f->get_param_types()[i]) {
						n = j->first;
					}
				}

				p::log(n, false);

				if (i + 1 < f->get_param_types().size()) {
					p::log(", ", false);
				}
			}

			p::log(")");
		}
	}

	if (!in) {
		p::log("Registered global variables:");
		for (auto m = s.m_global_objects.begin(); m != s.m_global_objects.end(); m++) {
			p::log(m->first);
		}
	}

	p::log("END SE DUMP ----------------------------");
}

void scriptingEngine::whatIs(chaiscript::Boxed_Value * in)
{
	initCheck();

	p::log("C++ Class: " + in->get_type_info().bare_name());

	auto s = chai->get_state().engine_state;

	// find the chai name
	for (auto j = s.m_types.begin(); j != s.m_types.end(); j++) {
		if (j->second == in->get_type_info()) {
			p::log("known by Chaiscript as " + j->first);
		}
	}
}

void scriptingEngine::schedule(sf::Time time, std::function<void()> fun)
{
	initCheck();

	Schedule ns;

	ns.time = time;
	ns.startTime = theGame()->getSimTime();

	ns.fun = fun;

	activeSchedules.push_back(ns);
}

// -------------------------------------------------------------
// FUNCTIONS
// -------------------------------------------------------------

void chaiPrint(const std::string& data, const std::string& file, const std::string& function, int line)
{
	p::diagnosticLog(p::CHAISCRIPT, file, function, line, data);
}

void chaiCrash(const std::string &data)
{
	p::fatal(data);
}

void handleEvalError(const chaiscript::exception::eval_error &e)
{
	p::warn("Eval exception was thrown;");

	std::string errorToLog = e.pretty_print();

	auto lines = splitString(errorToLog, '\n');

	for (unsigned int i = 0; i < lines.size(); i++) {
		if (lines[i] == "")
			continue;

		p::log("\t" + lines[i]);
	}

	if (CRASHWHENEVALFAILS) {
		p::fatal(errorToLog);
	}
}