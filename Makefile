# Needed for ARM cross compiling
CC = arm-linux-gnueabihf-g++
# Normal flags
CFLAGS = -shared -fPIC -Wall -Wextra -O6
# Reborn flags
CFLAGS += -DREBORN_HAS_PATCH_CODE -D_GLIBCXX_USE_CXX11_ABI=0

FILES = src/api.cpp src/inventory.cpp src/buttons.cpp src/world.cpp src/base64.cpp
TARGET = libextrapi.so

all: build

build:
	${CC} ${CFLAGS} ${FILES} -o ${TARGET}

install:
	pip3 install .
	cp ${TARGET} ${HOME}/.minecraft-pi/mods/${TARGET}

uninstall: clean
	rm ${HOME}/.minecraft-pi/mods/${TARGET} -f

clean:
	rm **/__pycache__ **/*.o **/*.so **/*.gch **/a.out **/*.pyc -rf
