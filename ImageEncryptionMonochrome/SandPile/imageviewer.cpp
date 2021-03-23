#include "imageviewer.h"

CImageView::CImageView(QWidget *parent)
    : QGraphicsView(parent)
{
    setCacheMode(CacheBackground);
    setTransformationAnchor(AnchorUnderMouse);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::HighQualityAntialiasing);
}

void CImageView::ZoomIn()
{
    ScaleView(qreal(1.2));
}

void CImageView::ZoomOut()
{
    ScaleView(1 / qreal(1.2));
}

void CImageView::ViewFit()
{
    fitInView(sceneRect(), Qt::KeepAspectRatio);
    m_isResized = true;

    if (sceneRect().width() > sceneRect().height())
        m_isLandscape = true;
    else
        m_isLandscape = false;
}

void CImageView::wheelEvent(QWheelEvent *pEvent)
{
    if (pEvent->modifiers() == Qt::ControlModifier)
    {
        if (pEvent->angleDelta().y() > 0)
            ZoomIn();
        else
            ZoomOut();
    }
    else if (pEvent->modifiers() == Qt::ShiftModifier)
    {
        ViewFit();
    }
    else if (pEvent->modifiers() == Qt::NoModifier)
    {
        QGraphicsView::wheelEvent(pEvent);
    }
}

void CImageView::ScaleView(qreal dScaleFactor)
{
    if(sceneRect().isEmpty())
        return;

    int iImgLength;
    int iViewportLength;
    qreal dExpRectLength;
    QRectF qrectExpectedRect = transform().scale(dScaleFactor, dScaleFactor).mapRect(sceneRect());

    if (m_isLandscape)
    {
        dExpRectLength = qrectExpectedRect.width();
        iViewportLength = viewport()->rect().width();
        iImgLength = sceneRect().width();
    }
    else
    {
        dExpRectLength = qrectExpectedRect.height();
        iViewportLength = viewport()->rect().height();
        iImgLength = sceneRect().height();
    }

    if (dExpRectLength < iViewportLength / 2) // minimum zoom : half of viewport
    {
        if (!m_isResized || dScaleFactor < 1)
            return;
    }
    else if (dExpRectLength > iImgLength * 10) // maximum zoom : x10
    {
        if (!m_isResized || dScaleFactor > 1)
            return;
    }
    else
    {
        m_isResized = false;
    }

    scale(dScaleFactor, dScaleFactor);
}

void CImageView::resizeEvent(QResizeEvent *pEvent)
{
    m_isResized = true;
    QGraphicsView::resizeEvent(pEvent);
}
