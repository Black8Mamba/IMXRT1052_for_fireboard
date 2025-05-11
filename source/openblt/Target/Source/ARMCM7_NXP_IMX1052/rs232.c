/************************************************************************************//**
* \file         Source/_template/rs232.c
* \brief        Bootloader RS232 communication interface source file.
* \ingroup      Target__template_rs232
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2019  by Feaser    http://www.feaser.com    All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option) any later
* version.
*
* OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE. See the GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License along with OpenBLT. It
* should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
*
* \endinternal
****************************************************************************************/

/************************************************************************************//**
* \defgroup   Target__template_rs232 RS232 UART driver of a port
* \brief      This module implements the RS232 UART driver of a microcontroller port. 
* \details    For the most parts, this driver is already implemented. The only parts that
*             need porting are the UART initialization, byte reception and byte
*             transmission.
* \ingroup    Target__template
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#define LOG_TAG    "rs232"
#define LOG_LVL    ELOG_LVL_DEBUG
#include "boot.h"                                /* bootloader generic header          */
#include "board.h"
#include "fsl_common.h"
#include "fsl_adapter_uart.h"
#include "elog.h"
#include "lwrb/lwrb.h"

#if (BOOT_COM_RS232_ENABLE > 0)
/* TODO ##Port Include microcontroller peripheral driver header files here. */
UART_HANDLE_DEFINE(uart5_handle);

/****************************************************************************************
* Macro definitions
****************************************************************************************/
/** \brief Timeout time for the reception of a CTO packet. The timer is started upon
 *         reception of the first packet byte.
 */
#define RS232_CTO_RX_PACKET_TIMEOUT_MS (200u)
/** \brief Timeout for transmitting a byte in milliseconds. */
#define RS232_BYTE_TX_TIMEOUT_MS       (200u)

static int recv_done = 0;
static int start_recv = 0;

void hal_uart_transfer_callback(hal_uart_handle_t handle, hal_uart_status_t status, void *callbackParam)
{
	recv_done = 1;
}


/****************************************************************************************
* Function prototypes
****************************************************************************************/
blt_bool Rs232ReceiveByte(blt_int8u *data);
void     Rs232TransmitByte(blt_int8u data);

/************************************************************************************//**
** \brief     Initializes the RS232 communication interface.
** \return    none.
**
****************************************************************************************/
void Rs232Init(void)
{
  /* TODO ##Port Perform compile time assertion to check that the configured UART channel
   * is actually supported by this driver. The example is for a driver where UART
   * channels 0 - 2 are supported. 
   */
//  ASSERT_CT((BOOT_COM_RS232_CHANNEL_INDEX < 0));
  uint32_t uartClkSrcFreq = BOARD_DebugConsoleSrcFreq();

  hal_uart_config_t uart_config;
  uart_config.baudRate_Bps = BOOT_COM_RS232_BAUDRATE;
  uart_config.enableRx = 1;
  uart_config.enableTx = 1;
  uart_config.instance = BOOT_COM_RS232_CHANNEL_INDEX;
  uart_config.srcClock_Hz = uartClkSrcFreq;
  uart_config.stopBitCount = 1;
  uart_config.enableRxRTS  = 0U;
  uart_config.enableTxCTS  = 0U;
  uart_config.parityMode   = kHAL_UartParityDisabled;
  uart_config.stopBitCount = kHAL_UartOneStopBit;
#if (defined(HAL_UART_ADAPTER_FIFO) && (HAL_UART_ADAPTER_FIFO > 0u))
  uart_config.txFifoWatermark = 0;
  uart_config.rxFifoWatermark = 0;
#endif
  hal_uart_status_t status =  HAL_UartInit((hal_uart_handle_t)&uart5_handle[0], (const hal_uart_config_t *)&uart_config);
  if (status != kStatus_HAL_UartSuccess)
  {
	  elog_e(LOG_TAG, "HAL_UartInit failed!");
  }

  status = HAL_UartInstallCallback((hal_uart_handle_t)&uart5_handle[0], hal_uart_transfer_callback, NULL);
  if (status != kStatus_HAL_UartSuccess)
  {
	  elog_e(LOG_TAG, "HAL_UartInstallCallback failed!");
  }

} /*** end of Rs232Init ***/


/************************************************************************************//**
** \brief     Transmits a packet formatted for the communication interface.
** \param     data Pointer to byte array with data that it to be transmitted.
** \param     len  Number of bytes that are to be transmitted.
** \return    none.
**
****************************************************************************************/
void Rs232TransmitPacket(blt_int8u *data, blt_int8u len)
{
  blt_int16u data_index;
  #if (BOOT_COM_RS232_CS_TYPE == 1)
  blt_int8u csByte = len;
  #endif

  /* verify validity of the len-paramenter */
  ASSERT_RT(len <= BOOT_COM_RS232_TX_MAX_DATA);

  /* first transmit the length of the packet */
  Rs232TransmitByte(len);

  /* transmit all the packet bytes one-by-one */
  for (data_index = 0; data_index < len; data_index++)
  {
    /* keep the watchdog happy */
    CopService();
    /* write byte */
    Rs232TransmitByte(data[data_index]);
    #if (BOOT_COM_RS232_CS_TYPE == 1)
    csByte += data[data_index];
    #endif
  }
  #if (BOOT_COM_RS232_CS_TYPE == 1)
  /* write checksum byte */
  Rs232TransmitByte(csByte);
  #endif
} /*** end of Rs232TransmitPacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface packet if one is present.
** \param     data Pointer to byte array where the data is to be stored.
** \param     len Pointer where the length of the packet is to be stored.
** \return    BLT_TRUE if a packet was received, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool Rs232ReceivePacket(blt_int8u *data, blt_int8u *len)
{
  /* one extra for length and two extra for possibly configured checksum byte(s). */
  static blt_int8u xcpCtoReqPacket[BOOT_COM_RS232_RX_MAX_DATA+3];
  static blt_int8u xcpCtoRxLength;
  static blt_bool  xcpCtoRxInProgress = BLT_FALSE;
  static blt_int32u xcpCtoRxStartTime = 0;
  #if (BOOT_COM_RS232_CS_TYPE == 1)
  blt_int8u  csLen = 1;
  blt_int8u  csByte;
  blt_int16u csIdx;
  #else
  blt_int8u  csLen = 0;
  #endif

  /* start of cto packet received? */
  if (xcpCtoRxInProgress == BLT_FALSE)
  {
    /* store the message length when received */
    if (Rs232ReceiveByte(&xcpCtoReqPacket[0]) == BLT_TRUE)
    {
      if ( (xcpCtoReqPacket[0] > 0) &&
           (xcpCtoReqPacket[0] <= BOOT_COM_RS232_RX_MAX_DATA) )
      {
        /* store the start time */
        xcpCtoRxStartTime = TimerGet();
        /* reset packet data count */
        xcpCtoRxLength = 0;
        /* indicate that a cto packet is being received */
        xcpCtoRxInProgress = BLT_TRUE;
      }
    }
  }
  else
  {
    /* store the next packet byte */
    if (Rs232ReceiveByte(&xcpCtoReqPacket[xcpCtoRxLength+1]) == BLT_TRUE)
    {
      /* increment the packet data count */
      xcpCtoRxLength++;

      /* check to see if the entire packet was received. */
      if (xcpCtoRxLength == (xcpCtoReqPacket[0] + csLen))
      {
        #if (BOOT_COM_RS232_CS_TYPE == 1)
        /* calculate the byte checksum. */
        csByte = 0;
        for (csIdx = 0; csIdx < xcpCtoRxLength; csIdx++)
        {
          csByte += xcpCtoReqPacket[csIdx];
        }
        /* verify the checksum. */
        if (csByte != xcpCtoReqPacket[xcpCtoRxLength])
        {
          /* cancel the packet reception due to invalid checksum. */
          xcpCtoRxInProgress = BLT_FALSE;
          return BLT_FALSE;
        }
        #endif
        /* subtract the checksum from the packet length. */
        xcpCtoRxLength -= csLen;
        /* copy the packet data */
        CpuMemCopy((blt_int32u)data, (blt_int32u)&xcpCtoReqPacket[1], xcpCtoRxLength);
        /* done with cto packet reception */
        xcpCtoRxInProgress = BLT_FALSE;
        /* set the packet length */
        *len = xcpCtoRxLength;
        /* packet reception complete */
        return BLT_TRUE;
      }
    }
    else
    {
      /* check packet reception timeout */
      if (TimerGet() > (xcpCtoRxStartTime + RS232_CTO_RX_PACKET_TIMEOUT_MS))
      {
        /* cancel cto packet reception due to timeout. note that that automaticaly
         * discards the already received packet bytes, allowing the host to retry.
         */
        xcpCtoRxInProgress = BLT_FALSE;
      }
    }
  }
  /* packet reception not yet complete */
  return BLT_FALSE;
} /*** end of Rs232ReceivePacket ***/

blt_int8u recv_data = 0;
/************************************************************************************//**
** \brief     Receives a communication interface byte if one is present.
** \param     data Pointer to byte where the data is to be stored.
** \return    BLT_TRUE if a byte was received, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool Rs232ReceiveByte(blt_int8u *data)
{
  blt_bool result = BLT_FALSE;

  /* TODO ##Port Check if a new byte was received on the configured channel. This is
   * typically done by checking the reception register not empty flag. If a new byte 
   * was received, read it out and store it in '*data'. Next, clear the reception flag
   * such that a new byte can be received again. Finally, set 'result' to BLT_TRUE to
   * indicate to the caller of this function that a new byte was received and stored.
   */

  if (start_recv == 0)
  {
	  recv_done = 0;
	  start_recv = 1;
	  hal_uart_status_t status = HAL_UartReceiveNonBlocking(&uart5_handle[0], &recv_data, 1);
//	  hal_uart_status_t status = HAL_UartReceiveBlocking(&uart5_handle[0], &recv_data, 1);
	  if (status != kStatus_HAL_UartSuccess)
	  {
		  log_e(LOG_TAG, "HAL_UartReceiveBlocking failed:%d!", status);
		  return BLT_FALSE;
	  }
	  return BLT_FALSE;
  }

  if (start_recv == 1 && recv_done == 1)
  {
	  start_recv = 0;
	  recv_done = 0;
	  *data = recv_data;
	  return BLT_TRUE;
  } else
  {
	  return BLT_FALSE;
  }

  return result;
} /*** end of Rs232ReceiveByte ***/


/************************************************************************************//**
** \brief     Transmits a communication interface byte.
** \param     data Value of byte that is to be transmitted.
** \return    none.
**
****************************************************************************************/
void Rs232TransmitByte(blt_int8u data)
{
  /* TODO ##Port Write the byte value in 'data' to the transmit register of the UART 
   * peripheral such that the transmission of the byte value is started.
   */
  
  /* TODO ##Port Wait in a loop, with timeout, until the UART peripheral reports that the
   * data was successfully completed. This is typically done by reading out a transmit
   * register empty flag.
   */
  hal_uart_status_t status = HAL_UartSendBlocking(&uart5_handle[0], &data, 1);
  if (status != kStatus_HAL_UartSuccess)
  {
	  while(1);
  }
  CopService();
} /*** end of Rs232TransmitByte ***/
#endif /* BOOT_COM_RS232_ENABLE > 0 */


/*********************************** end of rs232.c ************************************/
