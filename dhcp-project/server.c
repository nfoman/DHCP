#include"dhcp.h" 


typedef struct {
    unsigned char code;
    unsigned char length;
    unsigned long value;
} dhcp_option;

struct sockaddr_in serv,client;

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
    unsigned char chaddr[16]; // Client hardware address// Padding
    unsigned long magic_cookie; // Magic cookie
    dhcp_option options[308]; 
} DHCP_PACKET;

typedef struct {
    unsigned long ip;
    int status;
} dhcp_ip;


void addOption(dhcp_option *options, unsigned char option, unsigned char length, unsigned long value) {
    options[option].code = option;
    options[option].length = length;
    options[option].value =  value; 
}

int DHCP_CLIENT(DHCP_PACKET *packet, int type) {
    switch (type)
    {
    case DHCPDISCOVER:
        if (packet->options[DHCP_MESSAGE_TYPE].value == DHCPDISCOVER) {
        return DHCPDISCOVER;
        }
        break;
    case DHCPREQUEST:
        if (packet->options[DHCP_MESSAGE_TYPE].value == DHCPREQUEST) {
        return DHCPREQUEST;
        }
        break;
    
    default:
        break;
    }

}
void fill_ip(dhcp_ip *arr){
    for (size_t i = 0; i < ALL_ADDRESSES; i++)
    {   
        arr[i].ip = inet_addr("192.168.1.2");
        arr[i].status = 0;
    }
    
}

void fillDhcpOptions_server(DHCP_PACKET *packet,int type) {
    unsigned long netmask = inet_addr("255.255.255.0");
    
    // Add options to DHCP_PACKET
    if(type==DHCPOFFER){
        
        addOption(packet->options, DHCP_SUBNET_MASK,4, netmask);
        addOption(packet->options, DHCP_MESSAGE_TYPE,1,DHCPOFFER);;
        addOption(packet->options, DHCP_SERVER_IDENTIFIER,4, (unsigned long)inet_addr("192.168.1.1"));
        addOption(packet->options, DHCP_LEASE_TIME, 2,3600);
        addOption(packet->options, DHCP_END,1, 0);
    }
    if(type==DHCPACK){
        
        addOption(packet->options, DHCP_SUBNET_MASK,4, netmask);
        addOption(packet->options, DHCP_MESSAGE_TYPE,1, DHCPACK);
        addOption(packet->options, DHCP_SERVER_IDENTIFIER,4, (unsigned long)inet_addr("192.168.1.1"));
        addOption(packet->options, DHCP_LEASE_TIME, 2,3600);
        addOption(packet->options, DHCP_END,1, 0);
    }
}
void pull_ip(dhcp_ip *arr, DHCP_PACKET *packet){
    
    for (int i = 0; i<ALL_ADDRESSES;i++)
    {
        if (arr[i].status!=1)
        {
            packet->yiaddr=arr[i].ip;
            arr[i].status = 1;
            break;
        }
        
    }
    
}

void Fill_Server(DHCP_PACKET *packet,int type,dhcp_ip *arr){
    
    packet->op = 2;  // DHCP request
    packet->htype = 1;  // Ethernet
    packet->hlen = sizeof(packet->chaddr);
    packet->xid = htonl(123456789);  // Transaction ID
    packet->ciaddr = 0;  // Client IP address
    packet->flags = 0;  // Broadcast
    pull_ip(arr,packet);
    packet->magic_cookie = htonl(0x63825363);
    fillDhcpOptions_server(packet,type);
}


void Dhcp_Offer(DHCP_PACKET *packet, struct sockaddr_in client, int fd,dhcp_ip *arr){
    int check = 0;
    socklen_t size = sizeof(serv);
    int type = DHCPOFFER;   
    Fill_Server(packet, type,arr);

    check=recvfrom(fd, packet, sizeof(DHCP_PACKET), 0, (struct sockaddr *) &client, &size);
    if (check!=0)
    {
        if ((DHCP_CLIENT(packet,DHCPDISCOVER)) == DHCPDISCOVER) {
            Fill_Server(packet,type,arr);
            sendto(fd, packet, sizeof(DHCP_PACKET), 0, (struct sockaddr *) &client, sizeof(client));
        }

    }
      
}

void Dhcp_ACK(DHCP_PACKET *packet, struct sockaddr_in client, int fd,dhcp_ip *arr){
    int check = 0;
    socklen_t size = sizeof(serv);
    int type = DHCPACK;   
    while (1){
    check=recvfrom(fd, packet, sizeof(DHCP_PACKET), 0, (struct sockaddr *) &client, &size);
        if (check!=0)
        {
            if ((DHCP_CLIENT(packet,DHCPREQUEST)) == DHCPREQUEST) {
                Fill_Server(packet,type,arr);
                printf("%ld\n",packet->options[DHCP_MESSAGE_TYPE].value);
                sendto(fd, packet, sizeof(DHCP_PACKET), 0, (struct sockaddr *) &client, sizeof(client));

            }
        }
    }
         
}


int main(){
    dhcp_ip arr[ALL_ADDRESSES];
    DHCP_PACKET *packet = malloc(sizeof(DHCP_PACKET));
    if (packet == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    serv.sin_family = AF_INET;
    serv.sin_port=htons(SERVER_PORT);
    int fd;
    fill_ip(arr);
    fd=socket(AF_INET,SOCK_DGRAM,0);
    if (fd == -1) {
        perror("error in socket()");
        exit(EXIT_FAILURE);
    }
    inet_pton(AF_INET, "255.255.255.255", &(serv.sin_addr));
    int ret =bind(fd,(const struct sockaddr*)&serv, sizeof(serv));
    if (ret == -1) {
        perror("error in bind()");
        exit(EXIT_FAILURE);
    }
    socklen_t size=sizeof(serv);
    while (1)
    {
        Dhcp_Offer(packet,client,fd,arr);
        Dhcp_ACK(packet,client,fd,arr);
    }

}
