#ifndef DUCKCHAT_H
#define DUCKCHAT_H

/* Path names to unix domain sockets should not be longer than this */
#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 108
#endif

/* This tells gcc to "pack" the structure.  Normally, gcc will
 * inserting padding into a structure if it feels it is convenient.
 * When the structure is packed, gcc gaurantees that all the bytes
 * will fall exactly where specified. */
#define packed __attribute__((packed))

/* Define the length limits */
#define REQ_MAX 4096
#define USERNAME_MAX 32
#define CHANNEL_MAX 32
#define SAY_MAX 64
#define HOST_MAX 256

/* Define some types for designating request and text codes */
typedef int request_t;
typedef int text_t;
typedef int s2s_t;

/* Define codes for request types.  These are the messages sent to the server. */
#define REQ_LOGIN 0
#define REQ_LOGOUT 1
#define REQ_JOIN 2
#define REQ_LEAVE 3
#define REQ_SAY 4
#define REQ_LIST 5
#define REQ_WHO 6
#define REQ_KEEP_ALIVE 7 /* Only needed by graduate students */


/* Define codes for text types.  These are the messages sent to the client. */
#define TXT_SAY 0
#define TXT_LIST 1
#define TXT_WHO 2
#define TXT_ERROR 3

/*
#define S2S_JOIN 8
#define S2S_LEAVE 9
#define S2S_SAY 10
*/
//_____________________________________________________________________
/* This structure is used for a generic request type, to the server. */
struct request {
        request_t req_type;
} packed;
/* Once we've looked at req_type, we then cast the pointer to one of
 * the types below to look deeper into the structure.  Each of these
 * corresponds with one of the REQ_ codes above. */
struct request_login {
        request_t req_type; /* = REQ_LOGIN */
        char req_username[USERNAME_MAX];
} packed;

struct request_logout {
        request_t req_type; /* = REQ_LOGOUT */
} packed;

struct request_join {
        request_t req_type; /* = REQ_JOIN */
        char req_channel[CHANNEL_MAX]; 
} packed;

struct request_leave {
        request_t req_type; /* = REQ_LEAVE */
        char req_channel[CHANNEL_MAX]; 
} packed;

struct request_say {
        request_t req_type; /* = REQ_SAY */
        char req_channel[CHANNEL_MAX]; 
        char req_text[SAY_MAX];
} packed;

struct request_list {
        request_t req_type; /* = REQ_LIST */
} packed;

struct request_who {
        request_t req_type; /* = REQ_WHO */
        char req_channel[CHANNEL_MAX]; 
} packed;

struct request_keep_alive {
        request_t req_type; /* = REQ_KEEP_ALIVE */
} packed;

//_____________________________________________________________________
/* This structure is used for a generic text type, to the client. */
struct text {
        text_t txt_type;
} packed;

/* Once we've looked at txt_type, we then cast the pointer to one of
 * the types below to look deeper into the structure.  Each of these
 * corresponds with one of the TXT_ codes above. */
struct text_say {
        text_t txt_type; /* = TXT_SAY */
        char txt_channel[CHANNEL_MAX];
        char txt_username[USERNAME_MAX];
        char txt_text[SAY_MAX];
} packed;
/* This is a substructure used by struct text_list. */
struct channel_info {
        char ch_channel[CHANNEL_MAX];
} packed;

struct text_list {
        text_t txt_type; /* = TXT_LIST */
        int txt_nchannels;
        struct channel_info txt_channels[0]; // May actually be more than 0
} packed;
/* This is a substructure used by text_who. */
struct user_info {
        char us_username[USERNAME_MAX];
};

struct text_who {
        text_t txt_type; /* = TXT_WHO */
        int txt_nusernames;
        char txt_channel[CHANNEL_MAX]; // The channel requested
        struct user_info txt_users[0]; // May actually be more than 0
} packed;

struct text_error {
        text_t txt_type; /* = TXT_ERROR */
        char txt_error[SAY_MAX]; // Error message
};

#endif
