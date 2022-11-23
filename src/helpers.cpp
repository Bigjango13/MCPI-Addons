//#include <fstream>

#include <symbols/minecraft.h>
#include <libreborn/libreborn.h>
#include <mods/misc/misc.h>

#include "SDL/SDL.h"
#include "SDL/SDL_keyToInt.h"

#include "helpers.h"

unsigned char *minecraft;
static void mcpi_callback(unsigned char *mcpi){
    // Runs on every tick, sets the minecraft var.
    minecraft = mcpi;
}

unsigned char *get_minecraft(){
    return minecraft;
}

unsigned char *get_level(){
    unsigned char *level = *(unsigned char **) (minecraft + Minecraft_level_property_offset);
    return level;
}

// Uses a custom implementation of offsetCords becuase the builtin one uses ints
void offsetCords_float(unsigned char *offsetData, float *x, float *y, float *z){
    *x = *x - *(float *)(offsetData + 0x4);
    *y = *y - *(float *)(offsetData + 0x8);
    *z = *z - *(float *)(offsetData + 0xc);
}

// Sends a client side message
void send_client_message(std::string text) {
    // Gets the gui from the minecraft instance
    unsigned char *gui = minecraft + Minecraft_gui_property_offset;
    // Adds a message to the gui (aka chat).
    (*Gui_addMessage)(gui, text);
    // Logging
    std::string client_logging = "[CLIENT]: %s\n";
    #ifdef MCPI_EXTENDED
    // Use colored logs for MCPI++
    client_logging = "\x1b[32m[CLIENT]: %s\x1b[0m\n";
    #endif
    fprintf(stderr, client_logging.c_str(), text.c_str());
}

// Returns a vector of players
std::vector<unsigned char *> get_players(){
    return *(std::vector<unsigned char *> *) (get_level() + Level_players_property_offset);
}

// Returns the players username, if player ia NULL it uses the local player.
std::string get_username(unsigned char *player /*= NULL*/){
    if (player == NULL) {
        // Gets the player from the minecraft instance
        player = *(unsigned char **) (minecraft + Minecraft_player_property_offset);
    }
    // Gets the username from the player instance
    std::string *player_username = (std::string *) (player + Player_username_property_offset);
    return *player_username;
}

// Presses or releases a button based on scancode and sym
void press_button_from_code(bool press, int scancode, int sym){
    SDL_Event event;
    event.type = press ? SDL_KEYDOWN : SDL_KEYUP;
    event.key.state = press ? SDL_PRESSED : SDL_RELEASED;
    event.key.keysym.scancode = scancode;
    event.key.keysym.mod = (SDLMod) 0x0;
    event.key.keysym.sym = (SDLKey) sym;
    SDL_PushEvent(&event);
}

// Small wrapper for press_button_from_code that uses the scancode and key map
void press_button_from_key(bool press, std::string key){
    press_button_from_code(press, SDL_ScancodeMap[key], SDLKeyMap[key]);
}

static std::string name = "";
static std::string dir = "";
static bool in_game = false;
bool in_local_world(){
    // Taking advantage of not logging servers
    return in_game;
}

std::string get_world_name(){
    if (in_local_world()) return name;
    return "server";
}

std::string get_world_dir(){
    if (in_local_world()) return dir;
    return "_LastJoinedServer";
}

static void Minecraft_selectLevel_injection(unsigned char *minecraft, std::string const& level_dir, std::string const& level_name, LevelSettings const& settings) {
    // Call Original Method
    (*Minecraft_selectLevel)(minecraft, level_dir, level_name, settings);

    dir = level_dir;
    name = level_name;
    in_game = true;
}

static void Minecraft_leaveGame_injection(unsigned char *minecraft, bool save_remote_level) {
    // Call Original Method
    (*Minecraft_leaveGame)(minecraft, save_remote_level);

    in_game = false;
}

__attribute__((constructor)) static void init() {
    // Patch Minecraft::selectLevel in vtables
    patch_address((void *) 0x1023f8 /* MinecraftApp::selectLevel */, (void *) Minecraft_selectLevel_injection);
    patch_address((void *) 0x102740 /* Minecraft::selectLevel */, (void *) Minecraft_selectLevel_injection);
    // Patch Minecraft::leaveGame
    overwrite_calls((void *) Minecraft_leaveGame, (void *) Minecraft_leaveGame_injection);    // Runs on every tick.
    misc_run_on_update(mcpi_callback);
}
