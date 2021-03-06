// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: sendStatus.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "sendStatus.pb.h"

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

const ::google::protobuf::Descriptor* SendStatus_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  SendStatus_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_sendStatus_2eproto() {
  protobuf_AddDesc_sendStatus_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "sendStatus.proto");
  GOOGLE_CHECK(file != NULL);
  SendStatus_descriptor_ = file->message_type(0);
  static const int SendStatus_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SendStatus, status_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SendStatus, desc_),
  };
  SendStatus_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      SendStatus_descriptor_,
      SendStatus::default_instance_,
      SendStatus_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SendStatus, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(SendStatus, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(SendStatus));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_sendStatus_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    SendStatus_descriptor_, &SendStatus::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_sendStatus_2eproto() {
  delete SendStatus::default_instance_;
  delete SendStatus_reflection_;
}

void protobuf_AddDesc_sendStatus_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\020sendStatus.proto\022\010tutorial\"*\n\nSendStat"
    "us\022\016\n\006status\030\001 \002(\005\022\014\n\004desc\030\002 \002(\tB\032\n\013com."
    "ii.pbpoB\013ReturnValue", 100);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "sendStatus.proto", &protobuf_RegisterTypes);
  SendStatus::default_instance_ = new SendStatus();
  SendStatus::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_sendStatus_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_sendStatus_2eproto {
  StaticDescriptorInitializer_sendStatus_2eproto() {
    protobuf_AddDesc_sendStatus_2eproto();
  }
} static_descriptor_initializer_sendStatus_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int SendStatus::kStatusFieldNumber;
const int SendStatus::kDescFieldNumber;
#endif  // !_MSC_VER

SendStatus::SendStatus()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void SendStatus::InitAsDefaultInstance() {
}

SendStatus::SendStatus(const SendStatus& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void SendStatus::SharedCtor() {
  _cached_size_ = 0;
  status_ = 0;
  desc_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

SendStatus::~SendStatus() {
  SharedDtor();
}

void SendStatus::SharedDtor() {
  if (desc_ != &::google::protobuf::internal::kEmptyString) {
    delete desc_;
  }
  if (this != default_instance_) {
  }
}

void SendStatus::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* SendStatus::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return SendStatus_descriptor_;
}

const SendStatus& SendStatus::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_sendStatus_2eproto();
  return *default_instance_;
}

SendStatus* SendStatus::default_instance_ = NULL;

SendStatus* SendStatus::New() const {
  return new SendStatus;
}

void SendStatus::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    status_ = 0;
    if (has_desc()) {
      if (desc_ != &::google::protobuf::internal::kEmptyString) {
        desc_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool SendStatus::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 status = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &status_)));
          set_has_status();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_desc;
        break;
      }

      // required string desc = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_desc:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_desc()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->desc().data(), this->desc().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
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

void SendStatus::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required int32 status = 1;
  if (has_status()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->status(), output);
  }

  // required string desc = 2;
  if (has_desc()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->desc().data(), this->desc().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->desc(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* SendStatus::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required int32 status = 1;
  if (has_status()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->status(), target);
  }

  // required string desc = 2;
  if (has_desc()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->desc().data(), this->desc().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->desc(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int SendStatus::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 status = 1;
    if (has_status()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->status());
    }

    // required string desc = 2;
    if (has_desc()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->desc());
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

void SendStatus::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const SendStatus* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const SendStatus*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void SendStatus::MergeFrom(const SendStatus& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_status()) {
      set_status(from.status());
    }
    if (from.has_desc()) {
      set_desc(from.desc());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void SendStatus::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void SendStatus::CopyFrom(const SendStatus& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SendStatus::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;

  return true;
}

void SendStatus::Swap(SendStatus* other) {
  if (other != this) {
    std::swap(status_, other->status_);
    std::swap(desc_, other->desc_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata SendStatus::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = SendStatus_descriptor_;
  metadata.reflection = SendStatus_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace tutorial

// @@protoc_insertion_point(global_scope)
