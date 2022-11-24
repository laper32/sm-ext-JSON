#include <json/sm_interface.h>
#include <json/sm_handler.h>
#include <fstream>
#include <sstream>

// TODO: Reduce exception count

namespace json {

JSON g_JSON;

} // namespace json

IJSONObject* json::JSON::CreateObject()
{
	return new JSONObject();
}

IJSONArray* json::JSON::CreateArray()
{
	return new JSONArray();
}

IBaseJSON* json::JSON::GetFromHandle(Handle_t hndl)
{
	HandleError ec = {};
	IBaseJSON* raw = nullptr;
	HandleSecurity sec{ nullptr, myself->GetIdentity() };
	ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw);
	if (ec != HandleError_None)
	{
		smutils->LogError(myself, "%s", fmt::format("Error occured when reading handle {:#x} (Error code: {})", hndl, ec).c_str());
		return nullptr;
	}
	return raw;
}

Handle_t json::JSON::CreateHandle(JSONType type)
{
	if (type == Type_Array)
	{
		auto arr = CreateArray();
		HandleError ec = {};
		HandleSecurity sec = { nullptr, myself->GetIdentity() };
		Handle_t ret = handlesys->CreateHandleEx(hndl::hJSON, arr, &sec, nullptr, &ec);
		if (ec != HandleError_None)
		{
			smutils->LogError(myself, "%s", fmt::format("Error occured when creating handle. Error code: {}", ec).c_str());
			return BAD_HANDLE;
		}
		return ret;
	}
	
	if (type == Type_Object)
	{
		auto arr = CreateObject();
		HandleError ec = {};
		HandleSecurity sec = { nullptr, myself->GetIdentity() };
		Handle_t ret = handlesys->CreateHandleEx(hndl::hJSON, arr, &sec, nullptr, &ec);
		if (ec != HandleError_None)
		{
			smutils->LogError(myself, "%s", fmt::format("Error occured when creating handle. Error code: {}", ec).c_str());
			return BAD_HANDLE;
		}
		return ret;
	}

	smutils->LogError(myself, "Only accept Type_Array or Type_Object.");
	return BAD_HANDLE;
}

Handle_t json::JSON::ToHandle(IBaseJSON* json)
{
	HandleError ec = {};
	HandleSecurity sec = { nullptr, myself->GetIdentity() };
	Handle_t ret = handlesys->CreateHandleEx(hndl::hJSON, json, &sec, nullptr, &ec);
	if (ec != HandleError_None)
	{
		smutils->LogError(myself, "%s", fmt::format("Error occured when creating handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	return ret;
}

void json::JSON::FreeHandle(Handle_t hndl)
{
	HandleSecurity sec = { nullptr, myself->GetIdentity() };
	if (auto ec = handlesys->FreeHandle(hndl, &sec); ec != HandleError_None)
	{
		smutils->LogError(myself, "%s", fmt::format("Error when freeing handle. Error code: {}", ec).c_str());
	}
}

JSONType json::JSON::GetType(const char* text)
{
	if (!nlohmann::json::accept(text))
	{
		return Type_Unknown;
	}
	auto j = nlohmann::json::parse(text);
	if (j.is_array())
	{
		return Type_Array;
	}
	else if (j.is_object())
	{
		return Type_Object;
	}
	return Type_Unknown;
}

json::BaseJSON::BaseJSON() { }

json::BaseJSON::BaseJSON(const char* in)
{
	try
	{
		_impl = nlohmann::json::parse(in);
	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
	}

}

json::BaseJSON::~BaseJSON() { }

void json::BaseJSON::ImportFromString(const char* raw)
{
	try
	{
		auto j = nlohmann::json::parse(raw);
		_impl = j;

	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
	}
}

void json::BaseJSON::ImportFromFile(const char* file_path)
{
	std::ifstream ifs(file_path);
	if (ifs.fail()) {
		smutils->LogError(myself,
			"%s", fmt::format("No such file or directory, or we do not have permission. File path: \"{}\"", file_path).c_str()
		);
		//logger::json->Error(
		//	fmt::format("No such file or directory, or we do not have permission. File path: \"{}\"", file_path).c_str()
		//);
	}
	else
	{
		std::ostringstream oss;
		oss << ifs.rdbuf();
		ImportFromString(oss.str().c_str());
	}
}

const char* json::BaseJSON::Dump(int indent)
{
	std::string raw = _impl.dump(indent);
	char* ret = new char[raw.size() + 1] { '\0' };
	strcpy(ret, raw.c_str());
	return ret;
}

void json::BaseJSON::Clear()
{
	_impl.clear();
}

size_t json::BaseJSON::Size()
{
	return _impl.size();
}

nlohmann::json& json::BaseJSON::Get()
{
	return _impl;
}

json::JSONObjectIterator::JSONObjectIterator(JSONObject* in) : _in(*in), _it(in->Get().begin())
{

}

json::JSONObjectIterator::~JSONObjectIterator()
{

}

const char* json::JSONObjectIterator::Key()
{
	return _it.key().c_str();
}

bool json::JSONObjectIterator::More()
{
	return (_it != _in.Get().end());
}

void json::JSONObjectIterator::Next()
{
	++_it;
}

json::JSONObject::JSONObject()
{

}

json::JSONObject::JSONObject(const char* in)
{
	try
	{
		auto j = nlohmann::json::parse(in);
		if (j.is_object())
		{
			_impl = j;
		}
	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
	}

}

json::JSONObject::~JSONObject()
{
}

void json::JSONObject::ImportFromString(const char* raw)
{
	try
	{
		auto j = nlohmann::json::parse(raw);
		if (j.is_object())
		{
			_impl = j;
		}
	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
	}
}

void json::JSONObject::ImportFromFile(const char* file_path)
{
	std::ifstream ifs(file_path);
	if (ifs.fail())
	{
		smutils->LogError(myself,
			"%s", fmt::format("No such file or directory, or we do not have permission. File path: \"{}\"", file_path).c_str()
		);
		//logger::json->Error(
		//	fmt::format("No such file or directory, or we do not have permission. File path: \"{}\"", file_path).c_str()
		//);
	}
	else
	{
		std::ostringstream oss;
		oss << ifs.rdbuf();
		ImportFromString(oss.str().c_str());
	}
}

const char* json::JSONObject::Dump(int indent)
{
	std::string data = _impl.dump(indent);
	char* raw = new char[data.size() + 1] { '\0' };
	strcpy(raw, data.c_str());
	return raw;
}

IJSONObject* json::JSONObject::GetObject(const char* key)
{
	try
	{
		return new JSONObject(_impl[key].dump().c_str());
	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
		return nullptr;
	}
}

IJSONArray* json::JSONObject::GetArray(const char* key)
{
	try
	{
		return new JSONArray(_impl[key].dump().c_str());
	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
		return nullptr;
	}
}

template<typename T> inline T json::JSONObject::GetValue(const char* key)
{
	try
	{
		return _impl[key].get<T>();
	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
		return T();
	}
}

template<> inline const char* json::JSONObject::GetValue<const char*>(const char* key)
{
	try
	{
		std::string data = _impl[key].get<std::string>();
		char* ret = new char[data.size() + 1];
		strcpy(ret, data.c_str());
		return ret;
	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
		return "";
	}
}

bool json::JSONObject::GetBool(const char* key) { return GetValue<bool>(key); }
int8_t json::JSONObject::GetInt8(const char* key) { return GetValue<int8_t>(key); }
uint8_t json::JSONObject::GetUInt8(const char* key) { return GetValue<uint8_t>(key); }
int16_t json::JSONObject::GetInt16(const char* key) { return GetValue<int16_t>(key); }
uint16_t json::JSONObject::GetUInt16(const char* key) { return GetValue<uint16_t>(key); }
int32_t json::JSONObject::GetInt32(const char* key) { return GetValue<int32_t>(key); }
uint32_t json::JSONObject::GetUInt32(const char* key) { return GetValue<uint32_t>(key); }
int64_t json::JSONObject::GetInt64(const char* key) { return GetValue<int64_t>(key); }
uint64_t json::JSONObject::GetUInt64(const char* key) { return GetValue<uint64_t>(key); }
float json::JSONObject::GetFloat32(const char* key) { return GetValue<float>(key); }
double json::JSONObject::GetFloat64(const char* key) { return GetValue<double>(key); }
const char* json::JSONObject::GetString(const char* key) { return GetValue<const char*>(key); }

bool json::JSONObject::IsNull(const char* key)
{
	return _impl[key].is_null();
}

bool json::JSONObject::HasKey(const char* key)
{
	return _impl.contains(key);
}

void json::JSONObject::SetJSON(const char* key, const char* raw)
{
	try
	{
		_impl[key] = nlohmann::json::parse(raw);
	}
	catch (const std::exception& ex)
	{
		//logger::json->Error(ex.what());
	}
}

void json::JSONObject::SetBool(const char* key, bool value) { _impl[key] = value; }
void json::JSONObject::SetInt8(const char* key, int8_t value) { _impl[key] = value; }
void json::JSONObject::SetUInt8(const char* key, uint8_t value) { _impl[key] = value; }
void json::JSONObject::SetInt16(const char* key, int16_t value) { _impl[key] = value; }
void json::JSONObject::SetUInt16(const char* key, uint16_t value) { _impl[key] = value; }
void json::JSONObject::SetInt32(const char* key, int32_t value) { _impl[key] = value; }
void json::JSONObject::SetUInt32(const char* key, uint32_t value) { _impl[key] = value; }
void json::JSONObject::SetInt64(const char* key, int64_t value) { _impl[key] = value; }
void json::JSONObject::SetUInt64(const char* key, uint64_t value) { _impl[key] = value; }
void json::JSONObject::SetFloat32(const char* key, float value) { _impl[key] = value; }
void json::JSONObject::SetFloat64(const char* key, double value) { _impl[key] = value; }
void json::JSONObject::SetString(const char* key, const char* value) { _impl[key] = value; }
void json::JSONObject::SetNull(const char* key) { _impl[key] = nullptr; }

bool json::JSONObject::Remove(const char* key)
{
	if (_impl.contains(key)) {
		_impl.erase(key);
		return true;
	} return false;
}

void json::JSONObject::Clear()
{
	_impl.clear();
}

size_t json::JSONObject::Size()
{
	return _impl.size();
}

json::JSONArray::JSONArray()
{

}

json::JSONArray::JSONArray(const char* in)
{
	try
	{
		auto j = nlohmann::json::parse(in);
		if (j.is_array())
		{
			_impl = j;
		}
	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
	}
}

json::JSONArray::~JSONArray()
{

}

void json::JSONArray::ImportFromString(const char* raw)
{
	try
	{
		auto j = nlohmann::json::parse(raw);
		if (j.is_array())
		{
			_impl = j;
		}
	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
	}
}

void json::JSONArray::ImportFromFile(const char* file_path)
{
	std::ifstream ifs(file_path);
	if (ifs.fail()) 
	{
		smutils->LogError(myself,
			"%s", fmt::format("No such file or directory, or we do not have permission. File path: \"{}\"", file_path).c_str()
		);
		//logger::json->Error(
		//	fmt::format("No such file or directory, or we do not have permission. File path: \"{}\"", file_path).c_str()
		//);
	}
	else
	{
		std::ostringstream oss;
		oss << ifs.rdbuf();
		ImportFromString(oss.str().c_str());
	}
}

const char* json::JSONArray::Dump(int indent)
{
	std::string data = _impl.dump(indent);
	char* ret = new char[data.size() + 1] { '\0' };
	strcpy(ret, data.c_str());
	return ret;
}

IJSONArray* json::JSONArray::GetArray(size_t idx)
{
	try
	{
		return new JSONArray(_impl[idx].dump().c_str());
	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
		return nullptr;
	}
}

IJSONObject* json::JSONArray::GetObject(size_t idx)
{
	try
	{
		return new JSONObject(_impl[idx].dump().c_str());
	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
		return nullptr;
	}
}

template<typename T> inline T json::JSONArray::GetValue(size_t idx)
{
	try
	{
		return _impl[idx].get<bool>();
	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
		return T();
	}
}

template<> inline const char* json::JSONArray::GetValue<const char*>(size_t idx)
{
	try
	{
		std::string data = _impl[idx].get<std::string>();
		char* ret = new char[data.size() + 1];
		strcpy(ret, data.c_str());
		return ret;
	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
		return "";
	}
}

bool json::JSONArray::GetBool(size_t idx) { return GetValue<bool>(idx); }
int8_t json::JSONArray::GetInt8(size_t idx) { return GetValue<int8_t>(idx); }
uint8_t json::JSONArray::GetUInt8(size_t idx) { return GetValue<uint8_t>(idx); }
int16_t json::JSONArray::GetInt16(size_t idx) { return GetValue<int16_t>(idx); }
uint16_t json::JSONArray::GetUInt16(size_t idx) { return GetValue<uint16_t>(idx); }
int32_t json::JSONArray::GetInt32(size_t idx) { return GetValue<int32_t>(idx); }
uint32_t json::JSONArray::GetUInt32(size_t idx) { return GetValue<uint32_t>(idx); }
int64_t json::JSONArray::GetInt64(size_t idx) { return GetValue<int64_t>(idx); }
uint64_t json::JSONArray::GetUInt64(size_t idx) { return GetValue<uint64_t>(idx); }
float json::JSONArray::GetFloat32(size_t idx) { return GetValue<float>(idx); }
double json::JSONArray::GetFloat64(size_t idx) { return GetValue<double>(idx); }
const char* json::JSONArray::GetString(size_t idx) { return GetValue<const char*>(idx); }
bool json::JSONArray::IsNull(size_t idx) { return _impl[idx].is_null(); }

void json::JSONArray::SetJSON(size_t idx, const char* value)
{
	try
	{
		_impl[idx] = nlohmann::json::parse(value);
	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
	}
}

void json::JSONArray::SetBool(size_t idx, bool value) { _impl[idx] = value; }
void json::JSONArray::SetInt8(size_t idx, int8_t value) { _impl[idx] = value; }
void json::JSONArray::SetUInt8(size_t idx, uint8_t value) { _impl[idx] = value; }
void json::JSONArray::SetInt16(size_t idx, int16_t value) { _impl[idx] = value; }
void json::JSONArray::SetUInt16(size_t idx, uint16_t value) { _impl[idx] = value; }
void json::JSONArray::SetInt32(size_t idx, int32_t value) { _impl[idx] = value; }
void json::JSONArray::SetUInt32(size_t idx, uint32_t value) { _impl[idx] = value; }
void json::JSONArray::SetInt64(size_t idx, int64_t value) { _impl[idx] = value; }
void json::JSONArray::SetUInt64(size_t idx, uint64_t value) { _impl[idx] = value; }
void json::JSONArray::SetFloat32(size_t idx, float value) { _impl[idx] = value; }
void json::JSONArray::SetFloat64(size_t idx, double value) { _impl[idx] = value; }
void json::JSONArray::SetString(size_t idx, const char* value) { _impl[idx] = value; }
void json::JSONArray::SetNull(size_t idx) { _impl[idx] = nullptr; }

void json::JSONArray::PushJSON(const char* value)
{
	try
	{
		_impl.emplace_back(nlohmann::json::parse(value));

	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
	}
}

template<typename T> inline void json::JSONArray::PushValue(T value)
{
	try
	{
		_impl.emplace_back(value);

	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
	}
}

void json::JSONArray::PushBool(bool value) { PushValue(value); }
void json::JSONArray::PushInt8(int8_t value) { PushValue(value); }
void json::JSONArray::PushUInt8(uint8_t value) { PushValue(value); }
void json::JSONArray::PushInt16(int16_t value) { PushValue(value); }
void json::JSONArray::PushUInt16(uint16_t value) { PushValue(value); }
void json::JSONArray::PushInt32(int32_t value) { PushValue(value); }
void json::JSONArray::PushUInt32(uint32_t value) { PushValue(value); }
void json::JSONArray::PushInt64(int64_t value) { PushValue(value); }
void json::JSONArray::PushUInt64(uint64_t value) { PushValue(value); }
void json::JSONArray::PushFloat32(float value) { PushValue(value); }
void json::JSONArray::PushFloat64(double value) { PushValue(value); }
void json::JSONArray::PushString(const char* value) { PushValue(value); }
void json::JSONArray::PushNull() { PushValue(nullptr); }

void json::JSONArray::Remove(size_t idx)
{
	try
	{
		_impl.erase(idx);
	}
	catch (const std::exception& ex)
	{
		smutils->LogError(myself, "%s", ex.what());
		//logger::json->Error(ex.what());
	}
}

void json::JSONArray::Clear() { _impl.clear(); }
size_t json::JSONArray::Size() { return _impl.size(); }
