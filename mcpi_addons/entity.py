"""
    Modified entity.py - Wallee/Red-exe-Engineer
    This modified library removes unused entity constants and fixes broken ones.
    It also adds a Sheep and Tile class as well:
        Sheep is used for summoning dyed sheep entities.
        Tile is dynamically created based on block.py and is used for summoning tile drop entities.
"""

from . import block


class Entity:
    """ Minecraft PI entity description. Can be sent to Minecraft.entity.spawn """

    def __init__(self, id, name, data=0):
        self.id = id
        self.name = name
        self.data = data

    def __cmp__(self, rhs):
        return hash(self) - hash(rhs)

    def __eq__(self, rhs):
        return self.id == rhs.id

    def __hash__(self):
        return self.id

    def __iter__(self):
        """ Allows an Entity to be sent whenever id is needed """

        return iter((self.id,))

    def __repr__(self):
        return f'Entity({self.name} {self.id})'


class Sheep(Entity):
    """ Minecraft PI sheep description. Can be send to Minecraft.entity.spawn """

    def __init__(self, data, color):
        super().__init__(13, color, data)

    def __repr__(self):
        return f'Sheep({self.name}, {self.id}, {self.data})'


class Item(Entity):
    def __init__(self, data, name):
        super().__init__(64, name, data)

    def __repr__(self):
        return f'Item({self.name}, {self.id})'


# Passive
CHICKEN          = Entity(10, "CHICKEN")
COW              = Entity(11, "COW")
PIG              = Entity(12, "PIG")

# Sheep
WHITE_SHEEP      = Sheep(0, "WHITE")
ORANGE_SHEEP     = Sheep(1, "ORANGE")
MAGENTA_SHEEP    = Sheep(2, "MAGENTA")
LIGHT_BLUE_SHEEP = Sheep(3, "LIGHT_BLUE")
YELLOW_SHEEP     = Sheep(4, "YELLOW")
LIME_SHEEP       = Sheep(5, "LIME")
PINK_SHEEP       = Sheep(6, "PINK")
GRAY_SHEEP       = Sheep(7, "GRAY")
LIGHT_GRAY_SHEEP = Sheep(8, "LIGHT_GRAY")
CYAN_SHEEP       = Sheep(9, "CYAN")
PURPLE_SHEEP     = Sheep(10, "PURPLE")
BLUE_SHEEP       = Sheep(11, "BLUE")
BROWN_SHEEP      = Sheep(12, "BROWN")
GREEN_SHEEP      = Sheep(13, "GREEN")
RED_SHEEP        = Sheep(14, "RED")
BLACK_SHEEP      = Sheep(15, "BLACK")

SHEEP = WHITE_SHEEP

# Hostile
ZOMBIE           = Entity(32, "ZOMBIE")
CREEPER          = Entity(33, "CREEPER")
SKELETON         = Entity(34, "SKELETON")
SPIDER           = Entity(35, "SPIDER")
PIG_ZOMBIE       = Entity(36, "PIG_ZOMBIE")

# Tiles
TNT              = Entity(65, "TNT")
ARROW            = Entity(80, "ARROW")
SNOWBALL         = Entity(81, "SNOWBALL")
EGG              = Entity(82, "EGG")

# Item tiles
for name in block.__dir__():
    if name.isupper():
        tile = getattr(block, name)
        globals()[f'ITEM_{name}'] = Item(tile.id, name)

ITEM = ITEM_STONE

# Setup variables
del block, name, tile
