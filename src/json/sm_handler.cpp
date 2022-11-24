#include <json/sm_handler.h>

namespace json::hndl {

HandleType_t hJSON = 0;
JSONHandler sJSONHandler;

HandleType_t hJSONObjectIterator = 0;
JSONObjectIteratorHandler sJSONObjectIteratorHandler;

} // namespace json::hndl

void json::JSONHandler::OnHandleDestroy(HandleType_t type, void* object)
{
	delete (IBaseJSON*)(object);
}

void json::JSONObjectIteratorHandler::OnHandleDestroy(HandleType_t type, void* object)
{
	delete (IJSONObjectIterator*)(object);
}