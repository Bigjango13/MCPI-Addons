#include <string>

#include "libmcpi-r/minecraft.h"
#include "libmcpi-r/patch.h"
#include "libmcpi-r/util.h"
#include "libmcpi-r/misc.h"

#include "api.h"


typedef unsigned char *(*getEntitiyById_t)(unsigned char *level, int entityId);
static getEntitiyById_t getEntitiyById = (getEntitiyById_t) 0xa45a4;

static unsigned char *Tiles_backup[257];
static unsigned char *Items_backup[501];

static unsigned char **Item_items = (unsigned char **) 0x17b250;

unsigned char *minecraft;
unsigned char *get_minecraft(){
    return minecraft;
}

static void mcpi_callback(unsigned char *mcpi){
    // Runs on every tick, sets the minecraft var.
    minecraft = mcpi;
}

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

std::string get_username(){
    // Gets the player from the minecraft instance
    unsigned char *player = *(unsigned char **) (minecraft + Minecraft_player_property_offset);
    // Gets the username from the player instance
    std::string *player_username = (std::string *) (player + Player_username_property_offset);
    return *player_username;
}

std::string CommandServer_parse_injection(unsigned char *command_server, ConnectedClient &client, std::string const& command){
    // Get the command
    std::string base_command;
    int i = 0;
    while (base_command.back() != '('){
        base_command += command[i++];
    }
    // Remove the '(' at the end
    base_command.pop_back();
    // Get the args
    std::string args;
    while (args.back() != ')'){
        args += command[i++];
    }
    // Remove the ')' at the end
    args.pop_back();
    //INFO("Args: %s, Base: %s", args.c_str(), base_command.c_str());
    // Handle the command
    unsigned char *level = *(unsigned char **) (minecraft + Minecraft_level_property_offset);
    if (base_command == "world.getPlayerId"){
        // Get the entity id of a player from the name.
        std::string name = base64_decode(args);
        std::vector<unsigned char *> players = *(std::vector<unsigned char *> *) (level + Level_players_property_offset);
        for (unsigned char* player : players) {
            std::string *player_username = (std::string *) (player + Player_username_property_offset);
            if (*player_username == name){
                uint32_t id = *(uint32_t *) (player + 0x1c);
                return std::to_string(id) + "\n";
            }
        }
        return "0\n";
    } else if (base_command == "custom.getUsernames"){
       std::string usernames;
       std::vector<unsigned char *> players = *(std::vector<unsigned char *> *) (level + Level_players_property_offset);
       for (unsigned char* player : players) {
           std::string *player_username = (std::string *) (player + Player_username_property_offset);
           usernames += base64_encode(*player_username) + ", ";
       }
       return usernames + "\n";
    } else if (base_command == "custom.getUsername"){
        // Get the entity id of a player from the name.
        std::string ret = "";
        std::vector<unsigned char *> players = *(std::vector<unsigned char *> *) (level + Level_players_property_offset);
        for (unsigned char* player : players) {
            std::string *player_username = (std::string *) (player + Player_username_property_offset);
            std::string base64_username = base64_encode(*player_username);
            ret += base64_username+",";
        }
    } else if (base_command == "custom.postClient"){
        // Posts a message client side.
        send_client_message(args);
    } else if (base_command == "custom.postWithoutPrefix") {
        // Posts without the "<username> " prefix.
        // The prefix is added server side so it may not work on servers.
        unsigned char *server_side_network_handler = *(unsigned char**) (minecraft + Minecraft_network_handler_property_offset);
        (*ServerSideNetworkHandler_displayGameMessage)(server_side_network_handler, args);
    } else if (base_command == "custom.getUsername"){
        // Return the players username
        // The API uses newlines as EOL
        return get_username()+"\n";
    } else if (base_command == "custom.getSlot"){
        // Return data on a slot
        ItemInstance *inventory_item = get_slot(get_current_slot());
        if (inventory_item != NULL){
            return std::to_string(inventory_item->id)+"|"+std::to_string(inventory_item->auxiliary)+"|"+std::to_string(inventory_item->count)+"\n";
        }
        // Return a blank slot if empty
        return "0|0|0\n";
    } else if (base_command == "custom.give"){
        // Give the player an item
        int id, auxiliary, count;
        sscanf(args.c_str(), "%d|%d|%d", &id, &auxiliary, &count);
        // Don't allow invalid IDs
        if ((*(Item_items + id) == NULL && *(Tile_tiles + id) == NULL) || id == 333 || id < -1) { return "Failed\n"; }
        ItemInstance *inventory_item = get_slot(get_current_slot());
        if (inventory_item != NULL){
            if (-2 != id){
                inventory_item->id = id;
            }
            if (-2 != auxiliary){
                inventory_item->auxiliary = auxiliary;
            }
            if (-2 != count){
                inventory_item->count = count;
            }
            return "Worked\n";
        } else {
            send_client_message("Cannot work on empty slot");
            return "Failed\n";
        }
    } else if (base_command == "custom.press"){
        // Starts pressing a key
        press_button_from_key(true, args);
    } else if (base_command == "custom.unpress"){
        // Stops pressing a key
        press_button_from_key(false, args);
    } else if (base_command == "custom.worldDir"){
        // Returns the current worlds directory
        std::string name = get_world_dir();
        if (name == "") return "_LastJoinedServer\n";
        return name+"\n";
    } else if (base_command == "custom.worldName"){
        // Returns the current worlds name
        std::string name = get_world_name();
        return name+"\n";
    } else if (base_command == "custom.particle"){
        // Level_addParticle doesn't take normal x, y, and z. It takes offsetted xyz (no negitives), this is handled by minecraft.py
        float x, y, z;
        char particle_char[100];
        sscanf(args.c_str(), "%[^|]|%f|%f|%f", particle_char, &x, &y, &z);
        std::string particle = particle_char;
        (*Level_addParticle)(level, particle, x, y, z, 0.0, 0.0, 0.0, 0);
    } else if (base_command == "custom.inventory"){
        // opens the inventory
        unsigned char *screen = (unsigned char *) ::operator new(TOUCH_INGAME_BLOCK_SELECTION_SCREEN_SIZE);
        ALLOC_CHECK(screen);
        screen = (*Touch_IngameBlockSelectionScreen)(screen);
        (*Minecraft_setScreen)(minecraft, screen);
    } else if (base_command == "custom.overrideTile"){
        // Overrides a tile (-1 .. 256).
        int before, after;
        sscanf(args.c_str(), "%i,%i", &before, &after);
        // Makes sure the tiles exists and is valid.
        if (*(Tile_tiles + after) != NULL && after <= 256 && before <= 256) {
            // Caches the orginal block so that it can be restored
            if (Tiles_backup[before] == NULL) Tiles_backup[before] = *(Tile_tiles + before);
            if (Tiles_backup[after] == NULL) Tiles_backup[after] = *(Tile_tiles + after);
            *(Tile_tiles + before) = Tiles_backup[after];
        }
    } else if (base_command == "custom.overrideItem"){
        // Overrides an item (256 .. 511)
        int before, after;
        sscanf(args.c_str(), "%i,%i", &before, &after);
        if (*(Item_items + after) != NULL && before <= 511) {
            // Caches the orginal item so that it can be restored
            if (Items_backup[before] == NULL) Items_backup[before] = *(Item_items + before);
            if (Items_backup[after] == NULL) Items_backup[after] = *(Item_items + after);
            *(Item_items + before) = Items_backup[after];
        }
    } else if (base_command == "custom.resetOverrides"){
        for (int32_t i=0; i <= 255; i++) {
            if (Tiles_backup[i] != NULL){
                *(Tile_tiles + i) = Tiles_backup[i];
            }
        }
        for (i=0; i <= 500; i++){
            if (Items_backup[i] != NULL){
                *(Item_items + i) = Items_backup[i];
            }
        }
    } else if (base_command == "custom.debug"){
        DEBUG("%s", args.c_str());
    } else if (base_command == "custom.info"){
        INFO("%s", args.c_str());
    } else if (base_command == "custom.warn"){
        WARN("%s", args.c_str());
    } else if (base_command == "custom.err"){
        ERR("%s", args.c_str());
    } else {
        // Call original method
       std::string ret = (*CommandServer_parse)(command_server, client, command);
       // Return it
       return ret;
    }
    // Return values must not have been needed
    return "";
}

__attribute__((constructor)) static void init() {
    // Call the custom version of CommandServer_parse instead of the real one.
    overwrite_calls((void *) CommandServer_parse, (void *) CommandServer_parse_injection);
    // Runs on every tick.
    misc_run_on_update(mcpi_callback);
}
