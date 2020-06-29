#include <sdl/Initialize.h>
#include <sdl/External.h>

namespace sdl
{
std::atomic<size_t> Initialize::mCounter(0);

Initialize::Initialize()
{
    if (!mCounter++)
    {
        SDL_Init(SDL_INIT_VIDEO);
    }
}

Initialize::~Initialize()
{
    if (!--mCounter)
    {
        SDL_VideoQuit();
        SDL_Quit();
    }
}

}
