#ifndef _DETAIL_EXT_H
#define _DETAIL_EXT_H

#pragma once
#include <detail/shared.h>

namespace detail {

bool SDK_OnLoad(char* error, size_t maxlen, bool late);
void SDK_OnUnload();
void SDK_OnAllLoaded();

}

#endif // !_DETAIL_EXT_H
