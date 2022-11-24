#ifndef _JSON_SM_INTERFACE_H
#define _JSON_SM_INTERFACE_H

#pragma once

#include <json/shared.h>

namespace json {

class JSON : public IJSON
{
public:
	IJSONObject* CreateObject();
	IJSONArray* CreateArray();
	IBaseJSON* GetFromHandle(Handle_t hndl);
	Handle_t CreateHandle(JSONType type);
	Handle_t ToHandle(IBaseJSON* json);
	void FreeHandle(Handle_t hndl);
	JSONType GetType(const char* key);
};

extern JSON g_JSON;

class BaseJSON : public IBaseJSON
{
public:
	BaseJSON();
	BaseJSON(const char* in);
	~BaseJSON();
	void ImportFromString(const char* raw);
	void ImportFromFile(const char* file_path);
	const char* Dump(int indent);
	void Clear();
	size_t Size();
	nlohmann::json& Get();
protected:
	nlohmann::json _impl;
};

class JSONObject : public BaseJSON, public IJSONObject
{
public:
	JSONObject();

	JSONObject(const char* in);

	~JSONObject();

	void ImportFromString(const char* raw);

	void ImportFromFile(const char* file_path);

	const char* Dump(int indent);

	IJSONObject* GetObject(const char* key);

	IJSONArray* GetArray(const char* key);

	template<typename T> inline T GetValue(const char* key);

	bool GetBool(const char* key);

	int8_t GetInt8(const char* key);

	uint8_t GetUInt8(const char* key);

	int16_t GetInt16(const char* key);

	uint16_t GetUInt16(const char* key);

	int32_t GetInt32(const char* key);

	uint32_t GetUInt32(const char* key);

	int64_t GetInt64(const char* key);

	uint64_t GetUInt64(const char* key);

	float GetFloat32(const char* key);

	double GetFloat64(const char* key);

	const char* GetString(const char* key);

	bool IsNull(const char* key);

	bool HasKey(const char* key);

	void SetJSON(const char* key, const char* raw);

	void SetBool(const char* key, bool value);

	void SetInt8(const char* key, int8_t value);

	void SetUInt8(const char* key, uint8_t value);

	void SetInt16(const char* key, int16_t value);

	void SetUInt16(const char* key, uint16_t value);

	void SetInt32(const char* key, int32_t value);

	void SetUInt32(const char* key, uint32_t value);

	void SetInt64(const char* key, int64_t value);

	void SetUInt64(const char* key, uint64_t value);

	void SetFloat32(const char* key, float value);

	void SetFloat64(const char* key, double value);

	void SetString(const char* key, const char* value);

	void SetNull(const char* key);

	bool Remove(const char* key);

	void Clear();

	size_t Size();
};

class JSONObjectIterator : public IJSONObjectIterator
{
	JSONObject& _in;
	//nlohmann::json _in;
	nlohmann::json::iterator _it;
public:
	JSONObjectIterator(JSONObject* in);
	//JSONObjectIterator(const char* in);
	~JSONObjectIterator();
	const char* Key();
	bool More();
	void Next();
};

class JSONArray : public BaseJSON, public IJSONArray
{
public:
	JSONArray();

	JSONArray(const char* in);

	~JSONArray();

	void ImportFromString(const char* raw);

	void ImportFromFile(const char* file_path);

	const char* Dump(int indent);

	IJSONArray* GetArray(size_t idx);

	IJSONObject* GetObject(size_t idx);

	template<typename T> inline T GetValue(size_t idx);

	bool GetBool(size_t idx);

	int8_t GetInt8(size_t idx);

	uint8_t GetUInt8(size_t idx);

	int16_t GetInt16(size_t idx);

	uint16_t GetUInt16(size_t idx);

	int32_t GetInt32(size_t idx);

	uint32_t GetUInt32(size_t idx);

	int64_t GetInt64(size_t idx);

	uint64_t GetUInt64(size_t idx);

	float GetFloat32(size_t idx);

	double GetFloat64(size_t idx);

	const char* GetString(size_t idx);

	bool IsNull(size_t idx);

	void SetJSON(size_t idx, const char* value);

	void SetBool(size_t idx, bool value);

	void SetInt8(size_t idx, int8_t value);

	void SetUInt8(size_t idx, uint8_t value);

	void SetInt16(size_t idx, int16_t value);

	void SetUInt16(size_t idx, uint16_t value);

	void SetInt32(size_t idx, int32_t value);

	void SetUInt32(size_t idx, uint32_t value);

	void SetInt64(size_t idx, int64_t value);

	void SetUInt64(size_t idx, uint64_t value);

	void SetFloat32(size_t idx, float value);

	void SetFloat64(size_t idx, double value);

	void SetString(size_t idx, const char* value);

	void SetNull(size_t idx);

	void PushJSON(const char* value);

	template<typename T> inline void PushValue(T value);

	void PushBool(bool value);

	void PushInt8(int8_t value);

	void PushUInt8(uint8_t value);

	void PushInt16(int16_t value);

	void PushUInt16(uint16_t value);

	void PushInt32(int32_t value);

	void PushUInt32(uint32_t value);

	void PushInt64(int64_t value);

	void PushUInt64(uint64_t value);

	void PushFloat32(float value);

	void PushFloat64(double value);

	void PushString(const char* value);

	void PushNull();

	void Remove(size_t idx);

	void Clear();

	size_t Size();
};

} // namespace json

#endif // !_JSON_SM_INTERFACE_H
