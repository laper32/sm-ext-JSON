#include <detail/ext.h>

bool detail::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
	sharesys->RegisterLibrary(myself, "JSON");

	return true;
}

void detail::SDK_OnUnload()
{
}

void detail::SDK_OnAllLoaded()
{

}