#include <symbols/minecraft.h>

#include "helpers.h"
#include "extra.h"

uchar *get_inventory() {
    // Get minecraft and the player
    uchar *minecraft = get_minecraft();
    uchar *player = *(uchar **) (minecraft + Minecraft_player_property_offset);
    if (player != NULL) {
        // Get the player inventory
        uchar *inventory = *(uchar **) (player + Player_inventory_property_offset);
        return inventory;
    }
    // The player doesn't exist
    return NULL;
}

// Gets the item from the slot number.
ItemInstance *get_item_at_slot(int slot) {
    if (slot == -256) {
        return NULL;
    }
    uchar *inventory = get_inventory();
    if (inventory != NULL) {
        uchar *inventory_vtable = *(uchar **) inventory;
        FillingContainer_getItem_t FillingContainer_getItem = *(FillingContainer_getItem_t *) (inventory_vtable + FillingContainer_getItem_vtable_offset);
        ItemInstance *inventory_item = (*FillingContainer_getItem)(inventory, slot);
        return inventory_item;
    }
    return NULL;
}

// Gets the current slot
int get_current_slot() {
    uchar *inventory = get_inventory();
    if (inventory != NULL) {
        // Get the slot number
        int selected_slot = *(int *) (inventory + Inventory_selectedSlot_property_offset);
        int linked_slots_length = *(int *) (inventory + FillingContainer_linked_slots_length_property_offset);
        if (selected_slot < linked_slots_length) {
            int *linked_slots = *(int **) (inventory + FillingContainer_linked_slots_property_offset);
            selected_slot = linked_slots[selected_slot];
        }
        return selected_slot;
    }
    // With luck, this value will never be valid.
    return -256;
}
