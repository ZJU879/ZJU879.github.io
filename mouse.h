#ifndef _MOUSE_H
#define _MOUSE_H

enum MESSAGE_TYPE {
	ACK = 0,
	NACK = 1,
	LOGIN = 2,
	REPORT = 3,
	CONTROL = 4,
	LOGOUT = 5
};

typedef struct packet {
	unsigned char message_type;
	unsigned char* payload;
	int size;
	int content_length;
}packet;

typedef packet* (*packingfunc)(packet*, void*);
typedef packingfunc parsingfunc;

#ifdef __cplusplus
extern "C" {
#endif

int mouse_init(int device_id, char* host_name, int port);
int mouse_login(char* device_secret);

int mouse_report(packingfunc func, void* data);
int mouse_control_send(packingfunc func, void* data);
packet* mouse_control_recv(int device_id, int control_id);
int mouse_logout();

packet* packet_allocate();
packet* packet_reallocate(packet* p, int new_size);
void packet_free(packet* p);
int send_packet(packet* p);
packet* recv_packet();

void packet_put_int(packet* p, int n);
void packet_put_float(packet* p, float n);
void packet_put_double(packet* p, double n);
void packet_put_char(packet* p, char n);
void packet_put_buffer(packet* p, unsigned char* buffer, int length);

#ifdef __cplusplus
}
#endif

#define PACKET_TYPE(p) (((packet*)(p))->message_type)
#define PACKET_ACK_CHECK(p) (PACKET_TYPE(p) == ACK)
#define PACKET_NACK_CHECK(p) (PACKET_TYPE(p) == NACK)

#endif /* _MOUSE_H */
