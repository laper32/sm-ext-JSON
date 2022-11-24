#ifndef _JSON_MAIN_H
#define _JSON_MAIN_H

#pragma once

#include <json/shared.h>

namespace json {

	bool SDK_OnLoad(char* error, size_t maxlen, bool late);
	void SDK_OnUnload();
	void SDK_OnAllLoaded();

} // namespace json

#endif // !_JSON_MAIN_H