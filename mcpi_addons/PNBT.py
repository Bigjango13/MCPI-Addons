import json
import os
import struct
import sys

tag = {
    "end": 0,
    "byte": 1,
    "short": 2,
    "int": 3,
    "long": 4,
    "float": 5,
    "double": 6,
    "byte_array": 7,
    "string": 8,
    "list": 9,
    "compound": 10,
    "int_array": 11,
    "long_array": 12,
}

stream = {"data": b"", "offset": 0}


def reset_stream():
    stream["data"] = b""
    stream["offset"] = 0


def read_stream(length):
    stream["offset"] += length
    return stream["data"][stream["offset"] - length : stream["offset"]]


def read(data):
    reset_stream()
    stream["data"] = data
    return read_compound_tag()


def read_file(file_path):
    if os.path.isfile(file_path):
        data = open(file_path, "rb").read()
        file_base_name = os.path.splitext(os.path.basename(file_path))[0]
        if file_base_name == "level":
            return read(data[8:])
        if file_base_name == "entities":
            return read(data[12:])
        return read(data)


def read_compound_tag():
    tree = {}
    while not len(stream["data"]) <= stream["offset"]:
        tag_type = struct.unpack("B", read_stream(1))[0]
        if tag_type == tag["end"]:
            break
        tag_name = read_type(tag["string"])
        tag_value = read_type(tag_type)
        tree[tag_name] = {"type": tag_type, "value": tag_value}
    return tree


def read_type(nbt_type):
    if nbt_type == tag["byte"]:
        return struct.unpack("b", read_stream(1))[0]
    elif nbt_type == tag["short"]:
        return struct.unpack("<h", read_stream(2))[0]
    elif nbt_type == tag["int"]:
        return struct.unpack("<l", read_stream(4))[0]
    elif nbt_type == tag["long"]:
        return struct.unpack("<q", read_stream(8))[0]
    elif nbt_type == tag["float"]:
        return struct.unpack("<f", read_stream(4))[0]
    elif nbt_type == tag["double"]:
        return struct.unpack("<d", read_stream(8))[0]
    elif nbt_type == tag["byte_array"]:
        byte_count = struct.unpack("<l", read_stream(4))[0]
        tag_value = []
        for i in range(0, byte_count):
            tag_value.append(struct.unpack("b", read_stream(1))[0])
        return tag_value
    elif nbt_type == tag["string"]:
        string_length = struct.unpack("<H", read_stream(2))[0]
        return read_stream(string_length).decode("latin1")
    elif nbt_type == tag["list"]:
        list_type = struct.unpack("B", read_stream(1))[0]
        list_item_count = struct.unpack("<l", read_stream(4))[0]
        tag_value = []
        for i in range(0, list_item_count):
            tag_value.append(read_type(list_type))
        return {"type": list_type, "value": tag_value}
    elif nbt_type == tag["compound"]:
        return read_compound_tag()
    elif nbt_type == tag["int_array"]:
        int_count = struct.unpack("<l", read_stream(4))[0]
        tag_value = []
        for i in range(0, int_count):
            tag_value.append(struct.unpack("L", read_stream(4))[0])
        return tag_value
    elif nbt_type == tag["long_array"]:
        long_count = struct.unpack("<l", read_stream(4))[0]
        tag_value = []
        for i in range(0, long_count):
            tag_value.append(struct.unpack("Q", read_stream(8))[0])
        return tag_value
