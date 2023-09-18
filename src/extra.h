#include <symbols/minecraft.h>

typedef unsigned char uchar;
typedef uchar *(*getEntityById_t)(uchar *level, int entityId);
static getEntityById_t getEntityById = (getEntityById_t) 0xa45a4;

static uint32_t Entity_id_property_offset = 0x1c; // int32_t
static uint32_t AgableMob_age_property_offset = 0xbfc; // int32_t
static uint32_t TntEntity_fuse_property_offset = 0xd0; // int32_t
static uint32_t ItemEntity_instance_property_offset = 0xd0; // ItemInstance
static uint32_t TileEntity_tile_property_offset = 0xd0; // int
static uint32_t TileEntity_lifetime_property_offset = 0xd8; // int
static uint32_t ArrowEntity_isCritical_property_offset = 0xd8; // bool

typedef ItemInstance *(*Player_getArmor_t)(uchar *player, int32_t slot);
static Player_getArmor_t Player_getArmor = (Player_getArmor_t) 0x8fda4;

typedef void (*offsetCords_t)(uchar *offsetData, int *x, int *y, int *z);
static offsetCords_t offsetCords = (offsetCords_t) 0x27c98;

static uchar **Item_items = (uchar **) 0x17b250; // Comment this out if you're using MCPI-Reborn Extended!

typedef uchar *(*MobFactory_getStaticTestMob_t)(int mob, uchar *level);
static MobFactory_getStaticTestMob_t MobFactory_getStaticTestMob = (MobFactory_getStaticTestMob_t) 0x18844;
typedef uchar *(*MobFactory_CreateMob_t)(int mob, uchar *level);
static MobFactory_CreateMob_t MobFactory_CreateMob = (MobFactory_CreateMob_t) 0x18184;
typedef uchar *(*EntityFactory_CreateEntity_t)(int id, uchar *level);
static EntityFactory_CreateEntity_t EntityFactory_CreateEntity = (EntityFactory_CreateEntity_t) 0x7d794;

static uint8_t Level_entities_property_offset = 0x20;
typedef uchar *(*Level_addEntity_t)(uchar *level, uchar *entity);
static Level_addEntity_t Level_addEntity = (Level_addEntity_t) 0xa7cbc;

typedef int (*Entity_getEntityTypeId_t)();
static uint32_t Entity_getEntityTypeId_vtable_offset = 0xdc;

typedef void (*Entity_moveTo_t)(uchar *entity, float x, float y, float z, float pitch, float yaw);
static Entity_moveTo_t Entity_moveTo = (Entity_moveTo_t) 0x7a834;

typedef void (*Sheep_setColor_t)(uchar *sheep, int color);
static Sheep_setColor_t Sheep_setColor = (Sheep_setColor_t) 0x86274;

typedef unsigned char *(*AgableMob_setAge_t)(uchar *mob, int age);
static AgableMob_setAge_t AgableMob_setAge = (AgableMob_setAge_t) 0x7a058;
