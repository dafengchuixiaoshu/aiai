// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: OutputResultData.proto

#ifndef PROTOBUF_OutputResultData_2eproto__INCLUDED
#define PROTOBUF_OutputResultData_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace com {
namespace mm {
namespace pb {
namespace po {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_OutputResultData_2eproto();
void protobuf_AssignDesc_OutputResultData_2eproto();
void protobuf_ShutdownFile_OutputResultData_2eproto();

class OutputResultData;
class ReturnUser;

// ===================================================================

class OutputResultData : public ::google::protobuf::Message {
 public:
  OutputResultData();
  virtual ~OutputResultData();

  OutputResultData(const OutputResultData& from);

  inline OutputResultData& operator=(const OutputResultData& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const OutputResultData& default_instance();

  void Swap(OutputResultData* other);

  // implements Message ----------------------------------------------

  OutputResultData* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const OutputResultData& from);
  void MergeFrom(const OutputResultData& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .com.mm.pb.po.ReturnUser list = 1;
  inline int list_size() const;
  inline void clear_list();
  static const int kListFieldNumber = 1;
  inline const ::com::mm::pb::po::ReturnUser& list(int index) const;
  inline ::com::mm::pb::po::ReturnUser* mutable_list(int index);
  inline ::com::mm::pb::po::ReturnUser* add_list();
  inline const ::google::protobuf::RepeatedPtrField< ::com::mm::pb::po::ReturnUser >&
      list() const;
  inline ::google::protobuf::RepeatedPtrField< ::com::mm::pb::po::ReturnUser >*
      mutable_list();

  // optional int32 resultCode = 2;
  inline bool has_resultcode() const;
  inline void clear_resultcode();
  static const int kResultCodeFieldNumber = 2;
  inline ::google::protobuf::int32 resultcode() const;
  inline void set_resultcode(::google::protobuf::int32 value);

  // optional string resultMSG = 3;
  inline bool has_resultmsg() const;
  inline void clear_resultmsg();
  static const int kResultMSGFieldNumber = 3;
  inline const ::std::string& resultmsg() const;
  inline void set_resultmsg(const ::std::string& value);
  inline void set_resultmsg(const char* value);
  inline void set_resultmsg(const char* value, size_t size);
  inline ::std::string* mutable_resultmsg();
  inline ::std::string* release_resultmsg();
  inline void set_allocated_resultmsg(::std::string* resultmsg);

  // @@protoc_insertion_point(class_scope:com.mm.pb.po.OutputResultData)
 private:
  inline void set_has_resultcode();
  inline void clear_has_resultcode();
  inline void set_has_resultmsg();
  inline void clear_has_resultmsg();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedPtrField< ::com::mm::pb::po::ReturnUser > list_;
  ::std::string* resultmsg_;
  ::google::protobuf::int32 resultcode_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];

  friend void  protobuf_AddDesc_OutputResultData_2eproto();
  friend void protobuf_AssignDesc_OutputResultData_2eproto();
  friend void protobuf_ShutdownFile_OutputResultData_2eproto();

  void InitAsDefaultInstance();
  static OutputResultData* default_instance_;
};
// -------------------------------------------------------------------

class ReturnUser : public ::google::protobuf::Message {
 public:
  ReturnUser();
  virtual ~ReturnUser();

  ReturnUser(const ReturnUser& from);

  inline ReturnUser& operator=(const ReturnUser& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ReturnUser& default_instance();

  void Swap(ReturnUser* other);

  // implements Message ----------------------------------------------

  ReturnUser* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ReturnUser& from);
  void MergeFrom(const ReturnUser& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional int32 id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline ::google::protobuf::int32 id() const;
  inline void set_id(::google::protobuf::int32 value);

  // optional string faceUrl = 2;
  inline bool has_faceurl() const;
  inline void clear_faceurl();
  static const int kFaceUrlFieldNumber = 2;
  inline const ::std::string& faceurl() const;
  inline void set_faceurl(const ::std::string& value);
  inline void set_faceurl(const char* value);
  inline void set_faceurl(const char* value, size_t size);
  inline ::std::string* mutable_faceurl();
  inline ::std::string* release_faceurl();
  inline void set_allocated_faceurl(::std::string* faceurl);

  // optional string username = 3;
  inline bool has_username() const;
  inline void clear_username();
  static const int kUsernameFieldNumber = 3;
  inline const ::std::string& username() const;
  inline void set_username(const ::std::string& value);
  inline void set_username(const char* value);
  inline void set_username(const char* value, size_t size);
  inline ::std::string* mutable_username();
  inline ::std::string* release_username();
  inline void set_allocated_username(::std::string* username);

  // optional int32 gender = 4;
  inline bool has_gender() const;
  inline void clear_gender();
  static const int kGenderFieldNumber = 4;
  inline ::google::protobuf::int32 gender() const;
  inline void set_gender(::google::protobuf::int32 value);

  // optional string faceVedio = 5;
  inline bool has_facevedio() const;
  inline void clear_facevedio();
  static const int kFaceVedioFieldNumber = 5;
  inline const ::std::string& facevedio() const;
  inline void set_facevedio(const ::std::string& value);
  inline void set_facevedio(const char* value);
  inline void set_facevedio(const char* value, size_t size);
  inline ::std::string* mutable_facevedio();
  inline ::std::string* release_facevedio();
  inline void set_allocated_facevedio(::std::string* facevedio);

  // optional double longitude = 6;
  inline bool has_longitude() const;
  inline void clear_longitude();
  static const int kLongitudeFieldNumber = 6;
  inline double longitude() const;
  inline void set_longitude(double value);

  // optional double latitude = 7;
  inline bool has_latitude() const;
  inline void clear_latitude();
  static const int kLatitudeFieldNumber = 7;
  inline double latitude() const;
  inline void set_latitude(double value);

  // optional string nickName = 8;
  inline bool has_nickname() const;
  inline void clear_nickname();
  static const int kNickNameFieldNumber = 8;
  inline const ::std::string& nickname() const;
  inline void set_nickname(const ::std::string& value);
  inline void set_nickname(const char* value);
  inline void set_nickname(const char* value, size_t size);
  inline ::std::string* mutable_nickname();
  inline ::std::string* release_nickname();
  inline void set_allocated_nickname(::std::string* nickname);

  // @@protoc_insertion_point(class_scope:com.mm.pb.po.ReturnUser)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_faceurl();
  inline void clear_has_faceurl();
  inline void set_has_username();
  inline void clear_has_username();
  inline void set_has_gender();
  inline void clear_has_gender();
  inline void set_has_facevedio();
  inline void clear_has_facevedio();
  inline void set_has_longitude();
  inline void clear_has_longitude();
  inline void set_has_latitude();
  inline void clear_has_latitude();
  inline void set_has_nickname();
  inline void clear_has_nickname();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* faceurl_;
  ::google::protobuf::int32 id_;
  ::google::protobuf::int32 gender_;
  ::std::string* username_;
  ::std::string* facevedio_;
  double longitude_;
  double latitude_;
  ::std::string* nickname_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(8 + 31) / 32];

  friend void  protobuf_AddDesc_OutputResultData_2eproto();
  friend void protobuf_AssignDesc_OutputResultData_2eproto();
  friend void protobuf_ShutdownFile_OutputResultData_2eproto();

  void InitAsDefaultInstance();
  static ReturnUser* default_instance_;
};
// ===================================================================


// ===================================================================

// OutputResultData

// repeated .com.mm.pb.po.ReturnUser list = 1;
inline int OutputResultData::list_size() const {
  return list_.size();
}
inline void OutputResultData::clear_list() {
  list_.Clear();
}
inline const ::com::mm::pb::po::ReturnUser& OutputResultData::list(int index) const {
  return list_.Get(index);
}
inline ::com::mm::pb::po::ReturnUser* OutputResultData::mutable_list(int index) {
  return list_.Mutable(index);
}
inline ::com::mm::pb::po::ReturnUser* OutputResultData::add_list() {
  return list_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::com::mm::pb::po::ReturnUser >&
OutputResultData::list() const {
  return list_;
}
inline ::google::protobuf::RepeatedPtrField< ::com::mm::pb::po::ReturnUser >*
OutputResultData::mutable_list() {
  return &list_;
}

// optional int32 resultCode = 2;
inline bool OutputResultData::has_resultcode() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void OutputResultData::set_has_resultcode() {
  _has_bits_[0] |= 0x00000002u;
}
inline void OutputResultData::clear_has_resultcode() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void OutputResultData::clear_resultcode() {
  resultcode_ = 0;
  clear_has_resultcode();
}
inline ::google::protobuf::int32 OutputResultData::resultcode() const {
  return resultcode_;
}
inline void OutputResultData::set_resultcode(::google::protobuf::int32 value) {
  set_has_resultcode();
  resultcode_ = value;
}

// optional string resultMSG = 3;
inline bool OutputResultData::has_resultmsg() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void OutputResultData::set_has_resultmsg() {
  _has_bits_[0] |= 0x00000004u;
}
inline void OutputResultData::clear_has_resultmsg() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void OutputResultData::clear_resultmsg() {
  if (resultmsg_ != &::google::protobuf::internal::kEmptyString) {
    resultmsg_->clear();
  }
  clear_has_resultmsg();
}
inline const ::std::string& OutputResultData::resultmsg() const {
  return *resultmsg_;
}
inline void OutputResultData::set_resultmsg(const ::std::string& value) {
  set_has_resultmsg();
  if (resultmsg_ == &::google::protobuf::internal::kEmptyString) {
    resultmsg_ = new ::std::string;
  }
  resultmsg_->assign(value);
}
inline void OutputResultData::set_resultmsg(const char* value) {
  set_has_resultmsg();
  if (resultmsg_ == &::google::protobuf::internal::kEmptyString) {
    resultmsg_ = new ::std::string;
  }
  resultmsg_->assign(value);
}
inline void OutputResultData::set_resultmsg(const char* value, size_t size) {
  set_has_resultmsg();
  if (resultmsg_ == &::google::protobuf::internal::kEmptyString) {
    resultmsg_ = new ::std::string;
  }
  resultmsg_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* OutputResultData::mutable_resultmsg() {
  set_has_resultmsg();
  if (resultmsg_ == &::google::protobuf::internal::kEmptyString) {
    resultmsg_ = new ::std::string;
  }
  return resultmsg_;
}
inline ::std::string* OutputResultData::release_resultmsg() {
  clear_has_resultmsg();
  if (resultmsg_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = resultmsg_;
    resultmsg_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void OutputResultData::set_allocated_resultmsg(::std::string* resultmsg) {
  if (resultmsg_ != &::google::protobuf::internal::kEmptyString) {
    delete resultmsg_;
  }
  if (resultmsg) {
    set_has_resultmsg();
    resultmsg_ = resultmsg;
  } else {
    clear_has_resultmsg();
    resultmsg_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// -------------------------------------------------------------------

// ReturnUser

// optional int32 id = 1;
inline bool ReturnUser::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ReturnUser::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ReturnUser::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ReturnUser::clear_id() {
  id_ = 0;
  clear_has_id();
}
inline ::google::protobuf::int32 ReturnUser::id() const {
  return id_;
}
inline void ReturnUser::set_id(::google::protobuf::int32 value) {
  set_has_id();
  id_ = value;
}

// optional string faceUrl = 2;
inline bool ReturnUser::has_faceurl() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ReturnUser::set_has_faceurl() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ReturnUser::clear_has_faceurl() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ReturnUser::clear_faceurl() {
  if (faceurl_ != &::google::protobuf::internal::kEmptyString) {
    faceurl_->clear();
  }
  clear_has_faceurl();
}
inline const ::std::string& ReturnUser::faceurl() const {
  return *faceurl_;
}
inline void ReturnUser::set_faceurl(const ::std::string& value) {
  set_has_faceurl();
  if (faceurl_ == &::google::protobuf::internal::kEmptyString) {
    faceurl_ = new ::std::string;
  }
  faceurl_->assign(value);
}
inline void ReturnUser::set_faceurl(const char* value) {
  set_has_faceurl();
  if (faceurl_ == &::google::protobuf::internal::kEmptyString) {
    faceurl_ = new ::std::string;
  }
  faceurl_->assign(value);
}
inline void ReturnUser::set_faceurl(const char* value, size_t size) {
  set_has_faceurl();
  if (faceurl_ == &::google::protobuf::internal::kEmptyString) {
    faceurl_ = new ::std::string;
  }
  faceurl_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* ReturnUser::mutable_faceurl() {
  set_has_faceurl();
  if (faceurl_ == &::google::protobuf::internal::kEmptyString) {
    faceurl_ = new ::std::string;
  }
  return faceurl_;
}
inline ::std::string* ReturnUser::release_faceurl() {
  clear_has_faceurl();
  if (faceurl_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = faceurl_;
    faceurl_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void ReturnUser::set_allocated_faceurl(::std::string* faceurl) {
  if (faceurl_ != &::google::protobuf::internal::kEmptyString) {
    delete faceurl_;
  }
  if (faceurl) {
    set_has_faceurl();
    faceurl_ = faceurl;
  } else {
    clear_has_faceurl();
    faceurl_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional string username = 3;
inline bool ReturnUser::has_username() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ReturnUser::set_has_username() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ReturnUser::clear_has_username() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ReturnUser::clear_username() {
  if (username_ != &::google::protobuf::internal::kEmptyString) {
    username_->clear();
  }
  clear_has_username();
}
inline const ::std::string& ReturnUser::username() const {
  return *username_;
}
inline void ReturnUser::set_username(const ::std::string& value) {
  set_has_username();
  if (username_ == &::google::protobuf::internal::kEmptyString) {
    username_ = new ::std::string;
  }
  username_->assign(value);
}
inline void ReturnUser::set_username(const char* value) {
  set_has_username();
  if (username_ == &::google::protobuf::internal::kEmptyString) {
    username_ = new ::std::string;
  }
  username_->assign(value);
}
inline void ReturnUser::set_username(const char* value, size_t size) {
  set_has_username();
  if (username_ == &::google::protobuf::internal::kEmptyString) {
    username_ = new ::std::string;
  }
  username_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* ReturnUser::mutable_username() {
  set_has_username();
  if (username_ == &::google::protobuf::internal::kEmptyString) {
    username_ = new ::std::string;
  }
  return username_;
}
inline ::std::string* ReturnUser::release_username() {
  clear_has_username();
  if (username_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = username_;
    username_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void ReturnUser::set_allocated_username(::std::string* username) {
  if (username_ != &::google::protobuf::internal::kEmptyString) {
    delete username_;
  }
  if (username) {
    set_has_username();
    username_ = username;
  } else {
    clear_has_username();
    username_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional int32 gender = 4;
inline bool ReturnUser::has_gender() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void ReturnUser::set_has_gender() {
  _has_bits_[0] |= 0x00000008u;
}
inline void ReturnUser::clear_has_gender() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void ReturnUser::clear_gender() {
  gender_ = 0;
  clear_has_gender();
}
inline ::google::protobuf::int32 ReturnUser::gender() const {
  return gender_;
}
inline void ReturnUser::set_gender(::google::protobuf::int32 value) {
  set_has_gender();
  gender_ = value;
}

// optional string faceVedio = 5;
inline bool ReturnUser::has_facevedio() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void ReturnUser::set_has_facevedio() {
  _has_bits_[0] |= 0x00000010u;
}
inline void ReturnUser::clear_has_facevedio() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void ReturnUser::clear_facevedio() {
  if (facevedio_ != &::google::protobuf::internal::kEmptyString) {
    facevedio_->clear();
  }
  clear_has_facevedio();
}
inline const ::std::string& ReturnUser::facevedio() const {
  return *facevedio_;
}
inline void ReturnUser::set_facevedio(const ::std::string& value) {
  set_has_facevedio();
  if (facevedio_ == &::google::protobuf::internal::kEmptyString) {
    facevedio_ = new ::std::string;
  }
  facevedio_->assign(value);
}
inline void ReturnUser::set_facevedio(const char* value) {
  set_has_facevedio();
  if (facevedio_ == &::google::protobuf::internal::kEmptyString) {
    facevedio_ = new ::std::string;
  }
  facevedio_->assign(value);
}
inline void ReturnUser::set_facevedio(const char* value, size_t size) {
  set_has_facevedio();
  if (facevedio_ == &::google::protobuf::internal::kEmptyString) {
    facevedio_ = new ::std::string;
  }
  facevedio_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* ReturnUser::mutable_facevedio() {
  set_has_facevedio();
  if (facevedio_ == &::google::protobuf::internal::kEmptyString) {
    facevedio_ = new ::std::string;
  }
  return facevedio_;
}
inline ::std::string* ReturnUser::release_facevedio() {
  clear_has_facevedio();
  if (facevedio_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = facevedio_;
    facevedio_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void ReturnUser::set_allocated_facevedio(::std::string* facevedio) {
  if (facevedio_ != &::google::protobuf::internal::kEmptyString) {
    delete facevedio_;
  }
  if (facevedio) {
    set_has_facevedio();
    facevedio_ = facevedio;
  } else {
    clear_has_facevedio();
    facevedio_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional double longitude = 6;
inline bool ReturnUser::has_longitude() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void ReturnUser::set_has_longitude() {
  _has_bits_[0] |= 0x00000020u;
}
inline void ReturnUser::clear_has_longitude() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void ReturnUser::clear_longitude() {
  longitude_ = 0;
  clear_has_longitude();
}
inline double ReturnUser::longitude() const {
  return longitude_;
}
inline void ReturnUser::set_longitude(double value) {
  set_has_longitude();
  longitude_ = value;
}

// optional double latitude = 7;
inline bool ReturnUser::has_latitude() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void ReturnUser::set_has_latitude() {
  _has_bits_[0] |= 0x00000040u;
}
inline void ReturnUser::clear_has_latitude() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void ReturnUser::clear_latitude() {
  latitude_ = 0;
  clear_has_latitude();
}
inline double ReturnUser::latitude() const {
  return latitude_;
}
inline void ReturnUser::set_latitude(double value) {
  set_has_latitude();
  latitude_ = value;
}

// optional string nickName = 8;
inline bool ReturnUser::has_nickname() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void ReturnUser::set_has_nickname() {
  _has_bits_[0] |= 0x00000080u;
}
inline void ReturnUser::clear_has_nickname() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void ReturnUser::clear_nickname() {
  if (nickname_ != &::google::protobuf::internal::kEmptyString) {
    nickname_->clear();
  }
  clear_has_nickname();
}
inline const ::std::string& ReturnUser::nickname() const {
  return *nickname_;
}
inline void ReturnUser::set_nickname(const ::std::string& value) {
  set_has_nickname();
  if (nickname_ == &::google::protobuf::internal::kEmptyString) {
    nickname_ = new ::std::string;
  }
  nickname_->assign(value);
}
inline void ReturnUser::set_nickname(const char* value) {
  set_has_nickname();
  if (nickname_ == &::google::protobuf::internal::kEmptyString) {
    nickname_ = new ::std::string;
  }
  nickname_->assign(value);
}
inline void ReturnUser::set_nickname(const char* value, size_t size) {
  set_has_nickname();
  if (nickname_ == &::google::protobuf::internal::kEmptyString) {
    nickname_ = new ::std::string;
  }
  nickname_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* ReturnUser::mutable_nickname() {
  set_has_nickname();
  if (nickname_ == &::google::protobuf::internal::kEmptyString) {
    nickname_ = new ::std::string;
  }
  return nickname_;
}
inline ::std::string* ReturnUser::release_nickname() {
  clear_has_nickname();
  if (nickname_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = nickname_;
    nickname_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void ReturnUser::set_allocated_nickname(::std::string* nickname) {
  if (nickname_ != &::google::protobuf::internal::kEmptyString) {
    delete nickname_;
  }
  if (nickname) {
    set_has_nickname();
    nickname_ = nickname;
  } else {
    clear_has_nickname();
    nickname_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace po
}  // namespace pb
}  // namespace mm
}  // namespace com

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_OutputResultData_2eproto__INCLUDED