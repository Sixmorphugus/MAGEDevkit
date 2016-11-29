#include "platform.h"
#include "Game.h"

#ifdef PLATFORM_WINDOWS

#include <windows.h>
#include <direct.h>

// internal stuff
FILE *cstream;
std::ofstream file;

std::string consoleInputBuffer, consoleInputBuffer2;
bool consoleInputOffset;
bool consoleInputWaiting;
bool consoleInputOnScreen;
int consoleHistoryLocation;
std::vector<std::string> consoleHistory;
std::string defaultWorkDir;

HANDLE hStdOut;
HANDLE hStdIn;

void consoleErase(unsigned int num, bool eraseAndRepeat = true) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD cursorPosition;
	COORD cursorPositionNew;

	GetConsoleScreenBufferInfo(hStdOut, &csbi);
	cursorPosition = csbi.dwCursorPosition;

	cursorPositionNew = cursorPosition;
	cursorPositionNew.X -= num;

	while (cursorPositionNew.X < 0) {
		cursorPositionNew.X += csbi.dwSize.X;
		cursorPositionNew.Y -= 1;
	}

	SetConsoleCursorPosition(hStdOut, cursorPositionNew);

	if (eraseAndRepeat) {
		for(unsigned int i = 0; i < num; i++)
			std::cout << " ";

		consoleErase(num, false);
	}
}

bool platform::init() {
	// redirect stderr to the console.log file
	if (freopen_s(&cstream, "console.log", "w", stderr) != 0) {
		p::showMessageBox("Can't start!", "The game files already seem to be in use.\nDid you run the game twice?\n\nWill abort.");
		abort();
	}

	p::info("Working in " + realPath("."));
	defaultWorkDir = realPath(".");

	// turn off sf::err
	file = std::ofstream("sfml.log");
	sf::err().rdbuf(file.rdbuf());

	// ...init any windows stuff we need later...
	consoleInputOffset = true;
	consoleInputWaiting = false;
	consoleInputOnScreen = false;
	consoleHistoryLocation = 0;

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	hStdIn = GetStdHandle(STD_INPUT_HANDLE);

	return true;
}

void platform::deInit() {
	// close the console log file
	fclose(cstream);
	file.close();
}

void platform::update() {
	// This is mostly code I wrote in 2014.
	// I'm so sorry.

	// It was originally written for the conio.h library, a library that is heavily depreciated and only still included in systems for
	// historical (hysterical) reasons.
	// This version of the code is adapted for Windows.h.

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hStdOut, &csbi);

	if (consoleInputOffset) {
		std::cout << "> " << consoleInputBuffer;
		consoleInputOffset = false;
		consoleInputOnScreen = true;
	}

	// Find out how many console events have happened:
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	GetNumberOfConsoleInputEvents(hStdIn, &numEvents);

	if (numEvents != 0) {
		// Create a buffer of that size to store the events
		INPUT_RECORD *eventBuffer = new INPUT_RECORD[numEvents];

		// Read the console events into that buffer, and save how
		// many events have been read into numEventsRead.
		ReadConsoleInput(hStdIn, eventBuffer, numEvents, &numEventsRead);

		for (DWORD i = 0; i < numEventsRead; ++i) {
			// Check the event type: was it a key?
			if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown) {
				char logIn = eventBuffer[i].Event.KeyEvent.uChar.AsciiChar; // key that was hit
				int specialId = (int)logIn; // ascii id of that key

				if (specialId > 31 && specialId < 127) { // these are our whitelisted letters
					std::cout << logIn;
					consoleInputBuffer = consoleInputBuffer + (char)logIn;
				}
				else if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_BACK) { // backspace
					if (consoleInputBuffer.size() > 0) {
						consoleErase(1);

						// now we just remove the last char of our _inBuff
						consoleInputBuffer = consoleInputBuffer.substr(0, consoleInputBuffer.size() - 1);
					}
				}
				else if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN) {
					// log user's input and start a new line
					std::cout << "\n";
					consoleInputOffset = true;

					consoleInputBuffer2 = consoleInputBuffer;
					consoleInputWaiting = true;

					writeToLogfile("> " + consoleInputBuffer2 + "\n");

					// update the history
					if (consoleHistory.size() > 0) {
						if (consoleHistory[consoleHistory.size() - 1] != consoleInputBuffer2) {
							consoleHistory.push_back(consoleInputBuffer2);
						}
					}
					else {
						consoleHistory.push_back(consoleInputBuffer2);
					}

					consoleHistoryLocation = consoleHistory.size(); // set history loc to 1 more than the array size						
					consoleInputBuffer = ""; // clear buffer

					consoleInputOnScreen = false;
				}
				else if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_UP) {
					// can we go up through the history
					if (consoleHistoryLocation - 1 > -1) {
						clearConsoleInput(false);

						consoleHistoryLocation -= 1;
						consoleInputBuffer = consoleHistory[consoleHistoryLocation];
					}
				}
				else if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_DOWN) {
					// can we go down through the history
					if (consoleHistoryLocation + 1 >= consoleHistory.size()) {
						clearConsoleInput(false);

						consoleHistoryLocation = consoleHistory.size();
						consoleInputBuffer = "";
					}
					else {
						clearConsoleInput(false);

						consoleHistoryLocation += 1;
						consoleInputBuffer = consoleHistory[consoleHistoryLocation];
					}
				}
			}
		}
	}
}

void platform::writeToLogfile(std::string in) {
	FILE* st = stderr;
	fputs(in.c_str(), st);

	file.flush();
}

void platform::writeToConsole(std::string in, unsigned int color) {
	clearConsoleInput(false);
	
	const WORD colors[] =
	{
		15, 3, 12, 7
	};

	// Remember how things were when we started
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hStdOut, &csbi);

	// Set current color
	SetConsoleTextAttribute(hStdOut, colors[color]);

	std::cout << in;

	// Keep users happy
	SetConsoleTextAttribute(hStdOut, csbi.wAttributes);
}

void platform::showMessageBox(std::string caption, std::string text, bool showErrorIcon) {
	// sf string
	sf::String c(caption);
	sf::String t(text);

	MessageBoxW(nullptr, t.toWideString().c_str(), c.toWideString().c_str(), showErrorIcon ? MB_ICONSTOP : MB_ICONINFORMATION);
}

std::vector<std::string> platform::dirList(std::string path, bool includeExtra) {
	std::vector<std::string> resultsVec;

	HANDLE hFind;
	WIN32_FIND_DATAW data;

	// use sf string for conversion
	sf::String pathSf(path);

	hFind = FindFirstFileW((pathSf + "/*.*").toWideString().c_str(), &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			resultsVec.push_back(sf::String(data.cFileName));
		} while (FindNextFileW(hFind, &data));
		FindClose(hFind);
	}
	else {
		p::info("INVALID_HANDLE_VALUE while executing windows dirList. Nothing will be returned.");
	}

	if (resultsVec.size() > 0) {
		resultsVec.erase(resultsVec.begin());
		resultsVec.erase(resultsVec.begin());
	}

	// move anything with no extension to the top of the list
	unsigned int numFolders = 0;
	for (unsigned int i = 0; i < resultsVec.size(); i++) {
		if (resultsVec[i].find(".") == std::string::npos) {
			std::string f = resultsVec[i];

			resultsVec.erase(resultsVec.begin() + i);
			resultsVec.insert(resultsVec.begin(), f);

			numFolders += 1;
			i = numFolders;
		}
	}

	if (includeExtra) {
		resultsVec.insert(resultsVec.begin(), "..");
	}

	return resultsVec;
}

std::string platform::realPath(std::string in) {
	WCHAR buffer[4250];
	WCHAR** lppPart = { NULL };

	sf::String i(in);
	
	GetFullPathNameW(i.toWideString().c_str(), 4250, buffer, lppPart);

	return sf::String(buffer).toAnsiString();
}

bool platform::fileDelete(std::string in) {
	sf::String i(in);

	bool r = DeleteFileW(i.toWideString().c_str());

	std::string errstring;

	switch (GetLastError()) {
	default:
		errstring = "Unknown Error";
		break;
	case ERROR_FILE_NOT_FOUND:
		errstring = "File Not Found";
		break;
	case ERROR_ACCESS_DENIED:
		errstring = "Permission Denied";
		break;
	case ERROR_INVALID_NAME:
		errstring = "Bad name";
		break;
	}

	if (!r) {
		p::warn("Can't delete: " + in + " (" + errstring + ")");
	}

	return r;
}

bool platform::getConsoleInput(std::string & input)
{
	if (consoleInputWaiting) {
		input = consoleInputBuffer2;
		consoleInputWaiting = false;

		return true;
	}

	return false;
}

void platform::clearConsoleInput(bool eraseCompletely) {
	if (!consoleInputOnScreen)
		return;

	consoleErase(consoleInputBuffer.size());

	// overwrite any "> " that might exist in the consoleManager readout
	consoleErase(2);

	consoleInputOnScreen = false;

	if (eraseCompletely) {
		consoleInputBuffer = "";
	}
	else {
		consoleInputOffset = true;
	}
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
