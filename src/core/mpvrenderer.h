#ifndef MPVRENDERER_H
#define MPVRENDERER_H

#include <QQuickFramebufferObject>
#include <QOpenGLFramebufferObject>

// Qt 6.7 requires fully-defined types for Q_PROPERTY metatype registration
#include "core/mpvengine.h"

struct mpv_handle;
struct mpv_render_context;

class MpvObject : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(MpvEngine* engine READ engine CONSTANT)

public:
    explicit MpvObject(QQuickItem *parent = nullptr);
    ~MpvObject() override;

    Renderer *createRenderer() const override;

    MpvEngine *engine() const { return m_engine; }
    mpv_render_context *renderContext() const { return m_renderCtx; }

public slots:
    void onUpdate();

private:
    void initRenderContext();

    MpvEngine *m_engine = nullptr;
    mpv_render_context *m_renderCtx = nullptr;
    bool m_renderCtxInitialized = false;
};

class MpvRenderer : public QQuickFramebufferObject::Renderer
{
public:
    explicit MpvRenderer(const MpvObject *obj);
    ~MpvRenderer() override;

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;
    void render() override;

private:
    const MpvObject *m_obj = nullptr;
};

#endif // MPVRENDERER_H
