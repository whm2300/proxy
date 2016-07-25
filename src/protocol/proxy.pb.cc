// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: proxy.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "proxy.pb.h"

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

namespace {

const ::google::protobuf::Descriptor* NewConnection_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  NewConnection_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_proxy_2eproto() {
  protobuf_AddDesc_proxy_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "proxy.proto");
  GOOGLE_CHECK(file != NULL);
  NewConnection_descriptor_ = file->message_type(0);
  static const int NewConnection_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(NewConnection, token_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(NewConnection, from_port_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(NewConnection, to_prot_),
  };
  NewConnection_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      NewConnection_descriptor_,
      NewConnection::default_instance_,
      NewConnection_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(NewConnection, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(NewConnection, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(NewConnection));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_proxy_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    NewConnection_descriptor_, &NewConnection::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_proxy_2eproto() {
  delete NewConnection::default_instance_;
  delete NewConnection_reflection_;
}

void protobuf_AddDesc_proxy_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\013proxy.proto\"B\n\rNewConnection\022\r\n\005token\030"
    "\001 \002(\005\022\021\n\tfrom_port\030\002 \002(\005\022\017\n\007to_prot\030\003 \002("
    "\005", 81);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "proxy.proto", &protobuf_RegisterTypes);
  NewConnection::default_instance_ = new NewConnection();
  NewConnection::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_proxy_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_proxy_2eproto {
  StaticDescriptorInitializer_proxy_2eproto() {
    protobuf_AddDesc_proxy_2eproto();
  }
} static_descriptor_initializer_proxy_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int NewConnection::kTokenFieldNumber;
const int NewConnection::kFromPortFieldNumber;
const int NewConnection::kToProtFieldNumber;
#endif  // !_MSC_VER

NewConnection::NewConnection()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void NewConnection::InitAsDefaultInstance() {
}

NewConnection::NewConnection(const NewConnection& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void NewConnection::SharedCtor() {
  _cached_size_ = 0;
  token_ = 0;
  from_port_ = 0;
  to_prot_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

NewConnection::~NewConnection() {
  SharedDtor();
}

void NewConnection::SharedDtor() {
  if (this != default_instance_) {
  }
}

void NewConnection::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* NewConnection::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return NewConnection_descriptor_;
}

const NewConnection& NewConnection::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_proxy_2eproto();
  return *default_instance_;
}

NewConnection* NewConnection::default_instance_ = NULL;

NewConnection* NewConnection::New() const {
  return new NewConnection;
}

void NewConnection::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    token_ = 0;
    from_port_ = 0;
    to_prot_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool NewConnection::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 token = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &token_)));
          set_has_token();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_from_port;
        break;
      }

      // required int32 from_port = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_from_port:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &from_port_)));
          set_has_from_port();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(24)) goto parse_to_prot;
        break;
      }

      // required int32 to_prot = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_to_prot:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &to_prot_)));
          set_has_to_prot();
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

void NewConnection::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required int32 token = 1;
  if (has_token()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->token(), output);
  }

  // required int32 from_port = 2;
  if (has_from_port()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->from_port(), output);
  }

  // required int32 to_prot = 3;
  if (has_to_prot()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->to_prot(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* NewConnection::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required int32 token = 1;
  if (has_token()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->token(), target);
  }

  // required int32 from_port = 2;
  if (has_from_port()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->from_port(), target);
  }

  // required int32 to_prot = 3;
  if (has_to_prot()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->to_prot(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int NewConnection::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required int32 token = 1;
    if (has_token()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->token());
    }

    // required int32 from_port = 2;
    if (has_from_port()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->from_port());
    }

    // required int32 to_prot = 3;
    if (has_to_prot()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->to_prot());
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

void NewConnection::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const NewConnection* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const NewConnection*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void NewConnection::MergeFrom(const NewConnection& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_token()) {
      set_token(from.token());
    }
    if (from.has_from_port()) {
      set_from_port(from.from_port());
    }
    if (from.has_to_prot()) {
      set_to_prot(from.to_prot());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void NewConnection::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void NewConnection::CopyFrom(const NewConnection& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool NewConnection::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000007) != 0x00000007) return false;

  return true;
}

void NewConnection::Swap(NewConnection* other) {
  if (other != this) {
    std::swap(token_, other->token_);
    std::swap(from_port_, other->from_port_);
    std::swap(to_prot_, other->to_prot_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata NewConnection::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = NewConnection_descriptor_;
  metadata.reflection = NewConnection_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)