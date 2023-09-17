#ifndef LUA_MEM_HHH
#define LUA_MEM_HHH

#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/call_function.hpp>

#define COMBOOL(b) (b ? -1 : 0)

enum FIELDTYPE_LUNALUA
{
    FIELD_INVALID = 0,
    FIELD_BYTE = 1,
    FIELD_WORD = 2,
    FIELD_DWORD = 3,
    FIELD_FLOAT = 4,
    FIELD_DFLOAT = 5,
    FIELD_STRING = 6,
    FIELD_BOOL = 7,

    FIELD_MAX = 7
};

#endif // LUA_MEM_HHH
