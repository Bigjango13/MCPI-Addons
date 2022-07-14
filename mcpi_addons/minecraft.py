import math
from base64 import b64encode, b64decode
from getpass import getuser
from os.path import exists

from .block import Block
from .connection import Connection
from .entity import Entity
from .event import BlockEvent, ChatEvent, ProjectileEvent
from .PNBT import read_file
from .util import flatten
from .vec3 import Vec3

""" Minecraft Pi low level api v0.1.1 with addons.

    Note: many methods have the parameter *arg. This solution makes it
    simple to allow different types, and variable number of arguments.
    The actual magic is a mix of flatten_parameters() and __iter__. Example:
    A Cube class could implement __iter__ to work in Minecraft.setBlocks(c, id).

    (Because of this, it's possible to "erase" arguments. CmdPlayer removes
     entityId, by injecting [] that flattens to nothing)

    @author: Aron Nieminen, Mojang AB"""


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
        CmdPositioner.__init__(self, connection, b"entity")

    def getEntities(self):
        """Return a list of entities near entity => [[entityId:int,entityTypeId:int,entityTypeName:str,posX:float,posY:float,posZ:float]]"""
        """If distanceFromPlayerInBlocks:int is not specified then default 10 blocks will be used"""
        s = self.conn.sendReceive(b"entity.getEntities")
        entities = [e for e in s.split("|") if e]
        return [
            [
                int(n.split(",")[0]),
                int(n.split(",")[1]),
                n.split(",")[2],
                float(n.split(",")[3]),
                float(n.split(",")[4]),
                float(n.split(",")[5]),
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

    def pollBlockHits(self):
        """Only triggered by sword => [BlockEvent]"""
        s = self.conn.sendReceive(b"events.block.hits")
        events = [e for e in s.split("|") if e]
        return [BlockEvent.Hit(*list(map(int, e.split(",")))) for e in events]


class Minecraft:
    """The main class to interact with a running instance of Minecraft Pi."""

    def __init__(self, connection):
        self.conn = connection

        self.basepath = "~/.minecraft-pi/games/com.mojang/minecraftWorlds/"

        self.camera = CmdCamera(connection)
        self.entity = CmdEntity(connection)
        self.player = CmdPlayer(connection)
        self.events = CmdEvents(connection)

    def getBlock(self, *args):
        """Get block (x,y,z) => id:int"""
        return int(self.conn.sendReceive(b"world.getBlock", intFloor(args)))

    def getBlockWithData(self, *args):
        """Get block with data (x,y,z) => Block"""
        ans = self.conn.sendReceive(b"world.getBlockWithData", intFloor(args))
        return Block(*list(map(int, ans.split(","))))

    def getBlocks(self, *args):
        """Get a cuboid of blocks (x0,y0,z0,x1,y1,z1) => [id:int]"""
        s = self.conn.sendReceive(b"world.getBlocks", intFloor(args))
        with open(s, "r") as f:
            return map(int, f.read().split(","))

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
        self.conn.send(b"custom.postClient", msg)

    def postWithoutPrefix(self, msg):
        """Post a message to chat without the username prefix"""
        self.conn.send(b"custom.postWithoutPrefix", msg)

    def getUsername(self):
        """Gets the players username"""
        return self.conn.sendReceive(b"custom.getUsername")

    def getUsernames(self):
        """Gets the all the players usernames"""
        usernames = self.conn.sendReceive(b"custom.getUsernames")
        return list(
            map(
                lambda i: b64decode(i.encode("latin-1")).decode("latin-1"),
                usernames.split(", "),
            )
        )[:-1]

    def getSlot(self):
        ret = self.conn.sendReceive(b"custom.getSlot").split("|")
        return {"id": ret[0], "auxiliary": ret[1], "count": ret[2]}

    def give(self, id, auxiliary=-2, count=-2):
        """Sets the current slot to something else"""
        args = "|".join(map(str, [id, auxiliary, count]))
        self.conn.sendReceive(b"custom.give", args)

    def press(self, key):
        """Presses a key"""
        self.conn.send(b"custom.press", key.upper())

    def unpress(self, key):
        """Releases a key"""
        self.conn.send(b"custom.unpress", key.upper())

    def worldName(self):
        return self.conn.sendReceive(b"custom.worldName")

    def worldDir(self):
        return self.conn.sendReceive(b"custom.worldDir")

    def particle(self, x, y, z, particle):
        """Spawns a particle"""
        args = "|".join(map(str, [str(particle).lower(), float(x), float(y), float(z)]))
        self.conn.send(b"custom.particle", args)

    def inventory(self):
        """Opens the inventory"""
        self.conn.send(b"custom.inventory")

    def overrideTile(self, before, after):
        """Overrides a tile"""
        self.conn.send(b"custom.overrideTile", before, after)

    def overrideItem(self, before, after):
        """Overrides a item"""
        self.conn.send(b"custom.overrideItem", before, after)

    def resetOverrides(self):
        """Resets tile and item overrides"""
        self.conn.send(b"custom.resetOverrides")

    def getBlocks3D(self, x, y, z, x2, y2, z2):
        """Get a cuboid of blocks in a way that preserves the 3D structure (x0,y0,z0,x1,y1,z1) => [[[id:int]]]"""
        s = self.conn.sendReceive(b"custom.getBlocks3D", intFloor([x, y, z, x2, y2, z2]))
        with open(s, "r") as f:
            data = f.read().strip().split(";")

        data = [i.split('|') for i in data]
        data = [[list(map(int, j.split(','))) for j in i] for i in data]
        return data

    def debug(self, msg):
        """Makes MCPI print a debug message"""
        self.conn.send(b"custom.debug", msg)

    def info(self, msg):
        """Makes MCPI print a info message"""
        self.conn.send(b"custom.info", msg)

    def warn(self, msg):
        """Makes MCPI print a warn message"""
        self.conn.send(b"custom.warn", msg)

    def err(self, msg):
        """Makes MCPI print a err message"""
        self.conn.send(b"custom.err", msg)

    def setting(self, setting, status):
        """Set a world setting (setting, status). keys: world_immutable, nametags_visible"""
        self.conn.send(b"world.setting", setting, 1 if bool(status) else 0)

    @staticmethod
    def create(address="localhost", port=4711):
        return Minecraft(Connection(address, port))


if __name__ == "__main__":
    mc = Minecraft.create()
    mc.postToChat("Hello, Minecraft!")
