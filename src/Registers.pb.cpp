// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "Registers.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace ztrace {

namespace {

const ::google::protobuf::EnumDescriptor* Reg_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_Registers_2eproto() {
  protobuf_AddDesc_Registers_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "Registers.proto");
  GOOGLE_CHECK(file != NULL);
  Reg_descriptor_ = file->enum_type(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_Registers_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
}

}  // namespace

void protobuf_ShutdownFile_Registers_2eproto() {
}

void protobuf_AddDesc_Registers_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\017Registers.proto\022\006ztrace*\201\007\n\003Reg\022\020\n\014REG"
    "_INVALID_\020\000\022\013\n\007REG_EDI\020\001\022\013\n\007REG_ESI\020\002\022\013\n"
    "\007REG_EBP\020\003\022\013\n\007REG_ESP\020\004\022\013\n\007REG_EBX\020\005\022\013\n\007"
    "REG_EDX\020\006\022\013\n\007REG_ECX\020\007\022\013\n\007REG_EAX\020\010\022\016\n\nR"
    "EG_SEG_CS\020\t\022\016\n\nREG_SEG_SS\020\n\022\016\n\nREG_SEG_D"
    "S\020\013\022\016\n\nREG_SEG_ES\020\014\022\016\n\nREG_SEG_FS\020\r\022\016\n\nR"
    "EG_SEG_GS\020\016\022\016\n\nREG_EFLAGS\020\017\022\013\n\007REG_EIP\020\020"
    "\022\n\n\006REG_AL\020\021\022\n\n\006REG_AH\020\022\022\n\n\006REG_AX\020\023\022\n\n\006"
    "REG_CL\020\024\022\n\n\006REG_CH\020\025\022\n\n\006REG_CX\020\026\022\n\n\006REG_"
    "DL\020\027\022\n\n\006REG_DH\020\030\022\n\n\006REG_DX\020\031\022\n\n\006REG_BL\020\032"
    "\022\n\n\006REG_BH\020\033\022\n\n\006REG_BX\020\034\022\n\n\006REG_BP\020\035\022\n\n\006"
    "REG_SI\020\036\022\n\n\006REG_DI\020\037\022\n\n\006REG_SP\020 \022\r\n\tREG_"
    "FLAGS\020!\022\n\n\006REG_IP\020\"\022\013\n\007REG_MM0\020#\022\013\n\007REG_"
    "MM1\020$\022\013\n\007REG_MM2\020%\022\013\n\007REG_MM3\020&\022\013\n\007REG_M"
    "M4\020\'\022\013\n\007REG_MM5\020(\022\013\n\007REG_MM6\020)\022\013\n\007REG_MM"
    "7\020*\022\014\n\010REG_EMM0\020+\022\014\n\010REG_EMM1\020,\022\014\n\010REG_E"
    "MM2\020-\022\014\n\010REG_EMM3\020.\022\014\n\010REG_EMM4\020/\022\014\n\010REG"
    "_EMM5\0200\022\014\n\010REG_EMM6\0201\022\014\n\010REG_EMM7\0202\022\014\n\010R"
    "EG_XMM0\0203\022\014\n\010REG_XMM1\0204\022\014\n\010REG_XMM2\0205\022\014\n"
    "\010REG_XMM3\0206\022\014\n\010REG_XMM4\0207\022\014\n\010REG_XMM6\0208\022"
    "\014\n\010REG_XMM7\0209\022\014\n\010REG_YMM0\020:\022\014\n\010REG_YMM1\020"
    ";\022\014\n\010REG_YMM2\020<\022\014\n\010REG_YMM3\020=\022\014\n\010REG_YMM"
    "4\020>\022\014\n\010REG_YMM5\020\?\022\014\n\010REG_YMM6\020@\022\014\n\010REG_Y"
    "MM7\020A", 925);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "Registers.proto", &protobuf_RegisterTypes);
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_Registers_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_Registers_2eproto {
  StaticDescriptorInitializer_Registers_2eproto() {
    protobuf_AddDesc_Registers_2eproto();
  }
} static_descriptor_initializer_Registers_2eproto_;

const ::google::protobuf::EnumDescriptor* Reg_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Reg_descriptor_;
}
bool Reg_IsValid(int value) {
  switch(value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
    case 64:
    case 65:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace ztrace

// @@protoc_insertion_point(global_scope)