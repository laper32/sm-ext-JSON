```cpp
#ifndef _SMEXT_JSON_INTERFACE_H
#define _SMEXT_JSON_INTERFACE_H

#pragma once

#include <IShareSys.h>

#define SMINTERFACE_JSON_NAME "JSON"
#define SMINTERFACE_JSON_VERSION 1

#undef GetObject

class IJSONArray;
class IJSONObject;

enum JSONType
{
	Type_Unknown,
	Type_POD,		// integral, floating point, const char*
	Type_Array,		// JSON Array
	Type_Object		// JSON Object
};

class IBaseJSON
{
public:
	virtual void ImportFromString(const char* raw) = 0;
	virtual void ImportFromFile(const char* file_path) = 0;
	virtual ~IBaseJSON() {}
	virtual const char* Dump(int indent = -1) = 0;
	virtual void Clear() = 0;
	virtual size_t Size() = 0;
};

class IJSONObjectIterator
{
public:
	virtual ~IJSONObjectIterator() {}
	virtual const char* Key() = 0;
	virtual bool More() = 0;
	virtual void Next() = 0;
};

class IJSONObject : public IBaseJSON
{
public:
	virtual ~IJSONObject() {}
	virtual IJSONObject* GetObject(const char* key) = 0;
	virtual IJSONArray* GetArray(const char* key) = 0;
	virtual bool GetBool(const char* key) = 0;
	// Noting that int8, uint8 <=> char, unsigned char
	// If return it directly, it will return char/uchar itself
	// This means that this is probably not returning a VALUE if you want!
	// If you want to do so, you should write it at least int16!
	virtual int8_t GetInt8(const char* key) = 0;
	virtual uint8_t GetUInt8(const char* key) = 0;
	virtual int16_t GetInt16(const char* key) = 0;
	virtual uint16_t GetUInt16(const char* key) = 0;
	virtual int32_t GetInt32(const char* key) = 0;
	virtual uint32_t GetUInt32(const char* key) = 0;
	virtual int64_t GetInt64(const char* key) = 0;
	virtual uint64_t GetUInt64(const char* key) = 0;
	virtual float GetFloat32(const char* key) = 0;
	virtual double GetFloat64(const char* key) = 0;
	virtual const char* GetString(const char* key) = 0;
	virtual bool IsNull(const char* key) = 0;
	virtual bool HasKey(const char* key) = 0;

	virtual void SetJSON(const char* key, const char* raw) = 0;
	virtual void SetBool(const char* key, bool value) = 0;
	virtual void SetInt8(const char* key, int8_t value) = 0;
	virtual void SetUInt8(const char* key, uint8_t value) = 0;
	virtual void SetInt16(const char* key, int16_t value) = 0;
	virtual void SetUInt16(const char* key, uint16_t value) = 0;
	virtual void SetInt32(const char* key, int32_t value) = 0;
	virtual void SetUInt32(const char* key, uint32_t value) = 0;
	virtual void SetInt64(const char* key, int64_t value) = 0;
	virtual void SetUInt64(const char* key, uint64_t value) = 0;
	virtual void SetFloat32(const char* key, float value) = 0;
	virtual void SetFloat64(const char* key, double value) = 0;
	virtual void SetString(const char* key, const char* value) = 0;
	virtual void SetNull(const char* key) = 0;
	virtual bool Remove(const char* key) = 0;
};

class IJSONArray : public IBaseJSON
{
public:
	~IJSONArray() {}
	virtual IJSONArray* GetArray(size_t idx) = 0;
	virtual IJSONObject* GetObject(size_t idx) = 0;
	virtual bool GetBool(size_t idx) = 0;
	// Noting that int8, uint8 <=> char, unsigned char
	// If return it directly, it will return char/uchar itself
	// This means that this is probably not returning a VALUE if you want!
	// If you want to do so, you should write it at least int16!
	virtual int8_t GetInt8(size_t idx) = 0;
	virtual uint8_t GetUInt8(size_t idx) = 0;
	virtual int16_t GetInt16(size_t idx) = 0;
	virtual uint16_t GetUInt16(size_t idx) = 0;
	virtual int32_t GetInt32(size_t idx) = 0;
	virtual uint32_t GetUInt32(size_t idx) = 0;
	virtual int64_t GetInt64(size_t idx) = 0;
	virtual uint64_t GetUInt64(size_t idx) = 0;
	virtual const char* GetString(size_t idx) = 0;
	virtual float GetFloat32(size_t idx) = 0;
	virtual double GetFloat64(size_t idx) = 0;
	virtual bool IsNull(size_t idx) = 0;

	virtual void SetJSON(size_t idx, const char* value) = 0;
	virtual void SetBool(size_t idx, bool value) = 0;
	virtual void SetInt8(size_t idx, int8_t value) = 0;
	virtual void SetUInt8(size_t idx, uint8_t value) = 0;
	virtual void SetInt16(size_t idx, int16_t value) = 0;
	virtual void SetUInt16(size_t idx, uint16_t value) = 0;
	virtual void SetInt32(size_t idx, int32_t value) = 0;
	virtual void SetUInt32(size_t idx, uint32_t value) = 0;
	virtual void SetInt64(size_t idx, int64_t value) = 0;
	virtual void SetUInt64(size_t idx, uint64_t value) = 0;
	virtual void SetFloat32(size_t idx, float value) = 0;
	virtual void SetFloat64(size_t idx, double value) = 0;
	virtual void SetString(size_t idx, const char* value) = 0;
	virtual void SetNull(size_t idx) = 0;

	virtual void PushJSON(const char* value) = 0;
	virtual void PushBool(bool value) = 0;
	virtual void PushInt8(int8_t value) = 0;
	virtual void PushUInt8(uint8_t value) = 0;
	virtual void PushInt16(int16_t value) = 0;
	virtual void PushUInt16(uint16_t value) = 0;
	virtual void PushInt32(int32_t value) = 0;
	virtual void PushUInt32(uint32_t value) = 0;
	virtual void PushInt64(int64_t value) = 0;
	virtual void PushUInt64(uint64_t value) = 0;
	virtual void PushFloat32(float value) = 0;
	virtual void PushFloat64(double value) = 0;
	virtual void PushString(const char* value) = 0;
	virtual void PushNull() = 0;

	virtual void Remove(size_t idx) = 0;

};

class IJSON : public SMInterface
{
public:
	virtual unsigned int GetInterfaceVersion()
	{
		return SMINTERFACE_JSON_VERSION;
	}
	virtual const char* GetInterfaceName()
	{
		return SMINTERFACE_JSON_NAME;
	}

	virtual IJSONObject* CreateObject() = 0;
	virtual IJSONArray* CreateArray() = 0;
	virtual IBaseJSON* GetFromHandle(Handle_t hndl) = 0;
	virtual Handle_t CreateHandle(JSONType type) = 0;
	virtual Handle_t ToHandle(IBaseJSON* json) = 0;
	virtual void FreeHandle(Handle_t hndl) = 0;
	// Check the JSON type of the text.
	// The result should only either Type_Array or Type_Object
	virtual JSONType GetType(const char* text) = 0;
};

#endif // !_SMEXT_JSON_INTERFACE_H

```
