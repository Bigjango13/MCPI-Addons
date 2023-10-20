# MCPI Addons

A Minecraft Pi Edition: Reborn mod to add more features to the API.

**Note:** requires Reborn v2.4.8 or later.

## Installing

First you will need to install the frontend from pip, to do that you can use `pip3 install mcpi-addons`.
If you don't want to (or can't) compile the backend then you can grab the newest version from the releases page.

## Compiling

Just run `./build.sh` to create the binary and run `mv libextrapi.so ~/.minecraft-pi/mods` to install the mod, this is needed because MCPI doesn't support any of these extensions that this api adds, so a C++ mod is loaded to intercept `CommandServer::parse`.

## How to use

Here is a simple "Hello World" example:
```python
# Import the api
from mcpi_addons.minecraft import Minecraft
# Initialize the api (MCPI must be open and in a world at this time)
mc = Minecraft.create()

# Post to chat
mc.postToChat("Hello world!")
```
For a tutorial see the 'Using the API' section of [stuffaboutcode.com](https://www.stuffaboutcode.com/p/minecraft.html).

## What does it do?

It adds these:

- `custom.log`
  - `custom.log.debug` (`mc.logging.debug(msg)`) Logs a message in debug mode.
  - `custom.log.info` (`mc.logging.info(msg)`) Logs a message.
  - `custom.warn` (`mc.logging.warn(msg)`) Logs a warning.
  - `custom.log.error` (`mc.logging.error(msg)`) Logs an error.
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
- `custom.player`
  - `custom.player.getHealth` (`mc.player.getHealth()`) Returns the players health.
  - `custom.player.setHealth` (`mc.player.setHealth(health)`) Sets the players health.
  - `custom.player.closeGUI` (`mc.player.closeGUI()`) Closes the current screen.
  - `custom.player.getGamemode` (`mc.player.getGamemode()`) Returns the players gamemode.
- `custom.entity`
  - `custom.entity.spawn` (`mc.entity.spawn(id, x, y, z, health = -1, dir = (0, 0), data = 0)`) spawns an entity of type `id` at `x, y, z`, with `health` health (or fuse/lifetime) pointing in `dir` direction with `data` data.
  - `custom.entity.setAge` (`mc.entity.setAge(id, age)`) changes an entities age, if it has an age (only animals do). If the age is negative it will be a baby for `abs(age) / 20` seconds.
  - `custom.entity.setSheepColor` (`mc.entity.setSheepColor(id, color)`) sets a sheeps color to `color`
- `custom.reborn`
  - `custom.reborn.getFeature` (`mc.reborn.getFeature(feature: string) -> bool`) Gets the status of a reborn feature
  - `custom.reborn.getVersion` (`mc.reborn.getVersion() -> string`) Gets the reborn version
- `events.chat`
  - `events.chat.post` (`mc.events.pollChatPosts() -> [string]`) return a list of messages shown in client side chat
  - `events.chat.size` (`mc.events.setChatSize(size = 64)`) clears the chat list and resets the size
- `entity.getAllEntities`
- `entity.getEntities`

I want to add more so please give me suggestions.

## Todo list

I am going to add theses features someday, but they aren't here now. Feel free to create a PR that adds them or other features!

- `player.setGamemode(gamemode: int)` Sets the players gamemode.
- `player.getOxygen() -> int` Gets the player oxygen.
- `player.setOxygen(oxygen: int)` Sets the players oxygen.
- `player.getInventory() -> int[]` Gets the player inventory.
- `player.setInventory(inventory: int[])` Sets the player inventory.
- `entity.getArmor(id: int) -> int[4]` Gets the players armor.
- `entity.setArmor(helmet: int, chestplate: int, leggings: int, boots: int)` Sets the players armor.
- `camera.getCameraState() -> int` Gets the camera state.
- `camera.setCameraState(state: int)` Sets the camera state.
- `minecraft.getVersion() -> str` 0.1.0 or 0.1.1, will be determined at compile time and will require Legacy support.
- `world.seed() -> string` Gets the worlds seed.

## Known bugs

- Using the particle `iconcrack` with `mc.particle` crashes the game, but using an invalid particle name is fine.
- `postToClient` really doesn't like it when you use `\n`. When posted they might also post a lot of garbage to server side chat.

## Extras

### Raspberry Juice compatibility
One day all of these will be supported.

* [x] `getBlocks`
* [x] `getPlayerEntityId`
* [x] `events.pollChatPosts`
* [ ] `player/entity.getRotation`
* [ ] `player/entity.getPitch`
* [ ] `player/entity.getDirection`

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

A list of tiles can be found [here](https://mcpirevival.miraheze.org/wiki/Minecraft:_Pi_Edition_Complete_Block_List) and a list of items [here](https://mcpirevival.miraheze.org/wiki/Minecraft:_Pi_Edition_Complete_Item_List).

### Entities

A list of entities can be found [here](https://mcpirevival.miraheze.org/wiki/Minecraft:_Pi_Edition_Complete_Entity_List).

## Changelog

- **1.2.4**
  - Fix `custom.world.name` while the mod is running on a server

- **1.2.3**, by [Skjeggegubben](https://github.com/Skjeggegubben)
  - Add `custom.entity.setAge` and `custom.entity.setSheepColor`

- **1.2.2**
  - Remove `print()` call left in by mistake
  - Add `entity.getEntities` and `entity.getAllEntities`

- **1.2.1**, by [Red-exe-Engineer](https://github.com/Red-exe-Engineer)
  - Update entity ids and Entity class on the python side
  - Add the sheep class and item class

- **1.2.0**, by [Red-exe-Engineer](https://github.com/Red-exe-Engineer)
  - Add `reborn.getVersion`, `reborn.getFeature`, `events.chat.posts`, and `events.chat.size`

- **1.1.1**
  - Add basic entity spawning.

- **1.1.0**
  - Fixed bug with causing args to be cut off at the first left parenthesis.
  - Fixed bug in `world.getBlocks` and `custom.getBlocks3D` causing them to target the wrong position.
  - Added tests.
  - Improved docs.
  - Many breaking API changes.
  - Many internal changes.
  - Added `custom.player.getHealth`, `custom.player.setHealth`, `custom.player.closeGUI`, and `custom.player.getGamemode`.

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
  - Stopped `getSlot` from crashing the game with invalid ids.
  - Added `press`, `unpress`, `worldName`, `worldDir`, `particle`, `getOffset`, and logging (`debug`, `info`, `warn`, `err`).
  - Uploaded to pypi and github.

- **Beta**
  - Added `getSlot` and `give`.

- **Alpha**
  - Had `getUsername`, `postWithoutPrefix`, and `postClient`.

## Screenshots

Here is a screenshot of using overrides and particles:
![ ](https://i.imgur.com/I8d8I0G_d.webp?maxwidth=760&fidelity=grand)

Here is a screenshot of using entity spawning with arrows and direction:
![ ](https://cdn.discordapp.com/attachments/740287938453045401/1073048085250461746/pic.png)

Here is a screenshot of using TNT entities and falling bedrock entities to make a cannon:
![ ](https://cdn.discordapp.com/attachments/740287938453045401/1073369250984632441/pic.png)
