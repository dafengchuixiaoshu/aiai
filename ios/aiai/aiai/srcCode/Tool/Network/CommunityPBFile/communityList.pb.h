// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: communityList.proto

#ifndef PROTOBUF_communityList_2eproto__INCLUDED
#define PROTOBUF_communityList_2eproto__INCLUDED

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
void  protobuf_AddDesc_communityList_2eproto();
void protobuf_AssignDesc_communityList_2eproto();
void protobuf_ShutdownFile_communityList_2eproto();

class CommunityList;
class CommunityList_Community;

// ===================================================================

class CommunityList_Community : public ::google::protobuf::Message {
 public:
  CommunityList_Community();
  virtual ~CommunityList_Community();

  CommunityList_Community(const CommunityList_Community& from);

  inline CommunityList_Community& operator=(const CommunityList_Community& from) {
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
  static const CommunityList_Community& default_instance();

  void Swap(CommunityList_Community* other);

  // implements Message ----------------------------------------------

  CommunityList_Community* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const CommunityList_Community& from);
  void MergeFrom(const CommunityList_Community& from);
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

  // @@protoc_insertion_point(class_scope:tutorial.CommunityList.Community)
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

  friend void  protobuf_AddDesc_communityList_2eproto();
  friend void protobuf_AssignDesc_communityList_2eproto();
  friend void protobuf_ShutdownFile_communityList_2eproto();

  void InitAsDefaultInstance();
  static CommunityList_Community* default_instance_;
};
// -------------------------------------------------------------------

class CommunityList : public ::google::protobuf::Message {
 public:
  CommunityList();
  virtual ~CommunityList();

  CommunityList(const CommunityList& from);

  inline CommunityList& operator=(const CommunityList& from) {
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
  static const CommunityList& default_instance();

  void Swap(CommunityList* other);

  // implements Message ----------------------------------------------

  CommunityList* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const CommunityList& from);
  void MergeFrom(const CommunityList& from);
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

  typedef CommunityList_Community Community;

  // accessors -------------------------------------------------------

  // repeated .tutorial.CommunityList.Community community = 8;
  inline int community_size() const;
  inline void clear_community();
  static const int kCommunityFieldNumber = 8;
  inline const ::tutorial::CommunityList_Community& community(int index) const;
  inline ::tutorial::CommunityList_Community* mutable_community(int index);
  inline ::tutorial::CommunityList_Community* add_community();
  inline const ::google::protobuf::RepeatedPtrField< ::tutorial::CommunityList_Community >&
      community() const;
  inline ::google::protobuf::RepeatedPtrField< ::tutorial::CommunityList_Community >*
      mutable_community();

  // @@protoc_insertion_point(class_scope:tutorial.CommunityList)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedPtrField< ::tutorial::CommunityList_Community > community_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_communityList_2eproto();
  friend void protobuf_AssignDesc_communityList_2eproto();
  friend void protobuf_ShutdownFile_communityList_2eproto();

  void InitAsDefaultInstance();
  static CommunityList* default_instance_;
};
// ===================================================================


// ===================================================================

// CommunityList_Community

// required string firstKey = 1;
inline bool CommunityList_Community::has_firstkey() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void CommunityList_Community::set_has_firstkey() {
  _has_bits_[0] |= 0x00000001u;
}
inline void CommunityList_Community::clear_has_firstkey() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void CommunityList_Community::clear_firstkey() {
  if (firstkey_ != &::google::protobuf::internal::kEmptyString) {
    firstkey_->clear();
  }
  clear_has_firstkey();
}
inline const ::std::string& CommunityList_Community::firstkey() const {
  return *firstkey_;
}
inline void CommunityList_Community::set_firstkey(const ::std::string& value) {
  set_has_firstkey();
  if (firstkey_ == &::google::protobuf::internal::kEmptyString) {
    firstkey_ = new ::std::string;
  }
  firstkey_->assign(value);
}
inline void CommunityList_Community::set_firstkey(const char* value) {
  set_has_firstkey();
  if (firstkey_ == &::google::protobuf::internal::kEmptyString) {
    firstkey_ = new ::std::string;
  }
  firstkey_->assign(value);
}
inline void CommunityList_Community::set_firstkey(const char* value, size_t size) {
  set_has_firstkey();
  if (firstkey_ == &::google::protobuf::internal::kEmptyString) {
    firstkey_ = new ::std::string;
  }
  firstkey_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* CommunityList_Community::mutable_firstkey() {
  set_has_firstkey();
  if (firstkey_ == &::google::protobuf::internal::kEmptyString) {
    firstkey_ = new ::std::string;
  }
  return firstkey_;
}
inline ::std::string* CommunityList_Community::release_firstkey() {
  clear_has_firstkey();
  if (firstkey_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = firstkey_;
    firstkey_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void CommunityList_Community::set_allocated_firstkey(::std::string* firstkey) {
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
inline bool CommunityList_Community::has_label() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void CommunityList_Community::set_has_label() {
  _has_bits_[0] |= 0x00000002u;
}
inline void CommunityList_Community::clear_has_label() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void CommunityList_Community::clear_label() {
  if (label_ != &::google::protobuf::internal::kEmptyString) {
    label_->clear();
  }
  clear_has_label();
}
inline const ::std::string& CommunityList_Community::label() const {
  return *label_;
}
inline void CommunityList_Community::set_label(const ::std::string& value) {
  set_has_label();
  if (label_ == &::google::protobuf::internal::kEmptyString) {
    label_ = new ::std::string;
  }
  label_->assign(value);
}
inline void CommunityList_Community::set_label(const char* value) {
  set_has_label();
  if (label_ == &::google::protobuf::internal::kEmptyString) {
    label_ = new ::std::string;
  }
  label_->assign(value);
}
inline void CommunityList_Community::set_label(const char* value, size_t size) {
  set_has_label();
  if (label_ == &::google::protobuf::internal::kEmptyString) {
    label_ = new ::std::string;
  }
  label_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* CommunityList_Community::mutable_label() {
  set_has_label();
  if (label_ == &::google::protobuf::internal::kEmptyString) {
    label_ = new ::std::string;
  }
  return label_;
}
inline ::std::string* CommunityList_Community::release_label() {
  clear_has_label();
  if (label_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = label_;
    label_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void CommunityList_Community::set_allocated_label(::std::string* label) {
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
inline bool CommunityList_Community::has_face() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void CommunityList_Community::set_has_face() {
  _has_bits_[0] |= 0x00000004u;
}
inline void CommunityList_Community::clear_has_face() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void CommunityList_Community::clear_face() {
  if (face_ != &::google::protobuf::internal::kEmptyString) {
    face_->clear();
  }
  clear_has_face();
}
inline const ::std::string& CommunityList_Community::face() const {
  return *face_;
}
inline void CommunityList_Community::set_face(const ::std::string& value) {
  set_has_face();
  if (face_ == &::google::protobuf::internal::kEmptyString) {
    face_ = new ::std::string;
  }
  face_->assign(value);
}
inline void CommunityList_Community::set_face(const char* value) {
  set_has_face();
  if (face_ == &::google::protobuf::internal::kEmptyString) {
    face_ = new ::std::string;
  }
  face_->assign(value);
}
inline void CommunityList_Community::set_face(const char* value, size_t size) {
  set_has_face();
  if (face_ == &::google::protobuf::internal::kEmptyString) {
    face_ = new ::std::string;
  }
  face_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* CommunityList_Community::mutable_face() {
  set_has_face();
  if (face_ == &::google::protobuf::internal::kEmptyString) {
    face_ = new ::std::string;
  }
  return face_;
}
inline ::std::string* CommunityList_Community::release_face() {
  clear_has_face();
  if (face_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = face_;
    face_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void CommunityList_Community::set_allocated_face(::std::string* face) {
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
inline bool CommunityList_Community::has_sendtime() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void CommunityList_Community::set_has_sendtime() {
  _has_bits_[0] |= 0x00000008u;
}
inline void CommunityList_Community::clear_has_sendtime() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void CommunityList_Community::clear_sendtime() {
  sendtime_ = GOOGLE_LONGLONG(0);
  clear_has_sendtime();
}
inline ::google::protobuf::int64 CommunityList_Community::sendtime() const {
  return sendtime_;
}
inline void CommunityList_Community::set_sendtime(::google::protobuf::int64 value) {
  set_has_sendtime();
  sendtime_ = value;
}

// required int32 praiseCount = 5;
inline bool CommunityList_Community::has_praisecount() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void CommunityList_Community::set_has_praisecount() {
  _has_bits_[0] |= 0x00000010u;
}
inline void CommunityList_Community::clear_has_praisecount() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void CommunityList_Community::clear_praisecount() {
  praisecount_ = 0;
  clear_has_praisecount();
}
inline ::google::protobuf::int32 CommunityList_Community::praisecount() const {
  return praisecount_;
}
inline void CommunityList_Community::set_praisecount(::google::protobuf::int32 value) {
  set_has_praisecount();
  praisecount_ = value;
}

// required int32 sid = 6;
inline bool CommunityList_Community::has_sid() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void CommunityList_Community::set_has_sid() {
  _has_bits_[0] |= 0x00000020u;
}
inline void CommunityList_Community::clear_has_sid() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void CommunityList_Community::clear_sid() {
  sid_ = 0;
  clear_has_sid();
}
inline ::google::protobuf::int32 CommunityList_Community::sid() const {
  return sid_;
}
inline void CommunityList_Community::set_sid(::google::protobuf::int32 value) {
  set_has_sid();
  sid_ = value;
}

// required string seq = 7;
inline bool CommunityList_Community::has_seq() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void CommunityList_Community::set_has_seq() {
  _has_bits_[0] |= 0x00000040u;
}
inline void CommunityList_Community::clear_has_seq() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void CommunityList_Community::clear_seq() {
  if (seq_ != &::google::protobuf::internal::kEmptyString) {
    seq_->clear();
  }
  clear_has_seq();
}
inline const ::std::string& CommunityList_Community::seq() const {
  return *seq_;
}
inline void CommunityList_Community::set_seq(const ::std::string& value) {
  set_has_seq();
  if (seq_ == &::google::protobuf::internal::kEmptyString) {
    seq_ = new ::std::string;
  }
  seq_->assign(value);
}
inline void CommunityList_Community::set_seq(const char* value) {
  set_has_seq();
  if (seq_ == &::google::protobuf::internal::kEmptyString) {
    seq_ = new ::std::string;
  }
  seq_->assign(value);
}
inline void CommunityList_Community::set_seq(const char* value, size_t size) {
  set_has_seq();
  if (seq_ == &::google::protobuf::internal::kEmptyString) {
    seq_ = new ::std::string;
  }
  seq_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* CommunityList_Community::mutable_seq() {
  set_has_seq();
  if (seq_ == &::google::protobuf::internal::kEmptyString) {
    seq_ = new ::std::string;
  }
  return seq_;
}
inline ::std::string* CommunityList_Community::release_seq() {
  clear_has_seq();
  if (seq_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = seq_;
    seq_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void CommunityList_Community::set_allocated_seq(::std::string* seq) {
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

// CommunityList

// repeated .tutorial.CommunityList.Community community = 8;
inline int CommunityList::community_size() const {
  return community_.size();
}
inline void CommunityList::clear_community() {
  community_.Clear();
}
inline const ::tutorial::CommunityList_Community& CommunityList::community(int index) const {
  return community_.Get(index);
}
inline ::tutorial::CommunityList_Community* CommunityList::mutable_community(int index) {
  return community_.Mutable(index);
}
inline ::tutorial::CommunityList_Community* CommunityList::add_community() {
  return community_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::tutorial::CommunityList_Community >&
CommunityList::community() const {
  return community_;
}
inline ::google::protobuf::RepeatedPtrField< ::tutorial::CommunityList_Community >*
CommunityList::mutable_community() {
  return &community_;
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

#endif  // PROTOBUF_communityList_2eproto__INCLUDED
