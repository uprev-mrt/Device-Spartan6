/**
  *@file spartan6.h
  *@brief Module for configuring a spartan 6 FPGA
  *@author Jason Berger
  *@date 03/14/2019
  *
  *@Datasheet https://www.xilinx.com/support/documentation/user_guides/ug380.pdf
  */

#include <stdint.h>
#include "Platforms/Common/mrt_platform.h"


typedef enum {
  SPARTAN_IDLE,
  SPARTAN_CONFIGURING,
  SPARTAN_DONE
}spartan_state_e;

//for more information see page 25 in Datasheet
typedef struct{
  mrt_gpio_port_t mDataPort;      //Port for D0-D7
  uint8_t mDataOffset;            //Offset on port of D0
  mrt_gpio_t mInitB;              //init B pin
  mrt_gpio_t mProgramB;         //Active-Low asynchronous full-chip reset.
  mrt_gpio_t mCfgClk;           //Clock for bus
  mrt_gpio_t mCfgDone;            //signal indicating configuration was successful
  spartan_state_e mState;
}spartan6_cfg_t;

/**
  *@brief initialize spartan 6 device to configure with selectmap
  *@param dev ptr to spartan6 config struct
  *@pre dev must be configured
  *@return status
  */
mrt_status_t spartan6_init(spartan6_cfg_t* dev );

/**
  *@brief start configuration process of device
  *@param dev ptr to spartan6 config struct
  *@return status
  */
mrt_status_t spartan6_start_config(spartan6_cfg_t* dev);

/**
  *@brief loads data from bitsream into device
  *@param dev ptr to spartan6 config struct
  *@param data ptr to data being loaded
  *@param len number of bytes being sent
  *@pre spartan6_start_config must be called before the first call to this function
  *@return state of device
  */
mrt_status_t spartan6_send_config(spartan6_cfg_t* dev, uint8_t* data, int len);

/**
  *@brief finishes up the configuration by running extra clock cycles
  *@param dev ptr to spartan device
  *@pre Before calling this, all data in the bitstream should be sent with the spartan_send_config() function
  *@return state of device
  */
spartan_state_e spartan6_finish_config(spartan6_cfg_t* dev);
