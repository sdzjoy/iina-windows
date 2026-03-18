/* mpv render GL API header stub
 * Full headers are downloaded by scripts/download-deps.ps1
 * See: https://github.com/mpv-player/mpv/blob/master/libmpv/render_gl.h
 */
#ifndef MPV_RENDER_GL_H_
#define MPV_RENDER_GL_H_

#include "render.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mpv_opengl_init_params {
    void *(*get_proc_address)(void *ctx, const char *name);
    void *get_proc_address_ctx;
} mpv_opengl_init_params;

typedef struct mpv_opengl_fbo {
    int fbo;
    int w;
    int h;
    int internal_format;
} mpv_opengl_fbo;

#define MPV_RENDER_API_TYPE_OPENGL "opengl"

#ifdef __cplusplus
}
#endif

#endif
