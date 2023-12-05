#include <symbols/minecraft.h>
#include <libreborn/libreborn.h>
#include <mods/misc/misc.h>

#include "SDL/SDL.h"
#include "SDL/SDL_keyToInt.h"

#include "helpers.h"
#include "extra.h"

uchar *minecraft;
static void mcpi_callback(uchar *mcpi) {
    // Runs on every tick, sets the minecraft var.
    minecraft = mcpi;
}

uchar *get_minecraft() {
    return minecraft;
}

uchar *get_level() {
    uchar *level = *(uchar **) (minecraft + Minecraft_level_property_offset);
    return level;
}

uchar *get_player() {
    uchar *player = *(uchar **) (minecraft + Minecraft_player_property_offset);
    return player;
}

// Uses a custom implementation of offsetCords becuase the builtin one uses ints
void offsetCords_float(uchar *offsetData, float *x, float *y, float *z) {
    *x = *x - *(float *)(offsetData + 0x4);
    *y = *y - *(float *)(offsetData + 0x8);
    *z = *z - *(float *)(offsetData + 0xc);
}

// Sends a client side message
void send_client_message(std::string text) {
    // Gets the gui from the minecraft instance
    uchar *gui = minecraft + Minecraft_gui_property_offset;
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
std::vector<uchar *> get_players() {
    return *(std::vector<uchar *> *) (get_level() + Level_players_property_offset);
}

// Returns the players username, if player ia NULL it uses the local player.
std::string get_username(uchar *player /*= NULL*/) {
    if (player == NULL) {
        // Gets the player from the minecraft instance
        player = *(uchar **) (minecraft + Minecraft_player_property_offset);
    }
    // Gets the username from the player instance
    std::string *player_username = (std::string *) (player + Player_username_property_offset);
    return *player_username;
}

// Presses or releases a button based on scancode and sym
void press_button_from_code(bool press, int scancode, int sym) {
    SDL_Event event;
    event.type = press ? SDL_KEYDOWN : SDL_KEYUP;
    event.key.state = press ? SDL_PRESSED : SDL_RELEASED;
    event.key.keysym.scancode = scancode;
    event.key.keysym.mod = (SDLMod) 0x0;
    event.key.keysym.sym = (SDLKey) sym;
    SDL_PushEvent(&event);
}

// Small wrapper for press_button_from_code that uses the scancode and key map
void press_button_from_key(bool press, std::string key) {
    press_button_from_code(press, SDL_ScancodeMap[key], SDLKeyMap[key]);
}

static std::string server = "";
static std::string name = "";
static std::string dir = "";
static bool in_game = false;
bool in_local_world() {
    // Taking advantage of not logging servers
    return in_game;
}

std::string get_world_name() {
    return name;
}

std::string get_world_dir() {
    if (in_local_world()) return dir;
    return "_LastJoinedServer";
}

std::string get_server_name() {
    return server;
}

static void Minecraft_selectLevel_injection(uchar *minecraft, std::string const& level_dir, std::string const& level_name, LevelSettings const& settings) {
    // Call Original Method
    (*Minecraft_selectLevel)(minecraft, level_dir, level_name, settings);

    dir = level_dir;
    name = level_name;
    in_game = true;
}

static uchar *Minecraft_selectLevel_ServerLevel_injection(
    uchar *serverlevel, uchar *storage, std::string const& level_name, LevelSettings const& settings, int param_5, uchar *dimension
) {
    // Call Original Method
    typedef uchar *(*ServerLevel_t)(uchar *serverlevel, uchar *storage, std::string const& level_name, LevelSettings const& settings, int param_5, uchar *dimension);
    ServerLevel_t ServerLevel = (ServerLevel_t) 0x7681c;
    uchar *ret = ServerLevel(serverlevel, storage, level_name, settings, param_5, dimension);

    // TODO: Get level dir too
    //dir = level_dir;
    name = level_name;
    in_game = true;

    return ret;
}

static void Minecraft_leaveGame_injection(uchar *minecraft, bool save_remote_level) {
    // Call Original Method
    (*Minecraft_leaveGame)(minecraft, save_remote_level);

    in_game = false;
}

// For servers
static bool Minecraft_joinMultiplayer_injection(uchar *self, uchar *server) {
    unsigned char *shared_string = *(unsigned char **) (server + RakNet_RakString_sharedString_property_offset);
    char *c_str = *(char **) (shared_string + RakNet_RakString_SharedString_c_str_property_offset);
    ::server = c_str;
    return Minecraft_joinMultiplayer(self, server);
}

__attribute__((constructor)) static void init() {
    // Patch Minecraft::selectLevel in vtables (doesn't work for server mode, as reborn don't use the vtables)
    patch_address((void *) 0x1023f8 /* MinecraftApp::selectLevel */, (void *) Minecraft_selectLevel_injection);
    patch_address((void *) 0x102740 /* Minecraft::selectLevel */, (void *) Minecraft_selectLevel_injection);
    // Patch Minecraft::selectLevel calling ServerLevel (for servers)
    overwrite_call((void *) 0x16f84, (void *) Minecraft_selectLevel_ServerLevel_injection);
    // Patch Minecraft::leaveGame
    overwrite_calls((void *) Minecraft_leaveGame, (void *) Minecraft_leaveGame_injection);    // Runs on every tick.
    // Patch Minecraft::joinMultiplayer
    overwrite_calls((void *) Minecraft_joinMultiplayer, (void *) Minecraft_joinMultiplayer_injection);

    misc_run_on_update(mcpi_callback);
}
