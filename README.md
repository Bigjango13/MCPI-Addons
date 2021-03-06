# MCPI Addons

A Minecraft Pi Edition: Reborn mod to add more features to the api.

## Installing

First you will need to install from pip, to do that you can use `pip3 install mcpi-addons`
If you don't want to compile it (or can't) then you can grab the newest version from the releases page.

## Compiling

Just run `make && make install` to create and install the bianary.

## What does it do?

It adds these:

- `custom.getUsername` (`mc.getUsername()`) to get the players username.
- `custom.postWithoutPrefix` (`mc.postWithoutPrefix(msg)`) to post directly to chat (avoids usernames and sanitation).
- `custom.postClient` (`mc.postToClient(msg)`) to post chat messages client side. It bypasses sanitization.
- `custom.getSlot` (`mc.getSlot()`) to get the contents of the current slot.
- `custom.give` (`mc.give(id, auxiliary = -2, count = -2)`) to change the current slot (-2 means it will stay the same, it isn't -1 becuase the id of -1 exists).
- `custom.press` (`mc.press(key)`) to simulate pressing and holding a key (for example `mc.press("W")` or `mc.press("SPACE")`).
- `custom.unpress` (`mc.unpress(key)`) to releases a key.
- `custom.worldName` (`mc.worldName()`) to get the name of the world.
- `custom.worldDir` (`mc.worldDir()`) to get the directory of the world.
- `custom.particle` (`mc.particle(x, y, z, particle)`) to spawn a particle at a location. x, y, and z are floats and automaticly have 0.5 added to them so that they are centered.
- `custom.debug` (`mc.debug(msg)`) prints a message to debug (only shown if `MCPI_DEBUG` = 1)
- `custom.info` (`mc.info(msg)`) prints a info message.
- `custom.warn` (`mc.warn(msg)`) prints a warning.
- `custom.err` (`mc.err(msg)`) prints a error and stops MCPI.
- `custom.getUsernames` (`mc.getUsernames()`) to get a list of the usernames that are playing
- `world.getPlayerId` (`mc.getPlayerEntityId(name)`) this gets the entity id of a player from their name. This doesn't use the `custom` base because it is from the [RaspberryJuice](https://www.spigotmc.org/resources/raspberryjuice.22724/) plugin which isn't compatible with MCPI or Reborn.
- `custom.overrideTile` (`mc.overrideTile(before, after)`) overrides the tile with the id of `before` to the tile with the id of `after` unless `after` is an invaid id.
- `custom.overrideItem` (`mc.overrideItem(before, after)`) overrides the item with the id of `before` to the item with the id of `after` unless `after` is an invaid id. (little testing, will have bugs).
- `custom.resetOverrides` (`mc.resetOverrides()`) resets item and tile overrides.
- `world.getBlocks` (`mc.getBlocks(x1, y1, z1, x2, y2, z2)`) to get a 1D list of blocks.
- `custom.getBlocks3D` (`mc.getBlocks3D(x1, y1, z1, x2, y2, z2)`) to get a 3D list of blocks.

I want to add more so please give me suggestions.

## Known bugs

- Using the particle `iconcrack` with `mc.particle` crashes the game, but using an invaild particle name is fine.
- `postToClient` really doesn't like it when you use `\x0a` along with some other chars. When posted they might also post a lot of garbage to server side chat. Once it wouldn't stop spamming chat until I used `htop` to kill reborn.

## Extras

### Raspberry Juice compatibility
One day all of these will be supported.

[x] `getBlocks`
[x] `getPlayerEntityId`
[ ] `player/entity.getRotation`
[ ] `player/entity.getPitch`
[ ] `player/entity.getDirection`
[ ] `events.pollChatPosts`
[ ] ChatEvent


### Particles

Particles are client side and only shown if the player is within 16 blocks.
Here is a particle list I found at `0x107511` in `minecraft-pi`
- `bubble` (only works in water)
- `crit`
- `flame`
- `lava`
- `smoke`
- `largesmoke`
- `reddust`
- `ironcrack` (crashes the game)
- `snowballpoof`
- `explode`

### Tiles/Items

A list of tiles can be found [here](https://wiki.mcpirevival.tk/wiki/Minecraft:_Pi_Edition_Complete_Block_List) and a list of items [here](https://wiki.mcpirevival.tk/wiki/Minecraft:_Pi_Edition_Complete_Item_List).

## Changelog

- **1.0.3**
  - Added `world.getBlocks`, `custom.getBlocks3D`.
  - Improved `custom.particle`.
  - Removed `getOffset` from `minecraft.py`.

- **1.0.2**
  - Added `custom.overrideTile`, `custom.overrideItem`, and `custom.resetOverrides`.

- **1.0.1**
  - Added functionality to `world.getPlayerId`.
  - Added `custom.getUsernames`.

- **1.0.0**
  - Stopped `getSlot` from crashing the game with invaild ids.
  - Added `press`, `unpress`, `worldName`, `worldDir`, `particle`, `getOffset`, and logging (`debug`, `info`, `warn`, `err`).
  - Uploaded to pypi and github.

- **Beta**
  - Added `getSlot` and `give`.

- **Alpha**
  - Had `getUsername`, `postWithoutPrefix`, and `postClient`.

## Screenshots

Here is a screenshot of using overrides and particles:
![ ](https://i.imgur.com/I8d8I0G_d.webp?maxwidth=760&fidelity=grand)