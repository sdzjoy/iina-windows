#include "mpvrenderer.h"
#include "mpvengine.h"

#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QQuickWindow>
#include <QDebug>

#include <mpv/client.h>
#include <mpv/render_gl.h>

static void *get_proc_address(void *ctx, const char *name)
{
    Q_UNUSED(ctx)
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx) return nullptr;
    return reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));
}

static void on_mpv_render_update(void *ctx)
{
    QMetaObject::invokeMethod(static_cast<MpvObject *>(ctx), "onUpdate",
                              Qt::QueuedConnection);
}

// --- MpvObject ---

MpvObject::MpvObject(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
    , m_engine(new MpvEngine(this))
{
    setMirrorVertically(true);
    setTextureFollowsItemSize(true);

    connect(this, &QQuickItem::windowChanged, this, [this](QQuickWindow *win) {
        if (win) {
            connect(win, &QQuickWindow::beforeSynchronizing, this, [this]() {
                if (!m_renderCtxInitialized) {
                    initRenderContext();
                    m_renderCtxInitialized = true;
                }
            }, Qt::DirectConnection);
        }
    });
}

MpvObject::~MpvObject()
{
    if (m_renderCtx) {
        mpv_render_context_set_update_callback(m_renderCtx, nullptr, nullptr);
        mpv_render_context_free(m_renderCtx);
        m_renderCtx = nullptr;
    }
}

void MpvObject::initRenderContext()
{
    if (m_renderCtx) return;
    if (!m_engine || !m_engine->handle()) return;

    mpv_opengl_init_params gl_init_params{
        get_proc_address,
        nullptr
    };

    mpv_render_param params[]{
        {MPV_RENDER_PARAM_API_TYPE,
         const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };

    int err = mpv_render_context_create(&m_renderCtx, m_engine->handle(), params);
    if (err < 0) {
        qWarning() << "Failed to create mpv render context:" << mpv_error_string(err);
        return;
    }

    mpv_render_context_set_update_callback(m_renderCtx, on_mpv_render_update, this);
    qDebug() << "mpv render context created successfully";
}

QQuickFramebufferObject::Renderer *MpvObject::createRenderer() const
{
    window()->setPersistentGraphics(true);
    window()->setPersistentSceneGraph(true);
    return new MpvRenderer(this);
}

void MpvObject::onUpdate()
{
    update();
}

// --- MpvRenderer ---

MpvRenderer::MpvRenderer(const MpvObject *obj)
    : m_obj(obj)
{
}

MpvRenderer::~MpvRenderer() = default;

QOpenGLFramebufferObject *MpvRenderer::createFramebufferObject(const QSize &size)
{
    return new QOpenGLFramebufferObject(size, QOpenGLFramebufferObject::CombinedDepthStencil);
}

void MpvRenderer::render()
{
    if (!m_obj || !m_obj->renderContext()) return;

    auto *fbo = framebufferObject();
    if (!fbo) return;

    mpv_opengl_fbo mpvFbo{
        static_cast<int>(fbo->handle()),
        fbo->width(),
        fbo->height(),
        0
    };

    int flipY = 0;

    mpv_render_param params[]{
        {MPV_RENDER_PARAM_OPENGL_FBO, &mpvFbo},
        {MPV_RENDER_PARAM_FLIP_Y, &flipY},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };

    mpv_render_context_render(m_obj->renderContext(), params);
}
