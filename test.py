from mcpi_addons.minecraft import Minecraft
from mcpi_addons.vec3 import Vec3
from sys import exit

mc = None
try:
    mc = Minecraft.create()
except ConnectionError:
    exit("Failed to connect to MCPI, is it running?")

tests = []
def test_func(errmsg, cls=""):
    """Tests a function"""
    def test_wrapper(func):
        def inner():
            fname = func.__name__
            fname = cls + "." + fname if cls else fname
            print("Testing " + fname + "...", end=" ", flush=True)
            try:
                func()
            except Exception as e:
                if cls:
                    print("Failed To", func.__doc__)
                else:
                    print(errmsg)
                if e.__str__().strip():
                    print(e)
                return
            print("Done!")
        tests.append(inner)
        return func
    return test_wrapper

def test_cls(cls):
    """Tests every function in a class"""
    for i in list(cls.__dict__.items()):
        if type(i[1]) == type(lambda:42):
            test_func(i[1].__doc__, cls.__name__)(i[1])
    return cls

@test_cls
class test_player:
    def username():
        """Test getUsername"""
        assert type(mc.player.getUsername()) == type("")

    def keys():
        """Test press/release"""
        mc.player.press("SPACE")
        mc.player.release("SPACE")

    def gamemode():
        """Test getGamemode"""
        assert mc.player.getGamemode() == 1

@test_cls
class test_override:
    def tileInvalid():
        """Override a tile with an invalid tile"""
        mc.override(1, 19)

    def tileValid():
        """Override a tile with a tile"""
        mc.override(2, 3)

    def tileItem():
        """Override a tile with an item"""
        mc.override(4, 256)

    def itemInvalid():
        """Override an item with an invalid item"""
        mc.override(257, 400)

    def itemValid():
        """Override a tile with a tile"""
        mc.override(258, 270)

    def itemTile():
        """Override an item with an tile"""
        mc.override(280, 49)

    def reset():
        mc.resetOverrides()

@test_cls
class test_world:
    def dir():
        assert type(mc.world.dir()) == type("")

    def name():
        assert type(mc.world.name()) == type("")

    def particle():
        mc.particle(0, 0, 0, "largesmoke")

# Run the tests
for test in tests:
    test()
