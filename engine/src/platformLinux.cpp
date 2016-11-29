#include "platform.h"
#include "Game.h"

#ifdef PLATFORM_LINUX

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

// namespace stuff
FILE *cstream;
std::ofstream file;
std::string defaultWorkDir;

// linux, for the most part, was not our development platform.
// Later, the console might work properly. As of now it can only print things.

bool platform::init() {
	// redirect stderr to the console.log file
	cstream = freopen("console.log", "w", stderr);

	// turn off sf::err
	file.open("sfml.log");
	sf::err().rdbuf(file.rdbuf());

	// ...init any linux stuff we need later...
	defaultWorkDir = getWorkDir();

	// show a warning
	//showMessageBox("Disclaimer", "The linux version of SUPER_ALCHEMIST is still very much work in progress; features may be broken or missing, and the game can do odd things.\nIf you're ok with that, feel free to continue!");

	return true;
}

void platform::deInit() {
	// close the console log
	fclose(cstream);
	file.flush();
}

void platform::update() {

}

void platform::writeToLogfile(std::string in) {
	FILE* st = stderr;
	fputs(in.c_str(), st);

	file.flush();
}

void platform::writeToConsole(std::string in, unsigned int color) {
	std::string colorData[] = { "/026[30m" };

	color = 0;

	std::cout << colorData[color] << in;
}

void platform::showMessageBox(std::string caption, std::string text, bool showErrorIcon) {
	// uses zenity; if this isn't available on the system, no message box will appear.
	std::string zenityString = "zenity --info --text=\"" + text + "\" --title=\"" + caption + "\"";
	FILE * f = popen(zenityString.c_str(), "r" );
	pclose( f );
}

std::vector<std::string> platform::dirList(std::string path, bool includeExtra) {
	std::vector<std::string> resultsVec;

	DIR *dp;
	struct dirent *dirp;
	if((dp  = opendir(("./" + path).c_str())) == NULL) {
		info("Unable to read linux directory " + path);
	}
	else {
        while ((dirp = readdir(dp)) != NULL) {
	    std::string nString = std::string(dirp->d_name);

	    if((nString != "." && nString != ".."))
        	resultsVec.push_back(nString);
        }

        closedir(dp);
	}

	if (includeExtra)
		resultsVec.insert(resultsVec.begin(), "..");

	return resultsVec;
}

std::string platform::realPath(std::string in) {
    return realpath(in.c_str(), NULL);
}

bool platform::fileDelete(std::string in) {
	return remove(in.c_str()) == 0;
}

bool platform::getConsoleInput(std::string &input) {
    return false;
}

void platform::clearConsoleInput(bool eraseCompletely) {

}

bool platform::setWorkDir(std::string path)
{
	path = realPath(path);

	bool s = chdir(path.c_str()) == 0;

	if (!s) {
		warn("Unable to work in directory \"" + path + "\"!");
	}

	return s;
}

void platform::resetWorkDir() {
	setWorkDir(defaultWorkDir);
}

std::string platform::getWorkDir()
{
	const size_t max = 1024;
	char buf[max];
	getcwd(buf, max);

	return std::string(buf);
}

#endif
