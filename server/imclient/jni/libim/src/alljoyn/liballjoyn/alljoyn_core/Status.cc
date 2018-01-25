
/**
 * @file
 * This file contains an enumerated list values that QStatus can return
 *
 * Note: This file is generated during the make process.
 */

/******************************************************************************
 * Copyright (c) 2009-2014, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include <stdio.h>
#include "Status.h"

#define CASE(_status) case _status: return #_status 
    
AJ_API const char* AJ_CALL QCC_StatusText(QStatus status)
{
    switch (status) {
        CASE(ER_OK);
        CASE(ER_FAIL);
        CASE(ER_UTF_CONVERSION_FAILED);
        CASE(ER_BUFFER_TOO_SMALL);
        CASE(ER_OS_ERROR);
        CASE(ER_OUT_OF_MEMORY);
        CASE(ER_SOCKET_BIND_ERROR);
        CASE(ER_INIT_FAILED);
        CASE(ER_WOULDBLOCK);
        CASE(ER_NOT_IMPLEMENTED);
        CASE(ER_TIMEOUT);
        CASE(ER_SOCK_OTHER_END_CLOSED);
        CASE(ER_BAD_ARG_1);
        CASE(ER_BAD_ARG_2);
        CASE(ER_BAD_ARG_3);
        CASE(ER_BAD_ARG_4);
        CASE(ER_BAD_ARG_5);
        CASE(ER_BAD_ARG_6);
        CASE(ER_BAD_ARG_7);
        CASE(ER_BAD_ARG_8);
        CASE(ER_INVALID_ADDRESS);
        CASE(ER_INVALID_DATA);
        CASE(ER_READ_ERROR);
        CASE(ER_WRITE_ERROR);
        CASE(ER_OPEN_FAILED);
        CASE(ER_PARSE_ERROR);
        CASE(ER_END_OF_DATA);
        CASE(ER_CONN_REFUSED);
        CASE(ER_BAD_ARG_COUNT);
        CASE(ER_WARNING);
        CASE(ER_EOF);
        CASE(ER_COMMON_ERRORS);
        CASE(ER_STOPPING_THREAD);
        CASE(ER_ALERTED_THREAD);
        CASE(ER_XML_MALFORMED);
        CASE(ER_AUTH_FAIL);
        CASE(ER_AUTH_USER_REJECT);
        CASE(ER_NO_SUCH_ALARM);
        CASE(ER_TIMER_FALLBEHIND);
        CASE(ER_SSL_ERRORS);
        CASE(ER_SSL_INIT);
        CASE(ER_SSL_CONNECT);
        CASE(ER_SSL_VERIFY);
        CASE(ER_EXTERNAL_THREAD);
        CASE(ER_CRYPTO_ERROR);
        CASE(ER_CRYPTO_TRUNCATED);
        CASE(ER_CRYPTO_KEY_UNAVAILABLE);
        CASE(ER_BAD_HOSTNAME);
        CASE(ER_CRYPTO_KEY_UNUSABLE);
        CASE(ER_EMPTY_KEY_BLOB);
        CASE(ER_CORRUPT_KEYBLOB);
        CASE(ER_INVALID_KEY_ENCODING);
        CASE(ER_DEAD_THREAD);
        CASE(ER_THREAD_RUNNING);
        CASE(ER_THREAD_STOPPING);
        CASE(ER_BAD_STRING_ENCODING);
        CASE(ER_CRYPTO_INSUFFICIENT_SECURITY);
        CASE(ER_CRYPTO_ILLEGAL_PARAMETERS);
        CASE(ER_CRYPTO_HASH_UNINITIALIZED);
        CASE(ER_THREAD_NO_WAIT);
        CASE(ER_TIMER_EXITING);
        CASE(ER_INVALID_GUID);
        CASE(ER_THREADPOOL_EXHAUSTED);
        CASE(ER_THREADPOOL_STOPPING);
        CASE(ER_INVALID_STREAM);
        CASE(ER_TIMER_FULL);
        CASE(ER_IODISPATCH_STOPPING);
        CASE(ER_SLAP_INVALID_PACKET_LEN);
        CASE(ER_SLAP_HDR_CHECKSUM_ERROR);
        CASE(ER_SLAP_INVALID_PACKET_TYPE);
        CASE(ER_SLAP_LEN_MISMATCH);
        CASE(ER_SLAP_PACKET_TYPE_MISMATCH);
        CASE(ER_SLAP_CRC_ERROR);
        CASE(ER_SLAP_ERROR);
        CASE(ER_SLAP_OTHER_END_CLOSED);
        CASE(ER_TIMER_NOT_ALLOWED);
        CASE(ER_NONE);
        CASE(ER_BUS_ERRORS);
        CASE(ER_BUS_READ_ERROR);
        CASE(ER_BUS_WRITE_ERROR);
        CASE(ER_BUS_BAD_VALUE_TYPE);
        CASE(ER_BUS_BAD_HEADER_FIELD);
        CASE(ER_BUS_BAD_SIGNATURE);
        CASE(ER_BUS_BAD_OBJ_PATH);
        CASE(ER_BUS_BAD_MEMBER_NAME);
        CASE(ER_BUS_BAD_INTERFACE_NAME);
        CASE(ER_BUS_BAD_ERROR_NAME);
        CASE(ER_BUS_BAD_BUS_NAME);
        CASE(ER_BUS_NAME_TOO_LONG);
        CASE(ER_BUS_BAD_LENGTH);
        CASE(ER_BUS_BAD_VALUE);
        CASE(ER_BUS_BAD_HDR_FLAGS);
        CASE(ER_BUS_BAD_BODY_LEN);
        CASE(ER_BUS_BAD_HEADER_LEN);
        CASE(ER_BUS_UNKNOWN_SERIAL);
        CASE(ER_BUS_UNKNOWN_PATH);
        CASE(ER_BUS_UNKNOWN_INTERFACE);
        CASE(ER_BUS_ESTABLISH_FAILED);
        CASE(ER_BUS_UNEXPECTED_SIGNATURE);
        CASE(ER_BUS_INTERFACE_MISSING);
        CASE(ER_BUS_PATH_MISSING);
        CASE(ER_BUS_MEMBER_MISSING);
        CASE(ER_BUS_REPLY_SERIAL_MISSING);
        CASE(ER_BUS_ERROR_NAME_MISSING);
        CASE(ER_BUS_INTERFACE_NO_SUCH_MEMBER);
        CASE(ER_BUS_NO_SUCH_OBJECT);
        CASE(ER_BUS_OBJECT_NO_SUCH_MEMBER);
        CASE(ER_BUS_OBJECT_NO_SUCH_INTERFACE);
        CASE(ER_BUS_NO_SUCH_INTERFACE);
        CASE(ER_BUS_MEMBER_NO_SUCH_SIGNATURE);
        CASE(ER_BUS_NOT_NUL_TERMINATED);
        CASE(ER_BUS_NO_SUCH_PROPERTY);
        CASE(ER_BUS_SET_WRONG_SIGNATURE);
        CASE(ER_BUS_PROPERTY_VALUE_NOT_SET);
        CASE(ER_BUS_PROPERTY_ACCESS_DENIED);
        CASE(ER_BUS_NO_TRANSPORTS);
        CASE(ER_BUS_BAD_TRANSPORT_ARGS);
        CASE(ER_BUS_NO_ROUTE);
        CASE(ER_BUS_NO_ENDPOINT);
        CASE(ER_BUS_BAD_SEND_PARAMETER);
        CASE(ER_BUS_UNMATCHED_REPLY_SERIAL);
        CASE(ER_BUS_BAD_SENDER_ID);
        CASE(ER_BUS_TRANSPORT_NOT_STARTED);
        CASE(ER_BUS_EMPTY_MESSAGE);
        CASE(ER_BUS_NOT_OWNER);
        CASE(ER_BUS_SET_PROPERTY_REJECTED);
        CASE(ER_BUS_CONNECT_FAILED);
        CASE(ER_BUS_REPLY_IS_ERROR_MESSAGE);
        CASE(ER_BUS_NOT_AUTHENTICATING);
        CASE(ER_BUS_NO_LISTENER);
        CASE(ER_BUS_NOT_ALLOWED);
        CASE(ER_BUS_WRITE_QUEUE_FULL);
        CASE(ER_BUS_ENDPOINT_CLOSING);
        CASE(ER_BUS_INTERFACE_MISMATCH);
        CASE(ER_BUS_MEMBER_ALREADY_EXISTS);
        CASE(ER_BUS_PROPERTY_ALREADY_EXISTS);
        CASE(ER_BUS_IFACE_ALREADY_EXISTS);
        CASE(ER_BUS_ERROR_RESPONSE);
        CASE(ER_BUS_BAD_XML);
        CASE(ER_BUS_BAD_CHILD_PATH);
        CASE(ER_BUS_OBJ_ALREADY_EXISTS);
        CASE(ER_BUS_OBJ_NOT_FOUND);
        CASE(ER_BUS_CANNOT_EXPAND_MESSAGE);
        CASE(ER_BUS_NOT_COMPRESSED);
        CASE(ER_BUS_ALREADY_CONNECTED);
        CASE(ER_BUS_NOT_CONNECTED);
        CASE(ER_BUS_ALREADY_LISTENING);
        CASE(ER_BUS_KEY_UNAVAILABLE);
        CASE(ER_BUS_TRUNCATED);
        CASE(ER_BUS_KEY_STORE_NOT_LOADED);
        CASE(ER_BUS_NO_AUTHENTICATION_MECHANISM);
        CASE(ER_BUS_BUS_ALREADY_STARTED);
        CASE(ER_BUS_BUS_NOT_STARTED);
        CASE(ER_BUS_KEYBLOB_OP_INVALID);
        CASE(ER_BUS_INVALID_HEADER_CHECKSUM);
        CASE(ER_BUS_MESSAGE_NOT_ENCRYPTED);
        CASE(ER_BUS_INVALID_HEADER_SERIAL);
        CASE(ER_BUS_TIME_TO_LIVE_EXPIRED);
        CASE(ER_BUS_HDR_EXPANSION_INVALID);
        CASE(ER_BUS_MISSING_COMPRESSION_TOKEN);
        CASE(ER_BUS_NO_PEER_GUID);
        CASE(ER_BUS_MESSAGE_DECRYPTION_FAILED);
        CASE(ER_BUS_SECURITY_FATAL);
        CASE(ER_BUS_KEY_EXPIRED);
        CASE(ER_BUS_CORRUPT_KEYSTORE);
        CASE(ER_BUS_NO_CALL_FOR_REPLY);
        CASE(ER_BUS_NOT_A_COMPLETE_TYPE);
        CASE(ER_BUS_POLICY_VIOLATION);
        CASE(ER_BUS_NO_SUCH_SERVICE);
        CASE(ER_BUS_TRANSPORT_NOT_AVAILABLE);
        CASE(ER_BUS_INVALID_AUTH_MECHANISM);
        CASE(ER_BUS_KEYSTORE_VERSION_MISMATCH);
        CASE(ER_BUS_BLOCKING_CALL_NOT_ALLOWED);
        CASE(ER_BUS_SIGNATURE_MISMATCH);
        CASE(ER_BUS_STOPPING);
        CASE(ER_BUS_METHOD_CALL_ABORTED);
        CASE(ER_BUS_CANNOT_ADD_INTERFACE);
        CASE(ER_BUS_CANNOT_ADD_HANDLER);
        CASE(ER_BUS_KEYSTORE_NOT_LOADED);
        CASE(ER_BUS_NO_SUCH_HANDLE);
        CASE(ER_BUS_HANDLES_NOT_ENABLED);
        CASE(ER_BUS_HANDLES_MISMATCH);
        CASE(ER_BUS_NO_SESSION);
        CASE(ER_BUS_ELEMENT_NOT_FOUND);
        CASE(ER_BUS_NOT_A_DICTIONARY);
        CASE(ER_BUS_WAIT_FAILED);
        CASE(ER_BUS_BAD_SESSION_OPTS);
        CASE(ER_BUS_CONNECTION_REJECTED);
        CASE(ER_DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER);
        CASE(ER_DBUS_REQUEST_NAME_REPLY_IN_QUEUE);
        CASE(ER_DBUS_REQUEST_NAME_REPLY_EXISTS);
        CASE(ER_DBUS_REQUEST_NAME_REPLY_ALREADY_OWNER);
        CASE(ER_DBUS_RELEASE_NAME_REPLY_RELEASED);
        CASE(ER_DBUS_RELEASE_NAME_REPLY_NON_EXISTENT);
        CASE(ER_DBUS_RELEASE_NAME_REPLY_NOT_OWNER);
        CASE(ER_DBUS_START_REPLY_ALREADY_RUNNING);
        CASE(ER_ALLJOYN_BINDSESSIONPORT_REPLY_ALREADY_EXISTS);
        CASE(ER_ALLJOYN_BINDSESSIONPORT_REPLY_FAILED);
        CASE(ER_ALLJOYN_JOINSESSION_REPLY_NO_SESSION);
        CASE(ER_ALLJOYN_JOINSESSION_REPLY_UNREACHABLE);
        CASE(ER_ALLJOYN_JOINSESSION_REPLY_CONNECT_FAILED);
        CASE(ER_ALLJOYN_JOINSESSION_REPLY_REJECTED);
        CASE(ER_ALLJOYN_JOINSESSION_REPLY_BAD_SESSION_OPTS);
        CASE(ER_ALLJOYN_JOINSESSION_REPLY_FAILED);
        CASE(ER_ALLJOYN_LEAVESESSION_REPLY_NO_SESSION);
        CASE(ER_ALLJOYN_LEAVESESSION_REPLY_FAILED);
        CASE(ER_ALLJOYN_ADVERTISENAME_REPLY_TRANSPORT_NOT_AVAILABLE);
        CASE(ER_ALLJOYN_ADVERTISENAME_REPLY_ALREADY_ADVERTISING);
        CASE(ER_ALLJOYN_ADVERTISENAME_REPLY_FAILED);
        CASE(ER_ALLJOYN_CANCELADVERTISENAME_REPLY_FAILED);
        CASE(ER_ALLJOYN_FINDADVERTISEDNAME_REPLY_TRANSPORT_NOT_AVAILABLE);
        CASE(ER_ALLJOYN_FINDADVERTISEDNAME_REPLY_ALREADY_DISCOVERING);
        CASE(ER_ALLJOYN_FINDADVERTISEDNAME_REPLY_FAILED);
        CASE(ER_ALLJOYN_CANCELFINDADVERTISEDNAME_REPLY_FAILED);
        CASE(ER_BUS_UNEXPECTED_DISPOSITION);
        CASE(ER_BUS_INTERFACE_ACTIVATED);
        CASE(ER_ALLJOYN_UNBINDSESSIONPORT_REPLY_BAD_PORT);
        CASE(ER_ALLJOYN_UNBINDSESSIONPORT_REPLY_FAILED);
        CASE(ER_ALLJOYN_BINDSESSIONPORT_REPLY_INVALID_OPTS);
        CASE(ER_ALLJOYN_JOINSESSION_REPLY_ALREADY_JOINED);
        CASE(ER_BUS_SELF_CONNECT);
        CASE(ER_BUS_SECURITY_NOT_ENABLED);
        CASE(ER_BUS_LISTENER_ALREADY_SET);
        CASE(ER_BUS_PEER_AUTH_VERSION_MISMATCH);
        CASE(ER_ALLJOYN_SETLINKTIMEOUT_REPLY_NOT_SUPPORTED);
        CASE(ER_ALLJOYN_SETLINKTIMEOUT_REPLY_NO_DEST_SUPPORT);
        CASE(ER_ALLJOYN_SETLINKTIMEOUT_REPLY_FAILED);
        CASE(ER_ALLJOYN_ACCESS_PERMISSION_WARNING);
        CASE(ER_ALLJOYN_ACCESS_PERMISSION_ERROR);
        CASE(ER_BUS_DESTINATION_NOT_AUTHENTICATED);
        CASE(ER_BUS_ENDPOINT_REDIRECTED);
        CASE(ER_BUS_AUTHENTICATION_PENDING);
        CASE(ER_BUS_NOT_AUTHORIZED);
        CASE(ER_PACKET_BUS_NO_SUCH_CHANNEL);
        CASE(ER_PACKET_BAD_FORMAT);
        CASE(ER_PACKET_CONNECT_TIMEOUT);
        CASE(ER_PACKET_CHANNEL_FAIL);
        CASE(ER_PACKET_TOO_LARGE);
        CASE(ER_PACKET_BAD_PARAMETER);
        CASE(ER_PACKET_BAD_CRC);
        CASE(ER_RENDEZVOUS_SERVER_DEACTIVATED_USER);
        CASE(ER_RENDEZVOUS_SERVER_UNKNOWN_USER);
        CASE(ER_UNABLE_TO_CONNECT_TO_RENDEZVOUS_SERVER);
        CASE(ER_NOT_CONNECTED_TO_RENDEZVOUS_SERVER);
        CASE(ER_UNABLE_TO_SEND_MESSAGE_TO_RENDEZVOUS_SERVER);
        CASE(ER_INVALID_RENDEZVOUS_SERVER_INTERFACE_MESSAGE);
        CASE(ER_INVALID_PERSISTENT_CONNECTION_MESSAGE_RESPONSE);
        CASE(ER_INVALID_ON_DEMAND_CONNECTION_MESSAGE_RESPONSE);
        CASE(ER_INVALID_HTTP_METHOD_USED_FOR_RENDEZVOUS_SERVER_INTERFACE_MESSAGE);
        CASE(ER_RENDEZVOUS_SERVER_ERR500_INTERNAL_ERROR);
        CASE(ER_RENDEZVOUS_SERVER_ERR503_STATUS_UNAVAILABLE);
        CASE(ER_RENDEZVOUS_SERVER_ERR401_UNAUTHORIZED_REQUEST);
        CASE(ER_RENDEZVOUS_SERVER_UNRECOVERABLE_ERROR);
        CASE(ER_RENDEZVOUS_SERVER_ROOT_CERTIFICATE_UNINITIALIZED);
        CASE(ER_BUS_NO_SUCH_ANNOTATION);
        CASE(ER_BUS_ANNOTATION_ALREADY_EXISTS);
        CASE(ER_SOCK_CLOSING);
        CASE(ER_NO_SUCH_DEVICE);
        CASE(ER_P2P);
        CASE(ER_P2P_TIMEOUT);
        CASE(ER_P2P_NOT_CONNECTED);
        CASE(ER_BAD_TRANSPORT_MASK);
        CASE(ER_PROXIMITY_CONNECTION_ESTABLISH_FAIL);
        CASE(ER_PROXIMITY_NO_PEERS_FOUND);
        CASE(ER_BUS_OBJECT_NOT_REGISTERED);
        CASE(ER_P2P_DISABLED);
        CASE(ER_P2P_BUSY);
        CASE(ER_BUS_INCOMPATIBLE_DAEMON);
        CASE(ER_P2P_NO_GO);
        CASE(ER_P2P_NO_STA);
        CASE(ER_P2P_FORBIDDEN);
        CASE(ER_ALLJOYN_ONAPPSUSPEND_REPLY_FAILED);
        CASE(ER_ALLJOYN_ONAPPSUSPEND_REPLY_UNSUPPORTED);
        CASE(ER_ALLJOYN_ONAPPRESUME_REPLY_FAILED);
        CASE(ER_ALLJOYN_ONAPPRESUME_REPLY_UNSUPPORTED);
        CASE(ER_BUS_NO_SUCH_MESSAGE);
        CASE(ER_ALLJOYN_REMOVESESSIONMEMBER_REPLY_NO_SESSION);
        CASE(ER_ALLJOYN_REMOVESESSIONMEMBER_NOT_BINDER);
        CASE(ER_ALLJOYN_REMOVESESSIONMEMBER_NOT_MULTIPOINT);
        CASE(ER_ALLJOYN_REMOVESESSIONMEMBER_NOT_FOUND);
        CASE(ER_ALLJOYN_REMOVESESSIONMEMBER_INCOMPATIBLE_REMOTE_DAEMON);
        CASE(ER_ALLJOYN_REMOVESESSIONMEMBER_REPLY_FAILED);
        CASE(ER_BUS_REMOVED_BY_BINDER);
        CASE(ER_BUS_MATCH_RULE_NOT_FOUND);
        CASE(ER_ALLJOYN_PING_FAILED);
        CASE(ER_ALLJOYN_PING_REPLY_UNREACHABLE);
        CASE(ER_UDP_MSG_TOO_LONG);
        CASE(ER_UDP_DEMUX_NO_ENDPOINT);
        CASE(ER_UDP_NO_NETWORK);
        CASE(ER_UDP_UNEXPECTED_LENGTH);
        CASE(ER_UDP_UNEXPECTED_FLOW);
        CASE(ER_UDP_DISCONNECT);
        CASE(ER_UDP_NOT_IMPLEMENTED);
        CASE(ER_UDP_NO_LISTENER);
        CASE(ER_UDP_STOPPING);
        CASE(ER_ARDP_BACKPRESSURE);
        CASE(ER_UDP_BACKPRESSURE);
        CASE(ER_ARDP_INVALID_STATE);
        CASE(ER_ARDP_TTL_EXPIRED);
        CASE(ER_ARDP_PERSIST_TIMEOUT);
        CASE(ER_ARDP_PROBE_TIMEOUT);
        CASE(ER_ARDP_REMOTE_CONNECTION_RESET);
        CASE(ER_UDP_BUSHELLO);
        CASE(ER_UDP_MESSAGE);
        CASE(ER_UDP_INVALID);
        CASE(ER_UDP_UNSUPPORTED);
        CASE(ER_UDP_ENDPOINT_STALLED);
        CASE(ER_ARDP_INVALID_RESPONSE);
        CASE(ER_ARDP_INVALID_CONNECTION);
        CASE(ER_UDP_LOCAL_DISCONNECT);
        CASE(ER_UDP_EARLY_EXIT);
        CASE(ER_UDP_LOCAL_DISCONNECT_FAIL);
        CASE(ER_ARDP_DISCONNECTING);
        CASE(ER_ALLJOYN_PING_REPLY_INCOMPATIBLE_REMOTE_ROUTING_NODE);
        CASE(ER_ALLJOYN_PING_REPLY_TIMEOUT);
        CASE(ER_ALLJOYN_PING_REPLY_UNKNOWN_NAME);
        CASE(ER_ALLJOYN_PING_REPLY_FAILED);
        CASE(ER_TCP_MAX_UNTRUSTED);
        CASE(ER_ALLJOYN_PING_REPLY_IN_PROGRESS);
        CASE(ER_LANGUAGE_NOT_SUPPORTED);
        CASE(ER_ABOUT_FIELD_ALREADY_SPECIFIED);
        CASE(ER_UDP_NOT_DISCONNECTED);
        CASE(ER_UDP_ENDPOINT_NOT_STARTED);
        CASE(ER_UDP_ENDPOINT_REMOVED);
        CASE(ER_ARDP_VERSION_NOT_SUPPORTED);
        CASE(ER_CONNECTION_LIMIT_EXCEEDED);
        CASE(ER_ARDP_WRITE_BLOCKED);
        CASE(ER_PERMISSION_DENIED);
        CASE(ER_ABOUT_DEFAULT_LANGUAGE_NOT_SPECIFIED);
        CASE(ER_ABOUT_SESSIONPORT_NOT_BOUND);
        CASE(ER_ABOUT_ABOUTDATA_MISSING_REQUIRED_FIELD);
        CASE(ER_ABOUT_INVALID_ABOUTDATA_LISTENER);
        CASE(ER_BUS_PING_GROUP_NOT_FOUND);
        CASE(ER_BUS_REMOVED_BY_BINDER_SELF);
        CASE(ER_INVALID_CONFIG);
        CASE(ER_ABOUT_INVALID_ABOUTDATA_FIELD_VALUE);
        CASE(ER_ABOUT_INVALID_ABOUTDATA_FIELD_APPID_SIZE);
    default:
        static char code[20];
#ifdef _WIN32
        _snprintf(code, sizeof(code), "<unknown>: 0x%04x", status);
#else
        snprintf(code, sizeof(code), "<unknown>: 0x%04x", status);
#endif
        return code;
    }
}
