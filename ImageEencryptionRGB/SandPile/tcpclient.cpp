#include "tcpclient.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <QMessageBox>

#define HOST_ADDRESS "127.0.0.1" // "185.127.66.104"
#define HOST_PORT 40004

std::map<QString, int> sRequestTypeMap = {
    std::make_pair(QString("First"), 0),
    std::make_pair(QString("Key"), 1),
    std::make_pair(QString("EncodedData"), 2),
    std::make_pair(QString("DecodedData"), 3),
    std::make_pair(QString("Img"), 4)
};

CClient::CClient()
{
    //
    /// Init socket and timer
    //
    m_pSocket = new QTcpSocket(this);
    m_pFlushTimer = new QTimer(this);

    //
    /// Make connections
    //
    connect(m_pSocket, &QTcpSocket::connected, this, &CClient::onSocketConnected);
    connect(m_pSocket, &QTcpSocket::readyRead, this, &CClient::onReadyRead);
    connect(m_pSocket, &QTcpSocket::disconnected, this, &CClient::onSocketDisconnected);
    connect(m_pFlushTimer, &QTimer::timeout, this, &CClient::onFlushTimerTick);

    //
    /// Flush data to std::out
    //
    m_pFlushTimer->setInterval(1000);
    m_pFlushTimer->start();
}

bool CClient::isConnected()
{
    return m_pSocket->state() == QAbstractSocket::ConnectedState;
}

void CClient::connectToServer()
{
    if (m_pSocket->state() == QAbstractSocket::UnconnectedState)
    {
        m_pSocket->connectToHost(HOST_ADDRESS, HOST_PORT);
        bool bConnected = m_pSocket->waitForConnected(2000);
        if (!bConnected)
        {
           (void)QMessageBox::critical(nullptr, tr("SandPile Encryptor"), tr(m_pSocket->errorString().toStdString().c_str()), QMessageBox::Close);
           exit(0);
        }
    }
}

void CClient::sendMessage(QString msg)
{
    QString msgToSend = msg;
    if (m_pSocket->state() != QAbstractSocket::ConnectedState)
    {
        return;
    }

    QByteArray data = msgToSend.toLatin1();
    int bytesRateCount = data.size() / 1000;
    int bytesRateRem = data.size() % 1000;

    int i = 0;
    for (; 0 != bytesRateCount; --bytesRateCount)
    {
        m_pSocket->write(data.mid(i, 1000));
        m_pSocket->waitForBytesWritten(5000);
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
        i += 1000;
    }

    qDebug() << "INFO  i" << i;
    qDebug() << "INFO  bytesRateRem" << bytesRateRem;
    qDebug() << "INFO  data len" << data.size();

    QByteArray sData = "";
    if (0 != bytesRateRem)
    {
        sData = data.mid(i, bytesRateRem);
    }

    sData = sData.append(QByteArray("END"));
    m_pSocket->write(sData);
    m_pSocket->waitForBytesWritten(5000);
    std::this_thread::sleep_for(std::chrono::microseconds(200));
}

void CClient::onEncode()
{

}

void CClient::onDecode()
{

}

void CClient::onGenerateKey()
{

}

void CClient::disconnectFromServer()
{
    qDebug() << "state: " << m_pSocket->state();
    if (m_pSocket->state() != QAbstractSocket::ConnectedState)
    {
        return;
    }

    qDebug() << "Client disconnecting";
    m_pSocket->disconnectFromHost();
}

void CClient::onSocketConnected()
{
    onReadyRead();
}

void CClient::onSocketDisconnected()
{
}

void CClient::onReadyRead()
{
    qDebug() << "INFO  Reading...";
    std::this_thread::sleep_for(std::chrono::microseconds(1000));
    QByteArray data = m_pSocket->readAll();
    m_pSocket->waitForBytesWritten(3000);
    if (data.isEmpty())
    {
        qDebug() << "INFO  Dat is empty";
        return;
    }
    QByteArray sEndMessage = "END";
    if (!data.endsWith(sEndMessage))
    {
        m_oMessageBuffer += data;
        qDebug() << "INFO  Write data to buffer";
        return;
    }
    else
    {
       m_oMessageBuffer += data.remove(data.size() - 3, 3);
    }

    QStringList sValueList = QString::fromStdString(m_oMessageBuffer.toStdString()).split(":");
    m_oMessageBuffer.clear();
    QString sRequestType = sValueList[0];
    sValueList.removeAll(sRequestType);

    switch (sRequestTypeMap[sRequestType])
    {
    case 0:
        {
            qDebug() << "INFO  First";
            emit sigFirst();
        }
        break;
    case 1:
        {
            qDebug() << "INFO  Key";
            emit sigKeyGenerated(sValueList[0]);
       }
        break;
    case 2:
        {
            qDebug() << "INFO  EncodedData";
            emit sigEncodedData(sValueList[0]);
        }
        break;
    case 3:
        {
            QStringList lstRows = sValueList[0].split('#');

            qDebug() << "************************";
            for (auto row : lstRows)
                qDebug() << row;
            qDebug() << "************************";

            qDebug() << "INFO  DecodedData";
            emit sigDecodedData(sValueList[0]);
        }
        break;
    case 4:
        {
            qDebug() << "INFO  Upload Image";
            emit sigImageUploaded(sValueList[0]);
        }
        break;
    default:
        break;
    }
}

void CClient::onFlushTimerTick()
{
    std::flush(std::cout);
}


