/* mpv render API header stub
 * Full headers are downloaded by scripts/download-deps.ps1
 * See: https://github.com/mpv-player/mpv/blob/master/libmpv/render.h
 */
#ifndef MPV_RENDER_H_
#define MPV_RENDER_H_

#include "client.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mpv_render_param_type {
    MPV_RENDER_PARAM_INVALID = 0,
    MPV_RENDER_PARAM_API_TYPE = 1,
    MPV_RENDER_PARAM_OPENGL_INIT_PARAMS = 2,
    MPV_RENDER_PARAM_OPENGL_FBO = 3,
    MPV_RENDER_PARAM_FLIP_Y = 4,
    MPV_RENDER_PARAM_DEPTH = 5,
    MPV_RENDER_PARAM_ICC_PROFILE = 6,
    MPV_RENDER_PARAM_AMBIENT_LIGHT = 7,
    MPV_RENDER_PARAM_NEXT_FRAME_INFO = 8,
    MPV_RENDER_PARAM_BLOCK_FOR_TARGET_TIME = 9,
    MPV_RENDER_PARAM_SKIP_RENDERING = 10
} mpv_render_param_type;

typedef struct mpv_render_param {
    mpv_render_param_type type;
    void *data;
} mpv_render_param;

typedef struct mpv_render_context mpv_render_context;

int mpv_render_context_create(mpv_render_context **res, mpv_handle *mpv, mpv_render_param *params);
int mpv_render_context_render(mpv_render_context *ctx, mpv_render_param *params);
void mpv_render_context_set_update_callback(mpv_render_context *ctx, void (*callback)(void *cb_ctx), void *cb_ctx);
void mpv_render_context_free(mpv_render_context *ctx);

#ifdef __cplusplus
}
#endif

#endif
