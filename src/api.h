#include "libmcpi-r/minecraft.h"

extern "C"{

// API
unsigned char *get_minecraft();
void send_client_message(std::string text);
std::string get_username();
std::string CommandServer_parse_injection(unsigned char *command_server, ConnectedClient &client, std::string const& command);

// Buttons
void press_button_from_code(bool press, int scancode, int sym);
void press_button_from_key(bool press, std::string key);

// Inventory
unsigned char *get_inventory();
ItemInstance *get_slot(int slot);
int get_current_slot();

// World
bool in_local_world();
std::string get_world_name();
std::string get_world_dir();

}
