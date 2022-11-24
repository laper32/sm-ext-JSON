#include <detail/ext.h>
#include <json/main.h>

Extension g_Ext;

SMEXT_LINK(&g_Ext);

bool Extension::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
	if (!detail::SDK_OnLoad(error, maxlen, late))
	{
		return false;
	}

	if (!json::SDK_OnLoad(error, maxlen, late))
	{
		return false;
	}

	return SDKExtension::SDK_OnLoad(error, maxlen, late);
}

void Extension::SDK_OnUnload()
{
	json::SDK_OnUnload();
	detail::SDK_OnUnload();
	return SDKExtension::SDK_OnUnload();
}

void Extension::SDK_OnAllLoaded()
{
	detail::SDK_OnAllLoaded();
	json::SDK_OnAllLoaded();
	return SDKExtension::SDK_OnAllLoaded();
}
