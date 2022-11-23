#include <symbols/minecraft.h>

typedef unsigned char *(*getEntitiyById_t)(unsigned char *level, int entityId);
static getEntitiyById_t getEntitiyById = (getEntitiyById_t) 0xa45a4;

static uint32_t Entity_id_property_offset = 0x1c; // int32_t

typedef ItemInstance *(*Player_getArmor_t)(unsigned char *player, int32_t slot);
static Player_getArmor_t Player_getArmor = (Player_getArmor_t) 0x8fda4;

typedef void (*offsetCords_t)(unsigned char *offsetData, int *x, int *y, int *z);
static offsetCords_t offsetCords = (offsetCords_t) 0x27c98;

static unsigned char **Item_items = (unsigned char **) 0x17b250;

