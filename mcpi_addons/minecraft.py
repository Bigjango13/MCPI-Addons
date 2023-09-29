import math
from base64 import b64encode, b64decode
from getpass import getuser
from os.path import exists

from .block import Block
from .connection import Connection
from .entity import Entity
from .event import BlockEvent
from .PNBT import read_file
from .util import flatten
from .vec3 import Vec3

"""
    Minecraft Pi low level api v0.1.1 with addons.

    Note: many methods have the parameter *arg. This solution makes it
    simple to allow different types, and variable number of arguments.
    The actual magic is a mix of flatten_parameters() and __iter__. Example:
    A Cube class could implement __iter__ to work in Minecraft.setBlocks(c, id).

    (Because of this, it's possible to "erase" arguments. CmdPlayer removes
     entityId, by injecting [] that flattens to nothing)

    @author: Aron Nieminen, Mojang AB
"""


def intFloor(*args):
    return [int(math.floor(x)) for x in flatten(args)]


class CmdPositioner:
    """Methods for setting and getting positions"""
    def __init__(self, connection, packagePrefix):
        self.conn = connection
        self.pkg = packagePrefix

    def getPos(self, id):
        """Get entity position (entityId:int) => Vec3"""
        s = self.conn.sendReceive(self.pkg + b".getPos", id)
        return Vec3(*list(map(float, s.split(","))))

    def setPos(self, id, *args):
        """Set entity position (entityId:int, x,y,z)"""
        self.conn.send(self.pkg + b".setPos", id, args)

    def getTilePos(self, id):
        """Get entity tile position (entityId:int) => Vec3"""
        s = self.conn.sendReceive(self.pkg + b".getTile", id)
        return Vec3(*list(map(int, s.split(","))))

    def setTilePos(self, id, *args):
        """Set entity tile position (entityId:int) => Vec3"""
        self.conn.send(self.pkg + b".setTile", id, intFloor(*args))

    def setting(self, setting, status):
        """Set a player setting (setting, status). keys: autojump"""
        self.conn.send(self.pkg + b".setting", setting, 1 if bool(status) else 0)


class CmdEntity(CmdPositioner):
    """Methods for entities"""
    def __init__(self, connection):
        # https://mcpirevival.miraheze.org/wiki/Minecraft:_Pi_Edition_Complete_Entity_List
        self.nameMap = {
            # Misc
            0: "Unknown",
            # Animals
            10: "Chicken", 11: "Cow", 12: "Pig", 13: "Sheep",
            # Monsters
            32: "Zombie", 33: "Creeper", 34: "Skeleton", 35: "Spider", 36: "PigZombie",
            # Item/blocks
            64: "ItemEntity", 65: "PrimedTnt", 66: "FallingTile",
            # Projectiles (and paintings)
            80: "Arrow", 81: "Snowball", 82: "ThrownEgg", 83: "Painting"
        }
        CmdPositioner.__init__(self, connection, b"entity")

    def getEntities(self, id=0, distance=10, typeId=-1):
        """
            Return a list of entities near entity => [[entityId:int,entityTypeId:int,entityTypeName:str,posX:float,posY:float,posZ:float
            If distanceFromPlayerInBlocks:int is not specified then default 10 blocks will be used
        """
        s = None;
        if id == 0 and typeId == -1:
           s = self.conn.sendReceive(b"entity.getAllEntities")
        else:
            s = self.conn.sendReceive(b"entity.getEntities", int(id), float(distance), int(typeId))

        entities = [e.split(',') for e in s.split("|") if e]
        return [
            [
                # Entity ID
                int(n[0]),
                # Entity type ID
                int(n[1]),
                # Entity type name
                self.nameMap.get(int(n[1]), "Invalid"),
                # X
                float(n[2]),
                # Y
                float(n[3]),
                # Z
                float(n[4])
            ]
            for n in entities
        ]

    def pollBlockHits(self, *args):
        """Only triggered by sword => [BlockEvent]"""
        s = self.conn.sendReceive(b"entity.events.block.hits", intFloor(args))
        events = [e for e in s.split("|") if e]
        return [BlockEvent.Hit(*list(map(int, e.split(",")))) for e in events]

    def clearEvents(self, *args):
        """Clear the entities events"""
        self.conn.send(b"entity.events.clear", intFloor(args))

    def spawn(self, id, x, y, z, health = -1, dir = (0, 0), data = 0):
        """Spawn entity"""
        if isinstance(id, Entity):
            # `data` overrides `id.data`
            id, data = id.id, data or id.data

        return int(
            self.conn.sendReceive(
                b"custom.entity.spawn",
                int(id), float(x), float(y), float(z),
                int(health), float(dir[0]), float(dir[1]),
                int(data)
            )
        )

    def setSheepColor(self, id, color):
        """Set color on a specific sheep, mc.entity.setSheepColor(id, 2)"""
        self.conn.send(b"custom.entity.setSheepColor", int(id), int(color))

    def setAge(self, id, age):
        """Set age on a specific animal mob (only animals) mc.entity.setAge(id, -20 * X) to change to a baby for X seconds"""
        self.conn.send(b"custom.entity.setAge", int(id), int(age))


class CmdPlayer(CmdPositioner):
    """Methods for the host (Raspberry Pi) player"""

    def __init__(self, connection):
        CmdPositioner.__init__(self, connection, b"player")
        self.conn = connection

    def getPos(self):
        return CmdPositioner.getPos(self, [])

    def setPos(self, *args):
        return CmdPositioner.setPos(self, [], args)

    def getTilePos(self):
        return CmdPositioner.getTilePos(self, [])

    def setTilePos(self, *args):
        return CmdPositioner.setTilePos(self, [], args)

    def pollBlockHits(self):
        """Only triggered by sword => [BlockEvent]"""
        s = self.conn.sendReceive(b"player.events.block.hits")
        events = [e for e in s.split("|") if e]
        return [BlockEvent.Hit(*list(map(int, e.split(",")))) for e in events]

    def clearEvents(self):
        """Clear the players events"""
        self.conn.send(b"player.events.clear")

    def getUsername(self):
        """Gets the players username"""
        return b64decode(self.conn.sendReceive(b"custom.username")).decode('latin1')

    def press(self, key):
        """Presses a key"""
        self.conn.send(b"custom.key.press", key.upper())

    def release(self, key):
        """Releases a key"""
        self.conn.send(b"custom.key.release", key.upper())

    def getHealth(self):
        return int(self.conn.sendReceive(b"custom.player.getHealth"))

    def setHealth(self, health):
        return self.conn.send(b"custom.player.setHealth", health)

    def getGamemode(self):
        return int(self.conn.sendReceive(b"custom.player.getGamemode"))

    def closeGUI(self):
        return self.conn.send(b"custom.player.closeGUI")

class CmdCamera:
    def __init__(self, connection):
        self.conn = connection

    def setNormal(self, *args):
        """Set camera mode to normal Minecraft view ([entityId])"""
        self.conn.send(b"camera.mode.setNormal", args)

    def setFixed(self):
        """Set camera mode to fixed view"""
        self.conn.send(b"camera.mode.setFixed")

    def setFollow(self, *args):
        """Set camera mode to follow an entity ([entityId])"""
        self.conn.send(b"camera.mode.setFollow", args)

    def setPos(self, *args):
        """Set camera entity position (x,y,z)"""
        self.conn.send(b"camera.setPos", args)


class CmdEvents:
    """Events"""

    def __init__(self, connection):
        self.conn = connection

    def clearAll(self):
        """Clear all old events"""
        self.conn.send(b"events.clear")

    def pollChatPosts(self):
        """Gets displayed messages => [string]"""
        ret = self.conn.sendReceive(b"events.chat.posts")
        if ret == '':
            return []
        return ret.split("\0")

    def setChatLog(self, size: int = 64):
        """Clears the log and sets a log size"""
        self.conn.send(b"events.chat.size", int(size))

    def pollBlockHits(self):
        """Only triggered by sword => [BlockEvent]"""
        s = self.conn.sendReceive(b"events.block.hits")
        events = [e for e in s.split("|") if e]
        return [BlockEvent.Hit(*list(map(int, e.split(",")))) for e in events]

class CmdInventory:
    """Inventory"""

    def __init__(self, connection):
        self.conn = connection

    def getHeldItem(self):
        ret = list(map(int, self.conn.sendReceive(b"custom.inventory.getSlot").split("|")))
        return {"id": ret[0], "auxiliary": ret[1], "count": ret[2]}

    def unsafeGive(self, id=-2, auxiliary=-2, count=-2):
        """Sets the current slot to something else, this is unsafe and may crash the game"""
        if isinstance(id, Block):
            id, auxiliary = id.id, (id.data if auxiliary == -2 else auxiliary)

        args = "|".join(map(str, [id, auxiliary, count]))
        self.conn.sendReceive(b"custom.inventory.unsafeGive", args)

    def give(self, id=-2, auxiliary=-2, count=-2):
        """Sets the current slot to something else"""
        if isinstance(id, Block):
            id, auxiliary = id.id, (id.data if auxiliary == -2 else auxiliary)

        args = "|".join(map(str, [id, auxiliary, count]))
        self.conn.sendReceive(b"custom.inventory.give", args)

class CmdReborn:
    """Reborn"""

    def __init__(self, connection):
        self.conn = connection

    def getVersion(self):
        """Returns the game's title => str"""
        return self.conn.sendReceive(b"custom.reborn.version")

    def getFeature(self, feature):
        """Returns wheater feature exists and is enabled => bool"""
        return self.conn.sendReceive(b"custom.reborn.feature", feature) == "true"


class CmdLog:
    """Logging"""

    def __init__(self, connection):
        self.conn = connection

    def debug(self, msg):
        """Makes MCPI print a debug message"""
        self.conn.send(b"custom.log.debug", msg)

    def info(self, msg):
        """Makes MCPI print a info message"""
        self.conn.send(b"custom.log.info", msg)

    def warn(self, msg):
        """Makes MCPI print a warn message"""
        self.conn.send(b"custom.log.warn", msg)

    def err(self, msg):
        """Makes MCPI print a err message"""
        self.conn.send(b"custom.log.err", msg)

class CmdWorld:
    """World"""

    def __init__(self, connection):
        self.conn = connection

    def name(self):
        return self.conn.sendReceive(b"custom.world.name")

    def dir(self):
        return self.conn.sendReceive(b"custom.world.dir")

class Minecraft:
    """The main class to interact with a running instance of Minecraft Pi."""

    def __init__(self, connection):
        self.conn = connection

        self.basepath = "~/.minecraft-pi/games/com.mojang/minecraftWorlds/"

        self.camera    = CmdCamera(connection)
        self.entity    = CmdEntity(connection)
        self.player    = CmdPlayer(connection)
        self.events    = CmdEvents(connection)
        self.inventory = CmdInventory(connection)
        self.logging   = CmdLog(connection)
        self.world     = CmdWorld(connection)
        self.reborn    = CmdReborn(connection)

    def getBlock(self, *args):
        """Get block (x,y,z) => id:int"""
        return int(self.conn.sendReceive(b"world.getBlock", intFloor(args)))

    def getBlockWithData(self, *args):
        """Get block with data (x,y,z) => Block"""
        ans = self.conn.sendReceive(b"world.getBlockWithData", intFloor(args))
        return Block(*list(map(int, ans.split(","))))

    def getBlocks(self, x, y, z, x2, y2, z2):
        """Get a cuboid of blocks (x,y,z,x2,y2,z2) => [id:int]"""
        s = self.conn.sendReceive(b"world.getBlocks", intFloor(args))
        with open(s, "r") as f:
            return map(int, f.read().split(","))

    def getBlocks3D(self, x, y, z, x2, y2, z2):
        """Get a cuboid of blocks in a way that preserves the 3D structure (x,y,z,x2,y2,z2) => [[[id:int]]]"""
        s = self.conn.sendReceive(b"world.getBlocks.3D", intFloor([x, y, z, x2, y2, z2]))
        with open(s, "r") as f:
            data = f.read().strip().split(";")

        data = [i.split('|') for i in data]
        data = [[list(map(int, j.split(','))) for j in i] for i in data]
        return data

    def setBlock(self, x, y, z, id, data=0):
        """Set block (x,y,z,id,[data])"""
        self.conn.send(b"world.setBlock", intFloor([x, y, z, id, data]))

    def setBlocks(self, *args):
        """Set a cuboid of blocks (x0,y0,z0,x1,y1,z1,id,[data])"""
        self.conn.send(b"world.setBlocks", intFloor(args))

    def getHeight(self, *args):
        """Get the height of the world (x,z) => int"""
        return int(self.conn.sendReceive(b"world.getHeight", intFloor(args)))

    def getPlayerEntityIds(self):
        """Get the entity ids of the connected players => [id:int]"""
        ids = self.conn.sendReceive(b"world.getPlayerIds")
        if len(ids) == 0:
            return []
        return list(map(int, ids.split("|")))

    def getPlayerEntityId(self, name):
        """Get the entity id of the named player => [id:int]"""
        return int(
            self.conn.sendReceive(
                b"world.getPlayerId",
                b64encode(name.encode("latin-1")).decode("latin-1"),
            )
        )

    def saveCheckpoint(self):
        """Save a checkpoint that can be used for restoring the world"""
        self.conn.send(b"world.checkpoint.save")

    def restoreCheckpoint(self):
        """Restore the world state to the checkpoint"""
        self.conn.send(b"world.checkpoint.restore")

    def postToChat(self, msg):
        """Post a message to the game chat"""
        self.conn.send(b"chat.post", msg)

    def postToClient(self, msg):
        """Post a message to the client side game chat"""
        self.conn.send(b"custom.post.client", msg)

    def postWithoutPrefix(self, msg):
        """Post a message to chat without the username prefix"""
        self.conn.send(b"custom.post.noPrefix", msg)

    def getUsernames(self):
        """Gets the all the players usernames"""
        usernames = self.conn.sendReceive(b"custom.username.all")
        return list(
            map(
                lambda i: b64decode(i.encode("latin-1")).decode("latin-1"),
                usernames.split(", "),
            )
        )[:-1]

    def particle(self, x, y, z, particle):
        """Spawns a particle"""
        args = "|".join(map(str, [str(particle).lower(), float(x), float(y), float(z)]))
        self.conn.send(b"custom.world.particle", args)

    def inventory(self):
        """Opens the inventory"""
        self.conn.send(b"custom.inventory")

    def override(self, before, after):
        """Overrides a tile or item"""
        if isinstance(before, Block):
            before = before.id

        if isinstance(after, Block):
            after = after.id

        self.conn.send(b"custom.override", before, after)

    def resetOverrides(self):
        """Resets tile and item overrides"""
        self.conn.send(b"custom.override.reset")

    def setting(self, setting, status):
        """Set a world setting (setting, status). keys: world_immutable, nametags_visible"""
        self.conn.send(b"world.setting", setting, 1 if bool(status) else 0)

    @staticmethod
    def create(address="localhost", port=4711):
        return Minecraft(Connection(address, port))


if __name__ == "__main__":
    mc = Minecraft.create()
    mc.postToChat("Hello, Minecraft!")
