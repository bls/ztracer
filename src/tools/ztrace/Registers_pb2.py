# Generated by the protocol buffer compiler.  DO NOT EDIT!

from google.protobuf import descriptor
from google.protobuf import message
from google.protobuf import reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)



DESCRIPTOR = descriptor.FileDescriptor(
  name='Registers.proto',
  package='ztrace',
  serialized_pb='\n\x0fRegisters.proto\x12\x06ztrace*\x81\x07\n\x03Reg\x12\x10\n\x0cREG_INVALID_\x10\x00\x12\x0b\n\x07REG_EDI\x10\x01\x12\x0b\n\x07REG_ESI\x10\x02\x12\x0b\n\x07REG_EBP\x10\x03\x12\x0b\n\x07REG_ESP\x10\x04\x12\x0b\n\x07REG_EBX\x10\x05\x12\x0b\n\x07REG_EDX\x10\x06\x12\x0b\n\x07REG_ECX\x10\x07\x12\x0b\n\x07REG_EAX\x10\x08\x12\x0e\n\nREG_SEG_CS\x10\t\x12\x0e\n\nREG_SEG_SS\x10\n\x12\x0e\n\nREG_SEG_DS\x10\x0b\x12\x0e\n\nREG_SEG_ES\x10\x0c\x12\x0e\n\nREG_SEG_FS\x10\r\x12\x0e\n\nREG_SEG_GS\x10\x0e\x12\x0e\n\nREG_EFLAGS\x10\x0f\x12\x0b\n\x07REG_EIP\x10\x10\x12\n\n\x06REG_AL\x10\x11\x12\n\n\x06REG_AH\x10\x12\x12\n\n\x06REG_AX\x10\x13\x12\n\n\x06REG_CL\x10\x14\x12\n\n\x06REG_CH\x10\x15\x12\n\n\x06REG_CX\x10\x16\x12\n\n\x06REG_DL\x10\x17\x12\n\n\x06REG_DH\x10\x18\x12\n\n\x06REG_DX\x10\x19\x12\n\n\x06REG_BL\x10\x1a\x12\n\n\x06REG_BH\x10\x1b\x12\n\n\x06REG_BX\x10\x1c\x12\n\n\x06REG_BP\x10\x1d\x12\n\n\x06REG_SI\x10\x1e\x12\n\n\x06REG_DI\x10\x1f\x12\n\n\x06REG_SP\x10 \x12\r\n\tREG_FLAGS\x10!\x12\n\n\x06REG_IP\x10\"\x12\x0b\n\x07REG_MM0\x10#\x12\x0b\n\x07REG_MM1\x10$\x12\x0b\n\x07REG_MM2\x10%\x12\x0b\n\x07REG_MM3\x10&\x12\x0b\n\x07REG_MM4\x10\'\x12\x0b\n\x07REG_MM5\x10(\x12\x0b\n\x07REG_MM6\x10)\x12\x0b\n\x07REG_MM7\x10*\x12\x0c\n\x08REG_EMM0\x10+\x12\x0c\n\x08REG_EMM1\x10,\x12\x0c\n\x08REG_EMM2\x10-\x12\x0c\n\x08REG_EMM3\x10.\x12\x0c\n\x08REG_EMM4\x10/\x12\x0c\n\x08REG_EMM5\x10\x30\x12\x0c\n\x08REG_EMM6\x10\x31\x12\x0c\n\x08REG_EMM7\x10\x32\x12\x0c\n\x08REG_XMM0\x10\x33\x12\x0c\n\x08REG_XMM1\x10\x34\x12\x0c\n\x08REG_XMM2\x10\x35\x12\x0c\n\x08REG_XMM3\x10\x36\x12\x0c\n\x08REG_XMM4\x10\x37\x12\x0c\n\x08REG_XMM6\x10\x38\x12\x0c\n\x08REG_XMM7\x10\x39\x12\x0c\n\x08REG_YMM0\x10:\x12\x0c\n\x08REG_YMM1\x10;\x12\x0c\n\x08REG_YMM2\x10<\x12\x0c\n\x08REG_YMM3\x10=\x12\x0c\n\x08REG_YMM4\x10>\x12\x0c\n\x08REG_YMM5\x10?\x12\x0c\n\x08REG_YMM6\x10@\x12\x0c\n\x08REG_YMM7\x10\x41')

_REG = descriptor.EnumDescriptor(
  name='Reg',
  full_name='ztrace.Reg',
  filename=None,
  file=DESCRIPTOR,
  values=[
    descriptor.EnumValueDescriptor(
      name='REG_INVALID_', index=0, number=0,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_EDI', index=1, number=1,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_ESI', index=2, number=2,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_EBP', index=3, number=3,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_ESP', index=4, number=4,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_EBX', index=5, number=5,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_EDX', index=6, number=6,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_ECX', index=7, number=7,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_EAX', index=8, number=8,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_SEG_CS', index=9, number=9,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_SEG_SS', index=10, number=10,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_SEG_DS', index=11, number=11,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_SEG_ES', index=12, number=12,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_SEG_FS', index=13, number=13,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_SEG_GS', index=14, number=14,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_EFLAGS', index=15, number=15,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_EIP', index=16, number=16,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_AL', index=17, number=17,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_AH', index=18, number=18,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_AX', index=19, number=19,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_CL', index=20, number=20,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_CH', index=21, number=21,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_CX', index=22, number=22,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_DL', index=23, number=23,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_DH', index=24, number=24,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_DX', index=25, number=25,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_BL', index=26, number=26,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_BH', index=27, number=27,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_BX', index=28, number=28,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_BP', index=29, number=29,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_SI', index=30, number=30,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_DI', index=31, number=31,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_SP', index=32, number=32,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_FLAGS', index=33, number=33,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_IP', index=34, number=34,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_MM0', index=35, number=35,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_MM1', index=36, number=36,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_MM2', index=37, number=37,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_MM3', index=38, number=38,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_MM4', index=39, number=39,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_MM5', index=40, number=40,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_MM6', index=41, number=41,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_MM7', index=42, number=42,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_EMM0', index=43, number=43,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_EMM1', index=44, number=44,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_EMM2', index=45, number=45,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_EMM3', index=46, number=46,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_EMM4', index=47, number=47,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_EMM5', index=48, number=48,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_EMM6', index=49, number=49,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_EMM7', index=50, number=50,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_XMM0', index=51, number=51,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_XMM1', index=52, number=52,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_XMM2', index=53, number=53,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_XMM3', index=54, number=54,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_XMM4', index=55, number=55,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_XMM6', index=56, number=56,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_XMM7', index=57, number=57,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_YMM0', index=58, number=58,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_YMM1', index=59, number=59,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_YMM2', index=60, number=60,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_YMM3', index=61, number=61,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_YMM4', index=62, number=62,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_YMM5', index=63, number=63,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_YMM6', index=64, number=64,
      options=None,
      type=None),
    descriptor.EnumValueDescriptor(
      name='REG_YMM7', index=65, number=65,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=28,
  serialized_end=925,
)


REG_INVALID_ = 0
REG_EDI = 1
REG_ESI = 2
REG_EBP = 3
REG_ESP = 4
REG_EBX = 5
REG_EDX = 6
REG_ECX = 7
REG_EAX = 8
REG_SEG_CS = 9
REG_SEG_SS = 10
REG_SEG_DS = 11
REG_SEG_ES = 12
REG_SEG_FS = 13
REG_SEG_GS = 14
REG_EFLAGS = 15
REG_EIP = 16
REG_AL = 17
REG_AH = 18
REG_AX = 19
REG_CL = 20
REG_CH = 21
REG_CX = 22
REG_DL = 23
REG_DH = 24
REG_DX = 25
REG_BL = 26
REG_BH = 27
REG_BX = 28
REG_BP = 29
REG_SI = 30
REG_DI = 31
REG_SP = 32
REG_FLAGS = 33
REG_IP = 34
REG_MM0 = 35
REG_MM1 = 36
REG_MM2 = 37
REG_MM3 = 38
REG_MM4 = 39
REG_MM5 = 40
REG_MM6 = 41
REG_MM7 = 42
REG_EMM0 = 43
REG_EMM1 = 44
REG_EMM2 = 45
REG_EMM3 = 46
REG_EMM4 = 47
REG_EMM5 = 48
REG_EMM6 = 49
REG_EMM7 = 50
REG_XMM0 = 51
REG_XMM1 = 52
REG_XMM2 = 53
REG_XMM3 = 54
REG_XMM4 = 55
REG_XMM6 = 56
REG_XMM7 = 57
REG_YMM0 = 58
REG_YMM1 = 59
REG_YMM2 = 60
REG_YMM3 = 61
REG_YMM4 = 62
REG_YMM5 = 63
REG_YMM6 = 64
REG_YMM7 = 65



# @@protoc_insertion_point(module_scope)
