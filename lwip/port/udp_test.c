/*
 * @Author: Jiyongjie
 * @Date: 2025-03-25 23:42:02
 * @LastEditors: Jiyongjie
 * @LastEditTime: 2025-03-27 22:51:00
 * @Description: file content
 */
/*
 * udp_test.c
 *
 *  Created on: 2025年3月25日
 *      Author: yongjie
 */

 #include "lwip/udp.h"
 #include "lwip/ip_addr.h"
#include "lwip/ip4_addr.h"
 #include <string.h>

 #define DEST_IP_ADDR "192.168.1.171"
 #define DEST_PORT 8888

 static struct udp_pcb *upcb = NULL;
 void udp_send_log(const char *log, size_t size)
 {
	 struct pbuf *log_p = NULL;
	 if (upcb == NULL)
	 {
		 return;
	 }

	 if (log == NULL)
	 {
		 return;
	 }

	 if (log_p == NULL)
	 {
		    // Create a pbuf to hold the "hello world" message
		 log_p = pbuf_alloc(PBUF_TRANSPORT, size, PBUF_RAM);
		    if (log_p == NULL) {
		        PRINTF("Failed to allocate pbuf\n");
		        return;
		    }
	 }

	    // Copy the message into the pbuf
	    memcpy(log_p->payload, log, size);

	    // Send the UDP packet
	    err_t err = udp_send(upcb, log_p);
	    if (err != ERR_OK) {
	        PRINTF("Failed to send UDP packet: %d\n", err);
	    } else {
	        // PRINTF("UDP packet sent to %s:%d\n", DEST_IP_ADDR, DEST_PORT);
	    }

	    // Free the pbuf
	    pbuf_free(log_p);
 }

 void udp_test_send(void) {
    struct pbuf *p;

    // Create a pbuf to hold the "hello world" message
    const char *msg = "hello world";
    p = pbuf_alloc(PBUF_TRANSPORT, strlen(msg), PBUF_RAM);
    if (p == NULL) {
        PRINTF("Failed to allocate pbuf\n");
        return;
    }

    // Copy the message into the pbuf
    memcpy(p->payload, msg, strlen(msg));

    // Send the UDP packet
    err_t err = udp_send(upcb, p);
    if (err != ERR_OK) {
        PRINTF("Failed to send UDP packet: %d\n", err);
    } else {
        // PRINTF("UDP packet sent to %s:%d\n", DEST_IP_ADDR, DEST_PORT);
    }

    // Free the pbuf
    pbuf_free(p);
}

static void udp_receive_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) 
{
    // PRINTF("Received UDP packet from %s:%d\n", ipaddr_ntoa(addr), port);

    // Print the payload
    if (p != NULL) {
        PRINTF("remote ip: %s\n", ipaddr_ntoa(addr));
        PRINTF("remote port: %d\n", port);
        PRINTF("Payload: ");
        for (int i = 0; i < p->len; i++) {
            PRINTF("%c", ((char *)p->payload)[i]);
        }
        PRINTF("\n");
    }

    // Free the pbuf
    pbuf_free(p);
}

void udp_test_init(void)
{
    ip_addr_t dest_ip;

    upcb = udp_new();
    if (upcb != NULL)
    {
        IP4_ADDR(&dest_ip, 192,168,1,171);
        err_t err = udp_connect(upcb, &dest_ip, DEST_PORT);
        if (err != ERR_OK)
        {
            PRINTF("Failed to connect UDP PCB:%d\n", err);
            udp_remove(upcb);
            return;
        }
    }
    else
    {
        PRINTF("udp_new failed\n");
    }

    err_t err = udp_bind(upcb, IP_ADDR_ANY, DEST_PORT);
    if (err != ERR_OK) {
        PRINTF("Failed to bind UDP PCB to port %d: %d\n", DEST_PORT, err);
        udp_remove(upcb);
        upcb = NULL;
        return;
    }

    udp_recv(upcb, udp_receive_callback, NULL);  
    PRINTF("UDP test initialized and listening on port %d\n", DEST_PORT);  
}
