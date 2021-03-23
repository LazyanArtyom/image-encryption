#include "tcpserver.h"
#include "logic.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <map>

#define HOST_ADDRESS "127.0.0.1" // "185.127.66.104"
#define HOST_PORT 40004
#define MAX_SOCKETS (4)

std::map<QString, int> sRequestTypeMap = {
    std::make_pair(QString("GenerateKey"), 0),
    std::make_pair(QString("Encode"), 1),
    std::make_pair(QString("Decode"), 2),
    std::make_pair(QString("Img"), 3)
};

CGameServer::CGameServer(QObject* parent):
    QTcpServer(parent)
{
    //
    /// Setup timer and connect to host
    //
    m_pFlushTimer = new QTimer(this);
    if (!listen(QHostAddress::Any, HOST_PORT))
    {
        std::cout << "Failed to listen" << std::endl;
        return;
    }
    //
    std::cout << "Listening on " << serverAddress().toString().toStdString() << ":" << HOST_PORT << std::endl;

    connect(this, &QTcpServer::newConnection, this, &CGameServer::onNewConnection);
    connect(m_pFlushTimer, &QTimer::timeout, this, &CGameServer::onFlushTimerTick);

    m_pFlushTimer->setInterval(1000);
    m_pFlushTimer->start();
    //
    /// init settings
    //
}

void CGameServer::sendMessage(QString sMessage, int user)
{
    QByteArray data = sMessage.toLatin1();
    int bytesRateCount = data.size() / 1000;
    int bytesRateRem = data.size() % 1000;

    int i = 0;
    for (; 0 != bytesRateCount; --bytesRateCount)
    {
        m_users[user]->write(data.mid(i, 1000));
        m_users[user]->waitForBytesWritten(5000);
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
    m_users[user]->write(sData);
    m_users[user]->waitForBytesWritten(5000);
    std::this_thread::sleep_for(std::chrono::microseconds(200));
}

QString CGameServer::ToString(const std::vector<std::vector<int> >& mtxImg) const
{
    int nRowSize = mtxImg.size();
    int nColSize = mtxImg[0].size();

    QString sMatrix;
    for (int nRow = 0; nRow < nRowSize; ++nRow)
    {
        for (int nCol = 0; nCol < nColSize - 2; nCol += 3)
        {
            sMatrix += QString::number(mtxImg[nRow][nCol]) + "." + QString::number(mtxImg[nRow][nCol + 1]) + "." + QString::number(mtxImg[nRow][nCol + 2]) + ".";
        }

        if (nRow < nRowSize - 1)
            sMatrix.replace(sMatrix.size() - 1, 1, "#");
    }
    if (sMatrix.endsWith('.'))
        sMatrix = sMatrix.remove(sMatrix.size() - 1, 1);

    return sMatrix;
}

std::vector<std::vector<int> > CGameServer::FromString(const QString& sMatrix)
{
    QStringList lstRows = sMatrix.split('#');
    qDebug() << "serwer height: " << lstRows.size();
    qDebug() << "server width: " << lstRows.at(0).split('.').size();

    std::vector<std::vector<int> > vecImg;
    vecImg.resize(lstRows.count());

    for (int nRow = 0; nRow < lstRows.count(); ++nRow)
    {
        QStringList lstCols = lstRows.at(nRow).split('.');
        vecImg[nRow].resize(lstCols.count());

        for (int nCol = 0; nCol < lstCols.count() - 2; nCol += 3)
        {
            vecImg[nRow][nCol] = lstCols[nCol].toInt();
            vecImg[nRow][nCol + 1] = lstCols[nCol + 1].toInt();
            vecImg[nRow][nCol + 2] = lstCols[nCol + 2].toInt();
        }
    }

    return vecImg;
}

void CGameServer::onNewConnection()
{
    if (m_oServerClients.size() >= m_nMaxClients)
    {
        std::cout << "Cannot form new connection. Server busy";

        QTcpSocket* pCurSocket = nextPendingConnection();
        pCurSocket->close();
        return;
    }

    QTcpSocket* pCurSocket = nextPendingConnection();
    connect(pCurSocket, &QTcpSocket::readyRead, this, &CGameServer::onReadyRead);
    connect(pCurSocket, &QTcpSocket::disconnected, this, &CGameServer::onDisconnected);

    qDebug() << "Client " << pCurSocket->socketDescriptor();
    m_oServerClients.insert(m_nUserCount, !bool(m_oServerClients.size()));
    m_users.insert(m_nUserCount, pCurSocket);
    m_oClientMessageBuffer[pCurSocket] = QByteArray();

    if (0 == m_nUserCount)
    {
        /// Send data to main clinet
        //
        sendMessage(QString("First"), m_nUserCount);
    }
    else
    {
        sendMessage(QString("Img:" + m_sImage), m_nUserCount);
    }
    ++m_nUserCount;

    return;
}

void CGameServer::onReadyRead()
{
    QTcpSocket* pSocket = (QTcpSocket *)sender();
    //
    /// IF client is inactive skip data processing
    /// Do not chagne this line
    //
    QByteArray sEndMessage = "END";
    int oCurrentClient = getCurrentUserID(pSocket);
    std::this_thread::sleep_for(std::chrono::microseconds(1000));
    QByteArray data = pSocket->readAll();
    if (!data.endsWith(sEndMessage))
    {
        m_oClientMessageBuffer[pSocket] += data;
        return;
    }
    else
    {
       qDebug() << "Data "  << data;
       m_oClientMessageBuffer[pSocket] += data.remove(data.size() - 3, 3);
       qDebug() << m_oClientMessageBuffer[pSocket];
    }

    QStringList sValueList = QString::fromStdString(m_oClientMessageBuffer[pSocket].toStdString()).split(",");
    m_oClientMessageBuffer[pSocket].clear();
    QString sRequestType = sValueList[0];
    sValueList.removeAll(sRequestType);
    qDebug() << sValueList;

    switch (sRequestTypeMap[sRequestType])
    {
    case 0:
        {
            // generate key
            qDebug() << "GenerateKey";
            if (m_pLogic != nullptr)
            {
                (void)m_pLogic->generateKey();
                //QString sMtxKey = ToString(m_pLogic->generateKey());
                sendMessage(QString("Key:"), oCurrentClient);
            }
        }
        break;
    case 1:
        {
            // encode
            qDebug() << "EncodeData";
            if (m_pLogic != nullptr)
            {
                QString sMtxImage = ToString(m_pLogic->encode());

                foreach (int user, m_users.keys())
                    sendMessage(QString("EncodedData:" + sMtxImage), user);
            }
        }
        break;
    case 2:
        {
            // decode
            qDebug() << "DecodeData";

            if (m_pLogic != nullptr)
            {
                std::vector<std::vector<int> > mtxImage;
                m_pLogic->getDecodedAndFinished(mtxImage);

                qDebug() << "#*#*#*#*#*#*#*#*######";
                for (auto row : mtxImage)
                {
                    for(auto elem : row)
                        std::cout << elem << " ";
                    std::cout << std::endl;
                }
                qDebug() << "#*#*#*#*#*#*#*#*######";

                foreach (int user, m_users.keys())
                {
                    QString sMtxImage = ToString(mtxImage);
                    sendMessage(QString("DecodedData:" + sMtxImage), user);
                }

                QString sMtxImage = ToString(mtxImage);
                QStringList lstRows = sMtxImage.split('#');

                qDebug() << "##############";
                for (auto row : lstRows)
                    qDebug() << row;
                qDebug() << "##############";
            }
        }
        break;
    case 3:
        {
            std::vector<std::vector<int> > mtxImg = FromString(sValueList[0]);
            m_pLogic = new CLogicEncoderRGB(mtxImg[0].size(), mtxImg.size());
            qDebug() << "Image sizeeeeeee " << mtxImg.size() << " : " << mtxImg[0].size();
            m_pLogic->setImage(mtxImg);
            m_sImage = sValueList[0];
            qDebug() << "Image saved";
        }
        break;
    default:
        break;
    }
}

void CGameServer::onFlushTimerTick()
{
    std::flush(std::cout);
}

void CGameServer::onDisconnected()
{
    QTcpSocket* pSocket = (QTcpSocket *) sender();
    int removeUser = getCurrentUserID(pSocket);
    if (removeUser != -1)
    {
        m_oServerClients.remove(removeUser);
        m_users.remove(removeUser);
        qDebug() << "Client" << removeUser << "disconnected from game";
    }

    if (0 == m_users.size())
        cleanup();
}


int CGameServer::getNextClientIndex(QTcpSocket* pSocket)
{
    int oCurrentUser = getCurrentUserID(pSocket);
    QList<int> keys = m_users.keys();
    for(size_t i = 0; i < keys.size(); ++i)
    {
        if (keys[i] == oCurrentUser)
        {
            if (i == keys.size() - 1)
                return 0;
            else
                return i + 1;
        }
    }
}

int CGameServer::getCurrentUserID(QTcpSocket* pSocket)
{
    foreach (int user, m_users.keys())
    {
        if (pSocket == m_users[user]) return user;
    }
    return -1;
}

void CGameServer::cleanup()
{
    m_nUserCount = 0;
    m_nCount = 0;
    m_oScoreMap.clear();
    m_oServerClients.clear();
    m_oClientNamesMap.clear();
    m_bGameOver = false;

    if (0 != m_users.size())
    {
        foreach(int user, m_users.keys())
        {
            m_users[user]->close();
            delete m_users[user];
            m_users[user] = nullptr;
        }
        m_users.clear();
    }
}

