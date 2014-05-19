
import struct, ZTrace_pb2, snappy, contextlib
from util import event_type
import cStringIO as StringIO
from micro_ops_io import *
from instruction_ops import *

__all__ = ['TraceFileReader']

class EventHandle(object):
    def __init__(self, bytes, offset):
        self.bytes = bytes
        self.offset = offset
        self.event = ZTrace_pb2.Event()
        self.event.ParseFromString(self.bytes)

    # TODO: it would actually be better to include
    # the event type in the "header" portion (outside
    # of the protobuf); this would make filtering a
    # lot faster because we could avoid deserializing
    # the protobufs for uninteresting message types.

def read_uint32(fp):
    return struct.unpack('I', fp.read(4))[0]

def read_uint64(fp):
    return struct.unpack('Q', fp.read(8))[0]

class TraceFileReader(object):
    def __init__(self, path):
        self.path = path
        self.fp = open(self.path, 'rb')
        self.arch = self._get_arch()

    def count_events(self):
        """Count events from file. This skips parsing any data so
        should be quite fast. Useful for progress bars etc."""
        count = 0
        self.fp.seek(0)
        while True:
            size = self._read_event_size()
            if not size:
                break
            self.fp.seek(size, 1) # Skip data
            count += 1
        return count

    def events(self):
        """Return an ordered sequence of events from in this trace.
        What actually gets returned is an EventHandle, so you have
        to do eh.event to get to the event data."""
        self.fp.seek(0)
        while True:
            eh = self._read_event()
            if not eh:
                break
            yield eh

    def event_at(self, offset):
        """Seek to the specified offset and read an event. The offset
        must point to the start of the encoded message length in the
        trace file."""
        self.fp.seek(offset)
        return self._read_event()
         
    def _get_arch(self):
        """Read the architecture of the trace. Currently, this is one
        of the following values:

        ZTrace_pb2.ARCH_X86    (32 bit x86)
        ZTrace_pb2.ARCH_X86_64 (64 bit x86)
        """
        eh = self.event_at(0)
        assert event_type(eh.event) == 'trace_start', \
            "Could not determine trace architecture!"
        return eh.event.trace_start.arch

    def _read_event(self):
        """Read an event message from fp. The fp is assumed to
        be aligned to the start of the message length."""
        offset = self.fp.tell()
        size = self._read_event_size()
        if size is None:
            return None
        event_bytes = self.fp.read(size)
        return EventHandle(event_bytes, offset)

    def _read_event_size(self):
        """Read event message lenth from current filepos; it's encoded
        in network byte order."""
        t = self.fp.read(4)
        if not t:
            return None
        size = struct.unpack('!I', t)[0]
        return size

    def _io_for_arch(self):
        """Get the micro op IO type (32 or 64 bit) for the current trace.
        Based on the architecture declared in trace_start event."""
        if self.arch == ZTrace_pb2.ARCH_X86:
            return IO_X86
        elif self.arch == ZTrace_pb2.ARCH_X86_64:
            return IO_X86_64
        else:
            assert 0, "Unknown trace file architecture!"

    def decode_micro_ops(self, event):
        """Parses the data field of a MicroEventLog event and returns
        a sequence of micro ops. """
        assert event.HasField('micro_event_log')
        io_class = self._io_for_arch()
        fp = StringIO.StringIO(snappy.decompress(event.micro_event_log.data))
        with contextlib.closing(fp):
            for op in MicroOpDecoder(io_class(fp)).decode_stream():
                yield op

    def decode_ins_ops(self, event):
        """Parses the data field of a MicroEventLog event and returns
        a sequence of instruction ops (micro ops, grouped by instruction)."""
        assert event.HasField('micro_event_log')
        io_class = self._io_for_arch()
        fp = StringIO.StringIO(snappy.decompress(event.micro_event_log.data))
        with contextlib.closing(fp):
            for ins_op in InstructionOpsDecoder(io_class(fp)).decode_stream():
                yield ins_op
