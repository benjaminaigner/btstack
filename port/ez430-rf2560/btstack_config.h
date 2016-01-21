
#ifndef __BTSTACK_CONFIG
#define __BTSTACK_CONFIG

#define EMBEDDED

#define ENABLE_BLE

#define HAVE_INIT_SCRIPT
#define HAVE_BZERO
#define HAVE_TICK

#define HAVE_EHCILL

#define ENABLE_LOG_INTO_HCI_DUMP
// #define ENABLE_LOG_ERROR
// #define ENABLE_LOG_INFO 

#define HCI_ACL_PAYLOAD_SIZE 52

// 
#define MAX_SPP_CONNECTIONS 1

#define MAX_NO_HCI_CONNECTIONS MAX_SPP_CONNECTIONS
#define MAX_NO_GATT_CLIENTS 0
#define MAX_NO_GATT_SUBCLIENTS 0
#define MAX_NO_L2CAP_SERVICES  2
#define MAX_NO_L2CAP_CHANNELS  (1+MAX_SPP_CONNECTIONS)
#define MAX_NO_RFCOMM_MULTIPLEXERS MAX_SPP_CONNECTIONS
#define MAX_NO_RFCOMM_SERVICES 1
#define MAX_NO_RFCOMM_CHANNELS MAX_SPP_CONNECTIONS
#define MAX_NO_BNEP_SERVICES 1
#define MAX_NO_BNEP_CHANNELS MAX_SPP_CONNECTIONS
#define MAX_NO_DB_MEM_DEVICE_LINK_KEYS  2
#define MAX_NO_DB_MEM_DEVICE_NAMES 0
#define MAX_NO_DB_MEM_SERVICES 1
#define MAX_NO_HFP_CONNECTIONS 0
#define MAX_NO_WHITELIST_ENTRIES 1
#define MAX_NO_SM_LOOKUP_ENTRIES 3
#define MAX_NO_SERVICE_RECORD_ITEMS 1

#endif


//
// btstack_config.h for Arduino port
//

#ifndef __BTSTACK_CONFIG
#define __BTSTACK_CONFIG

// Port related features
#define HAVE_INIT_SCRIPT
#define HAVE_BZERO
#define HAVE_TIME
#define HAVE_MALLOC

// BTstack features that can be enabled
#define ENABLE_BLE
#define ENABLE_LOG_DEBUG
#define ENABLE_LOG_ERROR
#define ENABLE_LOG_INFO 
#define ENABLE_LOG_INTO_HCI_DUMP
#define ENABLE_SDP_DES_DUMP
#define ENABLE_SDP_EXTRA_QUERIES

// BTstack configuration. buffers, sizes, ...
#define HCI_ACL_PAYLOAD_SIZE 52
#define HCI_INCOMING_PRE_BUFFER_SIZE 4

#endif
