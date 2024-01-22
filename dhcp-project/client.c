#include"dhcp.h" 


struct sockaddr_in serv, client;

typedef struct {
    unsigned char code;
    unsigned char length;
    unsigned long value;
} dhcp_option;

typedef struct {
    unsigned char op;        // DHCP message type
    unsigned char htype;     // Hardware address type
    unsigned char hlen;      // Hardware address length
    unsigned char hops;      // Number of relay agent hops
    unsigned long xid;       // Transaction ID
    unsigned short secs;     // Seconds since boot
    unsigned short flags;    // Flags
    unsigned long ciaddr;    // Client IP address
    unsigned long yiaddr;    // Your IP address
    unsigned long siaddr;    // Server IP address
    unsigned long giaddr;    // Gateway IP address
    unsigned char chaddr[16]; // Client hardware address // Padding
    unsigned long magic_cookie; // Magic cookie
    dhcp_option options[308]; 
} DHCP_PACKET;

void addOption(dhcp_option *options, unsigned char option, unsigned char length, unsigned long value) {
    
    options[option].code = option;
    options[option].length = length;
    options[option].value = value;
    
}

void fillDhcpOptions_client(DHCP_PACKET *packet,int type) {
     
    // Add options to DHCP_PACKET
    if(type==DHCPDISCOVER){
        addOption(packet->options, DHCP_MESSAGE_TYPE,1,DHCPDISCOVER);;
        addOption(packet->options, CLIENT_IDENTIFIER,6, 0xa85e45c602cd);
        addOption(packet->options, DHCP_END, 1,0);
    }
    else{

        addOption(packet->options, DHCP_MESSAGE_TYPE,1, DHCPREQUEST);
        addOption(packet->options, CLIENT_IDENTIFIER,6, 0xa85e45c602cd);
        addOption(packet->options, DHCP_REQUESTED_IP_ADDRESS, 4, packet->yiaddr);
        addOption(packet->options, DHCP_END,1, 0);  
    }   

}
   

void Fill_Client(DHCP_PACKET *packet, int type){
    packet->op = 1;  // DHCP request
    packet->htype = 1;  // Ethernet
    packet->hlen = sizeof(packet->chaddr);
    packet->xid = htonl(123456789);  // Transaction ID
    packet->ciaddr = 0;  // Client IP address
    packet->flags = 0;  // Broadcast
    memcpy(packet->chaddr, "\xa8\x5e\x45\xc6\x02\xcd", packet->hlen);
    packet->magic_cookie = htonl(0x63825363);
    fillDhcpOptions_client(packet, type);
    packet->yiaddr=0;
}

void Dhcp_Discover(DHCP_PACKET *packet, struct sockaddr_in serv, int fd){
    int check = 0;
    socklen_t size = sizeof(serv);
    int type = DHCPDISCOVER;   
    Fill_Client(packet, type);

    if(sendto(fd, packet, sizeof(DHCP_PACKET), 0, (struct sockaddr *)&serv, sizeof(serv))==-1){
        perror("sendto error()");
        exit(EXIT_FAILURE);
    }
    check=recvfrom(fd,packet,sizeof(DHCP_PACKET),0,(struct sockaddr*)&serv,&size);
    if (check==0){
        perror("recv error()");
        exit(EXIT_FAILURE);
    }
}

void Dhcp_Request(DHCP_PACKET *packet, struct sockaddr_in serv, int fd){
    int check = 0;
    socklen_t size = sizeof(serv);
    int type = DHCPREQUEST;
    Fill_Client(packet, type);

    if(sendto(fd, packet, sizeof(DHCP_PACKET), 0, (struct sockaddr *)&serv, sizeof(serv))==-1){
        perror("sendto error()");
        exit(EXIT_FAILURE);
    }
    check=recvfrom(fd,packet,sizeof(DHCP_PACKET),0,(struct sockaddr*)&serv,&size);
    if (check==0){
        perror("recv error()");
        exit(EXIT_FAILURE);
    }
    
}






int main(){
    DHCP_PACKET *packet = malloc(sizeof(DHCP_PACKET));
    if (packet == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in serv,client;
    int fd;
    int flag =1;  
    serv.sin_family = AF_INET;
    serv.sin_port=htons(SERVER_PORT);
    client.sin_family = AF_INET;
    client.sin_port=htons(CLIENT_PORT);
    client.sin_addr.s_addr=htonl(INADDR_ANY);
    inet_pton(AF_INET, "255.255.255.255", &(serv.sin_addr));
    fd=socket(AF_INET,SOCK_DGRAM,0);
    if (fd == -1) {
		perror("error in socket()");
		exit(EXIT_FAILURE);
	}
    setsockopt(fd,SOL_SOCKET,SO_BROADCAST,&flag,sizeof(flag));
    socklen_t size = sizeof(serv);
    
    Dhcp_Discover(packet, serv, fd);
    Dhcp_Request(packet, serv, fd);
    
    client.sin_addr.s_addr=packet->yiaddr;
    int ret =bind(fd,(const struct sockaddr*)&client,sizeof(client));
    if (ret == -1) {
        perror("error in bind()");
        exit(EXIT_FAILURE);
    }

}


