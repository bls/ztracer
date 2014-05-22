
import struct, binascii

__all__ = [ 'OpCodes', 'OpFlags', 'MicroOpDecoder' ]

class OpCodes(object):
    NEWPC = 0x01
    MEMREAD = 0x02
    MEMWRITE = 0x04
    CALL = 0x20
    RET = 0x40


class OpFlags(object):
    NONE = 0x00
    MEMREAD = 0x01
    MEMWRITE = 0x02
    CALL = 0x04
    RET = 0x08
    MASK = 0xFFFF


class OP_NEWPC(object):
    opcode = OpCodes.NEWPC
    flag = OpFlags.NONE
    
    def __init__(self, pc):
        self.pc = pc

    def __str__(self):
        return "OP_NEWPC pc=%x" % self.pc

    @staticmethod
    def decode(io):
        pc = io.read_word()
        return OP_NEWPC(pc)

    def encode(self, io):
        io.write_word(self.pc)


class OP_MEMREAD(object):
    opcode = OpCodes.MEMREAD
    flag = OpFlags.MEMREAD

    def __init__(self, ea, size):
        self.ea = ea
        self.size = size

    def __str__(self):
        return "OP_MEMREAD ea=%x size=%d" % \
               (self.ea, self.size)

    @staticmethod
    def decode(io):
        ea = io.read_word()
        size = io.read_uint32()
        return OP_MEMREAD(ea, size)

    def encode(self, io):
        io.write_word(self.ea)
        io.write_uint32(self.size)


class OP_MEMWRITE(object):
    opcode = OpCodes.MEMWRITE
    flag = OpFlags.MEMWRITE

    def __init__(self, ea, data):
        self.ea = ea
        self.data = data
        self.size = len(data)

    def __str__(self):
        return "OP_MEMWRITE ea=%x size=%d data='%s'" % \
               (self.ea, self.size, binascii.hexlify(self.data))

    @staticmethod
    def decode(io):
        ea = io.read_word()
        size = io.read_uint32()
        data = io.read(size)
        return OP_MEMWRITE(ea, data)

    def encode(self, io):
        io.write_word(self.ea)
        io.write_uint32(self.size)
        io.write(self.data)


class OP_CALL(object):
    opcode = OpCodes.CALL
    flag = OpFlags.CALL
    
    def __init__(self, target, sp, is_direct_call):
        self.target = target
        self.sp = sp
        self.is_direct_call = is_direct_call

    def __str__(self):
        return "OP_CALL target=%x sp=%x is_call_direct=%d" %\
               (self.target, self.sp, self.is_direct_call)

    @staticmethod
    def decode(io):
        target = io.read_word()
        sp = io.read_word()
        is_direct_call = io.read_bool()
        return OP_CALL(target, sp, is_direct_call)

    def encode(self, io):
        io.write_word(self.target)
        io.write_bool(self.is_direct_call)


class OP_RET(object):
    opcode = OpCodes.RET
    flag = OpFlags.RET
    
    def __init__(self, sp):
        self.sp = sp

    def __str__(self):
        return "OP_RET sp=%x" % (self.sp)

    @staticmethod
    def decode(io):
        sp = io.read_word()
        return OP_RET(sp)

    def encode(self, io):
        io.write_word(self.sp)


class MicroOpDecoder(object):
    opcode_to_class = {
        OpCodes.NEWPC:    OP_NEWPC,
        OpCodes.MEMREAD:  OP_MEMREAD,
        OpCodes.MEMWRITE: OP_MEMWRITE,
        OpCodes.CALL:     OP_CALL,
        OpCodes.RET:      OP_RET
    }

    def __init__(self, io):
        self.io = io

    def decode_stream(self):
        """Generator to decode micro ops from an io source."""
        io = self.io
        result = None

        while True:
            opcode = io.read(1)
            if not opcode:
                break
            else:
                opcode = ord(opcode)

            klass = MicroOpDecoder.opcode_to_class.get(opcode)
            yield klass.decode(io)
