#ifndef PC
/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include "fpioa.h"
#include "lcd.h"
#include "sysctl.h"
#include "nt35310.h"
#include "board_config.h"
#include <unistd.h>
#include <bsp.h>
#include "3dmain.hpp"
#include "gpiohs.h"
#include "3dconfig.hpp"
#include "fileio.h"
#include "sdcard.h"

//uint32_t g_lcd_gram[LCD_X_MAX * LCD_Y_MAX / 2] __attribute__((aligned(128)));

//for pc implementation:)
int filopen(const char *pathname,fil *f){
  return f_open(f,pathname,FA_READ);
}

int filread(fil *fp,void *buff,size_t byte){
  unsigned int ra;
  FRESULT r;
  r = f_read(fp,buff,byte,&ra);
  if(r)return 0/*fail*/;
  return ra;
}

static void io_set_power(void)
{
#if BOARD_LICHEEDAN
    sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);
#else
    sysctl_set_power_mode(SYSCTL_POWER_BANK1, SYSCTL_POWER_V18);
#endif
}

// overclock and voltageboost suported XD
//use  to configure core voltage.
#define CORE_VOLTAGE_GPIONUM (5)
int set_cpu_freq(uint32_t f){//MHz
  if(f<600){
    gpiohs_set_drive_mode(CORE_VOLTAGE_GPIONUM, GPIO_DM_INPUT);
    gpiohs_set_pin(CORE_VOLTAGE_GPIONUM, GPIO_PV_LOW);
  }else{
    gpiohs_set_drive_mode(CORE_VOLTAGE_GPIONUM, GPIO_DM_INPUT);
  }
  //Wait for voltage setting done.
  for(volatile int i=0;i<1000;i++);
#define MHz *1000000
  return sysctl_cpu_set_freq(f MHz)/1000000;
#undef MHz  
}

static void io_mux_init(void)
{
#if BOARD_LICHEEDAN
    fpioa_set_function(27, FUNC_SPI1_SCLK);
    fpioa_set_function(28, FUNC_SPI1_D0);
    fpioa_set_function(26, FUNC_SPI1_D1);
    fpioa_set_function(29, FUNC_GPIOHS7);

    fpioa_set_function(11, FUNC_GPIOHS0 + CORE_VOLTAGE_GPIONUM);
    fpioa_set_function(38, FUNC_GPIOHS0 + DCX_GPIONUM);
    fpioa_set_function(36, FUNC_SPI0_SS3);
    fpioa_set_function(39, FUNC_SPI0_SCLK);
    fpioa_set_function(37, FUNC_GPIOHS0 + RST_GPIONUM);
    sysctl_set_spi0_dvp_data(1);
#else
#error todo
    fpioa_set_function(8, FUNC_GPIOHS0 + DCX_GPIONUM);
    fpioa_set_function(6, FUNC_SPI0_SS3);
    fpioa_set_function(7, FUNC_SPI0_SCLK);
    sysctl_set_spi0_dvp_data(1);
#endif
}

uint64_t get_time(void)
{
    uint64_t v_cycle = read_cycle();
    return v_cycle * 1000000 / sysctl_clock_get_freq(SYSCTL_CLOCK_CPU);
}

int core1_function(void *ctx)
{
    uint64_t core = current_coreid();
    printf("Core %ld Hello world\n", core);
    vTask((void*)0);
    while(1);
}

int main(void)
{
  FATFS sdfs;
    io_mux_init();
#if BOARD_LICHEEDAN
 #if defined(OVER_VOLTAGE)
    gpiohs_set_drive_mode(CORE_VOLTAGE_GPIONUM, GPIO_DM_INPUT);
    gpiohs_set_pin(CORE_VOLTAGE_GPIONUM, GPIO_PV_LOW);
  //overclock settings, edit if you want
  #define MHz *1000000
    usleep(100000);
    sysctl_cpu_set_freq(600 MHz);
    usleep(100000);
  #undef MHz
 #endif
    io_set_power();
    lcd_init();
    lcd_set_direction(DIR_YX_RLDU);
#else
    lcd_set_direction(DIR_YX_RLUD);
#endif
    uint64_t core = current_coreid();
    printf("Core %ld Hello world\n", core);
    // make another thread\n
#if PROCESSNUM==2
    printf("core 1 registering...\n");
    register_core1(core1_function, 0);
    for(volatile int i=0;i<10000;i++);
#endif

    /* SD card init */
    if (sd_init())
      {
        printf("Fail to init SD card\n");
        return -1;
      }

    /* mount file system to SD card */
    if (f_mount(&sdfs, _T("0:"), 1))
      {
        printf("Fail to mount file system\n");
        return -1;
      }

    /* system start */
    printf("system start\n");

    main3d("model.pmd","motion.vmd");
    while(1);
}
#endif
