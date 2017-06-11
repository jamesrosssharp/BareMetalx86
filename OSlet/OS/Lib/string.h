#pragma once

#include "klib.h"

void lib_bzero(void* mem, int bytes);
void lib_memcpy(void* dest, void* src, unsigned int bytes);
int  lib_strncmp(const char* s1, const char* s2, unsigned int len);
int  lib_strlen(const char* s);
char lib_toupper(char c);
void lib_strtoupper(char* s);
void lib_trim(char** s);
bool lib_isspace(char c);
bool lib_isdigit(char c);
int  lib_atoi(char* s);
