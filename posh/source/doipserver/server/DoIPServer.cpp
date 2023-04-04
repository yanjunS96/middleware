#include "DoIPServer.h"

using namespace motovis::doip;

int TcpConnect::RecvTcpMessage(DoIpParsePtr ptr) noexcept
{
    if (nullptr == ptr)
    {
        MLOG_ERROR("DoIpParsePtr ptr is nullptr!");
        return -1;
    }
    sendVal.clear();
    int bytes{0};
    DoIpHeader header{0};

    /* 1.接收 DoIp 数据 */
    bytes = recvOneMessage((char *)&header, cHeaderSize);

    MLOG_INFO("recv data payload_len:{} {}", bytes, header.mpayloadLength);

    recvVal.resize(cHeaderSize + header.mpayloadLength);
    header.SerialVecBuff(recvVal);
    if (0 != header.mpayloadLength)
    {
        /******************* 获取数据 payload ***************/
        bytes = recvOneMessage((char *) &recvVal[cHeaderSize], header.mpayloadLength);
    }

    /* 2.数据解析 数据: m_udpHeader, 长度: readBytes */
    ptr->parseResponse(recvVal,sendVal, SOCKET_TYPE::TCP);

    /* 3.发送响应 */
    if (!sendVal.empty())
    {
        bytes = SendTcpMessage((const char *)sendVal.data(), sendVal.size());
    }
    return bytes;
}

/**
 * Sends a message back to the connected client
 * @param message           contains generic header and payload specific content
 * @param messageLength     length of the complete message
 * @return                  number of bytes written is returned,
 *                          or -1 if error occurred
 */
int TcpConnect::SendTcpMessage(const char* message, int messageLength) noexcept
{
    if (nullptr == message || 0 == messageLength)
    {
        MLOG_ERROR("message param is nullptr or messageLength is 0");
        return -1;
    }
    int sendbytes{0};
    int remainingPayload = messageLength;
    while(remainingPayload > 0) {
//        int readBytes = send(tcpSocket, &message[sendbytes], remainingPayload, 0);
        int readBytes;
        if(-1 == (readBytes = send(tcpSocket, &message[sendbytes], remainingPayload, MSG_NOSIGNAL)))
        {
            MLOG_ERROR("Tcp send failed, error:{} {}", errno, strerror(errno));
            if(EAGAIN == errno) /*发送缓冲区满了*/
            {
                MLOG_INFO("####send buff full, len: {}", readBytes);
                continue;
            }
            else if(EACCES == errno)
            {
                MLOG_ERROR("##error##The calling process doesn't have the appropriate privileges.");
                break;
            }
            else if(EBADF == errno)
            {
                MLOG_ERROR("##error##An invalid descriptor was specified.");
                closeSock();
                break;
            }
            else if(ECONNRESET == errno)
            {
                MLOG_ERROR("##error##A connection was forcibly closed by a peer.");
                closeSock();
                break;
            }
            else if(EDESTADDRREQ == errno)
            {
                MLOG_ERROR("##error##The socket is not connection-mode and no peer address is set.");
                break;
            }
            else if(EFAULT == errno)
            {
                MLOG_ERROR("##error##An invalid user space address was specified for a parameter.");
                break;
            }
            else if(EINTR == errno)
            {
                MLOG_ERROR("##error##A signal interrupted send() before any data was transmitted.");
                break;
            }
            else if(EIO == errno)
            {
                MLOG_ERROR("##error##An I/O error occurred while reading from or writing to the filesystem.");
                break;
            }
            else if(EMSGSIZE == errno)
            {
                MLOG_ERROR("##error##The socket requires that the message be sent atomically, but the size of the message made this impossible.");
                break;
            }
            else if(ENETDOWN == errno)
            {
                MLOG_ERROR("##error##The local network interface used to reach the destination is down." );
                break;
            }
            else if(ENETUNREACH == errno)
            {
                MLOG_ERROR("##error##No route to the network is present." );
                break;
            }
            else if(ENOBUFS == errno)
            {
                MLOG_ERROR("##error##The system couldn't allocate an internal buffer. The operation may succeed when buffers become available.");
                break;
            }
            else if(ENOTCONN == errno)
            {
                MLOG_ERROR("##error##The socket isn't connected or otherwise has not had the peer pre-specified.");
                break;
            }
            else if(ENOTSOCK == errno)
            {
                MLOG_ERROR("##error##The argument s isn't a socket.");
                break;
            }
            else if(EOPNOTSUPP == errno)
            {
                MLOG_ERROR("##error##The socket argument is associated with a socket that doesn't support one or more of the values set in flags.");
                break;
            }
            else if(EOVERFLOW == errno)
            {
                MLOG_ERROR("##error##An attempt was made to send an amount of data that when added to the size of the socket send message structure exceeds the allowable limit.");
                break;
            }
            else if(EPIPE == errno)
            {
                MLOG_ERROR("##error##The socket is shut down for writing, or the socket is connection-mode and is no longer connected. In the latter case, and if the socket is of type SOCK_STREAM, a SIGPIPE signal is generated to the calling thread.");
//                closeSock();
                break;
            }
            else if(EWOULDBLOCK == errno)
            {
                MLOG_ERROR("##error##The socket is marked nonblocking and the requested operation would block.");
                break;
            }
            else /*发送失败了*/
            {
                MLOG_ERROR("##error##send failed! fd:{}, ERRNO:{} {}",tcpSocket, errno, strerror(errno));
                break;
            }
            break;
        }
        else
        {
            sendbytes += readBytes;
            remainingPayload -= readBytes;
        }
    }
//    sendbytes = write(tcpSocket, message, messageLength);

    MLOG_INFO("send_len:{}", sendbytes);
    return sendbytes;
}

int TcpConnect::recvOneMessage(char *message, int messageLength) noexcept
{
    if (nullptr == message || 0 == messageLength)
    {
        MLOG_ERROR("message param is nullptr or messageLength is 0");
        return -1;
    }
    int payloadPos{0};
    int remainingPayload = messageLength;

    while(remainingPayload > 0) {
        int readBytes = recv(tcpSocket, &message[payloadPos], remainingPayload, 0);
        if(readBytes == 0)
        {
            /* 收到的数据长度为0, 链路关闭了 */
            MLOG_ERROR("Tcp recv failed, error:{} {}", errno, strerror(errno));
            payloadPos = 0;
//            closeSock();
            break;
        }
        else if (0 > readBytes)
        {
            if(ECONNRESET == errno)
            {
                MLOG_ERROR("ECONNRESET failed!error:{} {}", errno, strerror(errno));
            }
            else if(EBADF == errno)
            {
                MLOG_ERROR("Invalid descriptor s.");
                closeSock();
            }
            else if(EFAULT == errno)
            {
                MLOG_ERROR("The receive buffer is outside the process's address space.");
            }
            else if(EINTR == errno) /* 不应该close */
            {
                MLOG_ERROR("The receive was interrupted by delivery of a signal before any data was available.");
            }
            else if(ENOTCONN == errno)
            {
                MLOG_ERROR("The socket is associated with a connection-oriented protocol and hasn't been connected; see connect() and accept().");
            }
            else if(EOVERFLOW == errno)
            {
                MLOG_ERROR("An attempt was made to receive an amount of data that exceeds the allowable limit.!");
            }
            else if(EWOULDBLOCK == errno)
            {
                MLOG_ERROR("Either the socket is marked nonblocking and the receive operation would block, or a receive timeout had been set and the timeout expired before data was received.");
            }
            else
            {
                MLOG_ERROR("Tcp recv failed,readBytes:{} error:{} {}",readBytes, errno, strerror(errno));
            }
        }
        else
        {
            payloadPos += readBytes;
            remainingPayload -= readBytes;
        }
//        sleep(10);
    }

    return payloadPos;
}

void TcpConnect::reloadConnect(int fd, struct sockaddr_in & addr) noexcept
{
    tcpSocket = fd;
    memmove(&m_addr, &addr, sizeof(m_addr));
}

int TcpConnect::closeSock()
{
    if(tcpSocket < 0){return -1;}
    shutdown(tcpSocket, SHUT_RDWR);
    if(close(tcpSocket) < 0)
    {
        MLOG_ERROR("close error: {} {}", errno, strerror(errno));
    }
    tcpSocket = -1;
    return 0;
}


DoIPServer::DoIPServer(const DoIpInfoPtr _info) noexcept
:m_tcp_socket(-1)
,m_udp_socket(-1)
,p_info(nullptr != _info? _info: throw std::out_of_range("Invalid DoIpInfoPtr"))
{
    if ((1024 > p_info->ServerPort) || (65535 < p_info->ServerPort))
    {
        MLOG_ERROR("ServerPort is error!{}", p_info->ServerPort);
        p_info->ServerPort = cServerPort;
    }
    if ((1024 > p_info->ClientPort) || (65535 < p_info->ClientPort))
    {
        MLOG_ERROR("ClientPort is error!{}", p_info->ClientPort);
        p_info->ClientPort = cClientPort;
    }

    UdpInit(); //不可放入线程,否则 访问 p_info 会 dump
    TcpInit();
}

DoIPServer::~DoIPServer() noexcept
{
    MLOG_CRITICAL("~DoIPServer");
    closeTcpSocket();
    closeUdpSocket();
}


void DoIPServer::StartServer(const DoIpParsePtr ptr) noexcept
{
    if (nullptr == ptr)
    {
        throw std::out_of_range("TcpConnect DoIpParsePtr ptr is nullptr!");
    }
//    MLOG_INFO("udp_socket:{}!",m_udp_socket);
    std::thread(std::bind(&DoIPServer::UdpServerThread, this, ptr)).detach();
    std::thread(std::bind(&DoIPServer::TcpServerThread, this, ptr)).detach();
}

/*
 * Set up a tcp socket, so the socket is ready to accept a connection 
 */
bool DoIPServer::TcpInit() noexcept
{
    int ServerPort{p_info->ServerPort};
    if (-1 == m_tcp_socket)
    {
        struct sockaddr_in serveraddr;
        m_tcp_socket = socket(AF_INET, SOCK_STREAM, 0); //TCP 方式
        if (0 > m_tcp_socket)
        {
            MLOG_ERROR("socket creat failed!");
            return false;
        }
        memset( (void*)&serveraddr,0,sizeof(struct sockaddr_in) );

        MLOG_INFO("m_tcp_socket:{}", m_tcp_socket);

        //设置端口复用
        int optval = 1;
        if (-1 == (setsockopt(m_tcp_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(int))))
        {
            MLOG_ERROR("setsockopt failed, errno:{} {}",errno, strerror(errno));
            closeTcpSocket();
            return false;
        }

        serveraddr.sin_family = AF_INET;
        inet_aton("0.0.0.0",&serveraddr.sin_addr);
//        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        serveraddr.sin_port = htons(ServerPort);
        //binds the socket to the address and port number
        if (-1 == bind(m_tcp_socket, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr_in)))
        {
            MLOG_ERROR("bind failed, errno:{} {}",errno, strerror(errno));
            closeTcpSocket();
            return false;
        }

        if(-1 == listen(m_tcp_socket, 5))
        {
            MLOG_ERROR("listen failed, errno:{} {}",errno, strerror(errno));
            closeTcpSocket();
            return false;
        }

        FD_ZERO(&m_socketSet);
        FD_SET(m_tcp_socket, &m_socketSet); /* 加入监听套接字 */

        setNonBlocking(m_tcp_socket);
        return true;
    }
}

void DoIPServer::TcpServerThread(const DoIpParsePtr parse) noexcept
{
    int timems = -1;
    int fd{-1}, _maxfd{m_tcp_socket};
    while (0 < m_tcp_socket)
    {
        fd_set readSet = m_socketSet;
        timems = select(_maxfd + 1, &readSet, NULL, NULL, NULL);
        if (-1 == timems)
        {
            MLOG_ERROR("select() return with fd:{} {}, error:{} {}", m_tcp_socket, _maxfd, errno,
                       strerror(errno));
            continue;
        }
        if (FD_ISSET(m_tcp_socket, &readSet))
        {
            struct sockaddr_in addr;
            socklen_t len = sizeof(addr);
            fd = accept(m_tcp_socket, (struct sockaddr *) &addr, &len);
            if (0 >= fd)
            {
                MLOG_ERROR("accept failed, error:{} {}", errno, strerror(errno));
                continue;
            } else if (MAX_FD <= m_clientNum)
            {
                MLOG_ERROR("Sorry, Clients is full!");
                continue;
            }

            if (nullptr == this->m_client[fd])
            {
                m_clientNum++;
                this->m_client[fd] = ConnectPtr(new TcpConnect(fd,addr), [&](TcpConnect *p) {
                    delete p;
                    this->m_client[fd] = nullptr;
                    m_clientNum--;
                });
            } else {
                this->m_client[fd]->reloadConnect(fd,addr);
            }
            MLOG_WARN("{}, have a new TcpClient connect, now client_num:{}",timems, m_clientNum);

            FD_SET(fd, &m_socketSet);  /*将套接字加入到监听的 select 套接字集合*/
            _maxfd = (_maxfd > fd) ? _maxfd : fd;

            setNonBlocking(fd);

            /* 如果没有更多的就绪文件描述符继续回到上面select阻塞监听,负责处理未处理完的就绪文件描述符 */
            if (--timems == 0)
                continue;

        }

        /* //遍历所有客户端 接收客户端数据 */
        auto client_p = this->m_client.begin();
        /*for(int i = 0; i < CommServerInfo::s_usedCount;client_p++, i++)*/
        for (; client_p != this->m_client.end(); client_p++)
        {
            int tem_fd = client_p->first;
            /* 如果这个是否需要读 */
            if ((nullptr == client_p->second) || (0 > tem_fd))
            {
                MLOG_ERROR("tem_fd:{}", tem_fd);
                continue;
            }

            if (FD_ISSET(tem_fd, &readSet)) {
                //读数据, 解析
                client_p->second->RecvTcpMessage(parse);
                if (--timems == 0)
                    break;
            }
        }

        /*500ms循环一次*/
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    MLOG_ERROR("TcpServerThread exit,{}!",m_tcp_socket);
}

bool DoIPServer::UdpInit() noexcept
{
    if (-1 == m_udp_socket)
    {
//        MLOG_INFO("info value!");
        int ServerPort{p_info->ServerPort};
//        MLOG_INFO("info value!{}", ServerPort);
        struct sockaddr_in serveraddr;
        m_udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if(m_udp_socket < 0)
        {
            MLOG_ERROR("Create socket fail!error:{} {}", errno, strerror(errno));
            return false;
        }
//        MLOG_INFO("info value!");
        memset( (void*)&serveraddr,0, sizeof(struct sockaddr_in) );
//        MLOG_INFO("info value!");
//        setNonBlocking(m_udp_socket);
        //设置端口复用
        int optval = 1;
        if (-1 == (setsockopt(m_udp_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(int))))
        {
            MLOG_ERROR("setsockopt failed, errno:{} {}",errno, strerror(errno));
            closeUdpSocket();
            return false;
        }

        serveraddr.sin_family = AF_INET;
        MLOG_INFO("info value!");
        serveraddr.sin_port = htons(ServerPort);
        MLOG_INFO("info value!");
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); //ip地址-字符串转IP
        //binds the socket to any IP Address and the Port Number 13400
        if (0 > bind(m_udp_socket, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr_in)))
        {
            MLOG_ERROR("Udp bind failed,errno:{} {}", errno, strerror(errno));
            closeUdpSocket();
            return false;
        }
        //setting the IP Address for Multicast
//        setMulticastGroup("224.0.0.2");
//        if (!setMulticastGroup("192.168.25.1"));
        MLOG_INFO("m_udp_socket:{}", m_udp_socket);
        return true;
    }
}

void DoIPServer::UdpServerThread(const DoIpParsePtr parse) noexcept
{
    if (nullptr == parse)
    {
        MLOG_ERROR("DoIpParsePtr is nullptr!");
        return;
    }
//    MLOG_INFO("udp_socket:{}!",m_udp_socket);
    doip::DoIpHeader header;
    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(struct sockaddr_in);
    int readBytes{-1};

    while (0 < m_udp_socket)
    {
//        m_udpSendBuf.clear();
        memset(&header, 0, sizeof header);
        /* 1.接收 DoIp 数据 */
        MLOG_INFO("recv data payload_len:{} {}",cHeaderSize, header.mpayloadLength);
        readBytes = recvfrom(m_udp_socket, (char *) &header, cHeaderSize, 0, (struct sockaddr *) &clientaddr, &len); //读取client数据,有数据更新才读取，否则阻塞
        if (readBytes < 0)
        {
            MLOG_ERROR("Udp recv failed, error:{} {}", errno, strerror(errno));
            continue;
        }
        else if(0 == readBytes)
        {
            MLOG_INFO("sockfd:{} read over", m_udp_socket);
        }
        MLOG_INFO("recv data payload_len:{} {}, client IP:{}, port:{}"
            , readBytes, header.mpayloadLength, inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);

        m_udpRecvBuf.resize(cHeaderSize + header.mpayloadLength);
        header.SerialVecBuff(m_udpRecvBuf);

        if (0 != header.mpayloadLength)
        {
            /******************* 获取数据 payload ***************/
            readBytes = recvfrom(m_udp_socket, (char *) &m_udpRecvBuf[cHeaderSize], header.mpayloadLength, 0, (struct sockaddr *) &clientaddr, &len);
            if (readBytes < 0)
            {
                MLOG_ERROR("Udp recv failed, error:{} {}", errno, strerror(errno));
                continue;
            }
            MLOG_INFO("recv data payload_len:{} {}, client IP:{}, port:{}"
                , readBytes, header.mpayloadLength, inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
        }

        /* 2.数据解析 数据: m_udpHeader, 长度: readBytes */
        parse->parseResponse(m_udpRecvBuf, m_udpSendBuf, SOCKET_TYPE::UDP);

        /* 3.发送响应 */
        if (m_udpSendBuf.size())
        {
            readBytes = sendto(m_udp_socket, m_udpSendBuf.data(), m_udpSendBuf.size(), 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr));
//                    readBytes = sendUdpMessage((const char *) UdpsendVal.data(), UdpsendVal.size());
            MLOG_INFO("sendto value len:{}!",readBytes);
        }
        /* 100ms循环一次 */
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    MLOG_ERROR("UdpServerThread exit,{}!",m_udp_socket);
}

/*
 * Closes the socket for this server
 */
void DoIPServer::closeTcpSocket() noexcept{
    if (-1 != m_tcp_socket)
    {
        MLOG_CRITICAL("close tcp socket");
        close(m_tcp_socket);
        m_tcp_socket = -1;
    }
}

void DoIPServer::closeUdpSocket() noexcept{
    if (-1 != m_udp_socket)
    {
        MLOG_CRITICAL("close udp socket");
        close(m_udp_socket);
        m_udp_socket = -1;
    }
}

bool DoIPServer::setMulticastGroup(const char* address) noexcept
{
    if (nullptr == address)
    {
        MLOG_ERROR("address param is nullptr");
        return false;
    }
    int loop = 1;
    //set Option using the same Port for multiple Sockets
    int setPort = setsockopt(m_udp_socket, SOL_SOCKET, SO_REUSEADDR, &loop, sizeof(loop));
    if(setPort < 0)
    {
        MLOG_ERROR("setsockopt failed!error:{} {}", errno, strerror(errno));
        closeUdpSocket();
        return false;
    }
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(address);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    //set Option to join Multicast Group
    int setGroup = setsockopt(m_udp_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq));
    if(setGroup < 0)
    {
        MLOG_ERROR("Setting Address failed!error:{} {}", errno, strerror(errno));
        closeUdpSocket();
        return false;
    }
    return true;
}

int DoIPServer::sendVehicleAnnouncement() noexcept
{
    int broadcast{1};
    const char* address = "255.255.255.255";
    struct sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(struct sockaddr_in));
    //setting the destination port for the Announcement to 13401
    clientAddress.sin_port=htons(doip::cClientPort);
    int setAddressError = inet_aton(address,&(clientAddress.sin_addr));
    if(setAddressError != 0)
    {
        MLOG_INFO("Broadcast Address set succesfully");
    }
    
    int socketError = setsockopt(m_udp_socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast) );
    if(socketError == 0)
    {
        MLOG_INFO("Broadcast Option set successfully");
    }
    return socketError;
}

int DoIPServer::setNonBlocking(int fd, bool isnonblock)
{
    int flags, s;
    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        MLOG_ERROR("fcntl failed,errno:{} {}", errno, strerror(errno));
        return -1;
    }

    long arg = (isnonblock) ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    s = fcntl(fd, F_SETFL, arg);
    if (s == -1) {
        MLOG_ERROR("fcntl failed,errno:{} {}", errno, strerror(errno));
        return -1;
    }
    return 0;
}