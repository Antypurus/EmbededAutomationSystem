#include "UDPSocket.h"

#include "lwip/inet.h"
#include "../Wi-Fi/WiFi_STA.h"

UDPSocket::UDPSocket(const std::string& t_address, const SOCKET_PORT t_port)
:m_str_out_address(t_address), m_port(t_port)
{
    this->create_socket();
    this->configure_my_socket_address();
    this->configure_out_socket_address();
    this->bind_socket();
}

UDPSocket::UDPSocket(const SOCKET_PORT t_port)
{
    this->create_socket();
    this->configure_my_socket_address();
    this->bind_socket();
}

void UDPSocket::listen(const SOCKET_PORT t_port)
{
    this->m_port = t_port;
    this->configure_my_socket_address();

    this->bind_socket();
}

void UDPSocket::close_socket()
{
    close(this->m_socket_fd);
}

int UDPSocket::send(const std::string& t_data)
{
    const char* data = t_data.c_str();
    return sendto(this->m_socket_fd, data, t_data.length()+1, 0, (struct sockaddr*)&this->m_out_address, sizeof(this->m_out_address));
}

int UDPSocket::send(const std::string& t_out_address,const std::string& t_data)
{
    this->set_out_address(t_out_address);// TODO: find a better way to do this that can maintain cons correctness
    return this->send(t_data);
}

void UDPSocket::set_out_address(const std::string& t_out_address)
{
    this->m_str_out_address = t_out_address;
    this->configure_out_socket_address();
}

std::string UDPSocket::receive() const
{
    int recv_data;
    char data_buffer[80]; // TODO: Figure out ideal size for this buffer;
    recv_data = recv(this->m_socket_fd, data_buffer, sizeof(data_buffer), 0);
    if (recv_data > 0)
    {
        data_buffer[recv_data] = '\0';
        return data_buffer;// Implicit conversion from char* to std::string
    }
    return "";// No data received
}

void UDPSocket::configure_my_socket_address()
{
    const char* my_ip = WiFi_STA::get_ip().c_str();
    memset(&this->m_my_address, 0, sizeof(SOCKET_ADDRESS));
    this->m_my_address.sin_family = AF_INET;
    this->m_my_address.sin_addr.s_addr = inet_addr(my_ip);
    this->m_my_address.sin_port = htons(this->m_port);
}

void UDPSocket::configure_out_socket_address()
{
    memset(&this->m_out_address, 0, sizeof(SOCKET_ADDRESS));
    this->m_out_address.sin_family = AF_INET;
    this->m_out_address.sin_addr.s_addr = inet_addr(this->m_str_out_address.c_str());
    this->m_out_address.sin_port = htons(this->m_port);
}

void UDPSocket::create_socket()
{
    this->m_socket_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (this->m_socket_fd < 0)
    {
        printf("socket call failed");
        exit(0);
    }
    this->timeoutTimeInMiliSeconds.tv_usec = SOCKET_TIMEOUT_MS;
    lwip_setsockopt(this->m_socket_fd, SOL_SOCKET, SO_RCVTIMEO,&this->timeoutTimeInMiliSeconds, sizeof(struct timeval));
}

void UDPSocket::bind_socket()
{
    auto ret = bind(this->m_socket_fd, (struct sockaddr *)&this->m_my_address, sizeof(SOCKET_ADDRESS));
    std::cout<<"Bind Return Code:"<<ret<<std::endl;
}

void UDPSocket::set_multicast_ttl()
{
    uint8_t  ttl = 1;
    setsockopt(this->m_socket_fd,IPPROTO_IP,IP_MULTICAST_TTL,&ttl,sizeof(uint8_t ));
}

void UDPSocket::disable_multicast_loop()
{
    uint8_t loopback_val = 0;
    setsockopt(this->m_socket_fd, IPPROTO_IP, IP_MULTICAST_LOOP,
                     &loopback_val, sizeof(uint8_t));
}

void UDPSocket::join_multicast_group()
{
    this->disable_multicast_loop();

    struct ip_mreq imreq = { 0 };
    struct in_addr iaddr = { 0 };
    int err = 0;

    tcpip_adapter_ip_info_t ip_info = { 0 };
    tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info);
    
    imreq.imr_interface.s_addr = IPADDR_ANY;

    err = inet_aton(this->m_str_out_address.c_str(), &imreq.imr_multiaddr.s_addr);
    setsockopt(this->m_socket_fd, IPPROTO_IP, IP_MULTICAST_IF, &iaddr, sizeof(struct in_addr));
    err = setsockopt(this->m_socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imreq, sizeof(struct ip_mreq));
}