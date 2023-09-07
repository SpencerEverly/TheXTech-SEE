#include "sdl_proxy/sdl_stdinc.h"
#include "sdl_proxy/sdl_assert.h"

#include <unordered_map>
#include <functional>
#include <type_traits>
#include <limits>

template<class T>
T f2i_cast(double in)
{
    if(std::is_same<T, uint64_t>::value)
    {
        if(in < static_cast<double>(std::numeric_limits<uint64_t>::min()) || (in > 2.0 * 0x8000000000000000))
            in = SDL_fmod(in, 2.0 * 0x8000000000000000);
        return static_cast<T>(in);
    }

    if(in < static_cast<double>(std::numeric_limits<int64_t>::min()) || in > static_cast<double>(std::numeric_limits<int64_t>::max()))
        in = SDL_fmod(in, 0x8000000000000000);

    return static_cast<T>(static_cast<int64_t>(in));
}

