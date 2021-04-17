// Shared container for all property lists

#include <Lua/LuaBinding.h>
#include <Lua/LuaSerializers.h>
#include <Lua/Shared/LuaPropertyMapHelpers.h>
#include <GameDefinitions/Resources.h>

#include <Lua/Shared/LuaShared.inl>

namespace bg3se::lua
{

int CharacterGetInventoryItems(lua_State* L, esv::Character* self);
int CharacterGetNearbyCharacters(lua_State* L, esv::Character* self);
int CharacterGetSummons(lua_State* L, esv::Character* self);
int CharacterGetSkills(lua_State* L, esv::Character* self);
int CharacterGetSkillInfo(lua_State* L, esv::Character* self);

int ItemGetInventoryItems(lua_State* L, esv::Item* self);
int ItemGetNearbyCharacters(lua_State* L, esv::Item* self);
int ItemGetGeneratedBoosts(lua_State* L, esv::Item* self);

int StatusGetEngineType(lua_State* L, esv::Status* self);

// Lua property map and object proxy template specialization declarations

#define BEGIN_CLS(name) \
	char const* const ObjectProxyImpl<name>::TypeName = #name; \
	LuaPropertyMap<name> StaticLuaPropertyMap<name>::PropertyMap;

#define END_CLS()
#define INHERIT(base)
#define P(prop)
#define P_RO(prop)
#define P_REF(prop)
#define P_REF_PTR(prop)
#define PN(prop, name)
#define P_FUN(prop, fun)

#include <GameDefinitions/PropertyMaps/AllPropertyMaps.inl>

#undef BEGIN_CLS
#undef END_CLS
#undef INHERIT
#undef P
#undef P_RO
#undef P_REF
#undef P_REF_PTR
#undef PN
#undef P_FUN


// Property registrations

	void InitObjectProxyPropertyMaps(lua_State* L)
	{

#define BEGIN_CLS(cls) { \
	using PM = StaticLuaPropertyMap<cls>; \
	auto& pm = StaticLuaPropertyMap<cls>::PropertyMap;

#define END_CLS() }

#define INHERIT(base) { \
		auto& basePm = StaticLuaPropertyMap<base>::PropertyMap; \
		CopyProperties(basePm, pm); \
	}

#define P(prop) \
	pm.AddProperty(#prop, \
		[](lua_State* L, LifetimeHolder const& lifetime, PM::ObjectType* obj) { \
			return GenericGetProperty(L, lifetime, obj->prop); \
		}, \
		[](lua_State* L, LifetimeHolder const& lifetime, PM::ObjectType* obj, int index) { \
			return GenericSetProperty(L, lifetime, obj->prop, index); \
		} \
	);

#define P_RO(prop) \
	pm.AddProperty(#prop, \
		[](lua_State* L, LifetimeHolder const& lifetime, PM::ObjectType* obj) { \
			return GenericGetProperty(L, lifetime, obj->prop); \
		}, \
		[](lua_State* L, LifetimeHolder const& lifetime, PM::ObjectType* obj, int index) { \
			return false; \
		} \
	);

#define P_REF(prop) \
	pm.AddProperty(#prop, \
		[](lua_State* L, LifetimeHolder const& lifetime, PM::ObjectType* obj) { \
			ObjectProxy::Make<decltype(obj->prop)>(L, &obj->prop, lifetime); \
			return true; \
		}, \
		[](lua_State* L, LifetimeHolder const& lifetime, PM::ObjectType* obj, int index) { \
			return false; \
		} \
	);

#define P_REF_PTR(prop) \
	pm.AddProperty(#prop, \
		[](lua_State* L, LifetimeHolder const& lifetime, PM::ObjectType* obj) { \
			if (obj->prop) { \
				ObjectProxy::Make<std::remove_pointer<decltype(obj->prop)>::type>(L, obj->prop, lifetime); \
			} else { \
				push(L, nullptr); \
			} \
			return true; \
		}, \
		[](lua_State* L, LifetimeHolder const& lifetime, PM::ObjectType* obj, int index) { \
			return false; \
		} \
	);

#define PN(name, prop) \
	pm.AddProperty(#name, \
		[](lua_State* L, LifetimeHolder const& lifetime, PM::ObjectType* obj) { \
			return GenericGetProperty(L, lifetime, obj->prop); \
		}, \
		[](lua_State* L, LifetimeHolder const& lifetime, PM::ObjectType* obj, int index) { \
			return GenericSetProperty(L, lifetime, obj->prop, index); \
		} \
	);

#define P_FUN(name, fun) \
	pm.AddProperty(#name, \
		[](lua_State* L, LifetimeHolder const& lifetime, PM::ObjectType* obj) { \
			lua_pushcfunction(L, [](lua_State* L) -> int { \
				auto self = checked_get_proxy<PM::ObjectType>(L, 1); \
				return fun(L, self); \
			}); \
			return true; \
		}, \
		[](lua_State* L, LifetimeHolder const& lifetime, PM::ObjectType* obj, int index) { \
			return false; \
		} \
	);

#include <GameDefinitions/PropertyMaps/AllPropertyMaps.inl>

#undef BEGIN_CLS
#undef END_CLS
#undef INHERIT
#undef P
#undef P_RO
#undef P_REF
#undef P_REF_PTR
#undef PN
#undef P_FUN
	}
}
