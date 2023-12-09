#ifndef DHCP_H
#define DHCP_H
#include <stdint.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 67
#define CLIENT_PORT 68
#define BUF_SIZE 1024
#define DHCPDISCOVER  1
#define DHCPOFFER 2
#define DHCPREQUEST 3
#define DHCPACK 4
#define DHCP_OP 0
#define DHCP_HTYPE 1
#define DHCP_HLEN 2
#define DHCP_HOPS 3
#define DHCP_XID 4
#define DHCP_SECS 8
#define DHCP_FLAGS 10
#define DHCP_CIADDR 12
#define DHCP_YIADDR 16
#define DHCP_SIADDR 20
#define DHCP_GIADDR 24
#define DHCP_CHADDR 28
#define DHCP_MAGIC_COOKIE 236
#define ALL_ADDRESSES 100

#define DHCP_MESSAGE_TYPE 53
#define DHCP_SERVER_IDENTIFIER 54
#define DHCP_LEASE_TIME 51
#define DHCP_SUBNET_MASK 1
#define CLIENT_IDENTIFIER 61
#define DHCP_ROUTER 3
#define DHCP_DOMAIN_NAME_SERVER 6
#define DHCP_END 255
#define DHCP_REQUESTED_IP_ADDRESS 50

#endif