// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: userPage.proto

#ifndef PROTOBUF_userPage_2eproto__INCLUDED
#define PROTOBUF_userPage_2eproto__INCLUDED

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

namespace tutorial {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_userPage_2eproto();
void protobuf_AssignDesc_userPage_2eproto();
void protobuf_ShutdownFile_userPage_2eproto();

class UserPage;
class UserPage_Work;

// ===================================================================

class UserPage_Work : public ::google::protobuf::Message {
 public:
  UserPage_Work();
  virtual ~UserPage_Work();

  UserPage_Work(const UserPage_Work& from);

  inline UserPage_Work& operator=(const UserPage_Work& from) {
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
  static const UserPage_Work& default_instance();

  void Swap(UserPage_Work* other);

  // implements Message ----------------------------------------------

  UserPage_Work* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const UserPage_Work& from);
  void MergeFrom(const UserPage_Work& from);
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

  // required string firstKey = 1;
  inline bool has_firstkey() const;
  inline void clear_firstkey();
  static const int kFirstKeyFieldNumber = 1;
  inline const ::std::string& firstkey() const;
  inline void set_firstkey(const ::std::string& value);
  inline void set_firstkey(const char* value);
  inline void set_firstkey(const char* value, size_t size);
  inline ::std::string* mutable_firstkey();
  inline ::std::string* release_firstkey();
  inline void set_allocated_firstkey(::std::string* firstkey);

  // optional string label = 2;
  inline bool has_label() const;
  inline void clear_label();
  static const int kLabelFieldNumber = 2;
  inline const ::std::string& label() const;
  inline void set_label(const ::std::string& value);
  inline void set_label(const char* value);
  inline void set_label(const char* value, size_t size);
  inline ::std::string* mutable_label();
  inline ::std::string* release_label();
  inline void set_allocated_label(::std::string* label);

  // optional string face = 3;
  inline bool has_face() const;
  inline void clear_face();
  static const int kFaceFieldNumber = 3;
  inline const ::std::string& face() const;
  inline void set_face(const ::std::string& value);
  inline void set_face(const char* value);
  inline void set_face(const char* value, size_t size);
  inline ::std::string* mutable_face();
  inline ::std::string* release_face();
  inline void set_allocated_face(::std::string* face);

  // required int64 sendTime = 4;
  inline bool has_sendtime() const;
  inline void clear_sendtime();
  static const int kSendTimeFieldNumber = 4;
  inline ::google::protobuf::int64 sendtime() const;
  inline void set_sendtime(::google::protobuf::int64 value);

  // required int32 praiseCount = 5;
  inline bool has_praisecount() const;
  inline void clear_praisecount();
  static const int kPraiseCountFieldNumber = 5;
  inline ::google::protobuf::int32 praisecount() const;
  inline void set_praisecount(::google::protobuf::int32 value);

  // required int32 sid = 6;
  inline bool has_sid() const;
  inline void clear_sid();
  static const int kSidFieldNumber = 6;
  inline ::google::protobuf::int32 sid() const;
  inline void set_sid(::google::protobuf::int32 value);

  // required string seq = 7;
  inline bool has_seq() const;
  inline void clear_seq();
  static const int kSeqFieldNumber = 7;
  inline const ::std::string& seq() const;
  inline void set_seq(const ::std::string& value);
  inline void set_seq(const char* value);
  inline void set_seq(const char* value, size_t size);
  inline ::std::string* mutable_seq();
  inline ::std::string* release_seq();
  inline void set_allocated_seq(::std::string* seq);

  // @@protoc_insertion_point(class_scope:tutorial.UserPage.Work)
 private:
  inline void set_has_firstkey();
  inline void clear_has_firstkey();
  inline void set_has_label();
  inline void clear_has_label();
  inline void set_has_face();
  inline void clear_has_face();
  inline void set_has_sendtime();
  inline void clear_has_sendtime();
  inline void set_has_praisecount();
  inline void clear_has_praisecount();
  inline void set_has_sid();
  inline void clear_has_sid();
  inline void set_has_seq();
  inline void clear_has_seq();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* firstkey_;
  ::std::string* label_;
  ::std::string* face_;
  ::google::protobuf::int64 sendtime_;
  ::google::protobuf::int32 praisecount_;
  ::google::protobuf::int32 sid_;
  ::std::string* seq_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(7 + 31) / 32];

  friend void  protobuf_AddDesc_userPage_2eproto();
  friend void protobuf_AssignDesc_userPage_2eproto();
  friend void protobuf_ShutdownFile_userPage_2eproto();

  void InitAsDefaultInstance();
  static UserPage_Work* default_instance_;
};
// -------------------------------------------------------------------

class UserPage : public ::google::protobuf::Message {
 public:
  UserPage();
  virtual ~UserPage();

  UserPage(const UserPage& from);

  inline UserPage& operator=(const UserPage& from) {
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
  static const UserPage& default_instance();

  void Swap(UserPage* other);

  // implements Message ----------------------------------------------

  UserPage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const UserPage& from);
  void MergeFrom(const UserPage& from);
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

  typedef UserPage_Work Work;

  // accessors -------------------------------------------------------

  // optional string face = 1;
  inline bool has_face() const;
  inline void clear_face();
  static const int kFaceFieldNumber = 1;
  inline const ::std::string& face() const;
  inline void set_face(const ::std::string& value);
  inline void set_face(const char* value);
  inline void set_face(const char* value, size_t size);
  inline ::std::string* mutable_face();
  inline ::std::string* release_face();
  inline void set_allocated_face(::std::string* face);

  // optional string nickName = 2;
  inline bool has_nickname() const;
  inline void clear_nickname();
  static const int kNickNameFieldNumber = 2;
  inline const ::std::string& nickname() const;
  inline void set_nickname(const ::std::string& value);
  inline void set_nickname(const char* value);
  inline void set_nickname(const char* value, size_t size);
  inline ::std::string* mutable_nickname();
  inline ::std::string* release_nickname();
  inline void set_allocated_nickname(::std::string* nickname);

  // optional string sign = 3;
  inline bool has_sign() const;
  inline void clear_sign();
  static const int kSignFieldNumber = 3;
  inline const ::std::string& sign() const;
  inline void set_sign(const ::std::string& value);
  inline void set_sign(const char* value);
  inline void set_sign(const char* value, size_t size);
  inline ::std::string* mutable_sign();
  inline ::std::string* release_sign();
  inline void set_allocated_sign(::std::string* sign);

  // optional int32 isAttention = 4;
  inline bool has_isattention() const;
  inline void clear_isattention();
  static const int kIsAttentionFieldNumber = 4;
  inline ::google::protobuf::int32 isattention() const;
  inline void set_isattention(::google::protobuf::int32 value);

  // optional string faceVideo = 5;
  inline bool has_facevideo() const;
  inline void clear_facevideo();
  static const int kFaceVideoFieldNumber = 5;
  inline const ::std::string& facevideo() const;
  inline void set_facevideo(const ::std::string& value);
  inline void set_facevideo(const char* value);
  inline void set_facevideo(const char* value, size_t size);
  inline ::std::string* mutable_facevideo();
  inline ::std::string* release_facevideo();
  inline void set_allocated_facevideo(::std::string* facevideo);

  // optional int32 uid = 6;
  inline bool has_uid() const;
  inline void clear_uid();
  static const int kUidFieldNumber = 6;
  inline ::google::protobuf::int32 uid() const;
  inline void set_uid(::google::protobuf::int32 value);

  // repeated .tutorial.UserPage.Work work = 7;
  inline int work_size() const;
  inline void clear_work();
  static const int kWorkFieldNumber = 7;
  inline const ::tutorial::UserPage_Work& work(int index) const;
  inline ::tutorial::UserPage_Work* mutable_work(int index);
  inline ::tutorial::UserPage_Work* add_work();
  inline const ::google::protobuf::RepeatedPtrField< ::tutorial::UserPage_Work >&
      work() const;
  inline ::google::protobuf::RepeatedPtrField< ::tutorial::UserPage_Work >*
      mutable_work();

  // @@protoc_insertion_point(class_scope:tutorial.UserPage)
 private:
  inline void set_has_face();
  inline void clear_has_face();
  inline void set_has_nickname();
  inline void clear_has_nickname();
  inline void set_has_sign();
  inline void clear_has_sign();
  inline void set_has_isattention();
  inline void clear_has_isattention();
  inline void set_has_facevideo();
  inline void clear_has_facevideo();
  inline void set_has_uid();
  inline void clear_has_uid();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* face_;
  ::std::string* nickname_;
  ::std::string* sign_;
  ::std::string* facevideo_;
  ::google::protobuf::int32 isattention_;
  ::google::protobuf::int32 uid_;
  ::google::protobuf::RepeatedPtrField< ::tutorial::UserPage_Work > work_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(7 + 31) / 32];

  friend void  protobuf_AddDesc_userPage_2eproto();
  friend void protobuf_AssignDesc_userPage_2eproto();
  friend void protobuf_ShutdownFile_userPage_2eproto();

  void InitAsDefaultInstance();
  static UserPage* default_instance_;
};
// ===================================================================


// ===================================================================

// UserPage_Work

// required string firstKey = 1;
inline bool UserPage_Work::has_firstkey() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void UserPage_Work::set_has_firstkey() {
  _has_bits_[0] |= 0x00000001u;
}
inline void UserPage_Work::clear_has_firstkey() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void UserPage_Work::clear_firstkey() {
  if (firstkey_ != &::google::protobuf::internal::kEmptyString) {
    firstkey_->clear();
  }
  clear_has_firstkey();
}
inline const ::std::string& UserPage_Work::firstkey() const {
  return *firstkey_;
}
inline void UserPage_Work::set_firstkey(const ::std::string& value) {
  set_has_firstkey();
  if (firstkey_ == &::google::protobuf::internal::kEmptyString) {
    firstkey_ = new ::std::string;
  }
  firstkey_->assign(value);
}
inline void UserPage_Work::set_firstkey(const char* value) {
  set_has_firstkey();
  if (firstkey_ == &::google::protobuf::internal::kEmptyString) {
    firstkey_ = new ::std::string;
  }
  firstkey_->assign(value);
}
inline void UserPage_Work::set_firstkey(const char* value, size_t size) {
  set_has_firstkey();
  if (firstkey_ == &::google::protobuf::internal::kEmptyString) {
    firstkey_ = new ::std::string;
  }
  firstkey_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* UserPage_Work::mutable_firstkey() {
  set_has_firstkey();
  if (firstkey_ == &::google::protobuf::internal::kEmptyString) {
    firstkey_ = new ::std::string;
  }
  return firstkey_;
}
inline ::std::string* UserPage_Work::release_firstkey() {
  clear_has_firstkey();
  if (firstkey_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = firstkey_;
    firstkey_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void UserPage_Work::set_allocated_firstkey(::std::string* firstkey) {
  if (firstkey_ != &::google::protobuf::internal::kEmptyString) {
    delete firstkey_;
  }
  if (firstkey) {
    set_has_firstkey();
    firstkey_ = firstkey;
  } else {
    clear_has_firstkey();
    firstkey_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional string label = 2;
inline bool UserPage_Work::has_label() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void UserPage_Work::set_has_label() {
  _has_bits_[0] |= 0x00000002u;
}
inline void UserPage_Work::clear_has_label() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void UserPage_Work::clear_label() {
  if (label_ != &::google::protobuf::internal::kEmptyString) {
    label_->clear();
  }
  clear_has_label();
}
inline const ::std::string& UserPage_Work::label() const {
  return *label_;
}
inline void UserPage_Work::set_label(const ::std::string& value) {
  set_has_label();
  if (label_ == &::google::protobuf::internal::kEmptyString) {
    label_ = new ::std::string;
  }
  label_->assign(value);
}
inline void UserPage_Work::set_label(const char* value) {
  set_has_label();
  if (label_ == &::google::protobuf::internal::kEmptyString) {
    label_ = new ::std::string;
  }
  label_->assign(value);
}
inline void UserPage_Work::set_label(const char* value, size_t size) {
  set_has_label();
  if (label_ == &::google::protobuf::internal::kEmptyString) {
    label_ = new ::std::string;
  }
  label_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* UserPage_Work::mutable_label() {
  set_has_label();
  if (label_ == &::google::protobuf::internal::kEmptyString) {
    label_ = new ::std::string;
  }
  return label_;
}
inline ::std::string* UserPage_Work::release_label() {
  clear_has_label();
  if (label_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = label_;
    label_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void UserPage_Work::set_allocated_label(::std::string* label) {
  if (label_ != &::google::protobuf::internal::kEmptyString) {
    delete label_;
  }
  if (label) {
    set_has_label();
    label_ = label;
  } else {
    clear_has_label();
    label_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional string face = 3;
inline bool UserPage_Work::has_face() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void UserPage_Work::set_has_face() {
  _has_bits_[0] |= 0x00000004u;
}
inline void UserPage_Work::clear_has_face() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void UserPage_Work::clear_face() {
  if (face_ != &::google::protobuf::internal::kEmptyString) {
    face_->clear();
  }
  clear_has_face();
}
inline const ::std::string& UserPage_Work::face() const {
  return *face_;
}
inline void UserPage_Work::set_face(const ::std::string& value) {
  set_has_face();
  if (face_ == &::google::protobuf::internal::kEmptyString) {
    face_ = new ::std::string;
  }
  face_->assign(value);
}
inline void UserPage_Work::set_face(const char* value) {
  set_has_face();
  if (face_ == &::google::protobuf::internal::kEmptyString) {
    face_ = new ::std::string;
  }
  face_->assign(value);
}
inline void UserPage_Work::set_face(const char* value, size_t size) {
  set_has_face();
  if (face_ == &::google::protobuf::internal::kEmptyString) {
    face_ = new ::std::string;
  }
  face_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* UserPage_Work::mutable_face() {
  set_has_face();
  if (face_ == &::google::protobuf::internal::kEmptyString) {
    face_ = new ::std::string;
  }
  return face_;
}
inline ::std::string* UserPage_Work::release_face() {
  clear_has_face();
  if (face_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = face_;
    face_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void UserPage_Work::set_allocated_face(::std::string* face) {
  if (face_ != &::google::protobuf::internal::kEmptyString) {
    delete face_;
  }
  if (face) {
    set_has_face();
    face_ = face;
  } else {
    clear_has_face();
    face_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required int64 sendTime = 4;
inline bool UserPage_Work::has_sendtime() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void UserPage_Work::set_has_sendtime() {
  _has_bits_[0] |= 0x00000008u;
}
inline void UserPage_Work::clear_has_sendtime() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void UserPage_Work::clear_sendtime() {
  sendtime_ = GOOGLE_LONGLONG(0);
  clear_has_sendtime();
}
inline ::google::protobuf::int64 UserPage_Work::sendtime() const {
  return sendtime_;
}
inline void UserPage_Work::set_sendtime(::google::protobuf::int64 value) {
  set_has_sendtime();
  sendtime_ = value;
}

// required int32 praiseCount = 5;
inline bool UserPage_Work::has_praisecount() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void UserPage_Work::set_has_praisecount() {
  _has_bits_[0] |= 0x00000010u;
}
inline void UserPage_Work::clear_has_praisecount() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void UserPage_Work::clear_praisecount() {
  praisecount_ = 0;
  clear_has_praisecount();
}
inline ::google::protobuf::int32 UserPage_Work::praisecount() const {
  return praisecount_;
}
inline void UserPage_Work::set_praisecount(::google::protobuf::int32 value) {
  set_has_praisecount();
  praisecount_ = value;
}

// required int32 sid = 6;
inline bool UserPage_Work::has_sid() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void UserPage_Work::set_has_sid() {
  _has_bits_[0] |= 0x00000020u;
}
inline void UserPage_Work::clear_has_sid() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void UserPage_Work::clear_sid() {
  sid_ = 0;
  clear_has_sid();
}
inline ::google::protobuf::int32 UserPage_Work::sid() const {
  return sid_;
}
inline void UserPage_Work::set_sid(::google::protobuf::int32 value) {
  set_has_sid();
  sid_ = value;
}

// required string seq = 7;
inline bool UserPage_Work::has_seq() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void UserPage_Work::set_has_seq() {
  _has_bits_[0] |= 0x00000040u;
}
inline void UserPage_Work::clear_has_seq() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void UserPage_Work::clear_seq() {
  if (seq_ != &::google::protobuf::internal::kEmptyString) {
    seq_->clear();
  }
  clear_has_seq();
}
inline const ::std::string& UserPage_Work::seq() const {
  return *seq_;
}
inline void UserPage_Work::set_seq(const ::std::string& value) {
  set_has_seq();
  if (seq_ == &::google::protobuf::internal::kEmptyString) {
    seq_ = new ::std::string;
  }
  seq_->assign(value);
}
inline void UserPage_Work::set_seq(const char* value) {
  set_has_seq();
  if (seq_ == &::google::protobuf::internal::kEmptyString) {
    seq_ = new ::std::string;
  }
  seq_->assign(value);
}
inline void UserPage_Work::set_seq(const char* value, size_t size) {
  set_has_seq();
  if (seq_ == &::google::protobuf::internal::kEmptyString) {
    seq_ = new ::std::string;
  }
  seq_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* UserPage_Work::mutable_seq() {
  set_has_seq();
  if (seq_ == &::google::protobuf::internal::kEmptyString) {
    seq_ = new ::std::string;
  }
  return seq_;
}
inline ::std::string* UserPage_Work::release_seq() {
  clear_has_seq();
  if (seq_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = seq_;
    seq_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void UserPage_Work::set_allocated_seq(::std::string* seq) {
  if (seq_ != &::google::protobuf::internal::kEmptyString) {
    delete seq_;
  }
  if (seq) {
    set_has_seq();
    seq_ = seq;
  } else {
    clear_has_seq();
    seq_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// -------------------------------------------------------------------

// UserPage

// optional string face = 1;
inline bool UserPage::has_face() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void UserPage::set_has_face() {
  _has_bits_[0] |= 0x00000001u;
}
inline void UserPage::clear_has_face() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void UserPage::clear_face() {
  if (face_ != &::google::protobuf::internal::kEmptyString) {
    face_->clear();
  }
  clear_has_face();
}
inline const ::std::string& UserPage::face() const {
  return *face_;
}
inline void UserPage::set_face(const ::std::string& value) {
  set_has_face();
  if (face_ == &::google::protobuf::internal::kEmptyString) {
    face_ = new ::std::string;
  }
  face_->assign(value);
}
inline void UserPage::set_face(const char* value) {
  set_has_face();
  if (face_ == &::google::protobuf::internal::kEmptyString) {
    face_ = new ::std::string;
  }
  face_->assign(value);
}
inline void UserPage::set_face(const char* value, size_t size) {
  set_has_face();
  if (face_ == &::google::protobuf::internal::kEmptyString) {
    face_ = new ::std::string;
  }
  face_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* UserPage::mutable_face() {
  set_has_face();
  if (face_ == &::google::protobuf::internal::kEmptyString) {
    face_ = new ::std::string;
  }
  return face_;
}
inline ::std::string* UserPage::release_face() {
  clear_has_face();
  if (face_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = face_;
    face_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void UserPage::set_allocated_face(::std::string* face) {
  if (face_ != &::google::protobuf::internal::kEmptyString) {
    delete face_;
  }
  if (face) {
    set_has_face();
    face_ = face;
  } else {
    clear_has_face();
    face_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional string nickName = 2;
inline bool UserPage::has_nickname() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void UserPage::set_has_nickname() {
  _has_bits_[0] |= 0x00000002u;
}
inline void UserPage::clear_has_nickname() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void UserPage::clear_nickname() {
  if (nickname_ != &::google::protobuf::internal::kEmptyString) {
    nickname_->clear();
  }
  clear_has_nickname();
}
inline const ::std::string& UserPage::nickname() const {
  return *nickname_;
}
inline void UserPage::set_nickname(const ::std::string& value) {
  set_has_nickname();
  if (nickname_ == &::google::protobuf::internal::kEmptyString) {
    nickname_ = new ::std::string;
  }
  nickname_->assign(value);
}
inline void UserPage::set_nickname(const char* value) {
  set_has_nickname();
  if (nickname_ == &::google::protobuf::internal::kEmptyString) {
    nickname_ = new ::std::string;
  }
  nickname_->assign(value);
}
inline void UserPage::set_nickname(const char* value, size_t size) {
  set_has_nickname();
  if (nickname_ == &::google::protobuf::internal::kEmptyString) {
    nickname_ = new ::std::string;
  }
  nickname_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* UserPage::mutable_nickname() {
  set_has_nickname();
  if (nickname_ == &::google::protobuf::internal::kEmptyString) {
    nickname_ = new ::std::string;
  }
  return nickname_;
}
inline ::std::string* UserPage::release_nickname() {
  clear_has_nickname();
  if (nickname_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = nickname_;
    nickname_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void UserPage::set_allocated_nickname(::std::string* nickname) {
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

// optional string sign = 3;
inline bool UserPage::has_sign() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void UserPage::set_has_sign() {
  _has_bits_[0] |= 0x00000004u;
}
inline void UserPage::clear_has_sign() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void UserPage::clear_sign() {
  if (sign_ != &::google::protobuf::internal::kEmptyString) {
    sign_->clear();
  }
  clear_has_sign();
}
inline const ::std::string& UserPage::sign() const {
  return *sign_;
}
inline void UserPage::set_sign(const ::std::string& value) {
  set_has_sign();
  if (sign_ == &::google::protobuf::internal::kEmptyString) {
    sign_ = new ::std::string;
  }
  sign_->assign(value);
}
inline void UserPage::set_sign(const char* value) {
  set_has_sign();
  if (sign_ == &::google::protobuf::internal::kEmptyString) {
    sign_ = new ::std::string;
  }
  sign_->assign(value);
}
inline void UserPage::set_sign(const char* value, size_t size) {
  set_has_sign();
  if (sign_ == &::google::protobuf::internal::kEmptyString) {
    sign_ = new ::std::string;
  }
  sign_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* UserPage::mutable_sign() {
  set_has_sign();
  if (sign_ == &::google::protobuf::internal::kEmptyString) {
    sign_ = new ::std::string;
  }
  return sign_;
}
inline ::std::string* UserPage::release_sign() {
  clear_has_sign();
  if (sign_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = sign_;
    sign_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void UserPage::set_allocated_sign(::std::string* sign) {
  if (sign_ != &::google::protobuf::internal::kEmptyString) {
    delete sign_;
  }
  if (sign) {
    set_has_sign();
    sign_ = sign;
  } else {
    clear_has_sign();
    sign_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional int32 isAttention = 4;
inline bool UserPage::has_isattention() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void UserPage::set_has_isattention() {
  _has_bits_[0] |= 0x00000008u;
}
inline void UserPage::clear_has_isattention() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void UserPage::clear_isattention() {
  isattention_ = 0;
  clear_has_isattention();
}
inline ::google::protobuf::int32 UserPage::isattention() const {
  return isattention_;
}
inline void UserPage::set_isattention(::google::protobuf::int32 value) {
  set_has_isattention();
  isattention_ = value;
}

// optional string faceVideo = 5;
inline bool UserPage::has_facevideo() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void UserPage::set_has_facevideo() {
  _has_bits_[0] |= 0x00000010u;
}
inline void UserPage::clear_has_facevideo() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void UserPage::clear_facevideo() {
  if (facevideo_ != &::google::protobuf::internal::kEmptyString) {
    facevideo_->clear();
  }
  clear_has_facevideo();
}
inline const ::std::string& UserPage::facevideo() const {
  return *facevideo_;
}
inline void UserPage::set_facevideo(const ::std::string& value) {
  set_has_facevideo();
  if (facevideo_ == &::google::protobuf::internal::kEmptyString) {
    facevideo_ = new ::std::string;
  }
  facevideo_->assign(value);
}
inline void UserPage::set_facevideo(const char* value) {
  set_has_facevideo();
  if (facevideo_ == &::google::protobuf::internal::kEmptyString) {
    facevideo_ = new ::std::string;
  }
  facevideo_->assign(value);
}
inline void UserPage::set_facevideo(const char* value, size_t size) {
  set_has_facevideo();
  if (facevideo_ == &::google::protobuf::internal::kEmptyString) {
    facevideo_ = new ::std::string;
  }
  facevideo_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* UserPage::mutable_facevideo() {
  set_has_facevideo();
  if (facevideo_ == &::google::protobuf::internal::kEmptyString) {
    facevideo_ = new ::std::string;
  }
  return facevideo_;
}
inline ::std::string* UserPage::release_facevideo() {
  clear_has_facevideo();
  if (facevideo_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = facevideo_;
    facevideo_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void UserPage::set_allocated_facevideo(::std::string* facevideo) {
  if (facevideo_ != &::google::protobuf::internal::kEmptyString) {
    delete facevideo_;
  }
  if (facevideo) {
    set_has_facevideo();
    facevideo_ = facevideo;
  } else {
    clear_has_facevideo();
    facevideo_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional int32 uid = 6;
inline bool UserPage::has_uid() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void UserPage::set_has_uid() {
  _has_bits_[0] |= 0x00000020u;
}
inline void UserPage::clear_has_uid() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void UserPage::clear_uid() {
  uid_ = 0;
  clear_has_uid();
}
inline ::google::protobuf::int32 UserPage::uid() const {
  return uid_;
}
inline void UserPage::set_uid(::google::protobuf::int32 value) {
  set_has_uid();
  uid_ = value;
}

// repeated .tutorial.UserPage.Work work = 7;
inline int UserPage::work_size() const {
  return work_.size();
}
inline void UserPage::clear_work() {
  work_.Clear();
}
inline const ::tutorial::UserPage_Work& UserPage::work(int index) const {
  return work_.Get(index);
}
inline ::tutorial::UserPage_Work* UserPage::mutable_work(int index) {
  return work_.Mutable(index);
}
inline ::tutorial::UserPage_Work* UserPage::add_work() {
  return work_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tutorial::UserPage_Work >&
UserPage::work() const {
  return work_;
}
inline ::google::protobuf::RepeatedPtrField< ::tutorial::UserPage_Work >*
UserPage::mutable_work() {
  return &work_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace tutorial

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_userPage_2eproto__INCLUDED