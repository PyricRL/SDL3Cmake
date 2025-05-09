#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <algorithm>

#include <SDL3/SDL.h>

#include "../Backend/Cpp/Parse.hpp"

int generateData(int length, std::string Type);

int pullDataFromFunctions();

int displayDataToScreen(SDL_Renderer* renderer, SDL_Window* window, SDL_Surface* screenSurface, int x, int y, int w, int h, int bordorWidth);