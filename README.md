# MCPI Addons

A Minecraft Pi Edition: Reborn mod to add more features to the API.

## Installing

First you will need to install from pip, to do that you can use `pip3 install mcpi-addons`
If you don't want to compile it (or can't) then you can grab the newest version from the releases page.

## Compiling

Just run `./build.sh` to create the bianary and run `mv libextrapi.so ~/.minecraft-pi/mods` to install it.

## What does it do?

It adds these:

- `custom.log`
  - `custom.log.debug` (`mc.log.debug(msg)`) Logs a message in debug mode.
  - `custom.log.info` (`mc.log.info(msg)`) Logs a message.
  - `custom.warn` (`mc.log.warn(msg)`) Logs a warning.
  - `custom.log.error` (`mc.log.error(msg)`) Logs an error.
- `custom.inventory`
  - `custom.inventory.getSlot` (`mc.inventory.getHeldItem()`) Gets the id, auxiliary, and count of the current slot.
  - `custom.inventory.unsafeGive` (`mc.inventory.unsafeGive(id=-2, auxiliary=-2, count=-2)`) give the player the item without safety checking (-2 means don't change)
  - `custom.inventory.give` (`mc.inventory.give(id=-2, auxiliary=-2, count=-2)`) give the player the item without safety checking (-2 means don't change)
- `custom.override`
  - `custom.override.reset` (`mc.resetOverrides()`) resets all tile and item overrides.
  - `custom.override` (`mc.override(before, after)`) overrides the id `before` with the id of `after`.
- `world.getBlocks`
  - `world.getBlocks` (`mc.getBlocks(x, y, z, x2, y2, z2)`) Gets a flat list of the blocks between (x, y, z) and (x2, y2, z2).
  - `world.getBlocks.3D` (`mc.getBlocks3D(x, y, z, x2, y2, z2)`) Gets a 3D list of the blocks between (x, y, z) and (x2, y2, z2).
- `custom.post`
  - `custom.post.client` (`mc.postToClient(msg)`) Posts the message to the chat client side.
  - `custom.post.noPrefix` (`mc.postWithoutPrefix(msg)`) Posts the message without the username prefix.
- `custom.key`
  - `custom.key.press` (`mc.player.press(key)`) Presses a key.
  - `custom.key.release` (`mc.player.release(key)`) Releases a key.
- `world.getPlayerId` (`mc.getPlayerEntityId(name)`) Gets the id of a player from the name.
- `custom.username`
  - `custom.username` (`mc.player.getUsername()`) Gets the local players username.
  - `custom.username.all` (`mc.getUsernames()`) Gets a list of player usernames.
- `custom.world`
  - `custom.world.particle` (`mc.particle(x, y, z, particle)`) Spawns the particle at (x,y,z).
  - `custom.world.dir` (`mc.world.dir()`) Get the world folder.
  - `custom.world.name` (`mc.world.name()`) Get the world name.

I want to add more so please give me suggestions.

## Todo list

I am going to add theses features someday, but they aren't here now.

- `player.getHealth() -> int` Returns the players health.
- `playere.setHealth(health: int)` Sets the players health.
- `player.getGamemode() -> int` Gets the players gamemode.
- `player.setGamemode(gamemode: int)` Gets the players gamemode.
- `player.getOxygen() -> int` Gets the player oxygen.
- `player.setOxygen(oxygen: int)` Sets the players oxygen.
- `player.getInventory() -> int[]` Gets the player inventory.
- `player.setInventory(inventory: int[])` Sets the player inventory.
- `entity.getArmor(id: int) -> int[4]` Gets the players armor.
- `entity.setArmor(helmet: int, chestplate: int, leggings: int, boots: int)` Sets the players armor.
- `reborn.getFeature(feature: string, default: bool = False) -> bool` Gets the status of a reborn feature.
- `reborn.getVersion() -> string` Gets the reborn version.
- `camera.getCameraState() -> int` Gets the camera state.
- `camera.setCameraState(state: int)` Sets the camera state.
- `minecraft.getVersion() -> str` 0.1.0 or 0.1.1, will be determined at compile time and will require Legacy support.
- `world.seed() -> string` Gets the worlds seed.

## Known bugs

- Using the particle `iconcrack` with `mc.particle` crashes the game, but using an invaild particle name is fine.
- `postToClient` really doesn't like it when you use `\n`. When posted they might also post a lot of garbage to server side chat.

## Extras

### Raspberry Juice compatibility
One day all of these will be supported.

* [x] `getBlocks`
* [x] `getPlayerEntityId`
* [ ] `player/entity.getRotation`
* [ ] `player/entity.getPitch`
* [ ] `player/entity.getDirection`
* [ ] `events.pollChatPosts`
* [ ] ChatEvent


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

- **1.1.0**
  - Fixed bug with causing args to be cut off at the first left parenthesis.
  - Fixed bug in `world.getBlocks` and `custom.getBlocks3D` causing them to target the wrong position.
  - Added tests.
  - Improved docs.
  - Many breaking API changes.
  - Many internal changes.

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
