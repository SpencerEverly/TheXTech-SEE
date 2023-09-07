#ifndef LUA_MEM_HHH
#define LUA_MEM_HHH

enum FIELDTYPE
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
