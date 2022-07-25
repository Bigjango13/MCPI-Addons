#include <symbols/minecraft.h>

#include "api.h"

unsigned char *get_inventory(){
    // Get minecraft and the player
    unsigned char *minecraft = get_minecraft();
    unsigned char *player = *(unsigned char **) (minecraft + Minecraft_player_property_offset);
    if (player != NULL) {
        // Get the player inventory
        unsigned char *inventory = *(unsigned char **) (player + Player_inventory_property_offset);
        return inventory;
    }
    // The player doesn't exist
    return NULL;
}

ItemInstance *get_slot(int slot){
    // Gets the item from the slot number.
    if (slot==-256){return NULL;}
    unsigned char *inventory = get_inventory();
    if (inventory != NULL) {
        unsigned char *inventory_vtable = *(unsigned char **) inventory;
        FillingContainer_getItem_t FillingContainer_getItem = *(FillingContainer_getItem_t *) (inventory_vtable + FillingContainer_getItem_vtable_offset);
        ItemInstance *inventory_item = (*FillingContainer_getItem)(inventory, slot);
        return inventory_item;
    }
    return NULL;
}

int get_current_slot(){
    // Gets the current slot
    unsigned char *inventory = get_inventory();
    if (inventory != NULL) {
        // Get the slot number
        int32_t selected_slot = *(int32_t *) (inventory + Inventory_selectedSlot_property_offset);
        int32_t linked_slots_length = *(int32_t *) (inventory + FillingContainer_linked_slots_length_property_offset);
        if (selected_slot < linked_slots_length) {
            int32_t *linked_slots = *(int32_t **) (inventory + FillingContainer_linked_slots_property_offset);
            selected_slot = linked_slots[selected_slot];
        }
        return selected_slot;
    }
    // With luck, this value will never be valid.
    return -256;
}
