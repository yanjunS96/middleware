#ifndef DOIPSERVER_H
#define DOIPSERVER_H

#include <array>
#include <map>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <net/if.h>
#include <unistd.h>

#include "doip_config.hpp"
#include "./server/doip_parse.hpp"

namespace motovis{
    namespace doip{

        class TcpConnect{
        public:
            TcpConnect():tcpSocket(-1){}
            TcpConnect(int tcpSocket, struct sockaddr_in & addr):tcpSocket(tcpSocket),m_addr(addr){ }
            virtual ~TcpConnect(){ closeSock();}

            int RecvTcpMessage(DoIpParsePtr) noexcept;
            int SendTcpMessage(const char * message, int messageLength) noexcept;

            bool isSocketActive(){return -1 != tcpSocket;}

            int recvOneMessage( char* message, int messageLength) noexcept;

            void reloadConnect(int, struct sockaddr_in &) noexcept;

        protected:
            int closeSock();
        private:
            int tcpSocket;
            struct sockaddr_in m_addr;

            SerialVecBuff sendVal; //需要发送的数据
            SerialVecBuff recvVal; //接收的数据
        };

        using ConnectPtr= std::shared_ptr<TcpConnect>;


        class DoIPServer {
//            friend class DoIpControl;
            static constexpr int MAX_FD{16}; /* 16个客户端--防止使用的客户端资源不足 */
        public:
            DoIPServer(const DoIpInfoPtr) noexcept;
            ~DoIPServer() noexcept;

            void StartServer(const DoIpParsePtr) noexcept;

        protected:
            int setNonBlocking(int, bool = true);
            bool TcpInit() noexcept;
            void TcpServerThread(const DoIpParsePtr) noexcept;
            void closeTcpSocket() noexcept;

            int sendVehicleAnnouncement() noexcept;
            bool setMulticastGroup(const char* address) noexcept;

            bool UdpInit() noexcept;
            void UdpServerThread(const DoIpParsePtr) noexcept;
            void closeUdpSocket() noexcept;
        private:
            DoIpInfoPtr p_info;

            int m_tcp_socket;
            fd_set m_socketSet; /* 监听处理的套接字集合 */
            int m_clientNum{0};
            std::map<int, ConnectPtr> m_client;

            int m_udp_socket;
            SerialVecBuff m_udpSendBuf;
            SerialVecBuff m_udpRecvBuf;
        };

        using ServerPtr=std::shared_ptr<DoIPServer>;
    }
}

#endif /* DOIPSERVER_H */