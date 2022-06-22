#include <stdlib.h>
#include <string>

#include "libmcpi-r/minecraft.h"
#include "libmcpi-r/patch.h"
#include "libmcpi-r/util.h"

#include "api.h"

static std::string name;
static std::string dir;
bool in_game = false;

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
    // Patch Minecraft::selectLevel In VTables
    patch_address((void *) 0x1023f8 /* MinecraftApp::selectLevel */, (void *) Minecraft_selectLevel_injection);
    patch_address((void *) 0x102740 /* Minecraft::selectLevel */, (void *) Minecraft_selectLevel_injection);
    // Patch Minecraft::leaveGame
    overwrite_calls((void *) Minecraft_leaveGame, (void *) Minecraft_leaveGame_injection);
}