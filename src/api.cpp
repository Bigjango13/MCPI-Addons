#include <algorithm>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <libreborn/libreborn.h>
#include <symbols/minecraft.h>
#include <mods/misc/misc.h>
#include <mods/chat/chat.h>

#include "api.h"
#include "base64.h"
#include "extra.h"
#include "helpers.h"

std::vector<std::string> chat_log = {};

typedef std::string (*handle_t)(std::string command, std::string args, uchar *command_server);
std::map<std::string, handle_t> &get_handlers() {
    static std::map<std::string, handle_t> handlers = {};
    return handlers;
}

void add_command_handler(std::string name, handle_t handle) {
    get_handlers()[name] = handle;
}

std::string CommandServer_parse_injection(uchar *command_server, ConnectedClient &client, std::string const& command) {
    // Get the command, base is the name and root is the first two. For example:
    // command = "custom.logging.debug(Test)"
    // root_command = "custom.logging", base_command = "custom.logging.debug"
    // Most of the time they are the same
    std::string root_command = "";
    std::string base_command = "";
    int i = 0;
    while (base_command.back() != '(') {
        if (command[i] == '.') {
            root_command = base_command;
        }
        base_command += command[i];
        i++;
    }
    // Remove the '(' at the end
    base_command.pop_back();
    if (std::count(base_command.begin(), base_command.end(), '.') == 1) {
        root_command = base_command;
    }

    // Get the args
    std::string args;
    while (command[i] != '\0') {
        args += command[i++];
    }
    // Remove the ')\n' at the end
    args.pop_back();
    args.pop_back();

    // Handle the command
    std::map<std::string, handle_t>::const_iterator pos = get_handlers().find(root_command);
    if (pos != get_handlers().end()) {
        // Call handler
        return (pos->second)(base_command, args, command_server);
    }
    // Handler doesn't exist, call CommandServer::parse
    return (*CommandServer_parse)(command_server, client, command);
}

// Handle logging
std::string handle_logging(std::string command, std::string args, uchar *command_server) {
    // Handles logging
    if (command == "custom.log.debug") {
        DEBUG("%s", args.c_str());
    } else if (command == "custom.log.info") {
        INFO("%s", args.c_str());
    } else if (command == "custom.log.warn") {
        WARN("%s", args.c_str());
    } else if (command == "custom.log.err") {
        ERR("%s", args.c_str());
    }
    return "";
}

// Handle getSlot, give, and unsafeGive
std::string handle_inventory(std::string command, std::string args, uchar *command_server) {
    if (command == "custom.inventory.getSlot") {
        // Return data on the current slot
        ItemInstance *inventory_item = get_item_at_slot(get_current_slot());
        if (inventory_item != NULL) {
            return std::to_string(inventory_item->id) +
                "|" + std::to_string(inventory_item->auxiliary) +
                "|" + std::to_string(inventory_item->count) + "\n";
        }
        // Return a blank slot if empty
        return "0|0|0\n";
    }
    // If the command isn't give or unsafeGive then return
    if (
        command != "custom.inventory.unsafeGive"
        && command != "custom.inventory.give"
    ) {
        return "";
    }
    // Else prep for (unsafe) give
    int id, auxiliary, count;
    sscanf(args.c_str(), "%d|%d|%d", &id, &auxiliary, &count);
    // Safety
    if (command != "custom.inventory.unsafeGive") {
        // Don't allow invalid IDs
        if (
            (*(Item_items + id) == NULL && *(Tile_tiles + id) == NULL)
            // Special cases (333, -1, -2, 0)
            || (id == 333 || id < -2)
        ) {
            return "Failed\n";
        }
    }
    // Give it to the player
    ItemInstance *inventory_item = get_item_at_slot(get_current_slot());
    if (inventory_item != NULL) {
        // Don't change id if it's -2 or 0
        if (-2 != id) {
            inventory_item->id = id;
        }
        // Don't change auxiliary if it's -2
        if (-2 != auxiliary) {
            inventory_item->auxiliary = auxiliary;
        }
        // Don't change count if it's -2
        if (-2 != count) {
            inventory_item->count = count;
        }
    } else {
        send_client_message("Cannot work on empty slot");
        return "Failed\n";
    }
    return "Worked\n";
}

// Handles overriding tiles/items
std::string handle_override(std::string command, std::string args, uchar *command_server) {
    static uchar *Tiles_backup[257] = {};
    static uchar *Items_backup[501] = {};
    if (command == "custom.override.reset") {
        // Reset overrides
        int32_t i = 0;
        for (i=0; i <= 255; i++) {
            // Reset tiles
            if (Tiles_backup[i] != NULL) {
                Tile_tiles[i] = Tiles_backup[i];
            }
        }
        for (i=0; i <= 500; i++) {
            // Reset items
            if (Items_backup[i] != NULL) {
                Item_items[i] = Items_backup[i];
            }
        }
        return "";
    } else if (command != "custom.override") return "";
    int before, after;
    sscanf(args.c_str(), "%i,%i", &before, &after);
    if (before < 256) {
        // Overrides a tile (-1 .. 256).
        // Makes sure the tiles exists and is valid.
        if (Tile_tiles[after] != NULL && after <= 256 && before <= 256) {
            // Caches the orginal block so that it can be restored
            if (Tiles_backup[before] == NULL) Tiles_backup[before] = Tile_tiles[before];
            if (Tiles_backup[after] == NULL) Tiles_backup[after] = Tile_tiles[after];
            Tile_tiles[before] = Tiles_backup[after];
        }
    } else {
        // Overrides an item (256 .. 511)
        if (Item_items[after] != NULL && before <= 511) {
            // Caches the orginal item so that it can be restored
            if (Items_backup[before] == NULL) Items_backup[before] = Item_items[before];
            if (Items_backup[after] == NULL) Items_backup[after] = Item_items[after];
            Item_items[before] = Items_backup[after];
        }
    }
    return "";
}

// Handle getBlocks and getBlocks3D
std::string handle_getBlocks(std::string command, std::string args, uchar *command_server) {
    std::string tempFilename = "/tmp/mcpi-addons.getBlocks.txt";
    int x, y, z, x2, y2, z2;
    sscanf(args.c_str(), "%i,%i,%i,%i,%i,%i", &x, &y, &z, &x2, &y2, &z2);
    // Offset the cords
    uchar *offsetData = (uchar*)(command_server + 0x1c);
    offsetCords(offsetData, &x, &y, &z);
    offsetCords(offsetData, &x2, &y2, &z2);
    // Swap the cords so "?2 > ?" (where ? is x, y or z)
    if (x2<x) std::swap(x, x2);
    if (y2<y) std::swap(y, y2);
    if (z2<z) std::swap(z, z2);
    // Remove the tempfile if it exists
    remove(tempFilename.c_str());
    // Prepare for looping
    uchar *level = get_level();
    int id;
    int oy, oz;
    oy = y;
    oz = z;
    std::string ret;
    do {
        do {
            do {
                // Get the id and append it
                id = Level_getTile(level, x, y, z);
                ret += std::to_string(id) + ",";
                z++;
            } while (z2 >= z);
            ret.pop_back();
            // Use the normal separator normally, else the 3D list one.
            if (command != "world.getBlocks") {
                ret += "|";
            } else {
                ret += ",";
            }
            y++;
            z = oz;
        } while (y2 >= y);
        ret.pop_back();
        // Use the normal separator normally, else the 3D list one.
        if (command != "world.getBlocks") {
            ret += ";";
        } else {
            ret += ",";
        }
        x++;
        y = oy;
    } while (x2 >= x);
    // Remove the trailing semicolon
    ret.pop_back();
    std::ofstream tempFile(tempFilename);
    tempFile << ret + "\n";
    // Return the filename for openning.
    return tempFilename+"\n";
}

// Handle posting to client and without prefix
std::string handle_post(std::string command, std::string args, uchar *command_server) {
    if (command == "custom.post.client") {
        // Posts a message client side.
        send_client_message(args);
    } else if (command == "custom.post.noPrefix") {
        // Posts without the "<username> " prefix.
        // The prefix is added server side so it may not work when playing on a server.
        uchar *server_side_network_handler = *(uchar**) (get_minecraft() + Minecraft_network_handler_property_offset);
        (*ServerSideNetworkHandler_displayGameMessage)(server_side_network_handler, args);
    }
    return "";
}

// Handle pressing and releasing keys
std::string handle_key(std::string command, std::string args, uchar *command_server) {
    if (command == "custom.key.press") {
        // Starts pressing a key
        press_button_from_key(true, args);
    } else if (command == "custom.key.release") {
        // Stops pressing a key
        press_button_from_key(false, args);
    }
    return "";
}

// Handle looking up ids, getting usernames, and the players username
std::string handle_username(std::string command, std::string args, uchar *command_server) {
    if (command == "world.getPlayerId") {
        // Get the entity id of a player from the name.
        std::string name = base64_decode(args);
        for (uchar *player : get_players()) {
            std::string *player_username = (std::string *) (player + Player_username_property_offset);
            // Loop throught players to try and find the player with the right username
            if (*player_username == name) {
                // The user exists! Now get the id and return it.
                uint32_t id = *(uint32_t *) (player + Entity_id_property_offset);
                return std::to_string(id) + "\n";
            }
        }
        // The user wasn't found
        return "0\n";
    }
    if (command == "custom.username.all") {
       // Gets all the usernames
       std::string usernames;
       for (uchar *player : get_players()) {
           usernames += base64_encode(get_username(player)) + ", ";
       }
       return usernames + "\n";
    } else {
        // Gets the local players username
        return base64_encode(get_username()) + "\n";
    }
    return "";
}

// Handle getting the world name and dir
std::string handle_world(std::string command, std::string args, uchar *command_server) {
    if (command == "custom.world.particle") {
        // Level_addParticle doesn't take normal x, y, and z. It takes offsetted xyz (no negitives), this is handled by minecraft.py
        float x, y, z;
        char particle_char[100];
        sscanf(args.c_str(), "%[^|]|%f|%f|%f", particle_char, &x, &y, &z);
        std::string particle = particle_char;

        uchar *offsetData = (uchar*)(command_server + 0x1c);
        offsetCords_float(offsetData, &x, &y, &z);
        (*Level_addParticle)(get_level(), particle, x, y, z, 0.0, 0.0, 0.0, 0);
    } else if (command == "custom.world.dir") {
        // Returns the current worlds directory
        std::string name = get_world_dir();
        if (name == "") return "_LastJoinedServer\n";
        return name+"\n";
    } else if (command == "custom.world.name") {
        // Returns the current worlds name
        std::string name = get_world_name();
        return name+"\n";
    }
    return "";
}

std::string handle_player(std::string command, std::string args, uchar *command_server) {
    if (command == "custom.player.getHealth") {
        int *health = (int *) (get_player() + Mob_health_property_offset);
        return std::to_string(*health) + "\n";
    } else if (command == "custom.player.setHealth") {
        int *health = (int *) (get_player() + Mob_health_property_offset);
        sscanf(args.c_str(), "%i", health);
    } else if (command == "custom.player.closeGUI") {
        (*Minecraft_setScreen)(get_minecraft(), NULL);
    } else if (command == "custom.player.getGamemode") {
        // Gets the gamemode
        char gamemode = get_minecraft()[0xe51];
        return std::to_string((short) gamemode) + "\n";
    }
    return "";
}

std::string handle_entity(std::string command, std::string args, uchar *command_server) {
    uchar *level = get_level();
    if (command == "custom.entity.spawn") {
        // Spawns and returns the id
        // Get pos
        int id, health, data;
        float x, y, z;
        float dx, dy;
        sscanf(args.c_str(), "%i,%f,%f,%f,%i,%f,%f,%i", &id, &x, &y, &z, &health, &dx, &dy, &data);
        // Offset cords
        uchar *offsetData = (uchar*)(command_server + 0x1c);
        offsetCords_float(offsetData, &x, &y, &z);
        // Create mob
        uchar *mob = NULL;
        if (id < 0x40) {
            mob = (*MobFactory_CreateMob)(id, level);
        } else {
            mob = (*EntityFactory_CreateEntity)(id, level);
        }
        // Fail
        if (mob == NULL) {
            return "0\n";
        }
        // Set health
        if (health != -1) {
            *(int *)(mob + Mob_health_property_offset) = health;
        }
        // Set data
        if (id == 66) {
            // Falling tile type
            *(int *)(mob + TileEntity_tile_property_offset) = data;
            // Lifetime
            *(int *)(mob + TileEntity_lifetime_property_offset) = -health;
        } else if (id == 80) {
            // Arrow critcal
            *(bool *)(mob + ArrowEntity_isCritical_property_offset) = (data != 0);
        } else if (id == 13) {
            // Sheep color
            Sheep_setColor(mob, data);
        } else if (id == 64) {
            // Item
            ItemInstance *item =
                (ItemInstance *)(mob + ItemEntity_instance_property_offset);
            item->id = data;
            item->count = 1;
        } else if (id == 65) {
            // TNT fuse
            *(int *)(mob + TntEntity_fuse_property_offset) = health;
        }
        // Spawn
        (*Entity_moveTo)(mob, x, y, z, dx, dy);
        (*Level_addEntity)(level, mob);
        // Get id
        uint32_t entity_id = *(uint32_t *) (mob + Entity_id_property_offset);
        // Return the id
        return std::to_string(entity_id) + "\n";
    }
    return "";
}

std::string handle_pollChatPosts(std::string command, std::string args, uchar *command_server) {
    if (chat_log.size() == 0) {
        return "\n";
    }

    std::string history = "";

    for (const std::string& message : chat_log) {
        history += message;
        history.push_back('\0');
    }

    chat_log.clear();
    history.pop_back();
    return history + "\n";
}

HOOK(chat_send_message, void, (unsigned char *server_side_network_handler, char *username, char *message)) {
    std::string log = std::string(username) + '\0' + std::string(message);
    chat_log.push_back(log);
    ensure_chat_send_message();
    (*real_chat_send_message)(server_side_network_handler, username, message);

    if (chat_log.size() > 64) {
        chat_log.erase(chat_log.begin(), chat_log.begin() + (chat_log.size() - 3));
    }
}

__attribute__((constructor)) static void init() {
    // Call the custom version of CommandServer_parse instead of the real one.
    overwrite_calls((void *) CommandServer_parse, (void *) CommandServer_parse_injection);
    // Handlers
    add_command_handler("custom.log", handle_logging);
    add_command_handler("custom.inventory", handle_inventory);
    add_command_handler("custom.override", handle_override);
    // Get blocks doesn't use the "custom." style handlers for Raspberry Juice compat.
    add_command_handler("world.getBlocks", handle_getBlocks);
    add_command_handler("custom.post", handle_post);
    add_command_handler("custom.key", handle_key);
    // Username uses two handlers
    add_command_handler("custom.username", handle_username);
    add_command_handler("world.getPlayerId", handle_username);
    add_command_handler("custom.world", handle_world);
    add_command_handler("custom.player", handle_player);
    add_command_handler("custom.entity", handle_entity);
    // Add the event handler for pollChatPosts
    add_command_handler("events.pollChatPosts", handle_pollChatPosts);
}
