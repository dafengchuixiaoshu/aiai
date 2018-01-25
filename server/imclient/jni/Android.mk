# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
 
LOCAL_PATH := $(call my-dir)

LOCAL_NDK := D:\tools\android\android-ndk-r8b

LOCAL_TARGET_PLATFORM := android-19

include $(CLEAR_VARS)

LOCAL_CFLAGS += \
-D__linux \
-DPOSIX \
-DUSE_SQLITE \
-DXML_STATIC \
-DHAVE_EXPAT_CONFIG_H \
-DLINUX \
-DMECHANISM_PIPE \
-D_GLIBCXX_PERMIT_BACKWARD_HASH \
-Wno-psabi -Wno-write-strings -DQCC_OS_GROUP_POSIX -DQCC_OS_ANDROID -DQCC_CPU_ARM -DNDEBUG \
-g -Wall -c -fmessage-length=0

LOCAL_CPPFLAGS += $(LOCAL_CFLAGS) -fpermissive

LOCAL_C_INCLUDES += \
$(LOCAL_PATH)/libim/src/zlib-1.2.5 \
$(LOCAL_PATH)/libim/src/sqlite \
$(LOCAL_PATH)/libim/src/xmpp \
$(LOCAL_PATH)/libim/src/xcap \
$(LOCAL_PATH)/libim/src/utils \
$(LOCAL_PATH)/libim/src/libclient \
$(LOCAL_PATH)/libim/src/libim \
$(LOCAL_PATH)/libim/src/protobuf \
$(LOCAL_PATH)/libim/src/protobuf/libprotobuf \
$(LOCAL_PATH)/libim/src/alljoyn \
$(LOCAL_PATH)/libim/src/alljoyn/liballjoyn \
$(LOCAL_PATH)/libim/src/alljoyn/liballjoyn/alljoyn \
$(LOCAL_PATH)/libim/src/alljoyn/liballjoyn/common \
$(LOCAL_PATH)/libim/src/alljoyn/liballjoyn/alljoyn_core \
$(LOCAL_PATH)/libim/src/alljoyn/liballjoyn/alljoyn_core/router \
$(LOCAL_PATH)/libim/src/alljoyn/liballjoyn/alljoyn_core/router/ns \
$(LOCAL_PATH)/libim/src/alljoyn/liballjoyn/alljoyn_core/router/posix \
$(LOCAL_NDK)/sources/cxx-stl/gnu-libstdc++/4.6/include \
$(LOCAL_NDK)/sources/cxx-stl/gnu-libstdc++/4.6/libs/armeabi/include \
$(LOCAL_NDK)/platforms/$(LOCAL_TARGET_PLATFORM)/arch-arm/usr/include

# link library
LOCAL_LDLIBS := \
-L$(LOCAL_PATH)/libim/src/alljoyn/openssl \
-L$(LOCAL_NDK)/sources/cxx-stl/gnu-libstdc++/4.6/libs/armeabi \
-L$(LOCAL_NDK)/platforms/$(LOCAL_TARGET_PLATFORM)/arch-arm/usr/lib \
-llog -lz -ldl -lssl -lcrypto -lm -lc -lstdc++ -lgcc -lgnustl_static

LOCAL_CPP_EXTENSION := .cpp .cc
LOCAL_MODULE    := imsdk-jni

LOCAL_SRC_FILES += \
libim/src/zlib-1.2.5/adler32.c \
libim/src/zlib-1.2.5/compress.c \
libim/src/zlib-1.2.5/crc32.c \
libim/src/zlib-1.2.5/deflate.c \
libim/src/zlib-1.2.5/gzclose.c \
libim/src/zlib-1.2.5/gzlib.c \
libim/src/zlib-1.2.5/gzread.c \
libim/src/zlib-1.2.5/gzwrite.c \
libim/src/zlib-1.2.5/infback.c \
libim/src/zlib-1.2.5/inffast.c \
libim/src/zlib-1.2.5/inflate.c \
libim/src/zlib-1.2.5/inftrees.c \
libim/src/zlib-1.2.5/trees.c \
libim/src/zlib-1.2.5/uncompr.c \
libim/src/zlib-1.2.5/zutil.c 

LOCAL_SRC_FILES += \
libim/src/sqlite/DBUpdate.cpp \
libim/src/sqlite/SqliteImpl.cpp \
libim/src/sqlite/sqlite3.c 

LOCAL_SRC_FILES += \
libim/src/utils/thread_pool.cpp \
libim/src/utils/EncodeUtil.cpp \
libim/src/utils/File.cpp \
libim/src/utils/NativeCrashHandler.cpp

LOCAL_SRC_FILES += \
libim/src/xcap/AutoBuffer.cpp \
libim/src/xcap/Markup.cpp \
libim/src/xcap/RequestBuilder.cpp \
libim/src/xcap/RequestBuilder_main2.cpp \
libim/src/xcap/ResponseProcessor_main2.cpp \
libim/src/xcap/XCapCore.cpp

LOCAL_SRC_FILES += \
libim/src/xmpp/CXmppStack.cpp

LOCAL_SRC_FILES += \
libim/src/libim/CConfigure.cpp \
libim/src/libim/CLog.cpp \
libim/src/libim/CIMI.cpp \
libim/src/libim/CObserver.cpp \
libim/src/libim/CObserverContainer.cpp \
libim/src/libim/BizCommon.cpp \
libim/src/libim/CWorker.cpp \
libim/src/libim/CWorkerContainer.cpp \
libim/src/libim/CWorkLogin.cpp \
libim/src/libim/CWorkMessage.cpp\
libim/src/libim/CWorkGroup.cpp \
libim/src/libim/CWorkContact.cpp \
libim/src/libim/CWorkUpdateTime.cpp

LOCAL_SRC_FILES += \
libim/src/libclient/base64.cpp \
libim/src/libclient/immodule.cpp \
libim/src/libclient/immsg.cpp \
libim/src/libclient/imsocket.cpp \
libim/src/libclient/protobyte.cpp \
libim/src/libclient/protoxml.cpp \
libim/src/libclient/thread.cpp\
libim/src/libclient/httpsocket.cpp\
#libim/src/libclient/memsave.cpp 

LOCAL_SRC_FILES += \
libim/src/protobuf/im.pb.cc\
libim/src/protobuf/libprotobuf/coded_stream.cc \
libim/src/protobuf/libprotobuf/common.cc \
libim/src/protobuf/libprotobuf/descriptor.cc \
libim/src/protobuf/libprotobuf/descriptor.pb.cc \
libim/src/protobuf/libprotobuf/descriptor_database.cc \
libim/src/protobuf/libprotobuf/dynamic_message.cc \
libim/src/protobuf/libprotobuf/extension_set.cc \
libim/src/protobuf/libprotobuf/extension_set_heavy.cc \
libim/src/protobuf/libprotobuf/generated_message_reflection.cc \
libim/src/protobuf/libprotobuf/generated_message_util.cc \
libim/src/protobuf/libprotobuf/gzip_stream.cc \
libim/src/protobuf/libprotobuf/importer.cc \
libim/src/protobuf/libprotobuf/message.cc \
libim/src/protobuf/libprotobuf/message_lite.cc \
libim/src/protobuf/libprotobuf/once.cc \
libim/src/protobuf/libprotobuf/parser.cc \
libim/src/protobuf/libprotobuf/printer.cc \
libim/src/protobuf/libprotobuf/reflection_ops.cc \
libim/src/protobuf/libprotobuf/repeated_field.cc \
libim/src/protobuf/libprotobuf/service.cc \
libim/src/protobuf/libprotobuf/stringprintf.cc \
libim/src/protobuf/libprotobuf/structurally_valid.cc \
libim/src/protobuf/libprotobuf/strutil.cc \
libim/src/protobuf/libprotobuf/substitute.cc \
libim/src/protobuf/libprotobuf/text_format.cc \
libim/src/protobuf/libprotobuf/tokenizer.cc \
libim/src/protobuf/libprotobuf/unknown_field_set.cc \
libim/src/protobuf/libprotobuf/wire_format.cc \
libim/src/protobuf/libprotobuf/wire_format_lite.cc \
libim/src/protobuf/libprotobuf/zero_copy_stream.cc \
libim/src/protobuf/libprotobuf/zero_copy_stream_impl.cc \
libim/src/protobuf/libprotobuf/zero_copy_stream_impl_lite.cc \
libim/src/protobuf/libprotobuf/atomicops_internals_arm_gcc.cc

LOCAL_SRC_FILES += \
libim/src/alljoyn/liballjoyn/common/ASN1.cc \
libim/src/alljoyn/liballjoyn/common/BigNum.cc \
libim/src/alljoyn/liballjoyn/common/BufferedSink.cc \
libim/src/alljoyn/liballjoyn/common/BufferedSource.cc \
libim/src/alljoyn/liballjoyn/common/CertificateECC.cc \
libim/src/alljoyn/liballjoyn/common/CommonGlobals.cc \
libim/src/alljoyn/liballjoyn/common/Config.cc \
libim/src/alljoyn/liballjoyn/common/Crypto.cc \
libim/src/alljoyn/liballjoyn/common/CryptoECC.cc \
libim/src/alljoyn/liballjoyn/common/CryptoSRP.cc \
libim/src/alljoyn/liballjoyn/common/Debug.cc \
libim/src/alljoyn/liballjoyn/common/GUID.cc \
libim/src/alljoyn/liballjoyn/common/IODispatch.cc \
libim/src/alljoyn/liballjoyn/common/IPAddress.cc \
libim/src/alljoyn/liballjoyn/common/KeyBlob.cc \
libim/src/alljoyn/liballjoyn/common/Logger.cc \
libim/src/alljoyn/liballjoyn/common/Pipe.cc \
libim/src/alljoyn/liballjoyn/common/SLAPPacket.cc \
libim/src/alljoyn/liballjoyn/common/SLAPStream.cc \
libim/src/alljoyn/liballjoyn/common/SocketStream.cc \
libim/src/alljoyn/liballjoyn/common/Stream.cc \
libim/src/alljoyn/liballjoyn/common/StreamPump.cc \
libim/src/alljoyn/liballjoyn/common/String.cc \
libim/src/alljoyn/liballjoyn/common/StringSource.cc \
libim/src/alljoyn/liballjoyn/common/StringUtil.cc \
libim/src/alljoyn/liballjoyn/common/ThreadPool.cc \
libim/src/alljoyn/liballjoyn/common/Timer.cc \
libim/src/alljoyn/liballjoyn/common/Util.cc \
libim/src/alljoyn/liballjoyn/common/XmlElement.cc \
libim/src/alljoyn/liballjoyn/common/posix/Condition.cc \
libim/src/alljoyn/liballjoyn/common/posix/Environ.cc \
libim/src/alljoyn/liballjoyn/common/posix/Event.cc \
libim/src/alljoyn/liballjoyn/common/posix/FileStream.cc \
libim/src/alljoyn/liballjoyn/common/posix/IfConfigDarwin.cc \
libim/src/alljoyn/liballjoyn/common/posix/IfConfigLinux.cc \
libim/src/alljoyn/liballjoyn/common/posix/Mutex.cc \
libim/src/alljoyn/liballjoyn/common/posix/OSLogger.cc \
libim/src/alljoyn/liballjoyn/common/posix/RWLock.cc \
libim/src/alljoyn/liballjoyn/common/posix/Socket.cc \
libim/src/alljoyn/liballjoyn/common/posix/Thread.cc \
libim/src/alljoyn/liballjoyn/common/posix/UARTStreamDarwin.cc \
libim/src/alljoyn/liballjoyn/common/posix/UARTStreamLinux.cc \
libim/src/alljoyn/liballjoyn/common/posix/atomic.cc \
libim/src/alljoyn/liballjoyn/common/posix/osUtil.cc \
libim/src/alljoyn/liballjoyn/common/posix/time.cc \
libim/src/alljoyn/liballjoyn/common/crypto/openssl/CryptoAES.cc \
libim/src/alljoyn/liballjoyn/common/crypto/openssl/CryptoHash.cc \
libim/src/alljoyn/liballjoyn/common/crypto/openssl/CryptoRSA.cc \
libim/src/alljoyn/liballjoyn/common/crypto/openssl/CryptoRand.cc \
libim/src/alljoyn/liballjoyn/common/crypto/openssl/OpenSsl.cc

LOCAL_SRC_FILES += \
libim/src/alljoyn/liballjoyn/alljoyn_core/AboutData.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/AboutIcon.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/AboutIconObj.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/AboutIconProxy.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/AboutObj.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/AboutObjectDescription.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/AboutProxy.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/AllJoynCrypto.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/AllJoynPeerObj.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/AllJoynStd.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/AuthMechLogon.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/AuthMechPIN.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/AuthMechRSA.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/AuthMechSRP.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/AutoPinger.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/BusAttachment.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/BusEndpoint.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/BusObject.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/BusUtil.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/ClientRouter.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/ClientTransport.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/CompressionRules.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/CredentialAccessor.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/DBusCookieSHA1.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/DBusStd.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/EndpointAuth.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/InterfaceDescription.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/KeyExchanger.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/KeyStore.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/LocalTransport.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/Message.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/Message_Gen.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/Message_Parse.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/MethodTable.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/MsgArg.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/NullTransport.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/PasswordManager.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/PeerState.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/ProtectedAuthListener.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/ProxyBusObject.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/RemoteEndpoint.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/Rule.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/SASLEngine.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/SessionOpts.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/SignalTable.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/SignatureUtils.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/SimpleBusListener.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/Transport.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/TransportList.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/XmlHelper.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/Status.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/version.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/posix/ClientTransport.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/posix/NamedPipeClientTransport.cc

LOCAL_SRC_FILES += \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/AllJoynDebugObj.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/AllJoynObj.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/ArdpProtocol.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/Bus.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/BusController.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/ConfigDB.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/ConvertUTF.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/DaemonRouter.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/DaemonSLAPTransport.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/DaemonTransport.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/DBusObj.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/NameTable.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/PermissionMgr.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/RuleTable.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/SessionlessObj.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/TCPTransport.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/UDPTransport.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/VirtualEndpoint.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/ns/IpNameService.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/ns/IpNameServiceImpl.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/ns/IpNsProtocol.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/bundled/BundledRouter.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/posix/DaemonTransportPosix.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/posix/Socket.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/android/P2PConMan.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/android/P2PConManImpl.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/android/P2PHelperInterface.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/android/P2PNameService.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/android/P2PNameServiceImpl.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/android/PermissionDB.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/android/PermissionMgrAndroid.cc \
libim/src/alljoyn/liballjoyn/alljoyn_core/router/android/WFDTransport.cc
#libim/src/alljoyn/liballjoyn/alljoyn_core/router/posix/PermissionMgrPosix.cc \

LOCAL_SRC_FILES += \
libim/src/alljoyn/lanchat.cpp

LOCAL_SRC_FILES += \
thread_env_manager.cpp \
IMSDKJni.cpp

include $(BUILD_SHARED_LIBRARY)
