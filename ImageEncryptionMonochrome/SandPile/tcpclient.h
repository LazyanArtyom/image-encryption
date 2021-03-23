#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

///////////////////////////////////////////////////////////////////
/// \brief The SValue struct
//
struct SValue
{
    SValue(int nRow, int nCol, int nValue, short nPlvac)
        : nRow(nRow), nCol(nCol), nValue(nValue), nPlvac(nPlvac)
    {}

    int nRow;
    int nCol;
    int nValue;
    short nPlvac;
};

///////////////////////////////////////////////////////////////////
/// \brief The CClient class
//
class CClient : public QObject
{
    Q_OBJECT
public:
    CClient();

    bool isConnected();
    void connectToServer();
    void disconnectFromServer();
    void sendMessage(QString msg);

signals:
    void sigFirst();
    void sigKeyGenerated(QString sKey);
    void sigEncodedData(QString sEncodedData);
    void sigDecodedData(QString sDecodedData);
    void sigImageUploaded(QString sImage);
    void sigMsg(const QString& sMsg);

private slots:
    // slots from gui
    void onEncode();
    void onDecode();
    void onGenerateKey();

    void onSocketConnected();
    void onSocketDisconnected();
    void onReadyRead();
    void onFlushTimerTick();

private:
    QTcpSocket* m_pSocket     = nullptr;
    QTimer*     m_pFlushTimer = nullptr;
    QByteArray  m_oMessageBuffer;
};

#endif // !CLIENT_H
