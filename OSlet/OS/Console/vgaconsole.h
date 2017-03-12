
#pragma once

#include "console.h"

struct ConsoleInfo* vgaConsole_getConsoleInfo();
struct ConsoleInfo* vgaConsole_initVGAConsole(int port, int columns);
