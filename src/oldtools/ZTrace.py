
import ZTrace_pb2
import struct, argparse, cStringIO, snappy, binascii

OP_NEWPC    = 0x01
OP_MEMREAD  = 0x02
OP_MEMWRITE = 0x04
OP_REGWRITE = 0x08
OP_XMMWRITE = 0x10
OP_CALL     = 0x20
OP_RET      = 0x40

# TODO: subclassing?  Really, OP_CALL + OP_RET are just diff from
# other forms of MicroEvent ...
# TODO: fix this monstrosity, which is an egregious hack.
class MicroEvent(object):
    def __init__(self, type, pc, ea=None, data=None, size=None, target=None, sp=None, is_direct_call=None):
        self.type = type
        self.pc = pc
        self.ea = ea
        self.data = data
        self.size = size
        self.target = target
        self.sp = sp
        self.is_direct_call = is_direct_call
        if data is not None:
            assert self.size == len(self.data)
            
    def __str__(self):
        if self.type == OP_NEWPC:
            return 'PC %x' % (self.pc)
        elif self.type == OP_MEMREAD:
            return 'MR %x %x %d' % (self.pc, self.ea, self.size)
        elif self.type == OP_MEMWRITE:
            return 'MW %x %x "%s" (%d)' % (self.pc, self.ea, binascii.hexlify(self.data), self.size)
        elif self.type == OP_REGWRITE:
            return 'RW TBD'
        elif self.type == OP_XMMWRITE:
            return 'XW TBD'
        elif self.type == OP_CALL:
            return 'CALL %x %x %d' % (self.target, self.sp, self.is_direct_call)
        elif self.type == OP_RET:
            return 'RET %x' % (self.sp)
        else:
            assert 0

    def op_name(self):
        ops = {
            0x01: 'OP_NEWPC',
            0x02: 'OP_MEMREAD',
            0x04: 'OP_MEMWRITE',
            0x08: 'OP_REGWRITE',
            0x10: 'OP_XMMWRITE',
            0x20: 'OP_CALL',
            0x40: 'OP_RET'
        }
        return ops[self.type]

def read_uint32(fp):
    return struct.unpack('I', fp.read(4))[0]


def read_uint64(fp):
    return struct.unpack('Q', fp.read(8))[0]

def read_bool(fp):
    return struct.unpack('?', fp.read(1))[0]


class TraceFile(object):
    def __init__(self, path):
        self.path = path
        self.arch = None

    def events(self):
        """Return an ordered sequence of Events from in this trace."""
        msg = ZTrace_pb2.Event()
        with open(self.path) as fp:
            while fp:
                # Read packet length; length is encoded
                # in network byte order
                t = fp.read(4)
                if not t:
                    break # Termination
                size = struct.unpack('!I', t)[0]

                # Read packet and parse into protobuf
                data = fp.read(size)
                msg.ParseFromString(data)

                if msg.HasField('trace_start'):
                    self.arch = msg.trace_start.arch

                yield msg

    def decode_micro_events(self, msg):
        """Parses the data field of a MicroEventLog event and returns a sequence
        of MicroEvent messages."""
        if self.arch == ZTrace_pb2.ARCH_X86:
            read_ptr = read_uint32
        elif self.arch == ZTrace_pb2.ARCH_X86_64:
            read_ptr = read_uint64
        else:
            read_ptr = None  # TBD, die?

        if msg.HasField('micro_event_log'):
            fp = cStringIO.StringIO(snappy.decompress(msg.micro_event_log.data))
            curr_pc = 0
            ent = None
            while True:
                rec_type = fp.read(1)
                if not rec_type:
                    break
                else:
                    rec_type = ord(rec_type)

                if rec_type == OP_NEWPC:
                    pc = read_ptr(fp)
                    ent = MicroEvent(type=OP_NEWPC, pc=pc)
                    curr_pc = pc
                elif rec_type == OP_MEMREAD:
                    ea = read_ptr(fp)
                    size = read_uint32(fp)
                    ent = MicroEvent(type=OP_MEMREAD, pc=curr_pc, ea=ea, size=size)
                elif rec_type == OP_MEMWRITE:
                    ea = read_ptr(fp)
                    size = read_uint32(fp)
                    wdata = fp.read(size)
                    ent = MicroEvent(type=OP_MEMWRITE, pc=curr_pc, ea=ea, data=wdata, size=size)
                elif rec_type == OP_REGWRITE:
                    assert 0 # TBD
                elif rec_type == OP_XMMWRITE:
                    assert 0 # TBD
                elif rec_type == OP_CALL:
                    target = read_ptr(fp)
                    sp = read_ptr(fp)
                    is_direct_call = read_bool(fp)
                    ent = MicroEvent(type=OP_CALL, pc=curr_pc, target=target, sp=sp, is_direct_call=is_direct_call)
                elif rec_type == OP_RET:
                   sp = read_ptr(fp)
                   ent = MicroEvent(type=OP_RET, pc=curr_pc, sp=sp)
                else:
                    print "%x" % (ord(rec_type))
                    assert 0
        
                yield ent
        
    def get_event_type(self, msg):
        """Returns the field name in the Event which is not pid or tid; we
        expect this to look like"trace_start", "trace_end", etc.
        """
        top_level_required_fields = ['id', 'pid', 'tid', 'cross_ref']
        field_name = None
        for desc, val in msg.ListFields():
            if desc.name in top_level_required_fields:
                continue
            # We don't expect multiple types in one event message
            assert field_name is None
            field_name = desc.name
        return field_name
