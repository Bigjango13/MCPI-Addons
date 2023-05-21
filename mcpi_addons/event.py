from .vec3 import Vec3


class BlockEvent:
    """An Event related to blocks (e.g. placed, removed, hit)"""

    HIT = 0

    def __init__(self, type, x, y, z, face, entityId):
        self.type = type
        self.pos = Vec3(x, y, z)
        self.face = face
        self.entityId = entityId

    def __repr__(self):
        sType = {BlockEvent.HIT: "BlockEvent.HIT"}.get(self.type, "???")

        return "BlockEvent(%s, %d, %d, %d, %d, %d)" % (
            sType,
            self.pos.x,
            self.pos.y,
            self.pos.z,
            self.face,
            self.entityId,
        )

    @staticmethod
    def Hit(x, y, z, face, entityId):
        return BlockEvent(BlockEvent.HIT, x, y, z, face, entityId)


class ChatEvent:
    """An Event related to chat (e.g. posts)"""

    def __init__(self, player, message):
        self.player = player
        self.message = message

    def __repr__(self):
        return f'ChatEvent({self.player}, {self.message})')

    def __str__(self):
        return self.message


class ProjectileEvent:
    """An Event related to projectiles (e.g. placed, removed, hit)"""

    HIT = 0

    def __init__(self, type, x, y, z, face, originName, targetName):
        self.type = type
        self.pos = Vec3(x, y, z)
        self.face = face
        self.originName = originName
        self.targetName = targetName

    def __repr__(self):
        sType = {ProjectileEvent.HIT: "ProjectileEvent.HIT"}.get(self.type, "???")

        return "ProjectileEvent(%s, %d, %d, %d, %d, %s, %s)" % (
            sType,
            self.pos.x,
            self.pos.y,
            self.pos.z,
            self.face,
            self.originName,
            self.targetName,
        )

    @staticmethod
    def Hit(x, y, z, face, originName, targetName):
        return ProjectileEvent(BlockEvent.HIT, x, y, z, face, originName, targetName)
