
import binascii, struct, operator
from util import split_stream
from micro_ops import MicroOpDecoder, OpCodes, OpFlags

__all__ = [ 'InstructionOpsDecoder', 'InstructionOps' ]

class InstructionOpsDecoder(object):
    """Collection of Micro Ops for an instruction."""
    def __init__(self, io):
        self.io = io

    def decode_stream(self):
        """Generator to decode InstructionOps from an io source."""
        def is_newpc(op):
            return op.opcode == OpCodes.NEWPC

        it = MicroOpDecoder(self.io).decode_stream()        
        for ops in split_stream(it, is_newpc):
            yield InstructionOps(ops)


class InstructionOps(object):
    def __init__(self, ops):
        self.ops = ops
        self.flags = reduce(operator.or_, [x.flag for x in self.ops])

    def ops_by_mask(self, opcode_mask):
        """Get a list of micro ops matching specified mask."""
        ops = []
        for op in self.ops:
            if op.flag & opcode_mask:
                ops.append(op) 
        return ops

    @property
    def memread_ops(self):
        """Get memory writes for this instruciton."""
        return self.ops_by_mask(OpFlags.MEMREAD)

    @property
    def memwrite_ops(self):
        """Get memory reads for this instruction."""
        return self.ops_by_mask(OpFlags.MEMWRITE)

    @property
    def pc(self):
        return self.ops[0].pc

    def encode(self, io):
        """Encodes InstructionOps back to binary format."""
        for op in self.ops:
            io.write(chr(op.opcode))
            op.encode(io)

    def _self_check(self):
        """For unit testing."""
        assert len(self.ops) > 0
        assert self.ops[0].opcode == OpCodes.NEWPC
        for op in self.ops[1:]:
            assert op.opcode != OpCodes.NEWPC
        assert len(self.memread_ops) < 3
        assert len(self.memwrite_ops) < 3

