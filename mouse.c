#include "mouse.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/socket.h>
#include <sys/types.h>

#ifndef NDEBUG
#define dbg_print(format, args...) \
    fprintf(stdout, format, ##args)
#define fatal_error() assert(0)
#else
#define dbg_print(format, args...)
#define fatal_error() exit(EXIT_FAILURE)
#endif /* NDEBUG */

#define PAYLOAD_INITALLOC_SIZE 16

static int sockfd;
static int device_id;
static struct sockaddr_in server_addr;
static struct hostent* host;

static packet local_ACK;
static packet local_NACK;

int mouse_init(char* host_name, int port)
{
    //device_id = d;
    if ((host = gethostbyname(host_name)) == NULL) {
        fprintf(stderr, "Gethostname error\n");
        fatal_error();
    }
    printf("IP:%s\n",inet_ntoa(*((struct in_addr *)host->h_addr)));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Socket Error:%s\a\n", strerror(errno));
        fatal_error();
    }
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr*)host->h_addr);
    if (connect(sockfd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) == -1) {
        fprintf(stderr, "Connect Error:%s\a\n", strerror(errno));
        fatal_error();
    }
    return 0;
}

int mouse_login(int device_id,char* device_secret)
{
    int ret = -1;

    packet* p_login = packet_allocate();
    dbg_print("---Preparing Login packet---\n");
    p_login->message_type = LOGIN;
    packet_put_int(p_login, device_id);
    packet_put_buffer(p_login, (unsigned char*)device_secret, 32);
    dbg_print("---Sending Login packet---\n");
    send_packet(p_login);
    packet_free(p_login);
    dbg_print("---Waiting ACK packet---\n");
    packet* p_ACK = recv_packet();
    if (PACKET_ACK_CHECK(p_ACK)) {
        dbg_print("recv ACK.\n");
        ret = 0;
    }
    else if (PACKET_NACK_CHECK(p_ACK)) {
        dbg_print("recv NACK. Login Failed.\n");
    }
    else {
        dbg_print("Error: recv type: %02X\n", PACKET_TYPE(p_ACK));
        packet_free(p_ACK);
    }
    dbg_print("---Login end---\n\n");
    return ret;
}

int mouse_report(packingfunc func, void* data)
{
    packet* p_report = packet_allocate();
    dbg_print("---Preparing Report packet---\n");
    p_report->message_type = REPORT;
    p_report = func(p_report, data);
    //TODO: validate.
    dbg_print("---Sending Report packet---\n");
    send_packet(p_report);
    packet_free(p_report);
    dbg_print("---Waiting ACK packet---\n");
    packet* p_ACK = recv_packet();
    if (PACKET_ACK_CHECK(p_ACK)) {
        dbg_print("recv ACK.\n");
    }
    else if (PACKET_NACK_CHECK(p_ACK)) {
        dbg_print("recv NACK.\n");
    }
    else {
        dbg_print("Error: recv type: %02X\n", PACKET_TYPE(p_ACK));
        packet_free(p_ACK);
    }
    dbg_print("---Report end---\n\n");
    return 0;
}

int mouse_control_send(packingfunc func, void* data)
{
    //? Same as mouse_report?
    packet* p_control = packet_allocate();
    dbg_print("---Preparing Control packet(tx)---\n");
    p_control->message_type = CONTROL;
    p_control = func(p_control, data);
    dbg_print("---Sending Control packet---\n");
    send_packet(p_control);
    packet_free(p_control);
    dbg_print("---Waiting ACK packet---\n");
    packet* p_ACK = recv_packet();
    if (PACKET_ACK_CHECK(p_ACK)) {
        dbg_print("recv ACK.\n");
    }
    else if (PACKET_NACK_CHECK(p_ACK)) {
        dbg_print("recv NACK.\n");
    }
    else {
        dbg_print("Error: recv type: %02X\n", PACKET_TYPE(p_ACK));
        packet_free(p_ACK);
    }
    dbg_print("---Control end(tx)---\n\n");
    return 0;
}

packet* mouse_control_recv(int device_id, int control_id)
{
    packet* p_control = packet_allocate();
    dbg_print("---Preparing Control packet(poll)---\n");
    p_control->message_type = CONTROL;
    packet_put_buffer(p_control, (unsigned char*)"R", 1);
    packet_put_int(p_control, device_id);
    packet_put_int(p_control, control_id);
    dbg_print("---Sendinging Control packet---\n");
    send_packet(p_control);
    packet_free(p_control);
    dbg_print("---Control end(poll)---\n");

    dbg_print("---Waiting---\n");
    packet* p_ACK = recv_packet();

    if (PACKET_TYPE(p_ACK) != CONTROL) {
        return NULL;
    }
    else {
        return p_control;
    }
}

int mouse_logout()
{
    packet* p_logout = packet_allocate();
    dbg_print("---Preparing Logout packet---\n");
    p_logout->message_type = LOGOUT;
    dbg_print("---Sending Logout packet---\n");
    send_packet(p_logout);
    packet_free(p_logout);
    dbg_print("---Waiting ACK packet---\n");
    packet* p_ACK = recv_packet();
    if (PACKET_ACK_CHECK(p_ACK)) {
        dbg_print("recv ACK.\n");
    }
    else if (PACKET_NACK_CHECK(p_ACK)) {
        dbg_print("recv NACK.\n");
    }
    dbg_print("---Logout end---\n\n");
    return 0;
}

void _print_head(packet* p)
{
    dbg_print("Message type: ");
    switch (p->message_type) {
        case ACK:       dbg_print("ACK\n");     break;
        case NACK:      dbg_print("NACK\n");    break;
        case LOGIN:     dbg_print("LOGIN\n");   break;
        case REPORT:    dbg_print("REPORT\n");  break;
        case CONTROL:   dbg_print("CONTROL\n"); break;
        case LOGOUT:    dbg_print("LOGOUT\n");  break;
    }
}

void _print_payload(packet* p)
{
#ifndef NDEBUG
    int i = 0, length = p->content_length;
    unsigned char* sptr = p->payload;

    dbg_print("Payload content(%d bytes):\n", length);
    while (i < length) {
        dbg_print("%02X ", sptr[i++]);
        if (i % 8 == 0)
            dbg_print("\n");
    }
    /* display 8 bytes per line. */
    if (length % 8 != 0)
        dbg_print("\n");
#endif
}

int send_packet(packet* p)
{
    unsigned char* buffer;
    buffer = (unsigned char*)malloc(1 + p->content_length);
    if (buffer == NULL) {
        fprintf(stderr, "send_packet():buffer allocate error!\n");
        fatal_error();
    }

    memcpy(buffer, &(p->message_type), 1);
    memcpy(buffer + 1, p->payload, p->content_length);

    _print_head(p);
    _print_payload(p);
    send(sockfd, buffer, p->content_length + 1, 0);
    //TODO: Error detection
    dbg_print("%d byte has been sent.\n", p->content_length + 1);

    free(buffer);
    return 0;
}

#define RECV_BUFFER_SIZE 100
packet* recv_packet()
{
    int buflen = 0;
    packet* p = NULL;

    unsigned char buffer[RECV_BUFFER_SIZE] = {0};

    //block!
    while (1) {
        buflen = recv(sockfd, buffer, RECV_BUFFER_SIZE, 0);
        if (buflen > 0) {
            dbg_print("Recv packet @size %d\n", buflen);
            if (buffer[0] == ACK) {
                p = &local_ACK;
            }
            else if (buffer[0] == NACK) {
                p = &local_NACK;
            }
            else {
                p = packet_allocate();
                p->message_type = buffer[0];
                packet_put_buffer(p, buffer + 1, buflen - 1);
            }
            break;
        }
        else if (buflen < 0) {
            if (errno == EAGAIN||errno == EWOULDBLOCK||errno == EINTR) {
                continue;
            }
            else {
                break;
            }
        }
    }

    _print_head(p);
    _print_payload(p);

    return p;
}

packet* packet_allocate()
{
    packet* p;
    p = (packet*)malloc(sizeof(packet));
    if (p == NULL) {
        fprintf(stderr, "packet_allocate():packet allocate error!\n");
        fatal_error();
    }
    p->size = PAYLOAD_INITALLOC_SIZE;
    p->content_length = 0;
    p->payload = (unsigned char*)malloc(p->size);
    //TODO: check NULL here
    return p;
}

packet* packet_reallocate(packet* p, int new_size)
{
    unsigned char* buffer;
    p->size = new_size;
#if ENABLE_REALLOC
    buffer = (unsigned char*)realloc(p->payload, new_size);
#else
    buffer = (unsigned char*)malloc(new_size);
    memcpy(buffer, p->payload, p->content_length);
    free(p->payload);
#endif
    if (buffer == NULL) {
        fprintf(stderr, "packet_put_buffer():packet re-allocate error!\n");
        fatal_error(); //TOFIX: ?, what about packet_allocate?
    }
    p->payload = buffer;
    return p;
}

void packet_free(packet* p)
{
    if (p != NULL) {
        free(p->payload);
        free(p);
    }
}

void packet_put_int(packet* p, int n)
{
    dbg_print("packet_put_int():%d\n", n);
    packet_put_buffer(p, (unsigned char*)&n, sizeof(n));
}

void packet_put_float(packet* p, float n)
{
    dbg_print("packet_put_float():%f\n", n);
    packet_put_buffer(p, (unsigned char*)&n, sizeof(n));
}

void packet_put_double(packet* p, double n)
{
    dbg_print("packet_put_double():%lf\n", n);
    packet_put_buffer(p, (unsigned char*)&n, sizeof(n));
}

void packet_put_char(packet* p, char n)
{
    dbg_print("packet_put_char():%c\n", n);
    packet_put_buffer(p, (unsigned char*)&n, sizeof(n));
}

void packet_put_buffer(packet* p, unsigned char* buffer, int length)
{
    int re_alloc = 0, new_size = p->size;
    if (length == 0)
        return;

    while (new_size < p->content_length + length) {
        new_size *= 2;
        re_alloc = 1;
    }
    if (re_alloc) {
        packet_reallocate(p, new_size);
    }
#ifndef NDEBUG
    int i;
    for (i = 0; i < length; i++) {
        p->payload[p->content_length] = buffer[i];
        dbg_print("%d:%d\n", i, buffer[i]);
        p->content_length++;
    }
#else
    memcpy(p->payload + p->content_length, buffer, length);
    p->content_length += length;
#endif
}

static packet local_ACK = {
    ACK,
    NULL,
    0,
    0
};

static packet local_NACK = {
    NACK,
    NULL,
    0,
    0
};
