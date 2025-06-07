/*
 * bsp_phy.c
 *
 *  Created on: 2025年3月22日
 *      Author: yongjie
 */

#include <stdio.h>
#include "fsl_debug_console.h"
#include "board.h"
#include "bsp_phy.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "bsp_led.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief Defines the timeout macro. */
#define PHY_TIMEOUT_COUNT 0x3FFFFFFU

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*!
 * @brief Get the ENET instance from peripheral base address.
 *
 * @param base ENET peripheral base address.
 * @return ENET instance.
 */
extern uint32_t ENET_GetInstance(ENET_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/*! @brief Pointers to enet clocks for each instance. */
//extern clock_ip_name_t s_enetClock[FSL_FEATURE_SOC_ENET_COUNT];
extern const clock_ip_name_t s_enetClock[];
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Code
 ******************************************************************************/

status_t PHY_Init(ENET_Type *base, uint32_t phyAddr, uint32_t srcClock_Hz)
{
    uint32_t bssReg;
    uint32_t counter = PHY_TIMEOUT_COUNT;
    uint32_t idReg = 0;
    status_t result = kStatus_Success;
    uint32_t instance = ENET_GetInstance(base);
    uint32_t timeDelay;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Set SMI first. */
    CLOCK_EnableClock(s_enetClock[instance]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
    ENET_SetSMI(base, srcClock_Hz, false);

    /* Initialization after PHY stars to work. */
    while ((idReg != PHY_CONTROL_ID1) && (counter != 0))
    {
        PHY_Read(base, phyAddr, PHY_ID1_REG, &idReg);
        counter --;
    }

    if (!counter)
    {
        return kStatus_Fail;
    }

    /* Reset PHY. */
    counter = PHY_TIMEOUT_COUNT;
    result = PHY_Write(base, phyAddr, PHY_BASICCONTROL_REG, PHY_BCTL_RESET_MASK);
    if (result == kStatus_Success)
    {

        /* Set the negotiation. */
        result = PHY_Write(base, phyAddr, PHY_AUTONEG_ADVERTISE_REG,
                           (PHY_100BASETX_FULLDUPLEX_MASK | PHY_100BASETX_HALFDUPLEX_MASK |
                            PHY_10BASETX_FULLDUPLEX_MASK | PHY_10BASETX_HALFDUPLEX_MASK | 0x1U));
        if (result == kStatus_Success)
        {
            result = PHY_Write(base, phyAddr, PHY_BASICCONTROL_REG,
                               (PHY_BCTL_AUTONEG_MASK | PHY_BCTL_RESTART_AUTONEG_MASK));
            if (result == kStatus_Success)
            {
                /* Check auto negotiation complete. */
                while (counter --)
                {
                    result = PHY_Read(base, phyAddr, PHY_BASICSTATUS_REG, &bssReg);
                    if ( result == kStatus_Success)
                    {
                        if ((bssReg & PHY_BSTATUS_AUTONEGCOMP_MASK) != 0)
                        {
                            /* Wait a moment for Phy status stable. */
                            for (timeDelay = 0; timeDelay < PHY_TIMEOUT_COUNT; timeDelay ++)
                            {
                                __ASM("nop");
                            }
                            break;
                        }
                    }

                    if (!counter)
                    {
                        return kStatus_PHY_AutoNegotiateFail;
                    }
                }
            }
        }
    }

    return result;
}

status_t PHY_Write(ENET_Type *base, uint32_t phyAddr, uint32_t phyReg, uint32_t data)
{
    uint32_t counter;

    /* Clear the SMI interrupt event. */
    ENET_ClearInterruptStatus(base, ENET_EIR_MII_MASK);

    /* Starts a SMI write command. */
    ENET_StartSMIWrite(base, phyAddr, phyReg, kENET_MiiWriteValidFrame, data);

    /* Wait for SMI complete. */
    for (counter = PHY_TIMEOUT_COUNT; counter > 0; counter--)
    {
        if (ENET_GetInterruptStatus(base) & ENET_EIR_MII_MASK)
        {
            break;
        }
    }

    /* Check for timeout. */
    if (!counter)
    {
        return kStatus_PHY_SMIVisitTimeout;
    }

    /* Clear MII interrupt event. */
    ENET_ClearInterruptStatus(base, ENET_EIR_MII_MASK);

    return kStatus_Success;
}

status_t PHY_Read(ENET_Type *base, uint32_t phyAddr, uint32_t phyReg, uint32_t *dataPtr)
{
    assert(dataPtr);

    uint32_t counter;

    /* Clear the MII interrupt event. */
    ENET_ClearInterruptStatus(base, ENET_EIR_MII_MASK);

    /* Starts a SMI read command operation. */
    ENET_StartSMIRead(base, phyAddr, phyReg, kENET_MiiReadValidFrame);

    /* Wait for MII complete. */
    for (counter = PHY_TIMEOUT_COUNT; counter > 0; counter--)
    {
        if (ENET_GetInterruptStatus(base) & ENET_EIR_MII_MASK)
        {
            break;
        }
    }

    /* Check for timeout. */
    if (!counter)
    {
        return kStatus_PHY_SMIVisitTimeout;
    }

    /* Get data from MII register. */
    *dataPtr = ENET_ReadSMIData(base);

    /* Clear MII interrupt event. */
    ENET_ClearInterruptStatus(base, ENET_EIR_MII_MASK);

    return kStatus_Success;
}

status_t PHY_EnableLoopback(ENET_Type *base, uint32_t phyAddr, phy_loop_t mode, phy_speed_t speed, bool enable)
{
    status_t result;
    uint32_t data = 0;

    /* Set the loop mode. */
    if (enable)
    {
        if (mode == kPHY_LocalLoop)
        {
            if (speed == kPHY_Speed100M)
            {
                data = PHY_BCTL_SPEED_100M_MASK | PHY_BCTL_DUPLEX_MASK | PHY_BCTL_LOOP_MASK;
            }
            else
            {
                data = PHY_BCTL_DUPLEX_MASK | PHY_BCTL_LOOP_MASK;
            }
           return PHY_Write(base, phyAddr, PHY_BASICCONTROL_REG, data);
        }
        else
        {
            /* First read the current status in control register. */
            result = PHY_Read(base, phyAddr, PHY_CONTROL1_REG, &data);
            if (result == kStatus_Success)
            {
                return PHY_Write(base, phyAddr, PHY_CONTROL1_REG, (data | PHY_CTL1_REMOTELOOP_MASK));
            }
        }
    }
    else
    {
        /* Disable the loop mode. */
        if (mode == kPHY_LocalLoop)
        {
            /* First read the current status in control register. */
            result = PHY_Read(base, phyAddr, PHY_BASICCONTROL_REG, &data);
            if (result == kStatus_Success)
            {
                data &= ~PHY_BCTL_LOOP_MASK;
                return PHY_Write(base, phyAddr, PHY_BASICCONTROL_REG, (data | PHY_BCTL_RESTART_AUTONEG_MASK));
            }
        }
        else
        {
            /* First read the current status in control one register. */
            result = PHY_Read(base, phyAddr, PHY_CONTROL1_REG, &data);
            if (result == kStatus_Success)
            {
                return PHY_Write(base, phyAddr, PHY_CONTROL1_REG, (data & ~PHY_CTL1_REMOTELOOP_MASK));
            }
        }
    }
    return result;
}

status_t PHY_GetLinkStatus(ENET_Type *base, uint32_t phyAddr, bool *status)
{
    assert(status);

    status_t result = kStatus_Success;
    uint32_t data = 0;

    /* Read the basic status register. */
    result = PHY_Read(base, phyAddr, PHY_BASICSTATUS_REG, &data);
    if (result == kStatus_Success)
    {
        if (!(PHY_BSTATUS_LINKSTATUS_MASK & data))
        {
        	PRINTF("link down!\r\n");
            /* link down. */
            *status = false;
        }
        else
        {
        	PRINTF("link up!\r\n");
            /* link up. */
            *status = true;
        }
    }
    return result;
}

status_t PHY_GetLinkSpeedDuplex(ENET_Type *base, uint32_t phyAddr, phy_speed_t *speed, phy_duplex_t *duplex)
{
    assert(duplex);

    status_t result = kStatus_Success;
    uint32_t data, ctlReg;

    /* Read the control two register. */
    result = PHY_Read(base, phyAddr, PHY_CONTROL1_REG, &ctlReg);
    if (result == kStatus_Success)
    {
        data = ctlReg & PHY_CTL1_SPEEDUPLX_MASK;
        if ((PHY_CTL1_10FULLDUPLEX_MASK == data) || (PHY_CTL1_100FULLDUPLEX_MASK == data))
        {
            /* Full duplex. */
            *duplex = kPHY_FullDuplex;
        }
        else
        {
            /* Half duplex. */
            *duplex = kPHY_HalfDuplex;
        }

        data = ctlReg & PHY_CTL1_SPEEDUPLX_MASK;
        if ((PHY_CTL1_100HALFDUPLEX_MASK == data) || (PHY_CTL1_100FULLDUPLEX_MASK == data))
        {
            /* 100M speed. */
            *speed = kPHY_Speed100M;
        }
        else
        { /* 10M speed. */
            *speed = kPHY_Speed10M;
        }
    }

    return result;
}

void static_delay(void)
{
    volatile uint32_t i = 0;
    for (i = 0; i < 1000000; ++i)
    {
        __asm("NOP"); /* delay */
    }
}

enet_handle_t net_handle;
#define ENET_RING_NUM 1U
#define ENET_TXBD_NUM (3)
#define ENET_RXBD_NUM (5)
#define ENET_FRAME_MAX_FRAMELEN 1518U /*!< Default maximum Ethernet frame size without VLAN tag. */
#ifndef ENET_RXBUFF_SIZE
    #define ENET_RXBUFF_SIZE (ENET_FRAME_MAX_FRAMELEN)
#endif
#ifndef ENET_TXBUFF_SIZE
    #define ENET_TXBUFF_SIZE (ENET_FRAME_MAX_FRAMELEN)
#endif
#define FSL_ENET_BUFF_ALIGNMENT ENET_BUFF_ALIGNMENT
typedef uint8_t rx_buffer_t[SDK_SIZEALIGN(ENET_RXBUFF_SIZE, FSL_ENET_BUFF_ALIGNMENT)];
typedef uint8_t tx_buffer_t[SDK_SIZEALIGN(ENET_TXBUFF_SIZE, FSL_ENET_BUFF_ALIGNMENT)];

enet_buffer_config_t buffCfg[ENET_RING_NUM];

AT_NONCACHEABLE_SECTION_ALIGN(static enet_rx_bd_struct_t rxBuffDescrip_0[ENET_RXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
AT_NONCACHEABLE_SECTION_ALIGN(static enet_tx_bd_struct_t txBuffDescrip_0[ENET_TXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
SDK_ALIGN(static rx_buffer_t rxDataBuff_0[ENET_RXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
SDK_ALIGN(static tx_buffer_t txDataBuff_0[ENET_TXBD_NUM], FSL_ENET_BUFF_ALIGNMENT);
uint8_t mac_address[] = {0x1,0x2,0x3,0x4,0x5,0x6};

void enet_init(void)
{
	uint32_t sysClock = 0;
	sysClock = CLOCK_GetFreq(kCLOCK_CoreSysClk);
	IOMUXC_EnableMode(IOMUXC_GPR, kIOMUXC_GPR_ENET1TxClkOutputDir, true);
    gpio_pin_config_t gpio_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
    GPIO_PinInit(GPIO1, 9, &gpio_config);
    GPIO_PinInit(GPIO1, 10, &gpio_config);
    /* pull up the ENET_INT before RESET. */
    GPIO_WritePinOutput(GPIO1, 10, 1);
    GPIO_WritePinOutput(GPIO1, 9, 0);
    static_delay();
    GPIO_WritePinOutput(GPIO1, 9, 1);

    enet_config_t config;
    ENET_GetDefaultConfig(&config);

	PRINTF("%s:%d sysClock:%d, %d\r\n", __func__, __LINE__, sysClock, CLOCK_GetFreq(kCLOCK_IpgClk));
//#define EXAMPLE_CLOCK_FREQ   CLOCK_GetFreq(kCLOCK_IpgClk)
	status_t status = PHY_Init(ENET, BOARD_ENET0_PHY_ADDRESS, CLOCK_GetFreq(kCLOCK_IpgClk));
    if (kStatus_Success != status)
    {
    	PRINTF("Cannot initialize PHY.\r\n");
    	return;
    }

    bool link = false;
    uint32_t count = 0;
    phy_speed_t speed;
    phy_duplex_t duplex;

    while ((count < 0xFFFU) && (!link))
    {
        status =  PHY_GetLinkStatus(ENET, BOARD_ENET0_PHY_ADDRESS, &link);
        if (kStatus_Success != status)
        {
        	PRINTF("Cannot PHY_GetLinkStatus PHY.\r\n");
        	return;
        }

        if (link)
        {
        	PRINTF("link up!\r\n");
            /* Get the actual PHY link speed. */
            status = PHY_GetLinkSpeedDuplex(ENET, BOARD_ENET0_PHY_ADDRESS, &speed, &duplex);
            if (kStatus_Success != status)
            {
            	PRINTF("\r\nCannot PHY_GetLinkSpeedDuplex PHY.\r\n");
            	return;
            }
            /* Change the MII speed and duplex for actual link status. */
            config.miiSpeed = (enet_mii_speed_t)speed;
            config.miiDuplex = (enet_mii_duplex_t)duplex;
            PRINTF("speed:%d, duplex:%d\n", config.miiSpeed, config.miiDuplex);
            if (speed == kPHY_Speed100M)
            {
            	PRINTF("kPHY_Speed100M\r\n");
            } else
            {
            	PRINTF("kPHY_Speed10M\r\n");
            }

            if (duplex == kPHY_FullDuplex)
            {
            	PRINTF("kPHY_FullDuplex\r\n");
            } else
            {
            	PRINTF("kPHY_HalfDuplex\r\n");
            }
        }

        count++;
    }

    /* prepare the buffer configuration. */
    buffCfg[0].rxBdNumber = ENET_RXBD_NUM;                      /* Receive buffer descriptor number. */
    buffCfg[0].txBdNumber = ENET_TXBD_NUM;                      /* Transmit buffer descriptor number. */
    buffCfg[0].rxBuffSizeAlign = sizeof(rx_buffer_t);           /* Aligned receive data buffer size. */
    buffCfg[0].txBuffSizeAlign = sizeof(tx_buffer_t);           /* Aligned transmit data buffer size. */
    buffCfg[0].rxBdStartAddrAlign = &(rxBuffDescrip_0[0]); /* Aligned receive buffer descriptor start address. */
    buffCfg[0].txBdStartAddrAlign = &(txBuffDescrip_0[0]); /* Aligned transmit buffer descriptor start address. */
    buffCfg[0].rxBufferAlign = &(rxDataBuff_0[0][0]); /* Receive data buffer start address. */
    buffCfg[0].txBufferAlign =  &(txDataBuff_0[0][0]); /* Transmit data buffer start address. */

    /* Initialize the ENET module.*/
    status = ENET_Init(ENET, &net_handle, &config, &buffCfg[0], mac_address, CLOCK_GetFreq(kCLOCK_IpgClk));
    if (status != kStatus_Success)
    {
    	PRINTF("ENET_Init failed!\r\n");
    }

    ENET_ActiveRead(ENET);
}
