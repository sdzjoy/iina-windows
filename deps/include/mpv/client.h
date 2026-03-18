/* mpv client API header stub
 * Full headers are downloaded by scripts/download-deps.ps1
 * This stub exists only so the project structure is complete for review.
 * See: https://github.com/mpv-player/mpv/blob/master/libmpv/client.h
 */
#ifndef MPV_CLIENT_H_
#define MPV_CLIENT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mpv_error {
    MPV_ERROR_SUCCESS = 0,
    MPV_ERROR_GENERIC = -20
} mpv_error;

typedef enum mpv_format {
    MPV_FORMAT_NONE = 0,
    MPV_FORMAT_STRING = 1,
    MPV_FORMAT_OSD_STRING = 2,
    MPV_FORMAT_FLAG = 3,
    MPV_FORMAT_INT64 = 4,
    MPV_FORMAT_DOUBLE = 5,
    MPV_FORMAT_NODE = 6,
    MPV_FORMAT_NODE_ARRAY = 7,
    MPV_FORMAT_NODE_MAP = 8,
    MPV_FORMAT_BYTE_ARRAY = 9
} mpv_format;

typedef enum mpv_event_id {
    MPV_EVENT_NONE = 0,
    MPV_EVENT_SHUTDOWN = 1,
    MPV_EVENT_LOG_MESSAGE = 2,
    MPV_EVENT_START_FILE = 6,
    MPV_EVENT_END_FILE = 7,
    MPV_EVENT_FILE_LOADED = 8,
    MPV_EVENT_PROPERTY_CHANGE = 22,
    MPV_EVENT_COMMAND_REPLY = 24
} mpv_event_id;

typedef struct mpv_node_list {
    int num;
    struct mpv_node *values;
    char **keys;
} mpv_node_list;

typedef struct mpv_node {
    union {
        char *string;
        int flag;
        int64_t int64;
        double double_;
        mpv_node_list *list;
    } u;
    mpv_format format;
} mpv_node;

typedef struct mpv_event_property {
    const char *name;
    mpv_format format;
    void *data;
} mpv_event_property;

typedef struct mpv_event {
    mpv_event_id event_id;
    int error;
    uint64_t reply_userdata;
    void *data;
} mpv_event;

typedef struct mpv_handle mpv_handle;

const char *mpv_error_string(int error);
mpv_handle *mpv_create(void);
int mpv_initialize(mpv_handle *ctx);
void mpv_terminate_destroy(mpv_handle *ctx);
int mpv_set_option_string(mpv_handle *ctx, const char *name, const char *data);
int mpv_command_async(mpv_handle *ctx, uint64_t reply_userdata, const char **args);
int mpv_set_property(mpv_handle *ctx, const char *name, mpv_format format, void *data);
int mpv_set_property_string(mpv_handle *ctx, const char *name, const char *data);
int mpv_get_property(mpv_handle *ctx, const char *name, mpv_format format, void *data);
int mpv_observe_property(mpv_handle *mpv, uint64_t reply_userdata, const char *name, mpv_format format);
mpv_event *mpv_wait_event(mpv_handle *ctx, double timeout);
void mpv_set_wakeup_callback(mpv_handle *ctx, void (*cb)(void *d), void *d);
void mpv_free_node_contents(mpv_node *node);

#ifdef __cplusplus
}
#endif

#endif
