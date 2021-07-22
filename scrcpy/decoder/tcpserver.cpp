#include "tcpserver.h"
#include "videosocket.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent) {}

TcpServer::~TcpServer() {}

void TcpServer::incomingConnection(qintptr handle)
{
    if (m_isVideoSocket) {
        VideoSocket *socket = new VideoSocket();
        socket->setSocketDescriptor(handle);
        addPendingConnection(socket);

        // next is control socket
        m_isVideoSocket = false;
    } else {
        QTcpSocket *socket = new QTcpSocket();
        socket->setSocketDescriptor(handle);
        addPendingConnection(socket);
    }
}

bool TcpServer::getIsVideoSocket()
{
    return m_isVideoSocket;
}
void TcpServer::setIsVideoSocket(bool isVideoSocket)
{
    m_isVideoSocket = isVideoSocket;   
}
