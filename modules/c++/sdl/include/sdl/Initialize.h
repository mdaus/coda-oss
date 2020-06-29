#ifndef __SDL_INITIALIZE_H__
#define __SDL_INITIALIZE_H__

#include <atomic>
#include <cstddef>

namespace sdl
{
class Initialize
{
public:
    Initialize();
    ~Initialize();

    Initialize(const Initialize&) = delete;
    Initialize& operator=(const Initialize&) = delete;
private:
    static std::atomic<size_t> mCounter;

};
}

#endif
