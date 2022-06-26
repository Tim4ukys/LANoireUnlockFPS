#pragma once

//
// stl
#include <iostream>
#include <exception>
#include <cstdio>
#include <memory>

//
// win-api
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

//
// My
#include "Config.h"


#define RTHROW(what) throw std::runtime_error(what)