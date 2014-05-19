
import struct

__all__ = ['IO_X86', 'IO_X86_64', 'IO_DB']

class _IO(object):
    """IO wrapper for reading / writing micro op fields."""
    def __init__(self, fp):
        self._fp = fp

    def read(self, n):
        return self._fp.read(n)

    def write(self, val):
        self._fp.write(val)

    def read_uint32(self):
        return struct.unpack('I', self.read(4))[0]

    def read_uint64(self):
        return struct.unpack('Q', self.read(8))[0]

    def read_bool(self):
        return struct.unpack('?', self.read(1))[0]

    def write_uint32(self, val):
        self.write(struct.pack('I', val))

    def write_uint64(self, val):
        self.write(struct.pack('Q', val))

    def write_bool(self, val):
        self.write(struct.pack('?', val))

class IO_X86(_IO):
    """Reading / writing micro ops for x86 uses 32-bit types."""
    read_word = _IO.read_uint32
    write_word = _IO.write_uint32

class IO_X86_64(_IO):
    """Reading / writing micro ops for x86-64 uses 64-bit types."""
    read_word = _IO.read_uint64
    write_word = _IO.write_uint64

# When we write to the DB, we just always read/write 64-bit words.
IO_DB = IO_X86_64

