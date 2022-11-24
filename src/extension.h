#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

#include "smsdk_ext.h"

// You must disable this since this may corrupt the whole compilation process!
#undef min
#undef max
#undef clamp

class Extension : public SDKExtension
{
public:
	virtual bool SDK_OnLoad(char *error, size_t maxlen, bool late);
	virtual void SDK_OnUnload();
	virtual void SDK_OnAllLoaded();
};

#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
