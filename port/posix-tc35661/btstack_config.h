// config.h created by configure for BTstack  Tue Jun 4 23:10:20 CEST 2013

#ifndef __BTSTACK_CONFIG
#define __BTSTACK_CONFIG

// #define ENABLE_BLE

#define ENABLE_SDP
#define ENABLE_RFCOMM
#define REMOTE_DEVICE_DB remote_device_db_iphone
#define HAVE_SO_NOSIGPIPE
#define HAVE_TIME
#define HAVE_MALLOC
#define HAVE_BZERO
#define ENABLE_SDP_DES_DUMP
#define ENABLE_LOG_INFO 
#define ENABLE_LOG_ERROR
#define HCI_INCOMING_PRE_BUFFER_SIZE 14 // sizeof benep heade, avoid memcpy
#define HCI_ACL_PAYLOAD_SIZE (1691 + 4)
#define ENABLE_LOG_INTO_HCI_DUMP
#define ENABLE_SDP_DES_DUMP

#endif
