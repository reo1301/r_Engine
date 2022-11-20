#pragma once

//インクルード
//Define系
#include "../my_lib_def.h"

#define GAME_NAMESPACE my_game
#define GAME_NAMESPACE_BEGIN namespace GAME_NAMESPACE {
#define GAME_NAMESPACE_END }

GAME_NAMESPACE_BEGIN
GAME_NAMESPACE_END

using namespace MY_LIB_NAMESPACE;
using namespace GAME_NAMESPACE;