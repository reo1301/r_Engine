#pragma once

// 出力用にインクルード
#include <stdio.h>
#include <assert.h>

#define MY_LIB_NAMESPACE my_library
#define MY_LIB_NAMESPACE_BEGIN namespace MY_LIB_NAMESPACE {
#define MY_LIB_NAMESPACE_END }

MY_LIB_NAMESPACE_BEGIN
MY_LIB_NAMESPACE_END

using namespace MY_LIB_NAMESPACE;