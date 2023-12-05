#ifdef __cplusplus
#include <string>
#include <vector>

extern "C" {

std::vector<unsigned char *> get_players();
std::string get_username(unsigned char *player = NULL);

std::string get_world_name();
std::string get_world_dir();
std::string get_server_name();

void send_client_message(std::string text) ;
void press_button_from_key(bool press, std::string key);

#else
#define bool _Bool
#endif

unsigned char *get_minecraft();
unsigned char *get_level();
unsigned char *get_player();
unsigned char *get_inventory();

void offsetCords_float(unsigned char *offsetData, float *x, float *y, float *z);

void press_button_from_code(bool press, int scancode, int sym);

bool in_local_world();

ItemInstance *get_item_at_slot(int slot);
int get_current_slot();

#ifdef __cplusplus
}
#endif