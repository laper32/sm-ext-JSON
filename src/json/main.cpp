#include <json/main.h>
#include <json/sm_handler.h>
#include <json/sm_interface.h>
#include <json/sp_native.h>

bool json::SDK_OnLoad(char* error, size_t maxlength, bool late)
{
	sharesys->AddInterface(myself, &g_JSON);
	sharesys->AddNatives(myself, g_JSONNative);
	//sharesys->AddDependency(myself, "log4sm.ext", true, true);
	HandleAccess sec;
	handlesys->InitAccessDefaults(nullptr, &sec);
	sec.access[HandleAccess_Delete] = 0;

	hndl::hJSON = handlesys->CreateType("JSON", &hndl::sJSONHandler, 0, nullptr, &sec, myself->GetIdentity(), nullptr);
	hndl::hJSONObjectIterator = handlesys->CreateType(
		"JSONObjectIterator", &hndl::sJSONObjectIteratorHandler, 0, nullptr, &sec, myself->GetIdentity(), nullptr
	);

	return true;
}

void json::SDK_OnUnload()
{
	handlesys->RemoveType(hndl::hJSONObjectIterator, myself->GetIdentity());
	handlesys->RemoveType(hndl::hJSON, myself->GetIdentity());
}

void json::SDK_OnAllLoaded()
{
	// Since we are now adding interface.
	// If we need to add interface, means that we have to create logger after we finished up everything.
	//SM_GET_LATE_IFACE(LOG4SM, log4sm);
	//logger::json = log4sm->GetManager()->CreateLogger(false, "JSON");
}
