#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QKeyEvent>
#include <QWheelEvent>
#include <QGraphicsView>

class CImageView : public QGraphicsView
{
    Q_OBJECT
public:
    CImageView(QWidget *pParent = nullptr);
    ~CImageView() = default;

    void ViewFit();
    void ZoomIn();
    void ZoomOut();

protected:
    void wheelEvent(QWheelEvent *pEvent) override;
    void resizeEvent(QResizeEvent *pEvent) override;

private:
    void ScaleView(qreal scaleFactor);
    bool m_isResized = false;
    bool m_isLandscape = false;
};

#endif // IMAGEVIEWER_H
