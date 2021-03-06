// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: messageNum.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "messageNum.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace tutorial {

namespace {

const ::google::protobuf::Descriptor* MessageNum_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  MessageNum_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_messageNum_2eproto() {
  protobuf_AddDesc_messageNum_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "messageNum.proto");
  GOOGLE_CHECK(file != NULL);
  MessageNum_descriptor_ = file->message_type(0);
  static const int MessageNum_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MessageNum, num_),
  };
  MessageNum_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      MessageNum_descriptor_,
      MessageNum::default_instance_,
      MessageNum_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MessageNum, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MessageNum, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(MessageNum));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_messageNum_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    MessageNum_descriptor_, &MessageNum::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_messageNum_2eproto() {
  delete MessageNum::default_instance_;
  delete MessageNum_reflection_;
}

void protobuf_AddDesc_messageNum_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\020messageNum.proto\022\010tutorial\"\031\n\nMessageN"
    "um\022\013\n\003num\030\001 \002(\005B\037\n\013com.ii.pbpoB\020MessageN"
    "umReturn", 88);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "messageNum.proto", &protobuf_RegisterTypes);
  MessageNum::default_instance_ = new MessageNum();
  MessageNum::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_messageNum_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_messageNum_2eproto {
  StaticDescriptorInitializer_messageNum_2eproto() {
    protobuf_AddDesc_messageNum_2eproto();
  }
} static_descriptor_initializer_messageNum_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int MessageNum::kNumFieldNumber;
#endif  // !_MSC_VER

MessageNum::MessageNum()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void MessageNum::InitAsDefaultInstance() {
}

MessageNum::MessageNum(const MessageNum& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void MessageNum::SharedCtor() {
  _cached_size_ = 0;
  num_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

MessageNum::~MessageNum() {
  SharedDtor();
}

void MessageNum::SharedDtor() {
  if (this != default_instance_) {
  }
}

void MessageNum::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* MessageNum::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return MessageNum_descriptor_;
}

const MessageNum& MessageNum::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_messageNum_2eproto();
  return *default_instance_;
}

MessageNum* MessageNum::default_instance_ = NULL;

MessageNum* MessageNum::New() const {
  return new MessageNum;
}

void MessageNum::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    num_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool MessageNum::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 num = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &num_)));
          set_has_num();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void MessageNum::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required int32 num = 1;
  if (has_num()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->num(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* MessageNum::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required int32 num = 1;
  if (has_num()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->num(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int MessageNum::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 num = 1;
    if (has_num()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->num());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void MessageNum::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const MessageNum* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const MessageNum*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void MessageNum::MergeFrom(const MessageNum& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_num()) {
      set_num(from.num());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void MessageNum::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void MessageNum::CopyFrom(const MessageNum& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool MessageNum::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void MessageNum::Swap(MessageNum* other) {
  if (other != this) {
    std::swap(num_, other->num_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata MessageNum::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = MessageNum_descriptor_;
  metadata.reflection = MessageNum_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace tutorial

// @@protoc_insertion_point(global_scope)
