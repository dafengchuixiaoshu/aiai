// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: OutputResultData.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "OutputResultData.pb.h"

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

const ::google::protobuf::Descriptor* OutputResultData_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  OutputResultData_reflection_ = NULL;
const ::google::protobuf::Descriptor* ReturnUser_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ReturnUser_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_OutputResultData_2eproto() {
  protobuf_AddDesc_OutputResultData_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "OutputResultData.proto");
  GOOGLE_CHECK(file != NULL);
  OutputResultData_descriptor_ = file->message_type(0);
  static const int OutputResultData_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(OutputResultData, list_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(OutputResultData, resultcode_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(OutputResultData, resultmsg_),
  };
  OutputResultData_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      OutputResultData_descriptor_,
      OutputResultData::default_instance_,
      OutputResultData_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(OutputResultData, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(OutputResultData, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(OutputResultData));
  ReturnUser_descriptor_ = file->message_type(1);
  static const int ReturnUser_offsets_[8] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ReturnUser, id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ReturnUser, faceurl_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ReturnUser, username_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ReturnUser, gender_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ReturnUser, facevedio_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ReturnUser, longitude_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ReturnUser, latitude_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ReturnUser, nickname_),
  };
  ReturnUser_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ReturnUser_descriptor_,
      ReturnUser::default_instance_,
      ReturnUser_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ReturnUser, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ReturnUser, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ReturnUser));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_OutputResultData_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    OutputResultData_descriptor_, &OutputResultData::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ReturnUser_descriptor_, &ReturnUser::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_OutputResultData_2eproto() {
  delete OutputResultData::default_instance_;
  delete OutputResultData_reflection_;
  delete ReturnUser::default_instance_;
  delete ReturnUser_reflection_;
}

void protobuf_AddDesc_OutputResultData_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\026OutputResultData.proto\022\014com.mm.pb.po\"a"
    "\n\020OutputResultData\022&\n\004list\030\001 \003(\0132\030.com.m"
    "m.pb.po.ReturnUser\022\022\n\nresultCode\030\002 \001(\005\022\021"
    "\n\tresultMSG\030\003 \001(\t\"\225\001\n\nReturnUser\022\n\n\002id\030\001"
    " \001(\005\022\017\n\007faceUrl\030\002 \001(\t\022\020\n\010username\030\003 \001(\t\022"
    "\016\n\006gender\030\004 \001(\005\022\021\n\tfaceVedio\030\005 \001(\t\022\021\n\tlo"
    "ngitude\030\006 \001(\001\022\020\n\010latitude\030\007 \001(\001\022\020\n\010nickN"
    "ame\030\010 \001(\tB\023B\021OutputResultData1", 310);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "OutputResultData.proto", &protobuf_RegisterTypes);
  OutputResultData::default_instance_ = new OutputResultData();
  ReturnUser::default_instance_ = new ReturnUser();
  OutputResultData::default_instance_->InitAsDefaultInstance();
  ReturnUser::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_OutputResultData_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_OutputResultData_2eproto {
  StaticDescriptorInitializer_OutputResultData_2eproto() {
    protobuf_AddDesc_OutputResultData_2eproto();
  }
} static_descriptor_initializer_OutputResultData_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int OutputResultData::kListFieldNumber;
const int OutputResultData::kResultCodeFieldNumber;
const int OutputResultData::kResultMSGFieldNumber;
#endif  // !_MSC_VER

OutputResultData::OutputResultData()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void OutputResultData::InitAsDefaultInstance() {
}

OutputResultData::OutputResultData(const OutputResultData& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void OutputResultData::SharedCtor() {
  _cached_size_ = 0;
  resultcode_ = 0;
  resultmsg_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

OutputResultData::~OutputResultData() {
  SharedDtor();
}

void OutputResultData::SharedDtor() {
  if (resultmsg_ != &::google::protobuf::internal::kEmptyString) {
    delete resultmsg_;
  }
  if (this != default_instance_) {
  }
}

void OutputResultData::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* OutputResultData::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return OutputResultData_descriptor_;
}

const OutputResultData& OutputResultData::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_OutputResultData_2eproto();
  return *default_instance_;
}

OutputResultData* OutputResultData::default_instance_ = NULL;

OutputResultData* OutputResultData::New() const {
  return new OutputResultData;
}

void OutputResultData::Clear() {
  if (_has_bits_[1 / 32] & (0xffu << (1 % 32))) {
    resultcode_ = 0;
    if (has_resultmsg()) {
      if (resultmsg_ != &::google::protobuf::internal::kEmptyString) {
        resultmsg_->clear();
      }
    }
  }
  list_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool OutputResultData::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated .com.mm.pb.po.ReturnUser list = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_list:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_list()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(10)) goto parse_list;
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
        if (input->ExpectTag(26)) goto parse_resultMSG;
        break;
      }

      // optional string resultMSG = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_resultMSG:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_resultmsg()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->resultmsg().data(), this->resultmsg().length(),
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

void OutputResultData::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated .com.mm.pb.po.ReturnUser list = 1;
  for (int i = 0; i < this->list_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, this->list(i), output);
  }

  // optional int32 resultCode = 2;
  if (has_resultcode()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->resultcode(), output);
  }

  // optional string resultMSG = 3;
  if (has_resultmsg()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->resultmsg().data(), this->resultmsg().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      3, this->resultmsg(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* OutputResultData::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated .com.mm.pb.po.ReturnUser list = 1;
  for (int i = 0; i < this->list_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->list(i), target);
  }

  // optional int32 resultCode = 2;
  if (has_resultcode()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->resultcode(), target);
  }

  // optional string resultMSG = 3;
  if (has_resultmsg()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->resultmsg().data(), this->resultmsg().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->resultmsg(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int OutputResultData::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[1 / 32] & (0xffu << (1 % 32))) {
    // optional int32 resultCode = 2;
    if (has_resultcode()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->resultcode());
    }

    // optional string resultMSG = 3;
    if (has_resultmsg()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->resultmsg());
    }

  }
  // repeated .com.mm.pb.po.ReturnUser list = 1;
  total_size += 1 * this->list_size();
  for (int i = 0; i < this->list_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->list(i));
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

void OutputResultData::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const OutputResultData* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const OutputResultData*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void OutputResultData::MergeFrom(const OutputResultData& from) {
  GOOGLE_CHECK_NE(&from, this);
  list_.MergeFrom(from.list_);
  if (from._has_bits_[1 / 32] & (0xffu << (1 % 32))) {
    if (from.has_resultcode()) {
      set_resultcode(from.resultcode());
    }
    if (from.has_resultmsg()) {
      set_resultmsg(from.resultmsg());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void OutputResultData::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void OutputResultData::CopyFrom(const OutputResultData& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool OutputResultData::IsInitialized() const {

  return true;
}

void OutputResultData::Swap(OutputResultData* other) {
  if (other != this) {
    list_.Swap(&other->list_);
    std::swap(resultcode_, other->resultcode_);
    std::swap(resultmsg_, other->resultmsg_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata OutputResultData::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = OutputResultData_descriptor_;
  metadata.reflection = OutputResultData_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int ReturnUser::kIdFieldNumber;
const int ReturnUser::kFaceUrlFieldNumber;
const int ReturnUser::kUsernameFieldNumber;
const int ReturnUser::kGenderFieldNumber;
const int ReturnUser::kFaceVedioFieldNumber;
const int ReturnUser::kLongitudeFieldNumber;
const int ReturnUser::kLatitudeFieldNumber;
const int ReturnUser::kNickNameFieldNumber;
#endif  // !_MSC_VER

ReturnUser::ReturnUser()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void ReturnUser::InitAsDefaultInstance() {
}

ReturnUser::ReturnUser(const ReturnUser& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void ReturnUser::SharedCtor() {
  _cached_size_ = 0;
  id_ = 0;
  faceurl_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  username_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  gender_ = 0;
  facevedio_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  longitude_ = 0;
  latitude_ = 0;
  nickname_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ReturnUser::~ReturnUser() {
  SharedDtor();
}

void ReturnUser::SharedDtor() {
  if (faceurl_ != &::google::protobuf::internal::kEmptyString) {
    delete faceurl_;
  }
  if (username_ != &::google::protobuf::internal::kEmptyString) {
    delete username_;
  }
  if (facevedio_ != &::google::protobuf::internal::kEmptyString) {
    delete facevedio_;
  }
  if (nickname_ != &::google::protobuf::internal::kEmptyString) {
    delete nickname_;
  }
  if (this != default_instance_) {
  }
}

void ReturnUser::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ReturnUser::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ReturnUser_descriptor_;
}

const ReturnUser& ReturnUser::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_OutputResultData_2eproto();
  return *default_instance_;
}

ReturnUser* ReturnUser::default_instance_ = NULL;

ReturnUser* ReturnUser::New() const {
  return new ReturnUser;
}

void ReturnUser::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    id_ = 0;
    if (has_faceurl()) {
      if (faceurl_ != &::google::protobuf::internal::kEmptyString) {
        faceurl_->clear();
      }
    }
    if (has_username()) {
      if (username_ != &::google::protobuf::internal::kEmptyString) {
        username_->clear();
      }
    }
    gender_ = 0;
    if (has_facevedio()) {
      if (facevedio_ != &::google::protobuf::internal::kEmptyString) {
        facevedio_->clear();
      }
    }
    longitude_ = 0;
    latitude_ = 0;
    if (has_nickname()) {
      if (nickname_ != &::google::protobuf::internal::kEmptyString) {
        nickname_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool ReturnUser::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int32 id = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &id_)));
          set_has_id();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_faceUrl;
        break;
      }

      // optional string faceUrl = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_faceUrl:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_faceurl()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->faceurl().data(), this->faceurl().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_username;
        break;
      }

      // optional string username = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_username:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_username()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->username().data(), this->username().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(32)) goto parse_gender;
        break;
      }

      // optional int32 gender = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_gender:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &gender_)));
          set_has_gender();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(42)) goto parse_faceVedio;
        break;
      }

      // optional string faceVedio = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_faceVedio:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_facevedio()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->facevedio().data(), this->facevedio().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(49)) goto parse_longitude;
        break;
      }

      // optional double longitude = 6;
      case 6: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED64) {
         parse_longitude:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   double, ::google::protobuf::internal::WireFormatLite::TYPE_DOUBLE>(
                 input, &longitude_)));
          set_has_longitude();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(57)) goto parse_latitude;
        break;
      }

      // optional double latitude = 7;
      case 7: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED64) {
         parse_latitude:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   double, ::google::protobuf::internal::WireFormatLite::TYPE_DOUBLE>(
                 input, &latitude_)));
          set_has_latitude();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(66)) goto parse_nickName;
        break;
      }

      // optional string nickName = 8;
      case 8: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_nickName:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_nickname()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->nickname().data(), this->nickname().length(),
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

void ReturnUser::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional int32 id = 1;
  if (has_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->id(), output);
  }

  // optional string faceUrl = 2;
  if (has_faceurl()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->faceurl().data(), this->faceurl().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->faceurl(), output);
  }

  // optional string username = 3;
  if (has_username()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->username().data(), this->username().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      3, this->username(), output);
  }

  // optional int32 gender = 4;
  if (has_gender()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(4, this->gender(), output);
  }

  // optional string faceVedio = 5;
  if (has_facevedio()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->facevedio().data(), this->facevedio().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      5, this->facevedio(), output);
  }

  // optional double longitude = 6;
  if (has_longitude()) {
    ::google::protobuf::internal::WireFormatLite::WriteDouble(6, this->longitude(), output);
  }

  // optional double latitude = 7;
  if (has_latitude()) {
    ::google::protobuf::internal::WireFormatLite::WriteDouble(7, this->latitude(), output);
  }

  // optional string nickName = 8;
  if (has_nickname()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->nickname().data(), this->nickname().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      8, this->nickname(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* ReturnUser::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional int32 id = 1;
  if (has_id()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->id(), target);
  }

  // optional string faceUrl = 2;
  if (has_faceurl()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->faceurl().data(), this->faceurl().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->faceurl(), target);
  }

  // optional string username = 3;
  if (has_username()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->username().data(), this->username().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->username(), target);
  }

  // optional int32 gender = 4;
  if (has_gender()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(4, this->gender(), target);
  }

  // optional string faceVedio = 5;
  if (has_facevedio()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->facevedio().data(), this->facevedio().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        5, this->facevedio(), target);
  }

  // optional double longitude = 6;
  if (has_longitude()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteDoubleToArray(6, this->longitude(), target);
  }

  // optional double latitude = 7;
  if (has_latitude()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteDoubleToArray(7, this->latitude(), target);
  }

  // optional string nickName = 8;
  if (has_nickname()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->nickname().data(), this->nickname().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        8, this->nickname(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int ReturnUser::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional int32 id = 1;
    if (has_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->id());
    }

    // optional string faceUrl = 2;
    if (has_faceurl()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->faceurl());
    }

    // optional string username = 3;
    if (has_username()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->username());
    }

    // optional int32 gender = 4;
    if (has_gender()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->gender());
    }

    // optional string faceVedio = 5;
    if (has_facevedio()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->facevedio());
    }

    // optional double longitude = 6;
    if (has_longitude()) {
      total_size += 1 + 8;
    }

    // optional double latitude = 7;
    if (has_latitude()) {
      total_size += 1 + 8;
    }

    // optional string nickName = 8;
    if (has_nickname()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->nickname());
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

void ReturnUser::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const ReturnUser* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const ReturnUser*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void ReturnUser::MergeFrom(const ReturnUser& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_id()) {
      set_id(from.id());
    }
    if (from.has_faceurl()) {
      set_faceurl(from.faceurl());
    }
    if (from.has_username()) {
      set_username(from.username());
    }
    if (from.has_gender()) {
      set_gender(from.gender());
    }
    if (from.has_facevedio()) {
      set_facevedio(from.facevedio());
    }
    if (from.has_longitude()) {
      set_longitude(from.longitude());
    }
    if (from.has_latitude()) {
      set_latitude(from.latitude());
    }
    if (from.has_nickname()) {
      set_nickname(from.nickname());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void ReturnUser::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ReturnUser::CopyFrom(const ReturnUser& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ReturnUser::IsInitialized() const {

  return true;
}

void ReturnUser::Swap(ReturnUser* other) {
  if (other != this) {
    std::swap(id_, other->id_);
    std::swap(faceurl_, other->faceurl_);
    std::swap(username_, other->username_);
    std::swap(gender_, other->gender_);
    std::swap(facevedio_, other->facevedio_);
    std::swap(longitude_, other->longitude_);
    std::swap(latitude_, other->latitude_);
    std::swap(nickname_, other->nickname_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata ReturnUser::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ReturnUser_descriptor_;
  metadata.reflection = ReturnUser_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace po
}  // namespace pb
}  // namespace mm
}  // namespace com

// @@protoc_insertion_point(global_scope)