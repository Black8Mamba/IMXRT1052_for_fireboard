/*
 * bsp_key.c
 *
 *  Created on: 2024年12月15日
 *      Author: yongjie
 */
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "bsp_key.h"

static GPIO_Type *const s_gpioBases[] = GPIO_BASE_PTRS;

uint8_t Key_Scan(GPIO_Type* base, uint32_t pin)
{
  if(KEY_ON == GPIO_PinRead(base, pin))
  {
    while(KEY_ON == GPIO_PinRead(base, pin));
    return  KEY_ON;
  }
  else
    return KEY_OFF;
}

static uint32_t GPIO_GetInstance(GPIO_Type *base)
{
    uint32_t instance;

    for (instance = 0; instance < sizeof(s_gpioBases)/sizeof(s_gpioBases[0]); instance++)
    {
        if (s_gpioBases[instance] == base)
        {
            break;
        }
    }
    assert(instance < ARRAY_SIZE(s_gpioBases));
    return instance;
}

Key_State Key_Scan_Machine(GPIO_Type* base, uint32_t pin)
{
  static Key_State state[5][16] ={ KEY_NOT_PRESS };
  static uint32_t press_count[5][16] = { 0 };
  static uint32_t filtering_count[5][16] = { 0 };

  switch(state[GPIO_GetInstance(base)-1][pin])
  {
    case KEY_PRESS_RELEASE:
    case KEY_LONG_PRESS_RELEASE:
      state[GPIO_GetInstance(base)-1][pin] = KEY_NOT_PRESS;
      break;

    case KEY_NOT_PRESS:
        if(KEY_ON == GPIO_PinRead(base, pin))
        {
          __asm("NOP"); /* delay */

          if(filtering_count[GPIO_GetInstance(base)-1][pin]++ > KEY_FILTERING_COUNT)
          {
            state[GPIO_GetInstance(base)-1][pin] = KEY_PRESSING;
            filtering_count[GPIO_GetInstance(base)-1][pin] = 0;
          }
        }
      break;

    case KEY_PRESSING:
        if(KEY_OFF == GPIO_PinRead(base, pin))
        {
          __asm("NOP"); /* delay */

          if(filtering_count[GPIO_GetInstance(base)-1][pin]++ > KEY_FILTERING_COUNT)
          {
            state[GPIO_GetInstance(base)-1][pin] = KEY_PRESS_RELEASE;
            filtering_count[GPIO_GetInstance(base)-1][pin] = 0;
          }
        }
        else if(KEY_ON == GPIO_PinRead(base, pin))
        {
          __asm("NOP"); /* delay */

          if(press_count[GPIO_GetInstance(base)-1][pin]++ > KEY_LONG_PRESS_COUNT)
          {
            state[GPIO_GetInstance(base)-1][pin] = KEY_LONG_PRESSING;
            press_count[GPIO_GetInstance(base)-1][pin] = 0;
          }
        }
      break;

    case KEY_LONG_PRESSING:
        if(KEY_OFF == GPIO_PinRead(base, pin))
        {
          __asm("NOP"); /* delay */

          if(filtering_count[GPIO_GetInstance(base)-1][pin]++ > KEY_FILTERING_COUNT)
          {
            state[GPIO_GetInstance(base)-1][pin] = KEY_LONG_PRESS_RELEASE;
            filtering_count[GPIO_GetInstance(base)-1][pin] = 0;
          }
        }
      break;
  }

  return state[GPIO_GetInstance(base)-1][pin];
}
