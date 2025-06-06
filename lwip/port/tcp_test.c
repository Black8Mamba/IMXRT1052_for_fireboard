/*
 * tcp_test.c
 *
 *  Created on: 2025年3月27日
 *      Author: yongjie
 */


#include "lwip/tcp.h"
#include "lwip/ip_addr.h"
#include "lwip/ip4_addr.h"
#include <string.h>

#define DEST_IP_ADDR "192.168.1.171"
#define DEST_PORT 8888

static struct tcp_pcb *tpcb = NULL;

// Callback for when data is successfully sent
static err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    PRINTF("Data successfully sent:%d\r\n", len);
    return ERR_OK;
}

// Callback for when the connection is established
static int is_connect = 0;
static err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    if (err != ERR_OK) {
        PRINTF("Connection failed: %d\r\n", err);
        tcp_close(tpcb);
        return err;
    }

    is_connect = 1;
    PRINTF("Connected to server %s:%d\r\n", DEST_IP_ADDR, DEST_PORT);

    return ERR_OK;
}

void tcp_send(void)
{
	if (is_connect == 0)
	{
		return;
	}
    // Send "hello world" message
    const char *msg = "hello world tcp";
    err_t err = tcp_write(tpcb, msg, strlen(msg), TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
        PRINTF("Failed to send data: %d\n", err);
        tcp_close(tpcb);
        return err;
    }

    // Register the sent callback
    tcp_sent(tpcb, tcp_client_sent);
}

// Function to initialize the TCP client and connect to the server
void tcp_client_test_init(void) {
    ip_addr_t dest_ip;

    // Create a new TCP PCB
    tpcb = tcp_new();
    if (tpcb == NULL) {
    	PRINTF("Failed to create TCP PCB\r\n");
        return;
    }

    // Set the destination IP address
    ipaddr_aton(DEST_IP_ADDR, &dest_ip);

    // Connect to the server
    err_t err = tcp_connect(tpcb, &dest_ip, DEST_PORT, tcp_client_connected);
    if (err != ERR_OK) {
    	PRINTF("Failed to connect to server: %d\r\n", err);
        tcp_close(tpcb);
        tpcb = NULL;
        return;
    }

    PRINTF("Connecting to server %s:%d...\r\n", DEST_IP_ADDR, DEST_PORT);
}
