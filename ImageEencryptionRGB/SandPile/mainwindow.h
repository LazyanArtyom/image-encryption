#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QFileDialog>
#include <QMainWindow>
#include <QPushButton>
#include <QGraphicsScene>

#include "tcpclient.h"
#include "imageviewer.h"

class CMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    CMainWindow(QWidget *pParent = nullptr);
    ~CMainWindow() = default;

public:
    void SetImage(const QImage& oImg);

    //std::vector<std::vector<int>> FromImage(const QImage& oImg) const;
    //QImage ToImage(const std::vector<std::vector<int>>& vecImg) const;

    QString FromImage(const QImage& oImg);
    QImage  ToImage(const QString& sImg);
    void ShowKey(const QImage& oImg);

public slots:
    // slots from client
    void onFirst();
    void onKeyGenerated(QString sKey);
    void onEncodedData(QString sEncodedData);
    void onDecodedData(QString sDecodedData);
    void onImageUploaded(QString sImage);
    void onMsg(const QString& sMsg);


    void onSaveImage();
    void onUploadImage();
    void onFitToWindow();

private:
    void SetupUi();
    void connectToserver();
    void PrintVector(const std::vector<std::vector<int>>& vec) const;

private:
    CClient         m_oClient;
    // QComboBox*      m_pwGridType        = nullptr;
    QLabel*         m_pwMessageBox      = nullptr;
    QPushButton*    m_pbtnUploadImg     = nullptr;
    // QSpinBox*       m_pwNumParticipants = nullptr;
    QGraphicsScene* m_pGraphicsScene    = nullptr;
    CImageView*     m_pImageView        = nullptr;
    QImage::Format  m_eImageFormat      = QImage::Format_RGB32;

    bool     m_isFirst    = false;
    QAction* m_pactEncode = nullptr;
    QAction* m_pactDecode = nullptr;
    QAction* m_pactGenerateKey = nullptr;

    QLabel* m_plblKey = nullptr;
};
#endif // MAINWINDOW_H
