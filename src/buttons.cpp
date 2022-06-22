#include <string>

#include "libmcpi-r/log.h"
#include "SDL/SDL.h"
#include "SDL/SDL_keyToInt.h"

#include "api.h"

void press_button_from_code(bool press, int scancode, int sym){
    SDL_Event event;
    event.type = press ? SDL_KEYDOWN : SDL_KEYUP;
    event.key.state = press ? SDL_PRESSED : SDL_RELEASED;
    event.key.keysym.scancode = scancode;
    event.key.keysym.mod = (SDLMod) 0x0;
    event.key.keysym.sym = (SDLKey) sym;
    SDL_PushEvent(&event);
}

void press_button_from_key(bool press, std::string key){
    press_button_from_code(press, SDL_ScancodeMap[key], SDLKeyMap[key]);
}
