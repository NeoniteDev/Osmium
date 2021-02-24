#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <vector>
#include <fstream>
#include <iostream>
#include <regex>
#include <locale>
#include <winscard.h>

#include <dxgi.h>
#include <d3d11.h>
#include <Psapi.h>
#include <stdlib.h>
#include <Windows.h>
#include <detours.h>

#include "util.h"
#include "enums.h"
#include "constants.h"

#include "SDK.hpp"

using namespace SDK;