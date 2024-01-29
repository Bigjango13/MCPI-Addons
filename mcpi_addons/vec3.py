class Vec3:
    def __init__(self, x=0, y=0, z=0):
        """
        A three dimensional vector
        """
        self.x = x
        self.y = y
        self.z = z

    def __add__(self, rhs):
        """
        Adds two vectors
        If a scalar is passed, it will be treated as an equal-component vector
        """
        c = self.clone()
        c += rhs
        return c

    def __iadd__(self, rhs):
        """
        Adds two vectors
        If a scalar is passed, it will be treated as an equal-component vector
        """
        if type(rhs) == Vec3:
            self.x += rhs.x
            self.y += rhs.y
            self.z += rhs.z
        else:
            self.x += rhs
            self.y += rhs
            self.z += rhs
        return self

    def __abs__(self):
        """
        Component by component absolute
        Use Vec3.length() if you want the mathematically correct operation
        """
        self.x = abs(self.x)
        self.y = abs(self.y)
        self.z = abs(self.z)
        return self

    def length(self):
        """
        Returns the length of the vector
        """
        return self.lengthSqr() ** .5

    def lengthSqr(self):
        """
        Returns the squared length of the vector
        """
        return self.x * self.x + self.y * self.y  + self.z * self.z

    def __mul__(self, k):
        """
        Vector by scalar multiplication
        If multiplied by a vector, returns the component by component product
        """
        c = self.clone()
        c *= k
        return c

    def __imul__(self, k):
        """
        Vector by scalar multiplication
        If multiplied by a vector, returns the component by component product
        """
        if type(k) == Vec3:
            self.x *= k.x
            self.y *= k.y
            self.z *= k.z
        else:
            self.x *= k
            self.y *= k
            self.z *= k
        return self

    def __div__(self, k):
        """
        Vector by scalar division
        If divided by a vector, returns the component by component quotient
        """
        c = self.clone()
        c /= k
        return c

    def __idiv__(self, k):
        """
        Vector by scalar division
        If divided by a vector, returns the component by component quotient
        """
        if type(k) == Vec3:
            self.x /= k.x
            self.y /= k.y
            self.z /= k.z
        else:
            self.x /= k
            self.y /= k
            self.z /= k
        return self
    
    def clone(self):
        """
        Creates a new vector with the same values
        """
        return Vec3(self.x, self.y, self.z)

    def __neg__(self):
        """
        Negates the vector
        """
        return Vec3(-self.x, -self.y, -self.z)

    def __sub__(self, rhs):
        """
        Subtracts two vectors
        If a scalar is passed, it will be treated as an equal-component vector
        """
        return self.__add__(-rhs)

    def __isub__(self, rhs):
        """
        Subtracts two vectors
        If a scalar is passed, it will be treated as an equal-component vector
        """
        return self.__iadd__(-rhs)

    def __repr__(self):
        return "Vec3(%s,%s,%s)"%(self.x,self.y,self.z)

    def __iter__(self):
        return iter((self.x, self.y, self.z))

    def _map(self, func):
        """
        Maps the vector according to a function
        Use instead of map(func, vector)
        """
        self.x = func(self.x)
        self.y = func(self.y)
        self.z = func(self.z)
        
    def __eq__(self, rhs):
        if self.x == rhs.x and self.y == rhs.y and self.z == rhs.z:
            return True
        return False

    def __round__(self):
        c = self.clone()
        c._map(round)
        return c

    def dot(self, rhs):
        """
        Performs a dot product
        """
        return self.x * rhs.x + self.y * rhs.y + self.z * rhs.z

    def cross(self, rhs):
        """
        Performs a cross product
        """
        a = self
        b = rhs
        return Vec3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.z * b.z,
            a.x * b.y - a.y * b.x
        )

    def normalize(self):
        d = 1.0 / self.length()
        return self * d
        
    def iround(self): self._map(lambda v:int(v+0.5))
    def ifloor(self): self._map(int)

    def rotateLeft(self):  self.x, self.z = self.z, -self.x
    def rotateRight(self): self.x, self.z = -self.z, self.x

def testVec3():
    # Note: It's not testing everything

    # 1.1 Test initialization
    it = Vec3(1, -2, 3)
    assert it.x == 1
    assert it.y == -2
    assert it.z == 3

    assert it.x != -1
    assert it.y != +2
    assert it.z != -3

    # 2.1 Test cloning and equality
    clone = it.clone()
    assert it == clone
    it.x += 1
    assert it != clone

    # 3.1 Arithmetic
    a = Vec3(10, -3, 4)
    b = Vec3(-7, 1, 2)
    c = a + b
    assert c - a == b
    assert c - b == a
    assert a + a == a * 2

    assert a - a == Vec3(0,0,0)
    assert a + (-a) == Vec3(0,0,0)

    # Test repr
    e = eval(repr(it))
    assert e == it

if __name__ == "__main__":
    testVec3()
