#if defined _json_included
#endinput
#endif

#define _json_included

methodmap JSON < Handle {
    public native void ToFile(int indent = -1, const char[] path);
    public native void ToString(int indent = -1, char[] out, int maxlen);

    public native void Clear();

    property int Size {
        public native get();
    }
}

methodmap JSONObject < JSON {
    public native                       JSONObject();

    public native void                  ImportFromFile(const char[] path);
    public native void                  ImportFromString(const char[] str);

    public native JSONObject            GetObject(const char[] key);
    public native JSONArray             GetArray(const char[] key);
    public native bool                  GetBool(const char[] key);
    public native int                   GetInt(const char[] key);
    public native void                  GetInt64(const char[] key, char[] value, int maxlen);
    public native float                 GetFloat(const char[] key);
    public native void                  GetString(const char[] key, char[] value, int maxlen);
    
    public native bool                  IsNull(const char[] key);
    public native bool                  HasKey(const char[] key);

    public native void                  SetJSON(const char[] key, JSON value);
    public native void                  SetBool(const char[] key, bool value);
    public native void                  SetInt(const char[] key, int value);
    public native void                  SetInt64(const char[] key, const char[] value);
    public native void                  SetFloat(const char[] key, float value);
    public native void                  SetString(const char[] key, const char[] value);
    public native void                  SetNull(const char[] key);
    
    public native bool                  Remove(const char[] key);

    public native JSONObjectIterator    CreateIterator();
}

methodmap JSONObjectIterator < Handle {
    public native void Key(char[] key, int maxlen);
    public native bool More();
    public native void Next();
}

methodmap JSONArray < JSON {
    public native                       JSONArray();

    public native void                  ImportFromFile(const char[] path);
    public native void                  ImportFromString(const char[] str);
    
    public native JSONObject            GetObject(int index);
    public native JSONArray             GetArray(int index);
    public native bool                  GetBool(int index);
    public native int                   GetInt(int index);
    public native void                  GetInt64(int index, char[] value, int maxlen);
    public native float                 GetFloat(int index);
    public native void                  GetString(int index, char[] value, int maxlen);

    public native bool                  IsNull(int index);
    
    public native void                  SetJSON(int index, JSON value);
    public native void                  SetBool(int index, bool value);
    public native void                  SetInt(int index, int value);
    public native void                  SetInt64(int index, const char[] value);
    public native void                  SetFloat(int index, float value);
    public native void                  SetString(int index, const char[] value);
    public native void                  SetNull(int index);
    
    public native void                  PushJSON(JSON value);
    public native void                  PushBool(bool value);
    public native void                  PushInt(int value);
    public native void                  PushInt64(const char[] value);
    public native void                  PushFloat(float value);
    public native void                  PushString(const char[] value);
    public native void                  PushNull();
    
    public native bool                  Remove(int index);
}

public Extension __ext_json =
{
	name = "JSON",
	file = "json.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};
