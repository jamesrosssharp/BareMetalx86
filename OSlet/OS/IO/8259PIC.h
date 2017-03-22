
#pragma once

#include "io.h"

bool	io_initAndRemapPIC();
void 	io_acknowledgeInterruptPIC(unsigned char interrupt);
