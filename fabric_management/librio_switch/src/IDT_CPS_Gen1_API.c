/*
****************************************************************************
Copyright (c) 2014, Integrated Device Technology Inc.
Copyright (c) 2014, RapidIO Trade Association
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*************************************************************************
*/
#include <DAR_DB_Private.h>
#include <IDT_DSF_DB_Private.h>
#include <IDT_CPS_Gen2_API.h>
#include <IDT_Routing_Table_Config_API.h>
#include <IDT_Port_Config_API.h>
#include <IDT_Error_Management_API.h>
#include <IDT_CPS_Common_Test.h>
#include <CPS1848_registers.h>
#include <CPS1616_registers.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static DSF_Handle_t cpsgen1_driver_handle;
static uint32_t num_cpsgen1_driver_instances;

uint32_t IDT_CPSGEN1_DeviceSupported( DAR_DEV_INFO_t *dev_info )
{
    uint32_t rc = DAR_DB_NO_DRIVER;

    if ( IDT_CPS_VENDOR_ID == ( DECODE_VENDOR_ID( dev_info->devID ) ) )
    {
        if ( IDT_CPS8_DEV_ID == ( DECODE_DEVICE_ID( dev_info->devID ) ) )
        {
	    strcpy(dev_info->name, "CPS8");
            dev_info->dsf_h = cpsgen1_driver_handle ;
            dev_info->assyInfo = 0x100 ;
            dev_info->devInfo = 0 ;
            dev_info->srcOps = 0x4 ;
            dev_info->dstOps = 0 ;
            dev_info->features = 0x18000779 ;
            dev_info->extFPtrPortType = 0x9;
            dev_info->extFPtrForErr = 0 ;
            dev_info->extFPtrForLane = 0 ;
            dev_info->extFPtrForPort = 0x100 ;
            dev_info->extFPtrForVC = 0 ;
            dev_info->extFPtrForVOQ = 0 ;
            dev_info->swRtInfo = 0x00FF ;
            dev_info->swPortInfo = 0x1000 ;
            dev_info->swMcastInfo = 0x01000280 ;

            rc = RIO_SUCCESS ;
            num_cpsgen1_driver_instances++ ;
        }
        else if ( IDT_CPS12_DEV_ID == ( DECODE_DEVICE_ID( dev_info->devID ) ) )
        {
	    strcpy(dev_info->name, "CPS12");
            dev_info->dsf_h = cpsgen1_driver_handle ;
            dev_info->assyInfo = 0x100 ;
            dev_info->devInfo = 0 ;
            dev_info->srcOps = 0x4 ;
            dev_info->dstOps = 0 ;
            dev_info->features = 0x18000779 ;
            dev_info->extFPtrPortType = 0x9;
            dev_info->extFPtrForErr = 0 ;
            dev_info->extFPtrForLane = 0 ;
            dev_info->extFPtrForPort = 0x100 ;
            dev_info->extFPtrForVC = 0 ;
            dev_info->extFPtrForVOQ = 0 ;
            dev_info->swRtInfo = 0x00FF ;
            dev_info->swPortInfo = 0x1000 ;
            dev_info->swMcastInfo = 0x01000280 ;

            rc = RIO_SUCCESS ;
            num_cpsgen1_driver_instances++ ;
        }
        else if ( IDT_CPS16_DEV_ID == ( DECODE_DEVICE_ID( dev_info->devID ) ) )
        {
	    strcpy(dev_info->name, "CPS16");
            dev_info->dsf_h = cpsgen1_driver_handle ;
            dev_info->assyInfo = 0x100 ;
            dev_info->devInfo = 0 ;
            dev_info->srcOps = 0x4 ;
            dev_info->dstOps = 0 ;
            dev_info->features = 0x18000779 ;
            dev_info->extFPtrPortType = 0x9;
            dev_info->extFPtrForErr = 0 ;
            dev_info->extFPtrForLane = 0 ;
            dev_info->extFPtrForPort = 0x100 ;
            dev_info->extFPtrForVC = 0 ;
            dev_info->extFPtrForVOQ = 0 ;
            dev_info->swRtInfo = 0x00FF ;
            dev_info->swPortInfo = 0x1000 ;
            dev_info->swMcastInfo = 0x01000280 ;

            rc = RIO_SUCCESS ;
            num_cpsgen1_driver_instances++ ;
        }
        else if ( IDT_CPS6Q_DEV_ID == ( DECODE_DEVICE_ID( dev_info->devID ) ) )
        {
	    strcpy(dev_info->name, "CPS6Q");
            dev_info->dsf_h = cpsgen1_driver_handle ;
            dev_info->assyInfo = 0x100 ;
            dev_info->devInfo = 0 ;
            dev_info->srcOps = 0x4 ;
            dev_info->dstOps = 0 ;
            dev_info->features = 0x18000779 ;
            dev_info->extFPtrPortType = 0x9;
            dev_info->extFPtrForErr = 0 ;
            dev_info->extFPtrForLane = 0 ;
            dev_info->extFPtrForPort = 0x100 ;
            dev_info->extFPtrForVC = 0 ;
            dev_info->extFPtrForVOQ = 0 ;
            dev_info->swRtInfo = 0x00FF ;
            dev_info->swPortInfo = 0x1000 ;
            dev_info->swMcastInfo = 0x01000280 ;

            rc = RIO_SUCCESS ;
            num_cpsgen1_driver_instances++ ;
        }
        else if ( IDT_CPS10Q_DEV_ID == ( DECODE_DEVICE_ID( dev_info->devID ) ) )
        {
	    strcpy(dev_info->name, "CPS10Q");
            dev_info->dsf_h = cpsgen1_driver_handle ;
            dev_info->assyInfo = 0x100 ;
            dev_info->devInfo = 0 ;
            dev_info->srcOps = 0x4 ;
            dev_info->dstOps = 0 ;
            dev_info->features = 0x18000779 ;
            dev_info->extFPtrPortType = 0x9;
            dev_info->extFPtrForErr = 0 ;
            dev_info->extFPtrForLane = 0 ;
            dev_info->extFPtrForPort = 0x100 ;
            dev_info->extFPtrForVC = 0 ;
            dev_info->extFPtrForVOQ = 0 ;
            dev_info->swRtInfo = 0x00FF ;
            dev_info->swPortInfo = 0x1000 ;
            dev_info->swMcastInfo = 0x01000280 ;

            rc = RIO_SUCCESS ;
            num_cpsgen1_driver_instances++ ;
        }
        else if ( DECODE_DEVICE_ID( dev_info->devID ) == 0 )
        {
            /* Now fill out the DAR_info structure... */
            rc = DARDB_rioDeviceSupportedDefault( dev_info );
	    strcpy(dev_info->name, "BadCPSGen1");

            /* Index and information for DSF is the same as the DAR handle */
            dev_info->dsf_h = cpsgen1_driver_handle ;

            if ( rc == RIO_SUCCESS )
                num_cpsgen1_driver_instances++ ;
        }
    }

    return rc;
}

uint32_t bind_CPSGEN1_DSF_support( void )
{
    IDT_DSF_DB_t idt_driver;
    
    idt_driver.dev_type = 0x0300;

    idt_driver.idt_pc_clr_errs           = IDT_CPS_pc_clr_errs;
    idt_driver.idt_pc_dev_reset_config   = IDT_CPS_pc_dev_reset_config;
    idt_driver.idt_pc_get_config         = IDT_CPS_pc_get_config;
    idt_driver.idt_pc_get_status         = IDT_CPS_pc_get_status;
    idt_driver.idt_pc_reset_link_partner = IDT_CPS_pc_reset_link_partner;
    idt_driver.idt_pc_reset_port         = IDT_CPS_pc_reset_port;
    idt_driver.idt_pc_secure_port        = IDT_CPS_pc_secure_port;
    idt_driver.idt_pc_set_config         = IDT_CPS_pc_set_config;

    idt_driver.idt_rt_initialize      = IDT_CPS_rt_initialize;
    idt_driver.idt_rt_probe           = IDT_CPS_rt_probe;
    idt_driver.idt_rt_probe_all       = IDT_CPS_rt_probe_all;
    idt_driver.idt_rt_set_all         = IDT_CPS_rt_set_all;
    idt_driver.idt_rt_set_changed     = IDT_CPS_rt_set_changed;
    idt_driver.idt_rt_alloc_mc_mask   = IDT_DSF_rt_alloc_mc_mask;
    idt_driver.idt_rt_dealloc_mc_mask = IDT_DSF_rt_dealloc_mc_mask;
    idt_driver.idt_rt_change_rte      = IDT_CPS_rt_change_rte;
    idt_driver.idt_rt_change_mc_mask  = IDT_CPS_rt_change_mc_mask;

    idt_driver.idt_em_cfg_pw       = IDT_CPS_em_cfg_pw       ;
    idt_driver.idt_em_cfg_set      = IDT_CPS_em_cfg_set      ;
    idt_driver.idt_em_cfg_get      = IDT_CPS_em_cfg_get      ;
    idt_driver.idt_em_dev_rpt_ctl  = IDT_CPS_em_dev_rpt_ctl  ;
    idt_driver.idt_em_parse_pw     = IDT_CPS_em_parse_pw     ;
    idt_driver.idt_em_get_int_stat = IDT_CPS_em_get_int_stat ;
    idt_driver.idt_em_get_pw_stat  = IDT_CPS_em_get_pw_stat  ;
    idt_driver.idt_em_clr_events   = IDT_CPS_em_clr_events   ;
    idt_driver.idt_em_create_events= IDT_CPS_em_create_events;

    IDT_DSF_bind_driver( &idt_driver, &cpsgen1_driver_handle );

    return RIO_SUCCESS;
}

uint32_t bind_CPSGEN1_DAR_support( void )
{
    DAR_DB_Driver_t DB_info;

    /* Now bind the device driver... */
    DARDB_Init_Driver_Info( IDT_CPS_VENDOR_ID, &DB_info ) ;

    DB_info.rioDeviceSupported = IDT_CPSGEN1_DeviceSupported ;

    DARDB_Bind_Driver( &DB_info ) ;

    return RIO_SUCCESS;
}


#ifdef __cplusplus
}
#endif

