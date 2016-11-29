#pragma once
#include "StdAfx.h"
#include "SfmlAfx.h"
#include "constants.h"

namespace platform {
	enum LOGTYPE { INF = 0, WARNING = 1, FATAL = 2, CHAISCRIPT = 3 };

	MAGEDLL bool init();
	MAGEDLL void deInit();

	MAGEDLL void update();

	MAGEDLL void showMessageBox(std::string caption, std::string text, bool showErrorIcon = false);

	MAGEDLL void writeToLogfile(std::string info);
	MAGEDLL void writeToConsole(std::string info, unsigned int color = 0);

	MAGEDLL void log(std::string info, bool nl = true);
	MAGEDLL void diagnosticLog(LOGTYPE type, std::string file, std::string func, int line, std::string info, bool nl = true);
	MAGEDLL void diagnosticError(std::string file, std::string func, int line, std::string info);

	MAGEDLL std::vector<std::string> dirList(std::string path, bool includeExtra = false);
	MAGEDLL std::string realPath(std::string path);
	MAGEDLL bool fileDelete(std::string in);

	MAGEDLL bool setWorkDir(std::string path);
	MAGEDLL void resetWorkDir();
	MAGEDLL std::string getWorkDir();

	MAGEDLL bool getConsoleInput(std::string &input);
	MAGEDLL void clearConsoleInput(bool eraseCompletely = true);
	MAGEDLL std::string getLastLogged();

// macros
#define prints_v(string, nl) diagnostic_log(string, nl);

#ifdef _MSC_VER

#define info(string) diagnosticLog(p::INF, __FILE__, __FUNCTION__, __LINE__, string);
#define warn(string) diagnosticLog(p::WARNING, __FILE__, __FUNCTION__, __LINE__, string);
#define fatal(string) diagnosticError(__FILE__, __FUNCTION__, __LINE__, string);

#else

#define info(string) diagnosticLog(p::INF, __FILE__, __func__, __LINE__, string);
#define warn(string) diagnosticLog(p::WARNING, __FILE__, __func__, __LINE__, string);
#define fatal(string) diagnosticError(__FILE__, __func__, __LINE__, string);

#endif
}

namespace p = platform;
