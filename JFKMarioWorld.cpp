/*

	JFK Mario World cpp file. This is the main file of jfk mario world and the start of the game.

	Linux users do not support some features.

*/
#if defined(_WIN32)
#include <Windows.h>

#define USE_SDLMIXER_X
#define USE_FILESYSTEM

#elif defined(__linux__)

#define NDEBUG

#include <unistd.h>
#include <list>
void Sleep(int time) {
	usleep(time * 1000);
}
#endif

#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <ratio>
#include <chrono>
#include <ctype.h>
#include <cstring>
#include <cassert>
#if defined(USE_FILESYSTEM)
#include <filesystem>
#endif

using namespace std;
#if defined(USE_FILESYSTEM)
namespace fs = std::experimental::filesystem;
#endif

#include <SDL.h>
#include <SDL_image.h>
#if defined(USE_SDLMIXER_X)
#include <SDL2/SDL_mixer_ext.h>
#else
#include <SDL_mixer.h>
#endif

#if not defined(DISABLE_NETWORK)
#include <SFML/Network.hpp>
#endif


#if defined(__linux__)
#if not defined(DISABLE_NETWORK)
#define uint_fast8_t sf::Uint8
#define uint_fast16_t sf::Uint32
#define uint_fast32_t sf::Uint32
#define uint_fast64_t sf::Uint64
#define int_fast8_t sf::Int8
#define int_fast16_t sf::Int32
#define int_fast32_t sf::Int32
#define int_fast64_t sf::Int64
#endif

istream& getline(istream& stream, string& str)
{
	char ch;
	str.clear();
	while (stream.get(ch)) {
		if (ch == '\r') {
			stream.get(ch);
			if (ch == '\n') {
				break;
			}
		}
		str.push_back(ch);
	}
	return stream;
}
#endif


#include "cout.h"

#include "Global.h"
#include "ASM/ASM.h"
#include "config.h"

#include "Sprite_shared.h"

#include "LuaImplementation.h"

#include "Sound.h"


#include "Global.h"
#include "SDL_General.h"


#include "map16block.h"
#include "exanimation.h"
#include "Level.h"

#include "input.h"

#include "Sprite_system.h"
#include "mario.h"

#include "chat.h"

#include "game.h"


#if not defined(DISABLE_NETWORK)
#include "Netplay.h"
#endif

#include "sprite.h"
#include "renderer.h"

#include "zsnes_ui.h"

#if not defined(DISABLE_NETWORK)
#include "server.h"
#endif
#include "main.h"

int main(int argc, char* argv[])
{
	load_configuration();
#if defined(_WIN32)
	string t = "JFKMW Console - " + GAME_VERSION;
	std::wstring stemp = std::wstring(t.begin(), t.end());
	LPCWSTR sw = stemp.c_str();
	SetConsoleTitle(sw);
#endif
#if not defined(DISABLE_NETWORK)
	bool hosting = false;
	if (argc > 1)
	{
		if (strcmp(argv[1], "-h") == 0) {
			hosting = true;
		}
		if (strcmp(argv[1], "-c") == 0) {
			hosting = false;
		}
	}

	if (hosting) {
		cout << yellow << "[JFKMW] Welcome to JFK mario world " + GAME_VERSION + "! Hosting a server." << endl;
		if (argc > 2) {
			server_code(string(argv[2]));
		}
		else {
			server_code();
		}
	}
	else {
		cout << yellow << "[JFKMW] Welcome to JFK mario world " + GAME_VERSION + "!, Press R to reload the last level you played, Press Q if you want to play singleplayer, or W to connect to a server. Make sure you have typed in a option first in the UI! If you want to get updates, join the discord at https://discord.gg/fD5QQK3" << white << endl;
		player_code();
	}
#else
	player_code();
#endif
	cout << yellow << "[JFKMW] Quitting JFK mario world. Thanks for testing!" << white << endl;
	end_game();
	Sleep(1000);
	return 0;
}