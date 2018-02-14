#ifndef H_XD_LUA
#define H_XD_LUA

#include <xd/config.hpp>
#include <xd/factory.hpp>
#include <xd/lua/config.hpp>
#include <xd/lua/types.hpp>
#include <xd/lua/exceptions.hpp>
#include <xd/lua/virtual_machine.hpp>
#include <xd/lua/scheduler.hpp>

#ifndef LUABIND_CPLUSPLUS_LUA
extern "C"
{
#endif
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#ifndef LUABIND_CPLUSPLUS_LUA
}
#endif
#include <luabind/luabind.hpp>

#endif
