#include <json/sp_native.h>
#include <json/sm_handler.h>
#include <json/sm_interface.h>

#include <extsdk/interop.hpp>

#include <fstream>
#include <sstream>

sp_nativeinfo_t json::g_JSONNative[] = {
	{ "JSON.ToFile", api::ToFile },
	{ "JSON.ToString", api::ToString },

	{ "JSON.Clear", api::Clear },
	{ "JSON.Size.get", api::Size },

	{ "JSONObject.JSONObject", api::ObjectCreate },
	{ "JSONObject.ImportFromFile", api::ObjectImportFromFile},
	{ "JSONObject.ImportFromString", api::ObjectImportFromString },
	{ "JSONObject.GetObject", api::ObjectGetValueObject },
	{ "JSONObject.GetArray", api::ObjectGetValueArray },
	{ "JSONObject.GetBool", api::ObjectGetValueBool },
	{ "JSONObject.GetInt", api::ObjectGetValueInt },
	{ "JSONObject.GetInt64", api::ObjectGetValueInt64 },
	{ "JSONObject.GetFloat", api::ObjectGetValueFloat },
	{ "JSONObject.GetString", api::ObjectGetValueString },
	{ "JSONObject.IsNull", api::ObjectIsValueNull },
	{ "JSONObject.HasKey", api::ObjectHasKey },
	{ "JSONObject.SetJSON", api::ObjectSetValueJSON },
	{ "JSONObject.SetBool", api::ObjectSetValueBool },
	{ "JSONObject.SetInt", api::ObjectSetValueInt },
	{ "JSONObject.SetInt64", api::ObjectSetValueInt64 },
	{ "JSONObject.SetFloat", api::ObjectSetValueFloat },
	{ "JSONObject.SetString", api::ObjectSetValueString },
	{ "JSONObject.SetNull", api::ObjectSetValueNull },
	{ "JSONObject.Remove", api::ObjectRemoveKey },
	{ "JSONObject.CreateIterator", api::ObjectCreateIterator },

	{ "JSONObjectIterator.Key", api::ObjectIteratorKey },
	{ "JSONObjectIterator.More", api::ObjectIteratorMore },
	{ "JSONObjectIterator.Next", api::ObjectIteratorNext },

	{ "JSONArray.JSONArray", api::ArrayCreate },
	{ "JSONArray.ImportFromFile", api::ArrayImportFromFile },
	{ "JSONArray.ImportFromString", api::ArrayImportFromString },
	{ "JSONArray.GetObject", api::ArrayGetValueObject },
	{ "JSONArray.GetArray", api::ArrayGetValueArray },
	{ "JSONArray.GetBool", api::ArrayGetValueBool },
	{ "JSONArray.GetInt", api::ArrayGetValueInt },
	{ "JSONArray.GetInt64", api::ArrayGetValueInt64 },
	{ "JSONArray.GetFloat", api::ArrayGetValueFloat },
	{ "JSONArray.GetString", api::ArrayGetValueString },
	{ "JSONArray.IsNull", api::ArrayIsValueNull },
	{ "JSONArray.SetJSON", api::ArraySetValueJSON },
	{ "JSONArray.SetBool", api::ArraySetValueBool },
	{ "JSONArray.SetInt", api::ArraySetValueInt },
	{ "JSONArray.SetInt64", api::ArraySetValueInt64 },
	{ "JSONArray.SetFloat", api::ArraySetValueFloat },
	{ "JSONArray.SetString", api::ArraySetValueString },
	{ "JSONArray.PushJSON", api::ArrayPushValueJSON },
	{ "JSONArray.PushBool", api::ArrayPushValueBool },
	{ "JSONArray.PushInt", api::ArrayPushValueInt },
	{ "JSONArray.PushInt64", api::ArrayPushValueInt64 },
	{ "JSONArray.PushFloat", api::ArrayPushValueFloat },
	{ "JSONArray.PushString", api::ArrayPushValueString },
	{ "JSONArray.PushNull", api::ArrayPushValueNull },
	{ "JSONArray.Remove", api::ArrayRemoveIndex },

	{nullptr, nullptr}
};

cell_t json::api::ToFile(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec{ pContext->GetIdentity(), myself->GetIdentity() };
	IBaseJSON* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}

	int indent = params[2];
	
	std::string path_no_sm = {};
	extsdk::interop::cell2native(pContext, params[3], path_no_sm);
	std::ostringstream oss;
	oss << raw->Dump(indent);
	
	char file_path[PLATFORM_MAX_PATH] = {};
	smutils->BuildPath(Path_SM, file_path, sizeof(file_path), "%s", path_no_sm.c_str());
	std::ofstream ofs(file_path);
	ofs.write(oss.str().c_str(), oss.str().size());
	ofs.flush();

	return 0;
}

cell_t json::api::ToString(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec{ pContext->GetIdentity(), myself->GetIdentity() };
	IBaseJSON* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}

	int indent = params[2];

	extsdk::interop::native2cell(pContext, raw->Dump(indent), params[3], params[4]);

	return 0;
}

cell_t json::api::Clear(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec{ pContext->GetIdentity(), myself->GetIdentity() };
	IBaseJSON* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	raw->Clear();
	return 0;
}

cell_t json::api::Size(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec{ pContext->GetIdentity(), myself->GetIdentity() };
	IBaseJSON* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return 0;
	}
	return raw->Size();
}


cell_t json::api::ObjectCreate(IPluginContext* pContext, const cell_t* params)
{
	IJSONObject* obj = new JSONObject();
	HandleError ec = HandleError_None;
	auto hndl = handlesys->CreateHandle(hndl::hJSON, obj, pContext->GetIdentity(), myself->GetIdentity(), &ec);
	if (ec != HandleError_None)
	{
		pContext->ReportError("Error occured when creating handle. Error code: #%d", ec);
		return 0;
	}
	return hndl;
}

cell_t json::api::ObjectImportFromFile(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}

	std::string file_path;
	extsdk::interop::cell2native(pContext, params[2], file_path);

	char full_path[PLATFORM_MAX_PATH] = {};
	smutils->BuildPath(Path_SM, full_path, sizeof(full_path), "%s", file_path.c_str());
	raw->ImportFromFile(full_path);

	return 0;
}

cell_t json::api::ObjectImportFromString(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}

	std::string data;
	extsdk::interop::cell2native(pContext, params[2], data);
	raw->ImportFromString(data.c_str());

	return 0;
}

cell_t json::api::ObjectGetValueObject(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	std::string key;
	extsdk::interop::cell2native(pContext, params[2], key);
	auto obj = raw->GetObject(key.c_str());
	if (!obj)
	{
		return BAD_HANDLE;
	}
	Handle_t ret = handlesys->CreateHandle(hndl::hJSON, obj, nullptr, myself->GetIdentity(), &ec);
	if (ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	return ret;
}

cell_t json::api::ObjectGetValueArray(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	std::string key;
	extsdk::interop::cell2native(pContext, params[2], key);
	auto obj = raw->GetArray(key.c_str());
	if (!obj)
	{
		return BAD_HANDLE;
	}
	Handle_t ret = handlesys->CreateHandle(hndl::hJSON, obj, nullptr, myself->GetIdentity(), &ec);
	if (ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	return ret;
}

cell_t json::api::ObjectGetValueBool(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	std::string key;
	extsdk::interop::cell2native(pContext, params[2], key);
	return raw->GetBool(key.c_str());
}

cell_t json::api::ObjectGetValueInt(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	std::string key;
	extsdk::interop::cell2native(pContext, params[2], key);
	return raw->GetInt32(key.c_str());
}

cell_t json::api::ObjectGetValueInt64(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	std::string key;
	extsdk::interop::cell2native(pContext, params[2], key);
	extsdk::interop::native2cell(pContext, std::to_string(raw->GetInt64(key.c_str())), params[3], params[4]);
	return 0;
}

cell_t json::api::ObjectGetValueFloat(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	std::string key;
	extsdk::interop::cell2native(pContext, params[2], key);
	return sp_ftoc(raw->GetFloat32(key.c_str()));
}

cell_t json::api::ObjectGetValueString(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	std::string key;
	extsdk::interop::cell2native(pContext, params[2], key);
	extsdk::interop::native2cell(pContext, raw->GetString(key.c_str()), params[3], params[4]);
	return 0;
}

cell_t json::api::ObjectIsValueNull(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	std::string key;
	extsdk::interop::cell2native(pContext, params[2], key);
	return raw->IsNull(key.c_str());
}

cell_t json::api::ObjectHasKey(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	std::string key;
	extsdk::interop::cell2native(pContext, params[2], key);
	return raw->HasKey(key.c_str());
}

cell_t json::api::ObjectSetValueJSON(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	std::string key;
	extsdk::interop::cell2native(pContext, params[2], key);

	IBaseJSON* pValue = nullptr;
	Handle_t hValue = static_cast<Handle_t>(params[3]);
	if (ec = handlesys->ReadHandle(hValue, hndl::hJSON, &sec, (void**)&pValue); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading JSON value. Error code: {}", ec).c_str());
		return -1;
	}
	raw->SetJSON(key.c_str(), pValue->Dump());
	return 0;
}

cell_t json::api::ObjectSetValueBool(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	std::string key;
	extsdk::interop::cell2native(pContext, params[2], key);

	raw->SetBool(key.c_str(), params[3]);
	return 0;
}

cell_t json::api::ObjectSetValueInt(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	std::string key;
	extsdk::interop::cell2native(pContext, params[2], key);

	raw->SetInt32(key.c_str(), params[3]);
	return 0;
}

cell_t json::api::ObjectSetValueInt64(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	std::string key;
	extsdk::interop::cell2native(pContext, params[2], key);
	std::string value;
	extsdk::interop::cell2native(pContext, params[3], value);
	raw->SetInt64(key.c_str(), std::strtoll(value.c_str(), nullptr, 10));
	return 0;
}

cell_t json::api::ObjectSetValueFloat(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	std::string key;
	extsdk::interop::cell2native(pContext, params[2], key);

	float value;
	extsdk::interop::cell2native(pContext, params[3], value);

	raw->SetFloat32(key.c_str(), value);

	return 0;
}

cell_t json::api::ObjectSetValueString(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	std::string key, value;
	extsdk::interop::cell2native(pContext, params[2], key);
	extsdk::interop::cell2native(pContext, params[3], value);
	raw->SetString(key.c_str(), value.c_str());

	return 0;
}

cell_t json::api::ObjectSetValueNull(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	std::string key;
	extsdk::interop::cell2native(pContext, params[2], key);
	raw->SetNull(key.c_str());
	return 0;
}

cell_t json::api::ObjectRemoveKey(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	std::string key;
	extsdk::interop::cell2native(pContext, params[2], key);

	return raw->Remove(key.c_str());
}

cell_t json::api::ObjectCreateIterator(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObject* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	IJSONObjectIterator* proxy = new JSONObjectIterator((JSONObject*)std::ref(raw).get());
	Handle_t ret = handlesys->CreateHandle(hndl::hJSONObjectIterator, proxy, pContext->GetIdentity(), myself->GetIdentity(), &ec);
	if (ec != HandleError_None)
	{
		delete proxy;
		pContext->ReportError("%s", fmt::format("Error occured when creating handle. Error code: #{}", ec).c_str());
		return BAD_HANDLE;
	}
	return ret;
}

cell_t json::api::ObjectIteratorKey(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec{ pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObjectIterator* it = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSONObjectIterator, &sec, (void**)&it); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Invalid JSON object iterator handle {:#x} (Error code: {})", hndl, ec).c_str());
		return -1;
	}
	extsdk::interop::native2cell(pContext, it->Key(), params[2], params[3]);

	return 0;
}

cell_t json::api::ObjectIteratorMore(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec{ pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObjectIterator* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSONObjectIterator, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Invalid JSON object iterator handle {:#x} (Error code: {})", hndl, ec).c_str());
		return false;
	}
	return raw->More();
}

cell_t json::api::ObjectIteratorNext(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = HandleError_None;
	HandleSecurity sec{ pContext->GetIdentity(), myself->GetIdentity() };
	IJSONObjectIterator* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSONObjectIterator, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Invalid JSON object iterator handle {:#x} (Error code: {})", hndl, ec).c_str());
		return -1;
	}
	raw->Next();
	return 0;
}


cell_t json::api::ArrayCreate(IPluginContext* pContext, const cell_t* params)
{
	IJSONArray* obj = new JSONArray();
	HandleError ec = HandleError_None;
	auto hndl = handlesys->CreateHandle(hndl::hJSON, obj, pContext->GetIdentity(), myself->GetIdentity(), &ec);
	if (ec != HandleError_None)
	{
		pContext->ReportError("Error occured when creating handle. Error code: #%d", ec);
		return 0;
	}
	return hndl;
}

cell_t json::api::ArrayImportFromString(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}

	std::string data;
	extsdk::interop::cell2native(pContext, params[2], data);
	raw->ImportFromString(data.c_str());

	return 0;
}

cell_t json::api::ArrayImportFromFile(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}

	std::string file_path;
	extsdk::interop::cell2native(pContext, params[2], file_path);

	char full_path[PLATFORM_MAX_PATH] = {};
	smutils->BuildPath(Path_SM, full_path, sizeof(full_path), "%s", file_path.c_str());
	raw->ImportFromFile(full_path);

	return 0;
}

cell_t json::api::ArrayGetValueObject(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	cell_t idx;
	extsdk::interop::cell2native(pContext, params[2], idx);
	auto obj = raw->GetObject(idx);
	if (!obj)
	{
		return BAD_HANDLE;
	}
	Handle_t ret = handlesys->CreateHandle(hndl::hJSON, obj, nullptr, myself->GetIdentity(), &ec);
	if (ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	return ret;
}

cell_t json::api::ArrayGetValueArray(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	cell_t idx;
	extsdk::interop::cell2native(pContext, params[2], idx);
	auto obj = raw->GetArray(idx);
	if (!obj)
	{
		return BAD_HANDLE;
	}
	Handle_t ret = handlesys->CreateHandle(hndl::hJSON, obj, nullptr, myself->GetIdentity(), &ec);
	if (ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	return ret;
}

cell_t json::api::ArrayGetValueBool(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	cell_t idx;
	extsdk::interop::cell2native(pContext, params[2], idx);
	return raw->GetBool(idx);
}

cell_t json::api::ArrayGetValueInt(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	cell_t idx;
	extsdk::interop::cell2native(pContext, params[2], idx);
	return raw->GetInt32(idx);
}

cell_t json::api::ArrayGetValueInt64(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	cell_t idx;
	extsdk::interop::cell2native(pContext, params[2], idx);
	extsdk::interop::native2cell(pContext, std::to_string(raw->GetInt64(idx)), params[3], params[4]);
	return 0;
}

cell_t json::api::ArrayGetValueFloat(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	cell_t idx;
	extsdk::interop::cell2native(pContext, params[2], idx);
	return sp_ftoc(raw->GetFloat32(idx));
}

cell_t json::api::ArrayGetValueString(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	cell_t idx;
	extsdk::interop::cell2native(pContext, params[2], idx);
	extsdk::interop::native2cell(pContext, raw->GetString(idx), params[3], params[4]);
	return 0;
}

cell_t json::api::ArrayIsValueNull(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return BAD_HANDLE;
	}
	cell_t idx;
	extsdk::interop::cell2native(pContext, params[2], idx);
	return raw->IsNull(idx);
}

cell_t json::api::ArraySetValueJSON(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	cell_t idx;
	extsdk::interop::cell2native(pContext, params[2], idx);

	IBaseJSON* pValue = nullptr;
	Handle_t hValue = static_cast<Handle_t>(params[3]);
	if (ec = handlesys->ReadHandle(hValue, hndl::hJSON, &sec, (void**)&pValue); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading JSON value. Error code: {}", ec).c_str());
		return -1;
	}
	raw->SetJSON(idx, pValue->Dump());
	return 0;
}

cell_t json::api::ArraySetValueBool(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	cell_t idx;
	extsdk::interop::cell2native(pContext, params[2], idx);

	bool value;
	extsdk::interop::cell2native(pContext, params[3], value);
	raw->SetBool(idx, value);
	return 0;
}

cell_t json::api::ArraySetValueInt(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	cell_t idx;
	extsdk::interop::cell2native(pContext, params[2], idx);

	int value;
	extsdk::interop::cell2native(pContext, params[3], value);
	raw->SetInt32(idx, value);
	return 0;
}

cell_t json::api::ArraySetValueInt64(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	cell_t idx;
	extsdk::interop::cell2native(pContext, params[2], idx);

	std::string value;
	extsdk::interop::cell2native(pContext, params[3], value);
	raw->SetInt64(idx, std::strtoll(value.c_str(), nullptr, 10));
	return 0;
}

cell_t json::api::ArraySetValueFloat(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	cell_t idx;
	extsdk::interop::cell2native(pContext, params[2], idx);

	float value;
	extsdk::interop::cell2native(pContext, params[3], value);
	raw->SetFloat32(idx, value);
	return 0;
}

cell_t json::api::ArraySetValueString(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	cell_t idx;
	extsdk::interop::cell2native(pContext, params[2], idx);

	std::string value;
	extsdk::interop::cell2native(pContext, params[3], value);
	raw->SetString(idx, value.c_str());
	return 0;
}

cell_t json::api::ArraySetValueNull(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	cell_t idx;
	extsdk::interop::cell2native(pContext, params[2], idx);
	raw->SetNull(idx);
	return 0;
}

cell_t json::api::ArrayPushValueJSON(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	IBaseJSON* pValue = nullptr;
	Handle_t hValue = static_cast<Handle_t>(params[2]);
	if (ec = handlesys->ReadHandle(hValue, hndl::hJSON, &sec, (void**)&pValue); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading JSON value. Error code: {}", ec).c_str());
		return -1;
	}
	raw->PushJSON(pValue->Dump());
	return 0;
}

cell_t json::api::ArrayPushValueBool(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	bool value;
	extsdk::interop::cell2native(pContext, params[2], value);
	raw->PushBool(value);
	return 0;
}

cell_t json::api::ArrayPushValueInt(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	int value;
	extsdk::interop::cell2native(pContext, params[2], value);
	raw->PushInt32(value);
	return 0;
}

cell_t json::api::ArrayPushValueInt64(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	std::string value;
	extsdk::interop::cell2native(pContext, params[2], value);
	raw->PushInt64(std::strtoll(value.c_str(), nullptr, 10));
	return 0;
}

cell_t json::api::ArrayPushValueFloat(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	float value;
	extsdk::interop::cell2native(pContext, params[2], value);
	raw->PushFloat32(value);
	return 0;
}

cell_t json::api::ArrayPushValueString(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	std::string value;
	extsdk::interop::cell2native(pContext, params[2], value);
	raw->PushString(value.c_str());
	return 0;
}

cell_t json::api::ArrayPushValueNull(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	raw->PushNull();
	return 0;
}

cell_t json::api::ArrayRemoveIndex(IPluginContext* pContext, const cell_t* params)
{
	Handle_t hndl = static_cast<Handle_t>(params[1]);
	HandleError ec = {};
	HandleSecurity sec = { pContext->GetIdentity(), myself->GetIdentity() };
	IJSONArray* raw = nullptr;
	if (ec = handlesys->ReadHandle(hndl, hndl::hJSON, &sec, (void**)&raw); ec != HandleError_None)
	{
		pContext->ReportError("%s", fmt::format("Error occured when reading handle. Error code: {}", ec).c_str());
		return -1;
	}
	cell_t idx;
	extsdk::interop::cell2native(pContext, params[2], idx);
	raw->Remove(idx);
	return 0;
}
