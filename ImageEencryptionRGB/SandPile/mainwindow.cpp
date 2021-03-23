#include "mainwindow.h"

#include <QIcon>
#include <QLabel>
#include <QToolBar>
#include <QBoxLayout>
#include <QMessageBox>

#include <QImageReader>
#include <QImageWriter>
#include <QStandardPaths>

#include <QDebug>
#include <chrono>
#include <thread>
#include <iostream>

CMainWindow::CMainWindow(QWidget *pParent)
    : QMainWindow(pParent)
{
    SetupUi();
}

void CMainWindow::SetImage(const QImage &oImg)
{
    qDebug() << "Show Image";
    if (!m_pGraphicsScene->sceneRect().isEmpty())
        m_pGraphicsScene->clear();

    m_pGraphicsScene->setSceneRect(oImg.rect());
    m_pGraphicsScene->addPixmap(QPixmap::fromImage(oImg));

    m_pImageView->ViewFit();
}

/*
std::vector<std::vector<int> > CMainWindow::FromImage(const QImage &oImg) const
{
    std::vector<std::vector<int> > vecImg;
    vecImg.resize(oImg.height());

    for (int nRow = 0; nRow < oImg.height(); ++nRow)
    {
        vecImg[nRow].resize(oImg.width());
        for (int nCol = 0; nCol < oImg.width(); ++nCol)
        {
            int nRed = qRed(oImg.pixel(nCol, nRow));
            int nGreen = qGreen(oImg.pixel(nCol, nRow));
            int nBlue = qBlue(oImg.pixel(nCol, nRow));

            if ((nRed == 255) && (nGreen == 255) && (nBlue == 255))
               vecImg[nRow][nCol] = 1;
            else
               vecImg[nRow][nCol] = 0;
        }
    }

    return vecImg;
}

QImage CMainWindow::ToImage(const std::vector<std::vector<int> > &vecImg) const
{
    int nRowSize = vecImg.size();
    int nColSize = vecImg[0].size();

    QImage oImg(nColSize, nRowSize, m_eImageFormat);
    for (int nRow = 0; nRow < nRowSize; ++nRow)
    {
        for (int nCol = 0; nCol < nColSize; ++nCol)
        {
            if (vecImg[nRow][nCol] == 1)
                oImg.setPixelColor(nCol, nRow, QColor(255, 255, 255));
            else
                oImg.setPixelColor(nCol, nRow, QColor(0, 0, 0));
        }
    }

    return oImg;
} */

QImage CMainWindow::ToImage(const QString &sMatrix)
{
    QStringList lstRows = sMatrix.split('#');

    int nColSize = lstRows.at(0).split('.').count();
    int nRowSize = lstRows.count();

    QImage oImg(nColSize / 3, nRowSize, m_eImageFormat);
    for (int nRow = 0; nRow < lstRows.count(); ++nRow)
    {
        QStringList lstCols = lstRows.at(nRow).split('.');
        for (int nCol = 0; nCol < lstCols.count() - 2; nCol += 3)
        {
            oImg.setPixelColor(nCol / 3, nRow, QColor(lstCols[nCol].toInt(), lstCols[nCol + 1].toInt(), lstCols[nCol + 2].toInt()));
        }
    }

    qDebug() << "TOIMAGE img size: " << oImg.width() << ": " << oImg.height();
    return oImg;
}

void CMainWindow::ShowKey(const QImage &oImg)
{
    if (m_plblKey != nullptr)
        m_plblKey = new QLabel(this);

    m_plblKey->setPixmap(QPixmap::fromImage(oImg));
    m_plblKey->show();
}

void CMainWindow::onFirst()
{
    m_isFirst = true;
    // m_pactGenerateKey->setDisabled(false);
}

void CMainWindow::onKeyGenerated(QString sKey)
{
   // ShowKey(ToImage(sKey));
    if (m_isFirst)
    {
        // m_pactGenerateKey->setDisabled(true);
        m_pactEncode->setDisabled(false);
    }
    m_pwMessageBox->setText(tr("Key generated. Now you can encode the img."));
    m_pactGenerateKey->setDisabled(true);
}

void CMainWindow::onEncodedData(QString sEncodedData)
{
    SetImage(ToImage(sEncodedData));
    if (m_isFirst)
    {
        m_pactEncode->setDisabled(true);
        m_pactDecode->setDisabled(false);
    }
    m_pwMessageBox->setText(tr("Image encoded. Now you can decode."));
}

void CMainWindow::onDecodedData(QString sDecodedData)
{
//    QString cpy = sDecodedData;
//    auto sl = cpy.split("#");
//    for(auto str : sl)
//    {
//        qDebug()<<str;
//    }
    SetImage(ToImage(sDecodedData));
    // if (m_isFirst)
    // {
    m_pactDecode->setDisabled(true);
    m_pwMessageBox->setText(tr("Image decoded."));
    //}
}

void CMainWindow::onImageUploaded(QString sImage)
{
    SetImage(ToImage(sImage));
    m_pwMessageBox->setText(tr("Image loaded."));
}

void CMainWindow::onMsg(const QString &sMsg)
{

}

QString CMainWindow::FromImage(const QImage &oImg)
{
    qDebug() << "img size: " << oImg.width() << ": " << oImg.height();
    QString strImg;
    for (int nRow = 0; nRow < oImg.height(); ++nRow)
    {
        for (int nCol = 0; nCol < oImg.width(); ++nCol)
        {
            int nRed   = qRed(oImg.pixel(nCol, nRow));
            int nGreen = qGreen(oImg.pixel(nCol, nRow));
            int nBlue  = qBlue(oImg.pixel(nCol, nRow));
            strImg += QString::number(nRed) + "." + QString::number(nGreen) + "." + QString::number(nBlue) + ".";
        }

        if (nRow < oImg.height() - 1)
            strImg.replace(strImg.size() - 1, 1, "#");
    }
    if (strImg.endsWith('.'))
        strImg = strImg.remove(strImg.size() - 1, 1);

    QStringList lstRows = strImg.split('#');
    qDebug() << "client height: " << lstRows.size();
    qDebug() << "client width: " << lstRows.at(0).split('.').size();


    //strImg.remove(*strImg.end());
    return strImg;
}

void CMainWindow::SetupUi()
{
    setToolTip("SandPile");
    setWindowTitle("SandPile");
    setWindowIcon(QIcon("://Icons/sandPileLogo.png"));

    m_pImageView = new CImageView();
    setCentralWidget(m_pImageView);

    m_pGraphicsScene = new QGraphicsScene();
    m_pGraphicsScene->setItemIndexMethod(QGraphicsScene::NoIndex);

    // set background
//    QImage bground(50, 50, QImage::Format_RGB888);
//    for (int y = 0; y < 25; y++)
//    {
//        for (int x = 0; x < 25; x++)
//        {
//            bground.setPixel(x, y, qRgb(0xCA, 0xCA, 0xCA));
//            bground.setPixel(x + 25, y, qRgb(0xFF, 0xFF, 0xFF));
//            bground.setPixel(x, y + 25, qRgb(0xFF, 0xFF, 0xFF));
//            bground.setPixel(x + 25, y + 25, qRgb(0xCA, 0xCA, 0xCA));
//        }
//    }
//    m_pGraphicsScene->setBackgroundBrush(QPixmap::fromImage(bground));
    m_pGraphicsScene->setBackgroundBrush(QColor(Qt::gray));
    m_pImageView->setScene(m_pGraphicsScene);

    QToolBar* pToolBar = addToolBar("Main ToolBar");

    m_pactGenerateKey = new QAction(QIcon(QPixmap("://Icons/keys.svg")), tr("Generate Key"));
    // m_pactGenerateKey->setDisabled(true);
    pToolBar->addAction(m_pactGenerateKey);
    connect(m_pactGenerateKey, &QAction::triggered, [=]() {
        m_oClient.sendMessage("GenerateKey");
    });

    m_pactEncode = new QAction(QIcon(QPixmap("://Icons/encode.svg")), tr("Encode"));
    pToolBar->addAction(m_pactEncode);
    m_pactEncode->setDisabled(true);
    connect(m_pactEncode, &QAction::triggered, [=]() {
        m_oClient.sendMessage("Encode");
    });

    m_pactDecode = new QAction(QIcon(QPixmap("://Icons/decode.svg")), tr("Decode"));
    m_pactDecode->setDisabled(true);
    pToolBar->addAction(m_pactDecode);
    connect(m_pactDecode, &QAction::triggered, [=]() {
        m_oClient.sendMessage("Decode");
    });

    // Upload Image
    QAction* pactUploadImg = new QAction(QIcon(QPixmap("://Icons/upload.svg")), tr("Upload Image"));
    pToolBar->addAction(pactUploadImg);
    connect(pactUploadImg, &QAction::triggered, this, &CMainWindow::onUploadImage);

    // Save Image
    QAction* pactSaveImg = new QAction(QIcon(QPixmap("://Icons/saveImage.svg")), tr("Save Image"));
    pToolBar->addAction(pactSaveImg);
    connect(pactSaveImg, &QAction::triggered, this, &CMainWindow::onSaveImage);

    pToolBar->addSeparator();

    QWidget* pwSettings = new QWidget();
    QHBoxLayout* pHLayout = new QHBoxLayout();
    pwSettings->setLayout(pHLayout);

    // Grid Type
    // QLabel* plblGridType = new QLabel("Grid Type: ");
    // m_pwGridType = new QComboBox();
    // m_pwGridType->addItems(QStringList { tr("Thorus"), tr("Closed border"), tr("Open border") });
    // pHLayout->addWidget(plblGridType);
    // pHLayout->addWidget(m_pwGridType);

    // pToolBar->addSeparator();

    // Participants number
    // QLabel* plblNumParticipants = new QLabel("Participants: ");
    // m_pwNumParticipants = new QSpinBox();
    // m_pwNumParticipants->setRange(1, 10);
    // pHLayout->addWidget(plblNumParticipants);
    // pHLayout->addWidget(m_pwNumParticipants);

    // Message Box
    m_pwMessageBox = new QLabel();
    m_pwMessageBox->setAlignment(Qt::AlignCenter);
    m_pwMessageBox->setStyleSheet("color:red; background-color:yellow; border: 2px solid black; padding: 3px; font-weight:bold");
    m_pwMessageBox->setText(tr("Ready."));
    pHLayout->addWidget(m_pwMessageBox);


    pToolBar->addWidget(pwSettings);
    m_pGraphicsScene->addPixmap(QPixmap("://Icons/sandPile.png"));

    connect(&m_oClient, &CClient::sigFirst, this, &CMainWindow::onFirst);
    connect(&m_oClient, &CClient::sigKeyGenerated, this, &CMainWindow::onKeyGenerated);
    connect(&m_oClient, &CClient::sigDecodedData, this, &CMainWindow::onDecodedData);
    connect(&m_oClient, &CClient::sigEncodedData, this, &CMainWindow::onEncodedData);
    connect(&m_oClient, &CClient::sigImageUploaded, this, &CMainWindow::onImageUploaded);
    connect(&m_oClient, &CClient::sigMsg, this, &CMainWindow::onMsg);
    connectToserver();
}

void CMainWindow::PrintVector(const std::vector<std::vector<int> > &vec) const
{
    int nColSize = vec[0].size();
    int nRowSize = vec.size();

    for (int nRow = 0; nRow < nRowSize; ++nRow)
    {
        for (int nCol = 0; nCol < nColSize; ++nCol)
            std::cout << vec[nRow][nCol];
    }
}

void CMainWindow::onSaveImage()
{
    if (m_pGraphicsScene->sceneRect().isEmpty())
          return;

    m_pGraphicsScene->clearSelection();
    QImage oImg(m_pGraphicsScene->sceneRect().size().toSize(), m_eImageFormat);
    QPainter oPainter(&oImg);
    m_pGraphicsScene->render(&oPainter);

    QString qStrFilePath = QFileDialog::getSaveFileName(this,
          tr("Save Image"),
          QStandardPaths::writableLocation(QStandardPaths::CacheLocation).replace("cache", "newfile.jpg"),
          tr("JPG file (*.jpg);;PNG file (*.png);;BMP file (*.bmp)"));

    if (qStrFilePath.isEmpty())
      return;

    QImageWriter oWriter(qStrFilePath);
    if(!oWriter.canWrite())
    {
      QMessageBox msgBox;
      msgBox.setText("Cannot write file");
      msgBox.exec();
      return;
    }

    oWriter.write(oImg);
    m_pwMessageBox->setText(tr("Image saved."));
}

void CMainWindow::onUploadImage()
{
    QString qStrFilePath = QFileDialog::getOpenFileName(this,
        tr("Open Image"),
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation),
        tr("Image Files (*.png *.jpg *.bmp)"));

    if (qStrFilePath.isEmpty())
        return;

    QImageReader oReader(qStrFilePath);
    if (!oReader.canRead())
    {
        QMessageBox msgBox;
        msgBox.setText("Cannot read file");
        msgBox.exec();
        return;
    }

    if (!m_pGraphicsScene->sceneRect().isEmpty())
        m_pGraphicsScene->clear();

    QImage oImg = oReader.read();
    QImage oMonoImg = oImg.convertToFormat(m_eImageFormat);
    m_pGraphicsScene->setSceneRect(oImg.rect());

    m_pGraphicsScene->addPixmap(QPixmap::fromImage( ToImage(FromImage(oMonoImg)) ));
    //m_pGraphicsScene->addPixmap(QPixmap::fromImage(oMonoImg));
    m_pImageView->ViewFit();

    m_pwMessageBox->setText(tr("Image loaded."));
    m_oClient.sendMessage("Img," + FromImage(oMonoImg));
}

void CMainWindow::onFitToWindow()
{
    m_pImageView->ViewFit();
}

void CMainWindow::connectToserver()
{
    m_oClient.connectToServer();
    std::this_thread::sleep_for(std::chrono::microseconds(1000));
}
