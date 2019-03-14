/**
  *@file spartan6.c
  *@brief Module for configuring a spartan 6 FPGA
  *@author Jason Berger
  *@date 03/14/2019
  *
  *@Datasheet https://www.xilinx.com/support/documentation/user_guides/ug380.pdf
  */

#include "spartan6.h"

mrt_status_t spartan6_init(spartan6_cfg_t* dev )
{
  dev->mState = SPARTAN_IDLE;

  return MRT_STATUS_OK;
}


mrt_status_t spartan6_start_config(spartan6_cfg_t* dev)
{

  //make sure clock is low and assert the ProgamB/Reset line
  MRT_GPIO_WRITE(dev->mCfgClk, LOW);
  MRT_GPIO_WRITE(dev->mProgramB, LOW);

  MRT_DELAY_MS(10);

  //release reset
  MRT_GPIO_WRITE(dev->mProgramB, HIGH);

  //wait for device to indicate it is ready
  while(MRT_GPIO_READ(dev->mInitB) == LOW)
  {
    MRT_DELAY_MS(1);
  }

  dev->mState = SPARTAN_CONFIGURING;

  return MRT_STATUS_OK;
}


mrt_status_t spartan6_send_config(spartan6_cfg_t* dev, uint8_t* data, int len)
{
  //create mask by shifting 0xFF to data offset
  uint32_t mask = 0xFF << dev->mDataOffset;

  for(int i=0; i < len; i++)
  {
    //toggle clock low
    MRT_GPIO_WRITE(dev->mCfgClk,LOW);

    //write data to port
    MRT_GPIO_PORT_WRITE(dev->mDataPort, mask, data[i]);

    //toggle clock high (Data is samples on rising edge)
    MRT_GPIO_WRITE(dev->mCfgClk,HIGH);

  }

  return MRT_STATUS_OK;
}

spartan_state_e spartan6_finish_config(spartan6_cfg_t* dev)
{
  uint32_t timeout = 1024;

  uint8_t dummByte = 0xFF;

  while(MRT_GPIO_READ(dev->mCfgDone) == LOW)
	{
		spartan6_send_config(dev,&dummByte, 1);
		timeout--;
		if(timeout == 0)
			break;
	}

  if(MRT_GPIO_READ(dev->mCfgDone) == HIGH)
  {
    dev->mState = SPARTAN_DONE;
  }

  return dev->mState;
}
