// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ResultFilterOut.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "ResultFilterOut.pb.h"

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

namespace com {
namespace mm {
namespace pb {
namespace po {

namespace {

const ::google::protobuf::Descriptor* ResultFilterOut_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ResultFilterOut_reflection_ = NULL;
const ::google::protobuf::Descriptor* FilterResult_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  FilterResult_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_ResultFilterOut_2eproto() {
  protobuf_AddDesc_ResultFilterOut_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "ResultFilterOut.proto");
  GOOGLE_CHECK(file != NULL);
  ResultFilterOut_descriptor_ = file->message_type(0);
  static const int ResultFilterOut_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ResultFilterOut, resoucelist_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ResultFilterOut, resultcode_),
  };
  ResultFilterOut_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ResultFilterOut_descriptor_,
      ResultFilterOut::default_instance_,
      ResultFilterOut_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ResultFilterOut, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ResultFilterOut, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ResultFilterOut));
  FilterResult_descriptor_ = file->message_type(1);
  static const int FilterResult_offsets_[5] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FilterResult, type1_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FilterResult, type2_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FilterResult, key_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FilterResult, desc_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FilterResult, descphoto_),
  };
  FilterResult_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      FilterResult_descriptor_,
      FilterResult::default_instance_,
      FilterResult_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FilterResult, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(FilterResult, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(FilterResult));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_ResultFilterOut_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ResultFilterOut_descriptor_, &ResultFilterOut::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    FilterResult_descriptor_, &FilterResult::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_ResultFilterOut_2eproto() {
  delete ResultFilterOut::default_instance_;
  delete ResultFilterOut_reflection_;
  delete FilterResult::default_instance_;
  delete FilterResult_reflection_;
}

void protobuf_AddDesc_ResultFilterOut_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\025ResultFilterOut.proto\022\014com.mm.pb.po\"V\n"
    "\017ResultFilterOut\022/\n\013resouceList\030\001 \003(\0132\032."
    "com.mm.pb.po.FilterResult\022\022\n\nresultCode\030"
    "\002 \001(\005\"Z\n\014FilterResult\022\r\n\005type1\030\001 \001(\005\022\r\n\005"
    "type2\030\002 \001(\005\022\013\n\003key\030\003 \001(\t\022\014\n\004desc\030\004 \001(\t\022\021"
    "\n\tdescPhoto\030\005 \001(\tB\021B\017ResultFilterOut", 236);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "ResultFilterOut.proto", &protobuf_RegisterTypes);
  ResultFilterOut::default_instance_ = new ResultFilterOut();
  FilterResult::default_instance_ = new FilterResult();
  ResultFilterOut::default_instance_->InitAsDefaultInstance();
  FilterResult::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_ResultFilterOut_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_ResultFilterOut_2eproto {
  StaticDescriptorInitializer_ResultFilterOut_2eproto() {
    protobuf_AddDesc_ResultFilterOut_2eproto();
  }
} static_descriptor_initializer_ResultFilterOut_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int ResultFilterOut::kResouceListFieldNumber;
const int ResultFilterOut::kResultCodeFieldNumber;
#endif  // !_MSC_VER

ResultFilterOut::ResultFilterOut()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void ResultFilterOut::InitAsDefaultInstance() {
}

ResultFilterOut::ResultFilterOut(const ResultFilterOut& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void ResultFilterOut::SharedCtor() {
  _cached_size_ = 0;
  resultcode_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ResultFilterOut::~ResultFilterOut() {
  SharedDtor();
}

void ResultFilterOut::SharedDtor() {
  if (this != default_instance_) {
  }
}

void ResultFilterOut::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ResultFilterOut::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ResultFilterOut_descriptor_;
}

const ResultFilterOut& ResultFilterOut::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_ResultFilterOut_2eproto();
  return *default_instance_;
}

ResultFilterOut* ResultFilterOut::default_instance_ = NULL;

ResultFilterOut* ResultFilterOut::New() const {
  return new ResultFilterOut;
}

void ResultFilterOut::Clear() {
  if (_has_bits_[1 / 32] & (0xffu << (1 % 32))) {
    resultcode_ = 0;
  }
  resoucelist_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool ResultFilterOut::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated .com.mm.pb.po.FilterResult resouceList = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_resouceList:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_resoucelist()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(10)) goto parse_resouceList;
        if (input->ExpectTag(16)) goto parse_resultCode;
        break;
      }

      // optional int32 resultCode = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_resultCode:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &resultcode_)));
          set_has_resultcode();
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

void ResultFilterOut::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated .com.mm.pb.po.FilterResult resouceList = 1;
  for (int i = 0; i < this->resoucelist_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, this->resoucelist(i), output);
  }

  // optional int32 resultCode = 2;
  if (has_resultcode()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->resultcode(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* ResultFilterOut::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated .com.mm.pb.po.FilterResult resouceList = 1;
  for (int i = 0; i < this->resoucelist_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->resoucelist(i), target);
  }

  // optional int32 resultCode = 2;
  if (has_resultcode()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->resultcode(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int ResultFilterOut::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[1 / 32] & (0xffu << (1 % 32))) {
    // optional int32 resultCode = 2;
    if (has_resultcode()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->resultcode());
    }

  }
  // repeated .com.mm.pb.po.FilterResult resouceList = 1;
  total_size += 1 * this->resoucelist_size();
  for (int i = 0; i < this->resoucelist_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->resoucelist(i));
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

void ResultFilterOut::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const ResultFilterOut* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const ResultFilterOut*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void ResultFilterOut::MergeFrom(const ResultFilterOut& from) {
  GOOGLE_CHECK_NE(&from, this);
  resoucelist_.MergeFrom(from.resoucelist_);
  if (from._has_bits_[1 / 32] & (0xffu << (1 % 32))) {
    if (from.has_resultcode()) {
      set_resultcode(from.resultcode());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void ResultFilterOut::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ResultFilterOut::CopyFrom(const ResultFilterOut& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ResultFilterOut::IsInitialized() const {

  return true;
}

void ResultFilterOut::Swap(ResultFilterOut* other) {
  if (other != this) {
    resoucelist_.Swap(&other->resoucelist_);
    std::swap(resultcode_, other->resultcode_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata ResultFilterOut::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ResultFilterOut_descriptor_;
  metadata.reflection = ResultFilterOut_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int FilterResult::kType1FieldNumber;
const int FilterResult::kType2FieldNumber;
const int FilterResult::kKeyFieldNumber;
const int FilterResult::kDescFieldNumber;
const int FilterResult::kDescPhotoFieldNumber;
#endif  // !_MSC_VER

FilterResult::FilterResult()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void FilterResult::InitAsDefaultInstance() {
}

FilterResult::FilterResult(const FilterResult& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void FilterResult::SharedCtor() {
  _cached_size_ = 0;
  type1_ = 0;
  type2_ = 0;
  key_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  desc_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  descphoto_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

FilterResult::~FilterResult() {
  SharedDtor();
}

void FilterResult::SharedDtor() {
  if (key_ != &::google::protobuf::internal::kEmptyString) {
    delete key_;
  }
  if (desc_ != &::google::protobuf::internal::kEmptyString) {
    delete desc_;
  }
  if (descphoto_ != &::google::protobuf::internal::kEmptyString) {
    delete descphoto_;
  }
  if (this != default_instance_) {
  }
}

void FilterResult::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* FilterResult::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return FilterResult_descriptor_;
}

const FilterResult& FilterResult::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_ResultFilterOut_2eproto();
  return *default_instance_;
}

FilterResult* FilterResult::default_instance_ = NULL;

FilterResult* FilterResult::New() const {
  return new FilterResult;
}

void FilterResult::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    type1_ = 0;
    type2_ = 0;
    if (has_key()) {
      if (key_ != &::google::protobuf::internal::kEmptyString) {
        key_->clear();
      }
    }
    if (has_desc()) {
      if (desc_ != &::google::protobuf::internal::kEmptyString) {
        desc_->clear();
      }
    }
    if (has_descphoto()) {
      if (descphoto_ != &::google::protobuf::internal::kEmptyString) {
        descphoto_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool FilterResult::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int32 type1 = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &type1_)));
          set_has_type1();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_type2;
        break;
      }

      // optional int32 type2 = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_type2:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &type2_)));
          set_has_type2();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_key;
        break;
      }

      // optional string key = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_key:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_key()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->key().data(), this->key().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_desc;
        break;
      }

      // optional string desc = 4;
      case 4: {
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
        if (input->ExpectTag(42)) goto parse_descPhoto;
        break;
      }

      // optional string descPhoto = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_descPhoto:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_descphoto()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->descphoto().data(), this->descphoto().length(),
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

void FilterResult::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional int32 type1 = 1;
  if (has_type1()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->type1(), output);
  }

  // optional int32 type2 = 2;
  if (has_type2()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->type2(), output);
  }

  // optional string key = 3;
  if (has_key()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->key().data(), this->key().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      3, this->key(), output);
  }

  // optional string desc = 4;
  if (has_desc()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->desc().data(), this->desc().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      4, this->desc(), output);
  }

  // optional string descPhoto = 5;
  if (has_descphoto()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->descphoto().data(), this->descphoto().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      5, this->descphoto(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* FilterResult::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional int32 type1 = 1;
  if (has_type1()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->type1(), target);
  }

  // optional int32 type2 = 2;
  if (has_type2()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->type2(), target);
  }

  // optional string key = 3;
  if (has_key()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->key().data(), this->key().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->key(), target);
  }

  // optional string desc = 4;
  if (has_desc()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->desc().data(), this->desc().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        4, this->desc(), target);
  }

  // optional string descPhoto = 5;
  if (has_descphoto()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->descphoto().data(), this->descphoto().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        5, this->descphoto(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int FilterResult::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional int32 type1 = 1;
    if (has_type1()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->type1());
    }

    // optional int32 type2 = 2;
    if (has_type2()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->type2());
    }

    // optional string key = 3;
    if (has_key()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->key());
    }

    // optional string desc = 4;
    if (has_desc()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->desc());
    }

    // optional string descPhoto = 5;
    if (has_descphoto()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->descphoto());
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

void FilterResult::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const FilterResult* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const FilterResult*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void FilterResult::MergeFrom(const FilterResult& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_type1()) {
      set_type1(from.type1());
    }
    if (from.has_type2()) {
      set_type2(from.type2());
    }
    if (from.has_key()) {
      set_key(from.key());
    }
    if (from.has_desc()) {
      set_desc(from.desc());
    }
    if (from.has_descphoto()) {
      set_descphoto(from.descphoto());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void FilterResult::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void FilterResult::CopyFrom(const FilterResult& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool FilterResult::IsInitialized() const {

  return true;
}

void FilterResult::Swap(FilterResult* other) {
  if (other != this) {
    std::swap(type1_, other->type1_);
    std::swap(type2_, other->type2_);
    std::swap(key_, other->key_);
    std::swap(desc_, other->desc_);
    std::swap(descphoto_, other->descphoto_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata FilterResult::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = FilterResult_descriptor_;
  metadata.reflection = FilterResult_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace po
}  // namespace pb
}  // namespace mm
}  // namespace com

// @@protoc_insertion_point(global_scope)
