#ifndef _JSON_SM_HANDLER_H
#define _JSON_SM_HANDLER_H

#pragma once

#include <json/shared.h>

namespace json {

class JSONHandler : public IHandleTypeDispatch
{
public:
	virtual void OnHandleDestroy(HandleType_t type, void* object);
};

class JSONObjectIteratorHandler : public IHandleTypeDispatch
{
public:
	virtual void OnHandleDestroy(HandleType_t type, void* object);
};

namespace hndl {

extern HandleType_t hJSON;
extern JSONHandler sJSONHandler;

extern HandleType_t hJSONObjectIterator;
extern JSONObjectIteratorHandler sJSONObjectIteratorHandler;
} // namespace hndl

} // namespace json

#endif // !_JSON_SM_HANDLER_H
