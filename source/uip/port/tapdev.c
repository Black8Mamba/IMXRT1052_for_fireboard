/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: tapdev.c,v 1.8 2006/06/07 08:39:58 adam Exp $
 */

#define LOG_TAG    "tapdev"
#define LOG_LVL    ELOG_LVL_DEBUG

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "fsl_debug_console.h"
#include "elog.h"
#include "fsl_enet.h"
#include "bsp_phy.h"
#include "uip.h"
#include "uip_arp.h"
#include "uip_timer.h"

#define ENET_TIMEOUT        (0xFFFU)
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

extern struct uip_eth_addr uip_ethaddr;
extern enet_handle_t net_handle;
enet_handle_t handle;
uint8_t mac[] = {0x01,0x02,0x03,0x04,0x05,0x06};
/*---------------------------------------------------------------------------*/
void tapdev_init(void)
{
	for (int i = 0; i < 6; ++i)
	{
		uip_ethaddr.addr[i] = mac[i];
	}
	log_i("set mac:%x:%x:%x:%x:%x:%x\r\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

    enet_init();
}
/*---------------------------------------------------------------------------*/
uint32_t tapdev_read(void)
{

	status_t status;
	uint32_t length = 0;

	status = ENET_GetRxFrameSize(&net_handle, &length, 0);
	if (status == kStatus_Success)
	{
		ENET_ReadFrame(ENET, &net_handle, uip_buf, length, 0, NULL);
	} else
	{
		length = 0;
	}

	return length;
}
/*---------------------------------------------------------------------------*/
void tapdev_send(void)
{
    for (int counter = ENET_TIMEOUT; counter != 0U; counter--)
    {
        if (ENET_SendFrame(ENET, &net_handle, uip_buf, uip_len, 0, false, NULL) != kStatus_ENET_TxFrameBusy)
        {
            return;
        } else
        {
        	log_w("ENET_SendFrame failed!");
        }
    }
}

void uip_log(char *msg)
{
	elog_raw("%s\r\n", msg);
}

void uip_appcall(void)
{

}

void udp_8888_app(void)
{
    if (uip_poll())
    {
        char *tmp_dat = "the auto send!\r\n";
        uip_send((char *)tmp_dat,strlen(tmp_dat));
    }
    if (uip_newdata())
    {
        char *tmp_dat = "receive the data!\r\n";
        elog_raw("%d\r\n",uip_len);
        elog_raw("%s\r\n",(char *)uip_appdata);
//        uip_send((char *)tmp_dat,strlen(tmp_dat));
    }
}

void uip_udp_appcall(void)
{
	uip_udp_appstate_t *state = &(uip_udp_conn->appstate);

	if (uip_newdata())
	{
		switch(uip_udp_conn->lport)
		{
		case HTONS(8888):
				elog_raw("as string: %.*s\n", uip_datalen(), (char*)uip_appdata);
				break;
		default:
			break;
		}
	} else if (uip_poll())
	{
		switch(uip_udp_conn->rport)
		{
		case HTONS(8888):
				if (state->tx_flag == 1)
				{
					if (uip_len == 0)
					{
						uip_send(state->buffer, state->len);
						state->tx_flag = 0;
					}
				}

		break;
		}
	}
}

void uip_polling(void)
{
    uint8_t i;
    static struct timer periodic_timer, arp_timer;
    static uint8_t timer_ok=0;
    if(timer_ok==0)//仅初始化一次
    {
        timer_ok = 1;
        timer_set(&periodic_timer,CLOCK_SECOND/50);  //创建1个0.5秒的定时器
        timer_set(&arp_timer,CLOCK_SECOND*10);           //创建1个10秒的定时器
    }

	uip_len = tapdev_read();
	if (uip_len > 0)
	{
        if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
            uip_arp_arpin();
            if(uip_len>0){
                tapdev_send();
            }
        }

        if (BUF->type == htons(UIP_ETHTYPE_IP))
        {
        	uip_arp_ipin();
        	uip_input();
            if(uip_len > 0){
                uip_arp_out();
                tapdev_send();
            }
        }
	} else if (timer_expired(&periodic_timer))
	{
		timer_reset(&periodic_timer);
        for(i=0;i<UIP_CONNS;i++)
        {
            uip_periodic(i);
             if(uip_len>0)
            {
                uip_arp_out();
                tapdev_send();
            }
        }

#if UIP_UDP
        for(i=0;i<UIP_UDP_CONNS;i++)
        {
            uip_udp_periodic(i);    //处理UDP通信事件
            if(uip_len > 0)
            {
                uip_arp_out();
                tapdev_send();
            }
        }
#endif
	} else if(timer_expired(&arp_timer))
	{
        timer_reset(&arp_timer);
        uip_arp_timer();
	}
}


/*---------------------------------------------------------------------------*/
