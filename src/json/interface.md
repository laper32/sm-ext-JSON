> ent_cast.hpp
```cpp
#ifndef _EXTSDK_ENT_CAST_HPP
#define _EXTSDK_ENT_CAST_HPP

#pragma once

#ifndef META_NO_HL2SDK

#include <utility>
#include <tuple>
#include <concepts>
#include <iserverunknown.h>
#include <iservernetworkable.h>
#include <IPlayerHelpers.h>

namespace extsdk { inline namespace convert {

// forward decl
template<class From, class To> struct ConvertFunc;
template<class T> concept EntType_c = requires (const T & ent) { ConvertFunc<std::decay_t<T>, CBaseEntity*>()(ent); };
template<EntType_c To> struct Converter;
template<EntType_c TargetType> struct AutoEntity;
template<EntType_c TargetType> constexpr auto ent_cast = [](const auto& x) { return Converter<std::decay_t<TargetType>>()(x); };
template<EntType_c T> constexpr auto make_auto_ent = [](const auto& x) { return AutoEntity<T>(x); };

// impl
namespace detail {
template<class From, class...Dest> struct AdapterFunc;
template<class From> struct AdapterFunc<From> : std::identity {};
template<class From, class To> struct AdapterFunc<From, To> : ConvertFunc<From, To> {};
template<class From, class Mid, class...To> struct AdapterFunc<From, Mid, To...>
{
    auto operator()(From in) const
    {
        return AdapterFunc<Mid, To...>()(AdapterFunc<From, Mid>()(in));
    };
};

template<class From, class To, class = void> struct ConvertFuncSingle;
template<class T> struct ConvertFuncSingle<T, T> : std::identity {};
template<> struct ConvertFuncSingle<int, CBaseEntity*> { CBaseEntity* operator()(int id) const { return gamehelpers->ReferenceToEntity(gamehelpers->IndexToReference(id)); } };
template<> struct ConvertFuncSingle<CBaseHandle, CBaseEntity*> { CBaseEntity* operator()(const CBaseHandle& handle) const { return gamehelpers->ReferenceToEntity(handle.ToInt() | (1 << 31));; } };
template<> struct ConvertFuncSingle<IServerUnknown*, CBaseEntity*> { CBaseEntity* operator()(IServerUnknown* unknown) const { return unknown ? unknown->GetBaseEntity() : nullptr; } };
template<> struct ConvertFuncSingle<int, edict_t*> { edict_t* operator()(int id) const { return gamehelpers->EdictOfIndex(id); } };
template<> struct ConvertFuncSingle<IGamePlayer*, edict_t*> { edict_t* operator()(IGamePlayer* igp) const { return igp->GetEdict(); } };
template<> struct ConvertFuncSingle<IServerUnknown*, edict_t*> { edict_t* operator()(IServerUnknown* unknown) const { return unknown ? unknown->GetNetworkable()->GetEdict() : nullptr; } };
template<> struct ConvertFuncSingle<edict_t*, int> { int operator()(edict_t* edict) const { return gamehelpers->IndexOfEdict(edict); } };
template<> struct ConvertFuncSingle<CBaseEntity*, int> { int operator()(CBaseEntity* pEntity) const { return gamehelpers->ReferenceToIndex(gamehelpers->EntityToReference(pEntity)); } };
template<> struct ConvertFuncSingle<IGamePlayer*, int> { int operator()(IGamePlayer* igp) const { return igp->GetIndex(); } };
template<> struct ConvertFuncSingle<CBaseHandle, int> { int operator()(const CBaseHandle& handle) const { return gamehelpers->ReferenceToIndex(handle.ToInt() | (1 << 31)); } };
template<> struct ConvertFuncSingle<edict_t*, IGamePlayer*> { IGamePlayer* operator()(edict_t* edict) const { return playerhelpers->GetGamePlayer(edict); } };
template<> struct ConvertFuncSingle<int, IGamePlayer*> { IGamePlayer* operator()(int id) const { return playerhelpers->GetGamePlayer(id); } };
template<> struct ConvertFuncSingle<edict_t*, IServerUnknown*> { IServerUnknown* operator()(edict_t* edict) const { return edict ? edict->GetUnknown() : nullptr; } };
template<> struct ConvertFuncSingle<CBaseEntity*, IServerUnknown*> { IServerUnknown* operator()(CBaseEntity* pEntity) const { return reinterpret_cast<IServerUnknown*>(pEntity); } };
template<> struct ConvertFuncSingle<int, CBaseHandle> { CBaseHandle operator()(int id) const { return CBaseHandle(gamehelpers->IndexToReference(id) & ~(1 << 31)); } };
template<> struct ConvertFuncSingle<IServerUnknown*, CBaseHandle> { CBaseHandle operator()(IServerUnknown* unknown) const { return unknown ? unknown->GetRefEHandle() : CBaseHandle(); } };

// use adapters
template<> struct ConvertFuncSingle<edict_t*, CBaseEntity*> : AdapterFunc<edict_t*, IServerUnknown*, CBaseEntity*> {};
template<> struct ConvertFuncSingle<IGamePlayer*, CBaseEntity*> : AdapterFunc<IGamePlayer*, edict_t*, CBaseEntity*> {};
template<> struct ConvertFuncSingle<CBaseEntity*, edict_t*> : AdapterFunc<CBaseEntity*, IServerUnknown*, edict_t*> {};
template<> struct ConvertFuncSingle<CBaseHandle, edict_t*> : AdapterFunc<CBaseHandle, CBaseEntity*, edict_t*> {};
template<> struct ConvertFuncSingle<IServerUnknown*, int> : AdapterFunc<IServerUnknown*, edict_t*, int> {};
template<> struct ConvertFuncSingle<CBaseEntity*, IGamePlayer*> : AdapterFunc<CBaseEntity*, IServerUnknown*, edict_t*, IGamePlayer*> {};
template<> struct ConvertFuncSingle<CBaseHandle, IGamePlayer*> : AdapterFunc<CBaseHandle, int, IGamePlayer*> {};
template<> struct ConvertFuncSingle<IServerUnknown*, IGamePlayer*> : AdapterFunc<IServerUnknown*, edict_t*, IGamePlayer*> {};
template<> struct ConvertFuncSingle<int, IServerUnknown*> : AdapterFunc<int, CBaseEntity*, IServerUnknown*> {};
template<> struct ConvertFuncSingle<IGamePlayer*, IServerUnknown*> : AdapterFunc<IGamePlayer*, CBaseEntity*, IServerUnknown*> {};
template<> struct ConvertFuncSingle<CBaseHandle, IServerUnknown*> : AdapterFunc<CBaseHandle, CBaseEntity*, IServerUnknown*> {};
template<> struct ConvertFuncSingle<edict_t*, CBaseHandle> : AdapterFunc<edict_t*, IServerUnknown*, CBaseHandle> {};
template<> struct ConvertFuncSingle<CBaseEntity*, CBaseHandle> : AdapterFunc<CBaseEntity*, IServerUnknown*, CBaseHandle> {};
template<> struct ConvertFuncSingle<IGamePlayer*, CBaseHandle> : AdapterFunc<IGamePlayer*, int, CBaseHandle> {};

// covariant
template<class CBaseEntityDerived>
struct ConvertFuncSingle<CBaseEntityDerived*, CBaseEntity*, std::enable_if_t<std::is_base_of_v<CBaseEntity, CBaseEntityDerived> && !std::is_same_v<CBaseEntity, CBaseEntityDerived>>>
{
    CBaseEntity* operator()(CBaseEntityDerived* in) const {
        return static_cast<CBaseEntity*>(in);
    }
};
template<class CBaseEntityDerived> struct ConvertFuncSingle<CBaseEntity*, CBaseEntityDerived*, std::enable_if_t<std::is_base_of_v<CBaseEntity, CBaseEntityDerived> && !std::is_same_v<CBaseEntity, CBaseEntityDerived>>>
{
    CBaseEntityDerived* operator()(CBaseEntity* in) const {
        return static_cast<CBaseEntityDerived*>(in); // down-cast!!! no checked.
    }
};
template<class From, class CBaseEntityDerived> struct ConvertFuncSingle<From, CBaseEntityDerived*, std::enable_if_t<std::is_base_of_v<CBaseEntity, CBaseEntityDerived> && !std::is_same_v<CBaseEntity, CBaseEntityDerived> && !std::is_same_v<CBaseEntity*, From>>> : AdapterFunc<From, CBaseEntity*, CBaseEntityDerived*> {};
template<class CBaseEntityDerived, class To> struct ConvertFuncSingle<CBaseEntityDerived*, To, std::enable_if_t<std::is_base_of_v<CBaseEntity, CBaseEntityDerived> && !std::is_same_v<CBaseEntity, CBaseEntityDerived> && !std::is_same_v<CBaseEntity*, To>>> : AdapterFunc<CBaseEntityDerived*, CBaseEntity*, To> {};

// auto type
template<class From, class To> struct ConvertFuncSingle<AutoEntity<From>, To> : ConvertFuncSingle<From, To> {};
template<class From, class To> struct ConvertFuncSingle<From, AutoEntity<To>> : ConvertFuncSingle<From, To> {};
template<class From, class To> struct ConvertFuncSingle<AutoEntity<From>, AutoEntity<To>> : ConvertFuncSingle<From, To> {};

template<class FromList, class To> struct ConverterGenerator;
template<class To, template<class...> class TypeList, class...Froms> struct ConverterGenerator<TypeList<Froms...>, To> : ConvertFunc<Froms, To>... {};

} // namespace detail

template<class From, class To> struct ConvertFunc : detail::ConvertFuncSingle<From, To> {};
template<EntType_c To> struct Converter
{
    template<class InputType> To operator()(InputType&& in) const {
        return ConvertFunc<typename std::decay<InputType>::type, To>()(std::forward<InputType>(in));
    }
};

#if SMCPP_STATIC_UNIT_TEST
using AllEntTypeList = std::tuple<edict_t*, CBaseEntity*, int, IGamePlayer*, CBaseHandle, IServerUnknown*>;
template<class From, class To> struct ConverterUnitTest3 : std::is_invocable_r<To, ConvertFunc<From, To>, From> {};
template<class FromList, class To> struct ConverterUnitTest2;
template<class To, template<class...> class TypeList, class...Froms> struct ConverterUnitTest2<TypeList<Froms...>, To> : std::conjunction<ConverterUnitTest3<Froms, To>...> {};
template<class EntTypeList> struct ConverterUnitTest1;
template<template<class...> class TypeList, class...T> struct ConverterUnitTest1<TypeList<T...>> : std::conjunction<ConverterUnitTest2<TypeList<T...>, T>...> {};
static_assert(ConverterUnitTest1<AllEntTypeList>::value);
#endif

template<EntType_c TargetType> struct AutoEntity
{
    template<class InputType> AutoEntity(InputType&& in) : value(Converter<TargetType>()(std::forward<InputType>(in))) {}
    operator TargetType() const { return value; }
    template<class Ret = TargetType> decltype(&*std::declval<Ret>()) operator->() { return &*value; }
    TargetType value;
};
template<class TargetType> struct AutoEntity<AutoEntity<TargetType>> : AutoEntity<TargetType> {};
}} // namespace extsdk::convert

#endif // META_NO_HL2SDK

#endif // !_EXTSDK_ENT_CAST_HPP
```

> function.hpp
```cpp
#ifndef _EXTSDK_FUNCTION_HPP
#define _EXTSDK_FUNCTION_HPP

#pragma once

namespace extsdk { inline namespace function {

template<typename F, typename ...Args>
inline void WrapFunction(F&& fn, Args&&...args) {
    auto lambda = std::bind(std::forward<F>(fn), std::forward<Args>(args)...);
    using lambda_type = decltype(lambda);
    FRAMEACTION wrap_fn = +[](void* data) {
        lambda_type* lambda_ptr = reinterpret_cast<lambda_type*>(data);
        (*lambda_ptr)();
        delete lambda_ptr;
    };
    void* wrap_lambda = new lambda_type(std::move(lambda));
    g_pSM->AddFrameAction(wrap_fn, wrap_lambda);
}

}} // namespace extsdk::function

#endif // !_EXTSDK_FUNCTION_HPP
```

> interop.hpp
```cpp
#ifndef _EXTSDK_INTEROP_H
#define _EXTSDK_INTEROP_H

#include <algorithm>
#include <vector>
#include <string>
#include <functional>
#include <array>
#include <iterator>

#include <sp_vm_api.h>
#ifndef META_NO_HL2SDK
#include <Color.h>
#include <vector.h>
#endif // META_NO_HL2SDK

namespace extsdk::interop {
    // decl
    #if defined SMEXT_ENABLE_FORWARDSYS
    template<class Fn, class PtrType = IForward*> class ForwardCaller;
    #endif // SMEXT_ENABLE_FORWARDSYS
    template<class Fn, class PtrType = IPluginFunction*> class PluginFunctionCaller;

    // bool => bool
    // char => char
    // int => int / cell_t
    // float => float
    // Handle => Handle_t
    // string => std::string
    // array => std::vector / std::array
    // Function => PluginFunctionCaller / std::function
    template<class T>
    inline auto cell2native(IPluginContext* pContext, cell_t in, T& out)
        -> std::enable_if_t<std::is_integral_v<T> || std::is_enum_v<T>, int>
    {
        out = static_cast<T>(in);
        return 1;
    }
    template<class T>
    inline auto native2cell(IPluginContext* pContext, const T& in, cell_t* out)
        -> std::enable_if_t<std::is_integral_v<T> || std::is_enum_v<T>, int>
    {
        if (out)
            *out = static_cast<cell_t>(in);
        return 1;
    }

    template<class T>
    inline auto cell2native(IPluginContext* pContext, cell_t in, T& out)
        -> std::enable_if_t<std::is_floating_point_v<T>, int>
    {
        out = sp_ctof(in);
        return 1;
    }
    template<class T>
    inline auto native2cell(IPluginContext* pContext, const T& in, cell_t* out)
        -> std::enable_if_t<std::is_floating_point_v<T>, int>
    {
        if (out)
            *out = sp_ftoc(in);
        return 1;
    }

    inline int cell2native(IPluginContext* pContext, cell_t in, std::string& out)
    {
        char* str; pContext->LocalToString(in, &str);
        assert(str != nullptr);
        out = str;
        return 1;
    }
    inline int native2cell(IPluginContext* pContext, const std::string& in, cell_t out, cell_t maxbyte)
    {
        return pContext->StringToLocalUTF8(out, maxbyte, in.c_str(), nullptr);
    }

    #ifndef META_NO_HL2SDK
    inline int cell2native(IPluginContext* pContext, cell_t in, Color& out)
    {
        cell_t* vecParams;
        pContext->LocalToPhysAddr(in, &vecParams);
        out = Color(vecParams[0], vecParams[1], vecParams[2]);
        return 1;
    }
    inline int native2cell(IPluginContext* pContext, const Color& in, cell_t out)
    {
        cell_t* addr;
        pContext->LocalToPhysAddr(out, &addr);
        addr[0] = in.r();
        addr[1] = in.g();
        addr[2] = in.b();
        return 1;
    }
    inline int cell2native(IPluginContext* pContext, cell_t in, Vector& out)
    {
        cell_t* vecParams;
        pContext->LocalToPhysAddr(in, &vecParams);
        out = Vector(sp_ctof(vecParams[0]), sp_ctof(vecParams[1]), sp_ctof(vecParams[2]));
        return 1;
    }

    inline int native2cell(IPluginContext* pContext, const Vector& in, cell_t out)
    {
        cell_t* addr;
        pContext->LocalToPhysAddr(out, &addr);
        addr[0] = sp_ftoc(in.x);
        addr[1] = sp_ftoc(in.y);
        addr[2] = sp_ftoc(in.z);
        return 1;
    }

    inline int cell2native(IPluginContext* pContext, cell_t in, QAngle& out)
    {
        cell_t* angParams;
        pContext->LocalToPhysAddr(in, &angParams);
        out = QAngle(sp_ctof(angParams[0]), sp_ctof(angParams[1]), sp_ctof(angParams[2]));
        return 1;
    }

    inline int native2cell(IPluginContext* pContext, const QAngle& in, cell_t out)
    {
        cell_t* addr;
        pContext->LocalToPhysAddr(out, &addr);
        addr[0] = sp_ftoc(in.x);
        addr[1] = sp_ftoc(in.y);
        addr[2] = sp_ftoc(in.z);
        return 1;
    }
    #endif // META_NO_HL2SDK
    template<template<class> class FnType, class Ret, class...Args>
    inline auto cell2native(IPluginContext* pContext, cell_t in, FnType<Ret(Args...)>& out)
        -> std::enable_if_t<
        std::is_invocable_r_v<Ret, FnType<Ret(Args...)>, Args...> &&
        std::is_assignable_v<FnType<Ret(Args...)>, PluginFunctionCaller<Ret(Args...)>>,
        int>
    {
        IPluginFunction* pFunction = pContext->GetFunctionById(in);
        if (!pFunction)
        {
            return pContext->ThrowNativeError("Invalid function id (%X)", in);
        }
        out = PluginFunctionCaller<Ret(Args...)>(pFunction);
        return 1;
    }

    inline cell_t n2c(bool in) { return !!in; }
    inline cell_t n2c(cell_t in) { return in; }
    inline cell_t n2c(float in) { return sp_ftoc(in); }

    inline void func_push(ICallable* c, const cell_t& x) { c->PushCell(x); }
    inline void func_push(ICallable* c, cell_t& x) { c->PushCellByRef(&x); }
    inline void func_push(ICallable* c, const float& x) { c->PushFloat(x); }
    inline void func_push(ICallable* c, float& x) { c->PushFloatByRef(&x); }
    inline void func_push(ICallable* c, std::vector<cell_t>& in) { c->PushArray(in.data(), in.size(), SM_PARAM_COPYBACK); }
    template<std::size_t N> void func_push(ICallable* c, std::array<cell_t, N>& in) {
        c->PushArray(in.data(), in.size(), SM_PARAM_COPYBACK);
    }
    template<std::size_t N> void func_push(ICallable* c, cell_t(&in)[N]) { c->PushArray(in, N, SM_PARAM_COPYBACK); }
    template<class ArrayType> auto func_push(ICallable* c, const ArrayType& arr)
        -> decltype(std::begin(arr), std::end(arr), void())
    {
        std::vector<cell_t> in;
        std::transform(std::begin(arr), std::end(arr), std::back_inserter(in), [](const auto& x) { return n2c(x); });
        c->PushArray(in.data(), in.size());
    }
    inline void func_push(ICallable* c, const std::string& str) { c->PushString(str.c_str()); }
    inline void func_push(ICallable* c, std::string& str) {
        c->PushStringEx(str.data(), str.size(), SM_PARAM_STRING_UTF8, SM_PARAM_COPYBACK); 
    }
    inline void func_push(ICallable* c, const char* psz) { c->PushString(psz); }
    template<std::size_t N> void func_push(ICallable* c, char(&str)[N]) {
        c->PushStringEx(str, N, SM_PARAM_STRING_UTF8, SM_PARAM_COPYBACK);
    }

    #if defined SMEXT_ENABLE_FORWARDSYS
    template<class Fn, class PtrType> class ForwardCaller;
    template<class PtrType, class...Args> class ForwardCaller<cell_t(Args...), PtrType>
    {
        PtrType const m_pfn;
    public:
        ForwardCaller(PtrType pf) : m_pfn(std::move(pf)) {}
        cell_t operator()(Args ...args) const
        {
            cell_t result;
            (func_push(&*m_pfn, args), ..., m_pfn->Execute(&result));
            return result;
        }
        friend auto operator<=>(const ForwardCaller& a, const ForwardCaller& b)
        {
            return a.m_pfn->GetFunctionID() <=> b.m_pfn->GetFunctionID();
        }
    };
    template<class PtrType, class...Args> class ForwardCaller<void(Args...), PtrType>
    {
        PtrType const m_pfn;
    public:
        ForwardCaller(PtrType pf) : m_pfn(std::move(pf)) {}
        void operator()(const Args &...args) const
        {
            ForwardCaller<cell_t(Args...)>::operator()(args...);
        }
        friend auto operator<=>(const ForwardCaller& a, const ForwardCaller& b)
        {
            return a.m_pfn->GetFunctionID() <=> b.m_pfn->GetFunctionID();
        }
    };
    #endif // SMEXT_ENABLE_FORWARDSYS
    
    template<class Fn, class PtrType> class PluginFunctionCaller;
    template<class PtrType, class Ret, class...Args> class PluginFunctionCaller<Ret(Args...), PtrType>
    {
        PtrType const m_pfn;
    public:
        PluginFunctionCaller(PtrType pf) : m_pfn(std::move(pf)) {}
        Ret operator()(Args...args) const
        {
            cell_t result;
            (func_push(&*m_pfn, args), ..., m_pfn->Execute(&result));
            if constexpr (!std::is_void_v<Ret>)
            {
                Ret ret;
                cell2native(m_pfn->GetParentContext(), &result, ret);
                return ret;
            }
        }
        friend auto operator<=>(const PluginFunctionCaller& a, const PluginFunctionCaller& b)
        {
            return a.m_pfn->GetFunctionID() <=> b.m_pfn->GetFunctionID();
        }
    };

    template<class Fn, class PtrType> class NativeCaller;
    template<class Ret, class...Args> class NativeCaller<Ret(Args...), SPVM_NATIVE_FUNC>
    {
        IPluginContext* const m_pContext;
        SPVM_NATIVE_FUNC const m_pfn;

    public:
        NativeCaller(IPluginContext* pContext, SPVM_NATIVE_FUNC pf) : m_pContext(pContext), m_pfn(pf) {}

        Ret operator()(Args...args) const
        {
            int params_size = (native2cell(m_pContext, args, nullptr) + ...);

            std::vector<cell_t> params_vec(params_size + 1);

            params_vec[0] = sizeof...(Args);
            auto p = params_vec.data() + 1;
            int placeholder_args[] = { (p += native2cell(m_pContext, args, p)) ... };

            cell_t out = (*m_pfn)(m_pContext, params_vec.data());
            if constexpr (!std::is_void_v<Ret>)
            {
                Ret ret;
                cell2native(m_pContext, out, ret);
                return ret;
            }
        }
        friend auto operator<=>(const NativeCaller& a, const NativeCaller& b)
        {
            return std::tie(a.m_pfn, a.m_pContext) <=> std::tie(b.m_pfn, b.m_pContext);
        }
    };

    template<class...Args>
    void params2vars(IPluginContext* pContext, const cell_t* params, Args&...out)
    {
        int N = 1;
        ((N += cell2native(pContext, params[N], out)), ...);
    }

    template<class TupleType, std::size_t...I>
    void params2tuple_impl(IPluginContext* pContext, const cell_t* params, TupleType& ret, std::index_sequence<I...>)
    {
        params2vars(pContext, params, std::get<I>(ret)...);
    }

    template<class...Args>
    std::tuple<Args...> params2tuple(IPluginContext* pContext, const cell_t* params)
    {
        std::tuple<Args...> ret;
        params2tuple_impl(pContext, params, ret, std::index_sequence_for<Args...>());
        return ret;
    }
}

#endif // !_EXTSDK_INTEROP_H
```
