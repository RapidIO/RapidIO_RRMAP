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
#include "IDT_Tsi721.h"
#include "IDT_Tsi721_API.h"
#include "DAR_DB.h"
#include "DAR_DB_Private.h"
#include "DAR_Utilities.h"
#include "IDT_Common.h"
#include "IDT_Port_Config_API.h"
#include "IDT_Routing_Table_Config_API.h"
#include "IDT_Error_Management_API.h"
#include "IDT_DSF_DB_Private.h"

#include "string_util.h"

#ifdef __cplusplus
extern "C" {
#endif

// CHANGES
//
// Check port width computation for get_config and set_config
// Check set_config with "all ports" as an input parameter.  Endless loop?
//

#define EM_UPDATE_RESET_0 EM_FIRST_SUBROUTINE_0

static DSF_Handle_t Tsi721_driver_handle;
static uint32_t num_Tsi721_driver_instances;

uint32_t IDT_tsi721ReadReg( DAR_DEV_INFO_t *dev_info,
                                uint32_t  offset,
                                uint32_t  *readdata )
{
        uint32_t rc = RIO_SUCCESS;

	switch(offset) {
	case RIO_SW_PORT_INF: *readdata = 0x00000100;
				break;
	case TSI721_RIO_SR_RSP_TO: *readdata = 0x00000100;
		rc = ReadReg(dev_info, offset, readdata);
		*readdata = (*readdata) << 8; 
				break;
	/* Never enable reliable port-write reception.  Ever. */
	case TSI721_RIO_PW_CTL:
		rc = ReadReg(dev_info, offset, readdata);
		*readdata &= ~TSI721_RIO_PW_CTL_PWC_MODE;
		break;
		
	default:
		rc = ReadReg(dev_info, offset, readdata);
	};

        return rc;
}

uint32_t IDT_tsi721WriteReg( DAR_DEV_INFO_t *dev_info,
                                uint32_t  offset,
                                uint32_t  writedata )
{
        uint32_t rc = RIO_SUCCESS;
	uint32_t temp_data;

	switch (offset) {
	/* Correct register errata in Tsi721 */
	case TSI721_RIO_SR_RSP_TO: writedata = writedata >> 8;
			rc = WriteReg(dev_info, offset, writedata);
			break;
	/* Only support 8 bit device IDs */
	case TSI721_RIO_BASE_ID:
			temp_data = (writedata & TSI721_RIO_BASE_ID_BASE_ID)
				>> 16;
			rc = WriteReg(dev_info, offset, writedata);
			if (rc)
				break;
			rc = WriteReg(dev_info, TSI721_IB_DEVID, temp_data);
			break;
	case TSI721_RIO_PW_CTL:
		/* Never enable reliable port-write reception.  Ever. */
		writedata &= ~TSI721_RIO_PW_CTL_PWC_MODE;
		rc = WriteReg(dev_info, offset, writedata);
		break;
	default:
		rc = WriteReg(dev_info, offset, writedata);
	}
	return rc;
};
// Routing table entry value to use when requesting
// default route or packet discard (no route)
#define HW_DFLT_RT 0xFF

uint32_t IDT_tsi721DeviceSupported( DAR_DEV_INFO_t *DAR_info )
{
    uint32_t rc = DAR_DB_NO_DRIVER;

    if ( TSI721_RIO_DEVICE_VENDOR ==  ( DAR_info->devID & RIO_DEV_IDENT_VEND ) )
    {
        if ( (TSI721_RIO_DEVICE_ID) == ( (DAR_info->devID & RIO_DEV_IDENT_DEVI) >> 16) )
        {
            /* Now fill out the DAR_info structure... */
            rc = DARDB_rioDeviceSupportedDefault( DAR_info );

            /* Index and information for DSF is the same as the DAR handle */
            DAR_info->dsf_h = Tsi721_driver_handle;

            if ( rc == RIO_SUCCESS ) {
                num_Tsi721_driver_instances++ ;
                SAFE_STRNCPY(DAR_info->name, "Tsi721", sizeof(DAR_info->name));
            }
        }
    }
    return rc;
}

uint32_t bind_tsi721_DAR_support( void )
{
    DAR_DB_Driver_t DAR_info;

    DARDB_Init_Driver_Info( IDT_TSI_VENDOR_ID, &DAR_info );
    DAR_info.WriteReg = IDT_tsi721WriteReg;
    DAR_info.ReadReg = IDT_tsi721ReadReg;

    DAR_info.rioDeviceSupported = IDT_tsi721DeviceSupported;

    DARDB_Bind_Driver( &DAR_info );
    
    return RIO_SUCCESS;
}

typedef struct spx_ctl2_ls_check_info_t_TAG {
   uint32_t      ls_en_val;
   uint32_t      ls_sup_val;
   idt_pc_ls_t ls;
   uint32_t      prescalar_srv_clk;
} spx_ctl2_ls_check_info_t;

spx_ctl2_ls_check_info_t ls_check[] = {
   { RIO_SPX_CTL2_GB_1p25_EN , RIO_SPX_CTL2_GB_1p25 , idt_pc_ls_1p25 , 13 },
   { RIO_SPX_CTL2_GB_2p5_EN  , RIO_SPX_CTL2_GB_2p5  , idt_pc_ls_2p5  , 13 },
   { RIO_SPX_CTL2_GB_3p125_EN, RIO_SPX_CTL2_GB_3p125, idt_pc_ls_3p125, 16 },
   { RIO_SPX_CTL2_GB_5p0_EN  , RIO_SPX_CTL2_GB_5p0  , idt_pc_ls_5p0  , 25 },
   { RIO_SPX_CTL2_GB_6p25_EN , RIO_SPX_CTL2_GB_6p25 , idt_pc_ls_6p25 , 31 }, 
   { 0x00000000          , 0x00000000           , idt_pc_ls_last ,  0 } 
};

uint32_t idt_tsi721_pc_get_config  ( DAR_DEV_INFO_t           *dev_info, 
                                   idt_pc_get_config_in_t   *in_parms, 
                                   idt_pc_get_config_out_t  *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint32_t port_idx, idx;
    bool   misconfigured = false;
    uint32_t plmCtl, spxCtl, devStat, spxCtl2;
    int32_t  lane_num;
    struct DAR_ptl good_ptl;

    out_parms->num_ports = 0;
    out_parms->imp_rc        = 0;

    rc = DARrioGetPortList(dev_info, &in_parms->ptl, &good_ptl);
    if (RIO_SUCCESS != rc) {
         out_parms->imp_rc = PC_SET_CONFIG(0x1);
         goto idt_tsi721_pc_get_config_exit;
    };

    out_parms->num_ports = good_ptl.num_ports;
    for (port_idx = 0; port_idx < good_ptl.num_ports; port_idx++)
        out_parms->pc[port_idx].pnum = good_ptl.pnums[port_idx];

   // Always get LRTO
   { uint32_t lrto;
      rc = DARRegRead( dev_info, TSI721_RIO_SP_LT_CTL, &lrto); 
	  if (RIO_SUCCESS != rc) {
         	out_parms->imp_rc = PC_SET_CONFIG(0x2);
         	goto idt_tsi721_pc_get_config_exit;
	  }
	  out_parms->lrto = lrto >> 8;
    };
	
    // Always get LOG_RTO
    { uint32_t log_rto;
    	  rc = DARRegRead(dev_info, TSI721_RIO_SR_RSP_TO, &log_rto);
	  if (RIO_SUCCESS != rc) {
         	out_parms->imp_rc = PC_SET_CONFIG(0x3);
         	goto idt_tsi721_pc_get_config_exit;
	  }
	  out_parms->log_rto = ((log_rto >> 8) * 188) / 100;
    }

    for (port_idx = 0; port_idx < out_parms->num_ports; port_idx++)
    {
        out_parms->pc[port_idx].port_available = true;
        out_parms->pc[port_idx].pw = idt_pc_pw_last;
        out_parms->pc[port_idx].ls = idt_pc_ls_last;
        out_parms->pc[port_idx].iseq = idt_pc_is_one;
        out_parms->pc[port_idx].fc = idt_pc_fc_rx;
        out_parms->pc[port_idx].xmitter_disable = false;
        out_parms->pc[port_idx].port_lockout = false;
        out_parms->pc[port_idx].nmtc_xfer_enable = false;
        out_parms->pc[port_idx].rx_lswap = false;
        out_parms->pc[port_idx].tx_lswap = false;
        for (lane_num = 0; lane_num < IDT_PC_MAX_LANES; lane_num++) {
           out_parms->pc[port_idx].tx_linvert[lane_num] = false;
           out_parms->pc[port_idx].rx_linvert[lane_num] = false;
        };

        // Check that the RapidIO port has been enabled...
        rc = DARRegRead( dev_info, TSI721_DEVCTL, &devStat );
        if (RIO_SUCCESS != rc) 
        {
           out_parms->imp_rc = PC_GET_CONFIG(5);
           goto idt_tsi721_pc_get_config_exit;
        };

        out_parms->pc[port_idx].powered_up = ((devStat & TSI721_DEVCTL_SRBOOT_CMPL) &&
			                                  (devStat & TSI721_DEVCTL_PCBOOT_CMPL))?true:false;

        if (!out_parms->pc[port_idx].powered_up)
           goto idt_tsi721_pc_get_config_exit;

        // Check that RapidIO transmitter is enabled...
        rc = DARRegRead( dev_info, TSI721_RIO_SP_CTL, &spxCtl );
        if (RIO_SUCCESS != rc) {
           out_parms->imp_rc = PC_GET_CONFIG(8);
           goto idt_tsi721_pc_get_config_exit;       
        };

        out_parms->pc[port_idx].xmitter_disable = 
            (spxCtl & TSI721_RIO_SP_CTL_PORT_DIS)?true:false;

        // OK, port is enabled so it can train.
        // Check for port width overrides...
        rc = DARRegRead( dev_info, TSI721_RIO_SP_CTL, &spxCtl );
        if (RIO_SUCCESS != rc) 
        {
           out_parms->imp_rc = PC_GET_CONFIG(0x10);
           goto idt_tsi721_pc_get_config_exit;
        };
        switch (spxCtl & RIO_SPX_CTL_PTW_OVER) {
           case RIO_SPX_CTL_PTW_OVER_4x_NO_2X : 
           case RIO_SPX_CTL_PTW_OVER_NONE_2   : 
           case RIO_SPX_CTL_PTW_OVER_NONE : out_parms->pc[port_idx].pw = idt_pc_pw_4x;
                                               break;
           case RIO_SPX_CTL_PTW_OVER_1x_L0: out_parms->pc[port_idx].pw = idt_pc_pw_1x_l0;
                                               break;
           case RIO_SPX_CTL_PTW_OVER_1x_LR: out_parms->pc[port_idx].pw = idt_pc_pw_1x_l2;
                                               break;
           case RIO_SPX_CTL_PTW_OVER_2x_NO_4X : out_parms->pc[port_idx].pw = idt_pc_pw_2x;
                                               break;
           default                           : out_parms->pc[port_idx].pw = idt_pc_pw_last;
       };

       // Determine configured port speed...
       rc = DARRegRead( dev_info, TSI721_RIO_SP_CTL2, &spxCtl2 );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_GET_CONFIG(0x11);
          goto idt_tsi721_pc_get_config_exit;       
       };
       
       out_parms->pc[port_idx].ls = idt_pc_ls_last;
       misconfigured = false;

       for (idx = 0; (ls_check[idx].ls_en_val) && !misconfigured; idx++) {
           if (ls_check[idx].ls_en_val & spxCtl2) {
              if (!(ls_check[idx].ls_sup_val & spxCtl2)) {
                 misconfigured = true;
                 out_parms->pc[port_idx].ls = idt_pc_ls_last;
              } else {
                 if (idt_pc_ls_last != out_parms->pc[port_idx].ls) {
                    misconfigured = true;
                    out_parms->pc[port_idx].ls = idt_pc_ls_last;
                 } else {
                    out_parms->pc[port_idx].ls = ls_check[idx].ls;
                 };
              };
           };
       };

       out_parms->pc[port_idx].port_lockout = 
          (spxCtl & TSI721_RIO_SP_CTL_PORT_LOCKOUT)?true:false;

       out_parms->pc[port_idx].nmtc_xfer_enable = 
          ((spxCtl & (TSI721_RIO_SP_CTL_INP_EN|TSI721_RIO_SP_CTL_OTP_EN))
	== (TSI721_RIO_SP_CTL_INP_EN|TSI721_RIO_SP_CTL_OTP_EN));

       // Check for lane swapping & inversion
       // FIXME!!! LANE SWAPPING AND INVERSION NOT SUPPORTED
       rc = DARRegRead( dev_info, TSI721_RIO_PLM_SP_IMP_SPEC_CTL, &plmCtl );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_GET_CONFIG(0x20);
          goto idt_tsi721_pc_get_config_exit;       
       };

       if (plmCtl & TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SWAP_RX) {
          out_parms->pc[port_idx].rx_lswap = true;
       };

       if (plmCtl & TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SWAP_TX) {
          out_parms->pc[port_idx].tx_lswap = true;
       };
    };

idt_tsi721_pc_get_config_exit:       
    return rc;
}

// Note: in_parms contains the configuration to change to,
//       out_parms contains the current configuration...
//
#define NO_RESETS     false
#define MANAGE_RESETS true
uint32_t idt_tsi721_pc_set_config_with_resets  ( DAR_DEV_INFO_t           *dev_info, 
                                               idt_pc_set_config_in_t   *in_parms, 
                                               idt_pc_set_config_out_t  *out_parms,
                                               bool                      manage_resets )
{
    uint32_t rc = RIO_SUCCESS;
	uint32_t spxCtl, spxCtl2, devCtl, plmCtl;
	uint32_t saved_plmCtl, saved_devCtl;


    rc = DARRegRead( dev_info, TSI721_RIO_PLM_SP_IMP_SPEC_CTL, &saved_plmCtl );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_SET_CONFIG(2);
       goto exit;
    }
   
	rc = DARRegRead( dev_info, TSI721_DEVCTL, &saved_devCtl );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_SET_CONFIG(3);
       goto exit;
    }

    // Default is to ignore resets...
    plmCtl = saved_plmCtl & ~(TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SELF_RST       | 
		                      TSI721_RIO_PLM_SP_IMP_SPEC_CTL_PORT_SELF_RST);
	devCtl = saved_devCtl & ~TSI721_DEVCTL_SR_RST_MODE;

     plmCtl |= TSI721_RIO_PLM_SP_IMP_SPEC_CTL_PORT_SELF_RST;
	 devCtl |= TSI721_DEVCTL_SR_RST_MODE_SRIO_ONLY;

    rc = DARRegWrite( dev_info, TSI721_RIO_PLM_SP_IMP_SPEC_CTL, plmCtl );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_SET_CONFIG(8);
       goto exit;
    }
   
	rc = DARRegWrite( dev_info, TSI721_DEVCTL, devCtl );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_SET_CONFIG(9);
       goto exit;
    }

        // Check that RapidIO transmitter is enabled...
        if ((out_parms->pc[0].xmitter_disable != in_parms->pc[0].xmitter_disable) ||  
            (out_parms->pc[0].port_lockout != in_parms->pc[0].port_lockout) || 
            (out_parms->pc[0].nmtc_xfer_enable!= in_parms->pc[0].nmtc_xfer_enable) || 
            (out_parms->pc[0].pw           != in_parms->pc[0].pw          )) {

           rc = DARRegRead( dev_info, TSI721_RIO_SP_CTL, &spxCtl );
           if (RIO_SUCCESS != rc) {
              out_parms->imp_rc = PC_SET_CONFIG(0x20);
              goto exit;       
           };

           if (in_parms->pc[0].xmitter_disable) {
              spxCtl |= TSI721_RIO_SP_CTL_PORT_DIS;
           } else {
              spxCtl &= ~TSI721_RIO_SP_CTL_PORT_DIS;
           };

           if (in_parms->pc[0].port_lockout) {
              spxCtl |= TSI721_RIO_SP_CTL_PORT_LOCKOUT;
           } else {
              spxCtl &= ~TSI721_RIO_SP_CTL_PORT_LOCKOUT;
           };

           if (in_parms->pc[0].nmtc_xfer_enable) {
              spxCtl |= TSI721_RIO_SP_CTL_INP_EN | TSI721_RIO_SP_CTL_OTP_EN;
           } else {
              spxCtl &= ~(TSI721_RIO_SP_CTL_INP_EN | TSI721_RIO_SP_CTL_OTP_EN);
           };

           spxCtl &= ~TSI721_RIO_SP_CTL_OVER_PWIDTH;
           switch (in_parms->pc[0].pw) {
              case idt_pc_pw_2x: spxCtl |= RIO_SPX_CTL_PTW_OVER_2x_NO_4X;
                                 break;
              case idt_pc_pw_4x: spxCtl |= RIO_SPX_CTL_PTW_OVER_NONE;
                                 break;
              case idt_pc_pw_1x:
              case idt_pc_pw_1x_l0: spxCtl |= RIO_SPX_CTL_PTW_OVER_1x_L0;
                                    break;
              case idt_pc_pw_1x_l2: spxCtl |= RIO_SPX_CTL_PTW_OVER_1x_LR;
                                    break;
              default:
              case idt_pc_pw_1x_l1: out_parms->imp_rc = PC_SET_CONFIG(8);
                                    goto exit;       
           };

           rc = DARRegWrite( dev_info, TSI721_RIO_SP_CTL, spxCtl );
           if (manage_resets) {
              // Wait a while just in case a reset has occurred, and
              // ignore register access failures here...
              DAR_WaitSec( 1000000, 0 );

              rc = RIO_SUCCESS;
           };
           if (RIO_SUCCESS != rc) {
              out_parms->imp_rc = PC_SET_CONFIG(0x30);
              goto exit;       
           };
        };

        // Configure port speed, if necessary...
        if (out_parms->pc[0].ls != in_parms->pc[0].ls) {
		   uint32_t reg;
	       uint32_t fiveg_wa_val = (in_parms->pc[0].ls >= idt_pc_ls_5p0)?TSI721_WA_VAL_5G:TSI721_WA_VAL_3G;

           if (in_parms->pc[0].ls >= sizeof(ls_check)/sizeof(spx_ctl2_ls_check_info_t)) {
              out_parms->imp_rc = PC_SET_CONFIG(0x31);
              goto exit;       
           };

           rc = DARRegRead( dev_info, TSI721_RIO_SP_CTL2, &spxCtl2 );
           if (RIO_SUCCESS != rc) {
              out_parms->imp_rc = PC_SET_CONFIG(0x40);
              goto exit;       
           };
           spxCtl2 &= ~(TSI721_RIO_SP_CTL2_GB_6p25_EN  |
                        TSI721_RIO_SP_CTL2_GB_5p0_EN   |
                        TSI721_RIO_SP_CTL2_GB_3p125_EN |
                        TSI721_RIO_SP_CTL2_GB_2p5_EN   |
                        TSI721_RIO_SP_CTL2_GB_1p25_EN);
           spxCtl2 |= ls_check[(int)(in_parms->pc[0].ls)].ls_en_val;
           rc = DARRegWrite( dev_info, TSI721_RIO_SP_CTL2, spxCtl2 );
           if (manage_resets) {
              // Wait a while just in case a reset/reinitialization has occurred. 
              // Ignore register access failures here...
              DAR_WaitSec( 1000000, 0 );
              rc = RIO_SUCCESS;
           };
           if (RIO_SUCCESS != rc) {
              out_parms->imp_rc = PC_SET_CONFIG(0x41);
              goto exit;       
           };

		   // Apply 5G training work around, or remove it, as necessary
	       for (reg = 0; reg < TSI721_NUM_WA_REGS; reg++) {
              rc = DARRegWrite( dev_info, TSI721_5G_WA_REG0(reg), fiveg_wa_val );
              if (RIO_SUCCESS != rc) {
                 out_parms->imp_rc = PC_SET_CONFIG(0x3A);
                 goto exit;       
              };
	       };

	       if (in_parms->pc[0].ls > idt_pc_ls_3p125) {
	          for (reg = 0; reg < TSI721_NUM_WA_REGS; reg++) {
                 rc = DARRegWrite( dev_info, TSI721_5G_WA_REG1(reg), 0 );
                 if (RIO_SUCCESS != rc) {
                    out_parms->imp_rc = PC_SET_CONFIG(0x3C);
                    goto exit;       
                 };
              };
	       };
		   // Update PRESCALAR_SRV_CLK value
		   rc = DARRegWrite( dev_info, TSI721_RIO_PRESCALAR_SRV_CLK, ls_check[(int)(in_parms->pc[0].ls)].prescalar_srv_clk );
           if (RIO_SUCCESS != rc) {
              out_parms->imp_rc = PC_SET_CONFIG(0x3E);
              goto exit;       
           };
        };
       
       // Check for lane swapping & inversion
       // FIXME!!! LANE SWAPPING AND INVERSION NOT FULLY SUPPORTED, there are more options
       if ((out_parms->pc[0].tx_lswap != in_parms->pc[0].tx_lswap) || 
           (out_parms->pc[0].rx_lswap != in_parms->pc[0].rx_lswap)) {
          rc = DARRegRead( dev_info, TSI721_RIO_PLM_SP_IMP_SPEC_CTL, &plmCtl );
          if (RIO_SUCCESS != rc) {
             out_parms->imp_rc = PC_SET_CONFIG(0x40);
             goto exit;       
          };

          plmCtl &= ~(TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SWAP_RX | 
                      TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SWAP_TX );

          if (in_parms->pc[0].rx_lswap) {
               plmCtl |= TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SWAP_RX_3210;
          };
          if (in_parms->pc[0].tx_lswap) {
               plmCtl |= TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SWAP_TX_3210;
          };

          rc = DARRegWrite( dev_info, TSI721_RIO_PLM_SP_IMP_SPEC_CTL, plmCtl );
          if (manage_resets) {
             // Wait a while just in case a reset/reinitialization has occurred. 
             // Ignore register access failures here...
              DAR_WaitSec( 1000000, 0 );
             rc = RIO_SUCCESS;
          };
          if (RIO_SUCCESS != rc) {
             out_parms->imp_rc = PC_SET_CONFIG(0x50);
             goto exit;       
          };
       };

    plmCtl = plmCtl & ~(TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SELF_RST       | 
		                TSI721_RIO_PLM_SP_IMP_SPEC_CTL_PORT_SELF_RST);
	plmCtl |= saved_plmCtl & (TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SELF_RST       | 
		                      TSI721_RIO_PLM_SP_IMP_SPEC_CTL_PORT_SELF_RST);
	devCtl = devCtl        & ~(TSI721_DEVCTL_SR_RST_MODE);
	devCtl |= saved_devCtl &   TSI721_DEVCTL_SR_RST_MODE;

	rc = DARRegWrite( dev_info, TSI721_RIO_PLM_SP_IMP_SPEC_CTL, plmCtl );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_SET_CONFIG(0x60);
       goto exit;
    }
   
	rc = DARRegWrite( dev_info, TSI721_DEVCTL, devCtl );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_SET_CONFIG(0x61);
       goto exit;
    }
exit:
       return rc;
};

uint32_t idt_tsi721_pc_set_config  ( DAR_DEV_INFO_t           *dev_info, 
                                   idt_pc_set_config_in_t   *in_parms, 
                                   idt_pc_set_config_out_t  *out_parms )
{
    uint32_t rc = RIO_SUCCESS;
    idt_pc_get_config_in_t   curr_cfg_in;
    uint32_t devStat;

    out_parms->imp_rc = RIO_SUCCESS;
    out_parms->num_ports = 0;

    if ( (NUM_PORTS(dev_info) < in_parms->num_ports) &&
        !(RIO_ALL_PORTS      == in_parms->num_ports))
    {
        rc = RIO_ERR_INVALID_PARAMETER;
        out_parms->imp_rc = PC_SET_CONFIG(0x1);
        goto idt_tsi721_pc_set_config_exit;
    }

   if (!(RIO_ALL_PORTS   == in_parms->num_ports) &&
	    (in_parms->pc[0].pnum > 0              )   )
    {
        rc = RIO_ERR_INVALID_PARAMETER;
        out_parms->imp_rc = PC_SET_CONFIG(0x2);
        goto idt_tsi721_pc_set_config_exit;
    }

   // Always set LRTO.  LRTO coincidentally is in units of 100 nsec...
    rc = DARRegWrite( dev_info, TSI721_RIO_SP_LT_CTL,
		((in_parms->lrto) << 8) & TSI721_RIO_SP_LT_CTL_TVAL );
	if (RIO_SUCCESS != rc) {
        out_parms->imp_rc = PC_SET_CONFIG(0x1);
        goto idt_tsi721_pc_set_config_exit;
    }

   // Always set LOG_RTO 
   // Note: Tsi721 logical response timeout field does not appear at the 
   // correct location in the register.  We generically correct this in the
   // IDT_tsi721WriteReg routine, so we must write a generic value here.
    rc = DARRegWrite( dev_info, TSI721_RIO_SR_RSP_TO,
	((((in_parms->log_rto * 100) + 187)/ 188) << 8)
						& RIO_SP_RTO_CTL_TVAL);
	if (RIO_SUCCESS != rc) {
        out_parms->imp_rc = PC_SET_CONFIG(0x1);
        goto idt_tsi721_pc_set_config_exit;
    }

    curr_cfg_in.ptl.num_ports = RIO_ALL_PORTS;
    rc = idt_tsi721_pc_get_config( dev_info, &curr_cfg_in, out_parms );
    if (RIO_SUCCESS != rc) {
        out_parms->imp_rc = PC_SET_CONFIG(0x3);
        goto idt_tsi721_pc_set_config_exit;
    }
    
    // Work our way from most to least severe changes...
    //
    // If boot complete has not been set, the port is disabled, or we've got 
    // out-of-band register access, this implies that the RapidIO port can be
    // reconfigured without fear of resets.
    //
    // Perform all changes to port configuration and don't manage resets.
    
    if (!out_parms->pc[0].powered_up || out_parms->pc[0].xmitter_disable || in_parms->oob_reg_acc) {
       rc = idt_tsi721_pc_set_config_with_resets( dev_info, in_parms, out_parms, NO_RESETS );
       if (RIO_SUCCESS != rc) 
          goto idt_tsi721_pc_set_config_exit;

       if (in_parms->pc[0].powered_up) {
          // Set boot complete, now that the configuration is done...
          rc = DARRegRead( dev_info, TSI721_DEVSTAT, &devStat );
          if (RIO_SUCCESS != rc) 
          {
             out_parms->imp_rc = PC_SET_CONFIG(0x10);
             goto idt_tsi721_pc_set_config_exit;
          };

          devStat |= TSI721_DEVSTAT_SR_BOOT;
          rc = DARRegWrite( dev_info, TSI721_DEVSTAT, devStat );
          if (RIO_SUCCESS != rc) 
          {
             out_parms->imp_rc = PC_SET_CONFIG(0x11);
             goto idt_tsi721_pc_set_config_exit;
          };
       };
       goto idt_tsi721_pc_set_config_done;
    };

    // RapidIO port is powered up, port_dis is cleared, and the Tsi721
    // is being accessed in band.
    //
    // Disabling the port of the device will prevent future communication,
    // so don't do it...
    // Changing the lane speed of the device will cause a device reset, and
    // clear all of the device regsiters.  Don't do it.

    if ((out_parms->pc[0].xmitter_disable != in_parms->pc[0].xmitter_disable) ||
        (out_parms->pc[0].ls           != in_parms->pc[0].ls          )) {
       rc = RIO_ERR_NOT_SUP_BY_CONFIG;
       out_parms->imp_rc = PC_SET_CONFIG(5);
       goto idt_tsi721_pc_set_config_exit;
    };
                        
    // All other changes can be made without causing a reset of the device...
    // Note that they may cause link reinitialization...
    rc = idt_tsi721_pc_set_config_with_resets( dev_info, in_parms, out_parms, MANAGE_RESETS );
    if (RIO_SUCCESS != rc) 
       goto idt_tsi721_pc_set_config_exit;

idt_tsi721_pc_set_config_done:       
    rc = idt_tsi721_pc_get_config( dev_info, &curr_cfg_in, out_parms );
    
idt_tsi721_pc_set_config_exit:       
    return rc;
}
            
uint32_t idt_tsi721_pc_get_status  ( DAR_DEV_INFO_t         *dev_info, 
                                 idt_pc_get_status_in_t   *in_parms, 
                                 idt_pc_get_status_out_t  *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint8_t  port_idx;
    uint32_t errStat, spxCtl, devCtl;
    struct DAR_ptl good_ptl;

    out_parms->num_ports = 0;
    out_parms->imp_rc        = RIO_SUCCESS;

    rc = DARrioGetPortList(dev_info, &in_parms->ptl, &good_ptl);
    if (RIO_SUCCESS != rc) {
        out_parms->imp_rc = PC_GET_uint32_t(1);
          goto idt_tsi721_pc_get_status_exit;
    };

    out_parms->num_ports = good_ptl.num_ports;
    for (port_idx = 0; port_idx < good_ptl.num_ports; port_idx++)
        out_parms->ps[port_idx].pnum = good_ptl.pnums[port_idx];

    for (port_idx = 0; port_idx < out_parms->num_ports; port_idx++)
    {
       out_parms->ps[port_idx].pw = idt_pc_pw_last;
       out_parms->ps[port_idx].port_error = false;
       out_parms->ps[port_idx].input_stopped = false;
       out_parms->ps[port_idx].output_stopped = false;

       rc = DARRegRead( dev_info, TSI721_DEVCTL, &devCtl );
       if (RIO_SUCCESS != rc) 
       {
          out_parms->imp_rc = PC_GET_uint32_t(5);
          goto idt_tsi721_pc_get_status_exit;
       };

       out_parms->ps[port_idx].first_lane = 0;
       if ( devCtl & TSI721_DEVCTL_SRBOOT_CMPL ) {
          out_parms->ps[port_idx].num_lanes = true;
       } else {
          out_parms->ps[port_idx].num_lanes = false;
       };

       if (!out_parms->ps[port_idx].num_lanes)
          continue;

       // Port is available and powered up, so let's figure out the status...
       rc = DARRegRead( dev_info, TSI721_RIO_SP_ERR_STAT, &errStat );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_GET_uint32_t(0x30+port_idx);
          goto idt_tsi721_pc_get_status_exit;
       };

       rc = DARRegRead( dev_info, TSI721_RIO_SP_CTL, &spxCtl );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_GET_uint32_t(0x40+port_idx);
          goto idt_tsi721_pc_get_status_exit;
       };

       out_parms->ps[port_idx].port_ok        = 
           (errStat & TSI721_RIO_SP_ERR_STAT_PORT_OK        ) ? true : false;
       out_parms->ps[port_idx].input_stopped  = 
           (errStat & TSI721_RIO_SP_ERR_STAT_INPUT_ERR_STOP ) ? true : false;
       out_parms->ps[port_idx].output_stopped = 
           (errStat & TSI721_RIO_SP_ERR_STAT_OUTPUT_ERR_STOP) ? true : false;

       // Port Error is true if a PORT_ERR is present, OR
       // if a OUTPUT_FAIL is present when STOP_FAIL_EN is set.
       out_parms->ps[port_idx].port_error = 
           ((errStat & TSI721_RIO_SP_ERR_STAT_PORT_ERR    ) ||
           ((spxCtl  & TSI721_RIO_SP_CTL_STOP_FAIL_EN   ) && 
            (errStat & TSI721_RIO_SP_ERR_STAT_OUTPUT_FAIL)));

       // Baudrate and portwidth status are only defined when
       // PORT_OK is asserted... 
       if (out_parms->ps[port_idx].port_ok) {
          switch (spxCtl & TSI721_RIO_SP_CTL_INIT_PWIDTH) {
             case RIO_SPX_CTL_PTW_INIT_1x_L0: out_parms->ps[port_idx].pw = idt_pc_pw_1x_l0;
                                                 break;
             case RIO_SPX_CTL_PTW_INIT_1x_LR: out_parms->ps[port_idx].pw = idt_pc_pw_1x_l2;
                                                 break;
             case RIO_SPX_CTL_PTW_INIT_2x   : out_parms->ps[port_idx].pw = idt_pc_pw_2x;
                                                 break;
             case RIO_SPX_CTL_PTW_INIT_4x   : out_parms->ps[port_idx].pw = idt_pc_pw_4x;
                                                 break;
             default:  out_parms->ps[port_idx].pw = idt_pc_pw_last;
          };
       };
    };

idt_tsi721_pc_get_status_exit:
    return rc;
}

uint32_t reset_tsi721_lp( DAR_DEV_INFO_t          *dev_info, 
                        uint32_t                  *imp_rc )
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   uint32_t lr_cmd = STYPE1_LREQ_CMD_RST_DEV;
   uint32_t lr_resp;

   rc = DARRegWrite(dev_info, TSI721_RIO_SP_LM_REQ, lr_cmd);
   if (RIO_SUCCESS != rc) {
      *imp_rc = PC_RESET_LP(0x20);
      goto reset_tsi721_lp_exit;
   }

   rc = DARRegRead(dev_info, TSI721_RIO_SP_LM_RESP, &lr_resp);
   if (RIO_SUCCESS != rc) {
      *imp_rc = PC_RESET_LP(0x21);
      goto reset_tsi721_lp_exit;
   }

   if (!(lr_resp & TSI721_RIO_SP_LM_RESP_RESP_VLD)) {
      rc = RIO_ERR_READ_REG_RETURN_INVALID_VAL;
      *imp_rc = PC_RESET_LP(0x22);
   }

reset_tsi721_lp_exit:
  return rc;
};

uint32_t update_tsi721_reset_policy( DAR_DEV_INFO_t      *dev_info     , 
                                   idt_pc_rst_handling  rst_policy_in,
				   uint32_t              *saved_plmctl,
				   uint32_t              *saved_devctl,
				   uint32_t              *saved_rstint,
				   uint32_t              *saved_rstpw,  
                                   uint32_t              *imp_rc      );

uint32_t idt_tsi721_pc_reset_port  ( DAR_DEV_INFO_t          *dev_info, 
                                   idt_pc_reset_port_in_t  *in_parms, 
                                   idt_pc_reset_port_out_t *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    idt_pc_get_config_in_t  cfg_in;
    idt_pc_get_config_out_t cfg_out;
    uint8_t  port_idx;

    out_parms->imp_rc = RIO_SUCCESS;

    if ((RIO_ALL_PORTS      != in_parms->port_num) &&
        (in_parms->port_num >= NUM_PORTS(dev_info))) {
       out_parms->imp_rc = PC_RESET_PORT(1);
       goto exit;
    };

    out_parms->imp_rc = RIO_SUCCESS;

    if (RIO_ALL_PORTS == in_parms->port_num) {
       cfg_in.ptl.num_ports = RIO_ALL_PORTS;
    } else {
       cfg_in.ptl.num_ports = 1;
       cfg_in.ptl.pnums[0] = in_parms->port_num;
    };

    rc = idt_tsi721_pc_get_config( dev_info, &cfg_in, &cfg_out);
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = cfg_out.imp_rc;
       goto exit;
    }

    for (port_idx = 0; port_idx < cfg_out.num_ports; port_idx++) {
	   uint32_t devctl, plmctl, ctl2_saved, ctl2;
	   uint32_t rstint, rstpw;

       // Do not reset ports required for connectivity.
       // Also skip ports that are not available or powered down.
       if ((!cfg_out.pc[port_idx].port_available) ||
           (!cfg_out.pc[port_idx].powered_up    ))
          continue;

       rc = DARRegRead( dev_info, TSI721_RIO_SP_CTL2, &ctl2_saved );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_RESET_PORT(0x07);
          goto exit;       
       };

	ctl2 = ctl2_saved ^ (TSI721_RIO_SP_CTL2_GB_6p25_EN |
				TSI721_RIO_SP_CTL2_GB_5p0_EN |
				TSI721_RIO_SP_CTL2_GB_3p125_EN |
				TSI721_RIO_SP_CTL2_GB_2p5_EN |
				TSI721_RIO_SP_CTL2_GB_1p25_EN);

       rc = update_tsi721_reset_policy( dev_info, idt_pc_rst_port,  
		&plmctl, &devctl, &rstint, &rstpw,
		&out_parms->imp_rc);

       if (in_parms->reset_lp) {
          rc = reset_tsi721_lp( dev_info, &out_parms->imp_rc );
          if (RIO_SUCCESS != rc) {
          	goto exit;       
          }
       }

       /* Trigger a port reset by blipping the enabled baudrates.
        */

       rc = DARRegWrite( dev_info, TSI721_RIO_SP_CTL2, ctl2 );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_RESET_PORT(0x40);
          	goto exit;       
       };

       rc = DARRegWrite( dev_info, TSI721_RIO_SP_CTL2, ctl2_saved );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_RESET_PORT(0x41);
          	goto exit;       
       };

	/* Then restore the reset policy
	 */
       rc = DARRegWrite( dev_info, TSI721_DEVCTL, devctl );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_RESET_PORT(0x42);
          	goto exit;       
       };

       rc = DARRegWrite( dev_info, TSI721_RIO_PLM_SP_IMP_SPEC_CTL, plmctl );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_RESET_PORT(0x43);
          	goto exit;       
       };

	rc = DARRegWrite( dev_info, TSI721_RIO_EM_RST_INT_EN, rstint );
	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = PC_RESET_PORT(0x44);
		goto exit;       
	}

	rc = DARRegWrite( dev_info, TSI721_RIO_EM_RST_PW_EN, rstpw );
	if (RIO_SUCCESS != rc) {
		out_parms->imp_rc = PC_RESET_PORT(0x45);
		goto exit;       
	};
    };

exit:
    return rc;
}

uint32_t idt_tsi721_pc_reset_link_partner(
    DAR_DEV_INFO_t                   *dev_info, 
    idt_pc_reset_link_partner_in_t   *in_parms, 
    idt_pc_reset_link_partner_out_t  *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;

    out_parms->imp_rc = RIO_SUCCESS;

    if (NUM_PORTS(dev_info) <= in_parms->port_num)
    {
       out_parms->imp_rc = PC_RESET_LP(1);
       goto idt_tsi721_pc_reset_link_partner_exit;
    }

    rc = reset_tsi721_lp( dev_info, &out_parms->imp_rc );
    if (RIO_SUCCESS != rc) {
       goto idt_tsi721_pc_reset_link_partner_exit;
    };

    if (in_parms->resync_ackids) {
        rc = DARRegWrite( dev_info, TSI721_RIO_SP_ACKID_STAT,
                                    TSI721_RIO_SP_ACKID_STAT_CLR_OUTSTD_ACKID );  
       if (RIO_SUCCESS != rc) 
          out_parms->imp_rc = PC_RESET_LP(2);
    }
    
idt_tsi721_pc_reset_link_partner_exit:
    return rc;
}

#define PC_CLR_ERRS(x) (PC_CLR_ERRS_0+x)

uint32_t idt_tsi721_pc_clr_errs  ( DAR_DEV_INFO_t       *dev_info, 
                               idt_pc_clr_errs_in_t   *in_parms, 
                               idt_pc_clr_errs_out_t  *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint8_t port_idx;
    uint32_t dlay;
    uint32_t lresp = 0;
    uint32_t ackid_stat;
    uint32_t err_stat;
    idt_pc_get_status_in_t  status_in;
    idt_pc_get_status_out_t status_out;

    out_parms->imp_rc = RIO_SUCCESS;

    if (NUM_PORTS(dev_info) <= in_parms->port_num)
    {
       out_parms->imp_rc = PC_CLR_ERRS(1);
       goto idt_tsi721_pc_clr_errs_exit;
    }
    
    if (in_parms->clr_lp_port_err) 
    {
       if (!in_parms->num_lp_ports                  ||
          ( in_parms->num_lp_ports > IDT_MAX_PORTS) ||
          (NULL == in_parms->lp_dev_info          )) 
       {
          out_parms->imp_rc = PC_CLR_ERRS(2);
          goto idt_tsi721_pc_clr_errs_exit;
       };
       for (port_idx = 0; port_idx < in_parms->num_lp_ports; port_idx++) 
       {
          if (in_parms->lp_port_list[port_idx] >= NUM_PORTS(in_parms->lp_dev_info) ) 
          {
             out_parms->imp_rc = PC_CLR_ERRS(3);
             goto idt_tsi721_pc_clr_errs_exit;
          };
       }
    }

    // If the port is not PORT_OK, it is not possible to clear error conditions.
    status_in.ptl.num_ports   = 1;
    status_in.ptl.pnums[0] = in_parms->port_num;
    rc = idt_tsi721_pc_get_status( dev_info, &status_in, &status_out );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = status_out.imp_rc;
       goto idt_tsi721_pc_clr_errs_exit;
    }

    if ((status_out.num_ports != 1) || (!status_out.ps[0].port_ok)) {
       rc = RIO_ERR_ERRS_NOT_CL;  
       out_parms->imp_rc = PC_CLR_ERRS(4);
       goto idt_tsi721_pc_clr_errs_exit;
    }
    
    /* First, ensure input/output error-stopped conditions are cleared 
       on our port and the link partner by sending the magic control symbol.
    */

    rc = DARRegWrite( dev_info, TSI721_RIO_PLM_SP_LONG_CS_TX1, TSI721_MAGIC_CS );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_CLR_ERRS(0x10);
       goto idt_tsi721_pc_clr_errs_exit;
    }

    //  Delay while waiting for control symbol magic to complete.
    //  Should be > 5 usec, just to allow some margin for different link speeds
    //  and link partners.

    DAR_WaitSec( 5000, 0 );

    // Prepare to clear any port-err conditions that may exist on this port.
    //     Send link-request/input-status to learn what link partners
    //     next expected ackID is.
    rc = DARRegWrite( dev_info, TSI721_RIO_SP_LM_REQ, STYPE1_LREQ_CMD_PORT_STAT );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_CLR_ERRS(0x11); 
       goto idt_tsi721_pc_clr_errs_exit;
    };

    // Poll until we get a response.  Fail if no response is received.

    dlay = 10;
    while (!(lresp & TSI721_RIO_SP_LM_RESP_RESP_VLD) && dlay) {
       dlay--;
       rc = DARRegRead( dev_info, TSI721_RIO_SP_LM_RESP, &lresp );
       if (RIO_SUCCESS != rc) {
          out_parms->imp_rc = PC_CLR_ERRS(0x12); 
          goto idt_tsi721_pc_clr_errs_exit;
       }
    }

    if (!(lresp & TSI721_RIO_SP_LM_RESP_RESP_VLD)) {
       rc = RIO_ERR_NOT_EXPECTED_RETURN_VALUE;
       out_parms->imp_rc = PC_CLR_ERRS(0x13); 
       goto idt_tsi721_pc_clr_errs_exit;
    }

    // We have valid ackID information.  Update our local ackID status.
    // The act of updating our local ackID status will clear a local 
    // port-err condition.
    
    rc = DARRegRead( dev_info, TSI721_RIO_SP_ACKID_STAT, &ackid_stat );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_CLR_ERRS(0x14); 
       goto idt_tsi721_pc_clr_errs_exit;
    }

    lresp = (lresp & TSI721_RIO_SP_LM_RESP_ACK_ID_STAT) >> 5;
    ackid_stat = ackid_stat & TSI721_RIO_SP_ACKID_STAT_INB_ACKID;
    ackid_stat = ackid_stat | lresp | (lresp << 8);

    rc = DARRegWrite( dev_info, TSI721_RIO_SP_ACKID_STAT, ackid_stat );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_CLR_ERRS(0x15); 
       goto idt_tsi721_pc_clr_errs_exit;
    }

    if (in_parms->clr_lp_port_err) 
    {
	    // FIXME: THIS SECTION HAS NOT BEEN UPDATED, AND MAY NOT WORK...
       /* Update the link partners ackID status register to match that of
              the Tsi57x.
          Increment the expected inbound ackID to reflect the reception of
              the maintenance write packet.
          Link partners inbound value should be our outbound value, plus 1.
          Link partners outbound value should be our inbound value.
       */
     /*  lresp = ( ackid_stat + 1 ) & TSI721_RIO_SP_ACKID_STAT_OUTB_ACKID;
       ackid_stat = (ackid_stat & Tsi578_SPX_ACKID_STAT_INBOUND) >> 24;
       ackid_stat |= (ackid_stat << 8) | (lresp << 24);

       for (port_idx = 0; port_idx < in_parms->num_lp_ports; port_idx++) 
       {
          lp_port_num = in_parms->lp_port_list[port_idx];
          rc = DARRegWrite( in_parms->lp_dev_info, RIO_SPX_ACKID_ST(in_parms->lp_dev_info->extFPtrForPort, lp_port_num),
                            ackid_stat );
          if (RIO_SUCCESS != rc) 
          {
             // The write can fail because the incorrect port was selected.
             //    Call ourselves to clear errors on the local port, and then
             //    try the next link partner port.
             idt_pc_clr_errs_in_t  temp_in;
             idt_pc_clr_errs_out_t temp_out;
             uint32_t                temp_rc;

             memcpy(&temp_in, &in_parms, sizeof(idt_pc_clr_errs_in_t));
             temp_in.clr_lp_port_err = false;

             temp_rc = idt_tsi721_pc_clr_errs( dev_info, &temp_in, &temp_out );
             if (RIO_SUCCESS != temp_rc) 
             {
                rc = temp_rc;
                out_parms->imp_rc = PC_CLR_ERRS(0x16); 
                goto idt_tsi721_pc_clr_errs_exit;
             };
          };
       }; */
    };

    // Lastly, clear physical layer error status indications for the port.
    rc = DARRegRead( dev_info, TSI721_RIO_SP_ERR_STAT, &err_stat );
    if (RIO_SUCCESS != rc)
       goto idt_tsi721_pc_clr_errs_exit;

    rc = DARRegWrite( dev_info, TSI721_RIO_SP_ERR_STAT, err_stat );
    if (RIO_SUCCESS != rc)
       goto idt_tsi721_pc_clr_errs_exit;

    rc = DARRegRead( dev_info, TSI721_RIO_SP_ERR_STAT, &err_stat );
    if (RIO_SUCCESS != rc)
       goto idt_tsi721_pc_clr_errs_exit;

    if (err_stat & (TSI721_RIO_SP_ERR_STAT_PORT_ERR        | 
                    TSI721_RIO_SP_ERR_STAT_INPUT_ERR_STOP  |  
                    TSI721_RIO_SP_ERR_STAT_OUTPUT_ERR_STOP |  
                    TSI721_RIO_SP_ERR_STAT_OUTPUT_FAIL     ) ) 
    {
       rc = RIO_ERR_ERRS_NOT_CL;  
       out_parms->imp_rc = PC_CLR_ERRS(0x20);
       goto idt_tsi721_pc_clr_errs_exit;
    }; 
                         
idt_tsi721_pc_clr_errs_exit:
    return rc;
}

#define UPDATE_RESET(x) (EM_UPDATE_RESET_0+x)

uint32_t update_tsi721_reset_policy( DAR_DEV_INFO_t      *dev_info     , 
                                   idt_pc_rst_handling  rst_policy_in,
				   uint32_t              *saved_plmctl,
				   uint32_t              *saved_devctl,
				   uint32_t              *saved_rstint,
				   uint32_t              *saved_rstpw,  
                                   uint32_t              *imp_rc      )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint32_t plmCtl, devCtl, rstInt = 0, rstPw = 0;

    rc = DARRegRead( dev_info, TSI721_RIO_PLM_SP_IMP_SPEC_CTL, saved_plmctl );
    if (RIO_SUCCESS != rc) {
       *imp_rc = UPDATE_RESET(1);
       goto update_tsi721_reset_policy_exit;
    }
   
	rc = DARRegRead( dev_info, TSI721_DEVCTL, saved_devctl );
    if (RIO_SUCCESS != rc) {
       *imp_rc = UPDATE_RESET(2);
       goto update_tsi721_reset_policy_exit;
    }

    rc = DARRegRead( dev_info, TSI721_RIO_EM_RST_INT_EN, saved_rstint );
    if (RIO_SUCCESS != rc) {
       *imp_rc = UPDATE_RESET(3);
       goto update_tsi721_reset_policy_exit;
    }

    rc = DARRegRead( dev_info, TSI721_RIO_EM_RST_PW_EN, saved_rstpw );
    if (RIO_SUCCESS != rc) {
       *imp_rc = UPDATE_RESET(4);
       goto update_tsi721_reset_policy_exit;
    }

    // Default is to ignore resets...
    plmCtl = *saved_plmctl & ~(TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SELF_RST       | 
		TSI721_RIO_PLM_SP_IMP_SPEC_CTL_PORT_SELF_RST);
	devCtl = *saved_devctl & ~TSI721_DEVCTL_SR_RST_MODE;

    switch (rst_policy_in) {
       case idt_pc_rst_device: plmCtl |= TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SELF_RST | 
								         TSI721_RIO_PLM_SP_IMP_SPEC_CTL_RESET_REG;
		                       devCtl |= TSI721_DEVCTL_SR_RST_MODE_HOT_RST;
			       break;
       case idt_pc_rst_port:   plmCtl |= TSI721_RIO_PLM_SP_IMP_SPEC_CTL_PORT_SELF_RST;
		                       devCtl |= TSI721_DEVCTL_SR_RST_MODE_SRIO_ONLY;
			       break;
       case idt_pc_rst_int   : rstInt = TSI721_RIO_EM_RST_INT_EN_RST_INT_EN;
			       break;
       case idt_pc_rst_pw    : rstPw  = TSI721_RIO_EM_RST_PW_EN_RST_PW_EN;
			       break;
       case idt_pc_rst_ignore:
       default:
			       break;
    };

    rc = DARRegWrite( dev_info, TSI721_RIO_PLM_SP_IMP_SPEC_CTL, plmCtl );
    if (RIO_SUCCESS != rc) {
       *imp_rc = UPDATE_RESET(3);
       goto update_tsi721_reset_policy_exit;
    }

	rc = DARRegWrite( dev_info, TSI721_DEVCTL, devCtl );
    if (RIO_SUCCESS != rc) {
       *imp_rc = UPDATE_RESET(4);
       goto update_tsi721_reset_policy_exit;
    }

    rc = DARRegWrite( dev_info, TSI721_RIO_EM_RST_INT_EN, rstInt );
    if (RIO_SUCCESS != rc) {
       *imp_rc = UPDATE_RESET(5);
       goto update_tsi721_reset_policy_exit;
    }

    rc = DARRegWrite( dev_info, TSI721_RIO_EM_RST_PW_EN, rstPw );
    if (RIO_SUCCESS != rc) {
       *imp_rc = UPDATE_RESET(6);
       goto update_tsi721_reset_policy_exit;
    }

update_tsi721_reset_policy_exit:
   return rc;
};

#define PC_SECURE_PORT(x) (PC_SECURE_PORT_0+x)

uint32_t idt_tsi721_pc_secure_port  ( DAR_DEV_INFO_t          *dev_info, 
                                  idt_pc_secure_port_in_t   *in_parms, 
                                  idt_pc_secure_port_out_t  *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint32_t ftype_filt;
    struct DAR_ptl good_ptl;
    uint32_t unused1, unused2, unused3, unused4;

    out_parms->imp_rc = RIO_SUCCESS;

    rc = DARrioGetPortList(dev_info, &in_parms->ptl, &good_ptl);
    if (RIO_SUCCESS != rc) {
       goto idt_tsi721_pc_secure_port_exit;
        out_parms->imp_rc = PC_SECURE_PORT(1);
    };

    if (in_parms->rst >= idt_pc_rst_last)
    {
        out_parms->imp_rc = PC_SECURE_PORT(2);
        return rc;
    }

    // Take care of reset policy update
    out_parms->rst = in_parms->rst;
    rc = update_tsi721_reset_policy( dev_info, in_parms->rst, 
			&unused1, &unused2, &unused3, &unused4, 
			&out_parms->imp_rc );
    if (RIO_SUCCESS != rc) {
       goto idt_tsi721_pc_secure_port_exit;
    }

    // Update MECS policy.
    // Will always accept MECS physically.  Since this is an endpoint
    // with only one port, MECS cannot be forwarded.    
    
    out_parms->MECS_acceptance  = true;
    out_parms->MECS_participant = false;

    // Tsi721 allows maintenance packets to be filtered out.
    rc = DARRegRead( dev_info, TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL, &ftype_filt );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_SECURE_PORT(3);
       goto idt_tsi721_pc_secure_port_exit;
    }

    if (in_parms->MECS_acceptance) {
       ftype_filt |= TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F8_MR | TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F8_MW;
    } else {
       ftype_filt &= ~(TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F8_MR | TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F8_MW);
    };

    rc = DARRegWrite( dev_info, TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL, ftype_filt );
    if (RIO_SUCCESS != rc) {
       out_parms->imp_rc = PC_SECURE_PORT(4);
       goto idt_tsi721_pc_secure_port_exit;
    }

idt_tsi721_pc_secure_port_exit:
    return rc;
}

#define PC_DEV_RESET_CONFIG(x) (PC_DEV_RESET_CONFIG_0+x)

uint32_t idt_tsi721_pc_dev_reset_config(
    DAR_DEV_INFO_t                 *dev_info, 
    idt_pc_dev_reset_config_in_t   *in_parms, 
    idt_pc_dev_reset_config_out_t  *out_parms )
{
    uint32_t rc = RIO_ERR_INVALID_PARAMETER;
    uint32_t unused1, unused2, unused3, unused4;

    out_parms->rst = in_parms->rst;
    out_parms->imp_rc  = RIO_SUCCESS;

    if ((uint8_t)(out_parms->rst) >= (uint8_t)(idt_pc_rst_last)) {
       out_parms->imp_rc = PC_DEV_RESET_CONFIG(1);
       goto idt_tsi721_pc_dev_reset_config_exit;
    };

    rc = update_tsi721_reset_policy( dev_info, in_parms->rst, 
			&unused1, &unused2, &unused3, &unused4, 
			&out_parms->imp_rc );
    if (RIO_SUCCESS != rc) {
       goto idt_tsi721_pc_dev_reset_config_exit;
    };
       
idt_tsi721_pc_dev_reset_config_exit:
    return rc;
}

/*
uint32_t idt_tsi721_rt_initialize  ( DAR_DEV_INFO_t         *dev_info, 
                                 idt_rt_initialize_in_t   *in_parms, 
                                 idt_rt_initialize_out_t  *out_parms )
{
    return RIO_ERR_FEATURE_NOT_SUPPORTED;
}

// FIXME: In future, may modify these routines to manage received deviceIDs
//        rather than the routing table...  Should probably make this a 
//        separate capability...
uint32_t idt_tsi721_rt_probe     ( DAR_DEV_INFO_t            *dev_info, 
                                 idt_rt_probe_in_t         *in_parms, 
                                 idt_rt_probe_out_t        *out_parms )
{
    return RIO_ERR_FEATURE_NOT_SUPPORTED;
}

// FIXME: In future, may modify these routines to manage received deviceIDs
//        rather than the routing table...  Should probably make this a 
//        separate capability...
uint32_t idt_tsi721_rt_probe_all  ( DAR_DEV_INFO_t           *dev_info, 
                                 idt_rt_probe_all_in_t     *in_parms, 
                                 idt_rt_probe_all_out_t    *out_parms )
{
    return RIO_ERR_FEATURE_NOT_SUPPORTED;
}

uint32_t idt_tsi721_rt_set_all   ( DAR_DEV_INFO_t            *dev_info, 
                                 idt_rt_set_all_in_t       *in_parms, 
                                 idt_rt_set_all_out_t      *out_parms )
{
    return RIO_ERR_FEATURE_NOT_SUPPORTED;
}

uint32_t idt_tsi721_rt_set_changed   ( DAR_DEV_INFO_t            *dev_info, 
                                 idt_rt_set_changed_in_t       *in_parms, 
                                 idt_rt_set_changed_out_t      *out_parms )
{
    return RIO_ERR_FEATURE_NOT_SUPPORTED;
};

uint32_t idt_tsi721_rt_change_rte (
    DAR_DEV_INFO_t           *dev_info, 
    idt_rt_change_rte_in_t   *in_parms, 
    idt_rt_change_rte_out_t  *out_parms
)
{
    return RIO_ERR_FEATURE_NOT_SUPPORTED;
}

uint32_t idt_tsi721_rt_change_mc_mask (
    DAR_DEV_INFO_t               *dev_info, 
    idt_rt_change_mc_mask_in_t   *in_parms, 
    idt_rt_change_mc_mask_out_t  *out_parms
)
{
    return RIO_ERR_FEATURE_NOT_SUPPORTED;
}
*/

/* NOTE: TSI721_RIO_PW_CTL_PWC_MODE (Reliable port-write reception) is
* always disabled by this routine.
*/
uint32_t idt_tsi721_em_cfg_pw  ( DAR_DEV_INFO_t       *dev_info, 
                               idt_em_cfg_pw_in_t   *in_parms, 
                               idt_em_cfg_pw_out_t  *out_parms ) 
{
  uint32_t rc = RIO_ERR_INVALID_PARAMETER;
  uint32_t regData;
  uint32_t retx;

  out_parms->imp_rc = RIO_SUCCESS;

  if (in_parms->priority > 3) {
     out_parms->imp_rc = EM_CFG_PW(1);
     goto idt_tsi721_em_cfg_pw_exit;
  };
      
  // Configure destination ID for port writes.
  regData = ((uint32_t)(in_parms->port_write_destID)) << 16;
  if (tt_dev16 == in_parms->deviceID_tt) {
     regData |= TSI721_RIO_PW_TGT_ID_LRG_TRANS;
  } else {
     regData &= ~(TSI721_RIO_PW_TGT_ID_MSB_PW_ID | TSI721_RIO_PW_TGT_ID_LRG_TRANS);
  };

  rc = DARRegWrite( dev_info, TSI721_RIO_PW_TGT_ID, regData );
  if (RIO_SUCCESS != rc) {
     out_parms->imp_rc = EM_CFG_PW(2);
     goto idt_tsi721_em_cfg_pw_exit;
  };

  rc = DARRegRead( dev_info, TSI721_RIO_PW_TGT_ID, &regData );
  if (RIO_SUCCESS != rc) {
     out_parms->imp_rc = EM_CFG_PW(3);
     goto idt_tsi721_em_cfg_pw_exit;
  };

  out_parms->deviceID_tt = (regData & TSI721_RIO_PW_TGT_ID_LRG_TRANS)?tt_dev16:tt_dev8;
  out_parms->port_write_destID = (uint16_t)((regData & ( TSI721_RIO_PW_TGT_ID_PW_TGT_ID 
                                                     | TSI721_RIO_PW_TGT_ID_MSB_PW_ID )) >> 16);
  // Source ID for port writes is found in the TSI721_RIO_BASE_ID of the endpoint
  // Source ID for port-writes cannot be set by this routine.
  rc = DARRegRead( dev_info, TSI721_RIO_BASE_ID, &regData );
  if (RIO_SUCCESS != rc) {
     out_parms->imp_rc = EM_CFG_PW(3);
     goto idt_tsi721_em_cfg_pw_exit;
  };

  out_parms->srcID_valid      = true;
  out_parms->port_write_srcID = (tt_dev8 == out_parms->deviceID_tt)?((regData & TSI721_RIO_BASE_ID_BASE_ID)>> 16):
	                                                             (regData & TSI721_RIO_BASE_ID_LAR_BASE_ID);

  // Cannot configure port-write priority or CRF.
  
  out_parms->priority = 3;
  out_parms->CRF      = true;

  // Configure port-write re-transmission rate.
  // Assumption: it is better to choose a longer retransmission time than the value requested.
 
  regData = 0;
     retx = in_parms->port_write_re_tx * PORT_WRITE_RE_TX_NSEC;

  if (retx) {
     if ((retx <= 103000) && retx) {
	regData = TSI721_RIO_PW_CTL_PW_TIMER_103us;
     } else {
        if ((retx <= 205000) && retx) {
	       regData = TSI721_RIO_PW_CTL_PW_TIMER_205us;
        } else {
           if ((retx <= 410000) && retx) {
	          regData = TSI721_RIO_PW_CTL_PW_TIMER_410us;
           } else {
	          regData = TSI721_RIO_PW_CTL_PW_TIMER_820us;
	       };
		};
     };
  };

  rc = DARRegWrite( dev_info, TSI721_RIO_PW_CTL, regData );
  if (RIO_SUCCESS != rc) {
     out_parms->imp_rc = EM_CFG_PW(3);
     goto idt_tsi721_em_cfg_pw_exit;
  };

  rc = DARRegRead( dev_info, TSI721_RIO_PW_CTL, &regData );
  if (RIO_SUCCESS != rc) {
     out_parms->imp_rc = EM_CFG_PW(3);
     goto idt_tsi721_em_cfg_pw_exit;
  };

  switch (regData) {
     case 0:
         out_parms->port_write_re_tx = 0;
         break;
     case TSI721_RIO_PW_CTL_PW_TIMER_103us:
         out_parms->port_write_re_tx = 103000/PORT_WRITE_RE_TX_NSEC;
         break;
     case TSI721_RIO_PW_CTL_PW_TIMER_205us:
         out_parms->port_write_re_tx = 205000/PORT_WRITE_RE_TX_NSEC;
         break;
     case TSI721_RIO_PW_CTL_PW_TIMER_410us:
         out_parms->port_write_re_tx = 410000/PORT_WRITE_RE_TX_NSEC;
         break;
     case TSI721_RIO_PW_CTL_PW_TIMER_820us:
         out_parms->port_write_re_tx = 820000/PORT_WRITE_RE_TX_NSEC;
         break;
     default:
         out_parms->port_write_re_tx = regData;
         rc = RIO_ERR_READ_REG_RETURN_INVALID_VAL;
         out_parms->imp_rc = EM_CFG_PW(9);
  }

  idt_tsi721_em_cfg_pw_exit:
  return rc; 
};

uint32_t idt_tsi721_set_int_cfg( DAR_DEV_INFO_t       *dev_info, 
                               uint8_t                 pnum    ,
                               idt_em_notfn_ctl_t    notfn   ,
                               uint32_t               *imp_rc  )
{
	if (NULL != dev_info)
		*imp_rc = pnum + (int)notfn;

	return RIO_ERR_FEATURE_NOT_SUPPORTED;
}

uint32_t tsi721_em_determine_notfn( DAR_DEV_INFO_t       *dev_info  , 
                                  idt_em_notfn_ctl_t   *notfn      ,
                                  uint8_t                 pnum      ,
                                  uint32_t               *imp_rc    ) 
{
	if (NULL != dev_info)
		*imp_rc = pnum + *(int*)notfn;

    return RIO_ERR_FEATURE_NOT_SUPPORTED;
} 

uint32_t idt_tsi721_em_cfg_set  ( DAR_DEV_INFO_t        *dev_info, 
                                idt_em_cfg_set_in_t   *in_parms, 
                                idt_em_cfg_set_out_t  *out_parms ) 
{
	if (NULL != dev_info)
		out_parms->imp_rc = in_parms->ptl.num_ports;

    return RIO_ERR_FEATURE_NOT_SUPPORTED;
};

uint32_t idt_tsi721_em_cfg_get  ( DAR_DEV_INFO_t        *dev_info, 
                                idt_em_cfg_get_in_t   *in_parms, 
                                idt_em_cfg_get_out_t  *out_parms ) 
{
	if (NULL != dev_info)
		out_parms->imp_rc = in_parms->port_num;

    return RIO_ERR_FEATURE_NOT_SUPPORTED;
};

uint32_t idt_tsi721_em_dev_rpt_ctl  ( DAR_DEV_INFO_t            *dev_info, 
                                    idt_em_dev_rpt_ctl_in_t   *in_parms, 
                                    idt_em_dev_rpt_ctl_out_t  *out_parms ) 
{
	if (NULL != dev_info)
		out_parms->imp_rc = in_parms->ptl.num_ports;

    return RIO_SUCCESS;
}

uint32_t idt_tsi721_em_parse_pw  ( DAR_DEV_INFO_t         *dev_info, 
                                 idt_em_parse_pw_in_t   *in_parms, 
                                 idt_em_parse_pw_out_t  *out_parms ) 
{
	if (NULL != dev_info)
		out_parms->imp_rc = in_parms->num_events;

    return RIO_ERR_FEATURE_NOT_SUPPORTED;
}

uint32_t idt_tsi721_em_get_int_stat  ( DAR_DEV_INFO_t             *dev_info, 
                                     idt_em_get_int_stat_in_t   *in_parms, 
                                     idt_em_get_int_stat_out_t  *out_parms ) 
{
	if (NULL != dev_info)
		out_parms->imp_rc = in_parms->num_events;

    return RIO_ERR_FEATURE_NOT_SUPPORTED;
};

uint32_t idt_tsi721_em_get_pw_stat  ( DAR_DEV_INFO_t            *dev_info, 
                                    idt_em_get_pw_stat_in_t   *in_parms, 
                                    idt_em_get_pw_stat_out_t  *out_parms )
{
	if (NULL != dev_info)
		out_parms->imp_rc = in_parms->ptl.num_ports;

    return RIO_ERR_FEATURE_NOT_SUPPORTED;
};

uint32_t idt_tsi721_em_clr_events   ( DAR_DEV_INFO_t           *dev_info, 
                                    idt_em_clr_events_in_t   *in_parms, 
                                    idt_em_clr_events_out_t  *out_parms ) 
{
	if (NULL != dev_info)
		out_parms->imp_rc = in_parms->num_events;

    return RIO_ERR_FEATURE_NOT_SUPPORTED;
};

uint32_t idt_tsi721_em_create_events( DAR_DEV_INFO_t              *dev_info, 
                                    idt_em_create_events_in_t   *in_parms, 
                                    idt_em_create_events_out_t  *out_parms ) 
{
	if (NULL != dev_info)
		out_parms->imp_rc = in_parms->num_events;

    return RIO_ERR_FEATURE_NOT_SUPPORTED;
};

struct tsi721_dev_ctr {
	idt_sc_ctr_t	ctr_t;
	bool		split;	// Two counters, one register
	bool		tx;	// Transmit (true), or receive (false)
	bool		srio;	// RapidIO, or other interface
	uint32_t	os;     // Register offset
};

#define TSI721_SPLIT_2ND_CTR 0

const struct tsi721_dev_ctr tsi721_dev_ctrs[] = {
//   Enum counter               SPLIT  TX     SRIO   OFFSET
    {idt_sc_pcie_msg_rx,	false, false, false, TSI721_CPL_SMSG_CNT},
    {idt_sc_pcie_msg_tx,	false, true , false, TSI721_TXTLP_SMSG_CNT},
    {idt_sc_pcie_dma_rx,	false, false, false, TSI721_CPL_BDMA_CNT},
    {idt_sc_pcie_dma_tx,	false, true , false, TSI721_TXTLP_BDMA_CNT},
    {idt_sc_pcie_brg_rx,	false, false, false, TSI721_RXTLP_BRG_CNT},
    {idt_sc_pcie_brg_tx,	false, true , false, TSI721_TXTLP_BRG_CNT},
    {idt_sc_rio_msg_tx,		false, true , true , TSI721_TXPKT_SMSG_CNT},
    {idt_sc_rio_msg_rx,		false, false, true , TSI721_RXPKT_SMSG_CNT},
    {idt_sc_rio_msg_tx_rty,	false, true , true , TSI721_RETRY_GEN_CNT},
    {idt_sc_rio_msg_rx_rty,	false, false, true , TSI721_RETRY_RX_CNT},
    {idt_sc_rio_dma_tx,		false, true , true , TSI721_TXPKT_BDMA_CNT},
    {idt_sc_rio_dma_rx,		false, false, true , TSI721_RXRSP_BDMA_CNT},
    {idt_sc_rio_brg_tx,		false, true , true , TSI721_TXPKT_BRG_CNT},
    {idt_sc_rio_brg_rx,		false, false, true , TSI721_RXPKT_BRG_CNT},
    {idt_sc_rio_brg_rx_err,	false, false, true , TSI721_BRG_PKT_ERR_CNT},
	// The following 6 counters are 'split':  One counter register
	// holds two 16 bit counts.  The "total" count is always the most 
	// significant 16 bits of the counter, and the first counter 
	// specified in this list.  
    {idt_sc_rio_dbel_tx,    	true , true , true , TSI721_ODB_CNTX(0)},
    {idt_sc_rio_dbel_ok_rx,    	true , false, true , TSI721_SPLIT_2ND_CTR},
    {idt_sc_rio_dbel_tx, 	true , true , true , TSI721_ODB_CNTX(1)},
    {idt_sc_rio_dbel_ok_rx,    	true , false, true , TSI721_SPLIT_2ND_CTR},
    {idt_sc_rio_dbel_tx,  	true , true , true , TSI721_ODB_CNTX(2)},
    {idt_sc_rio_dbel_ok_rx,    	true , false, true , TSI721_SPLIT_2ND_CTR},
    {idt_sc_rio_dbel_tx,  	true , true , true , TSI721_ODB_CNTX(3)},
    {idt_sc_rio_dbel_ok_rx,    	true , false, true , TSI721_SPLIT_2ND_CTR},
    {idt_sc_rio_dbel_tx,  	true , true , true , TSI721_ODB_CNTX(4)},
    {idt_sc_rio_dbel_ok_rx,    	true , false, true , TSI721_SPLIT_2ND_CTR},
    {idt_sc_rio_dbel_tx,  	true , true , true , TSI721_ODB_CNTX(5)},
    {idt_sc_rio_dbel_ok_rx,    	true , false, true , TSI721_SPLIT_2ND_CTR},
    {idt_sc_rio_dbel_tx,  	true , true , true , TSI721_ODB_CNTX(6)},
    {idt_sc_rio_dbel_ok_rx,    	true , false, true , TSI721_SPLIT_2ND_CTR},
    {idt_sc_rio_dbel_tx,  	true , true , true , TSI721_ODB_CNTX(7)},
    {idt_sc_rio_dbel_ok_rx,    	true , false, true , TSI721_SPLIT_2ND_CTR},
    {idt_sc_rio_nwr_tx,		true , true , true , TSI721_NWR_CNT},
    {idt_sc_rio_nwr_ok_rx,	true , false, true , TSI721_SPLIT_2ND_CTR},
    {idt_sc_rio_mwr_tx,		true , true , true , TSI721_MWR_CNT},
    {idt_sc_rio_mwr_ok_rx,	true , false, true , TSI721_SPLIT_2ND_CTR}
};

#define TSI721_NUM_PERF_CTRS (sizeof(tsi721_dev_ctrs) / \
			sizeof(struct tsi721_dev_ctr))

uint32_t idt_tsi721_sc_init_dev_ctrs (
	 DAR_DEV_INFO_t				 *dev_info,
	 idt_sc_init_dev_ctrs_in_t  *in_parms,
	 idt_sc_init_dev_ctrs_out_t *out_parms)
{
	uint32_t rc = RIO_ERR_INVALID_PARAMETER;
	uint8_t cntr_i;
	idt_sc_ctr_val_t init_val = INIT_IDT_SC_CTR_VAL;
	struct DAR_ptl good_ptl;
	idt_sc_ctr_val_t *ctrs;

	out_parms->imp_rc = RIO_SUCCESS;

	if (NULL == in_parms->dev_ctrs) {
		  out_parms->imp_rc = SC_INIT_DEV_CTRS(0x01);
		  goto exit;
	}

	if (NULL == in_parms->dev_ctrs->p_ctrs) {
		out_parms->imp_rc = SC_INIT_DEV_CTRS(0x02);
		goto exit;
	};

	if (!in_parms->dev_ctrs->num_p_ctrs ||
			(in_parms->dev_ctrs->num_p_ctrs > IDT_MAX_PORTS) ||
			(in_parms->dev_ctrs->num_p_ctrs <
			in_parms->dev_ctrs->valid_p_ctrs)) {
		out_parms->imp_rc = SC_INIT_DEV_CTRS(0x03);
		goto exit;
	};

	rc = DARrioGetPortList(dev_info, &in_parms->ptl, &good_ptl);
	if ((RIO_SUCCESS != rc) || (TSI721_MAX_PORTS != good_ptl.num_ports)) {
		rc = RIO_ERR_INVALID_PARAMETER;
		out_parms->imp_rc = SC_INIT_DEV_CTRS(0x10);	
		goto exit;
	};

	ctrs = &in_parms->dev_ctrs->p_ctrs[0].ctrs[0];
	in_parms->dev_ctrs->valid_p_ctrs = TSI721_MAX_PORTS;
	in_parms->dev_ctrs->p_ctrs[0].pnum = 0;
	in_parms->dev_ctrs->p_ctrs[0].ctrs_cnt = TSI721_NUM_PERF_CTRS;
	for (cntr_i = 0; cntr_i < TSI721_NUM_PERF_CTRS; cntr_i++) {
		ctrs[cntr_i] = init_val;
		ctrs[cntr_i].sc = tsi721_dev_ctrs[cntr_i].ctr_t;
		ctrs[cntr_i].tx = tsi721_dev_ctrs[cntr_i].tx;
		ctrs[cntr_i].srio = tsi721_dev_ctrs[cntr_i].srio;
	};

	rc = RIO_SUCCESS;
exit:
	return rc;
};

uint32_t idt_tsi721_sc_read_ctrs(DAR_DEV_INFO_t  *dev_info,
                            idt_sc_read_ctrs_in_t    *in_parms,
                            idt_sc_read_ctrs_out_t   *out_parms)
{
	uint32_t rc = RIO_ERR_INVALID_PARAMETER;
	uint8_t cntr;
	struct DAR_ptl good_ptl;
	idt_sc_ctr_val_t *ctrs;

	out_parms->imp_rc = RIO_SUCCESS;

	if (NULL == in_parms->dev_ctrs) {
		out_parms->imp_rc = SC_READ_CTRS(0x01);
		goto exit;
	};

	if (NULL == in_parms->dev_ctrs->p_ctrs) {
		out_parms->imp_rc = SC_READ_CTRS(0x02);
		goto exit;
	};

	if (!in_parms->dev_ctrs->num_p_ctrs ||
		(in_parms->dev_ctrs->num_p_ctrs > IDT_MAX_PORTS) ||
		(in_parms->dev_ctrs->num_p_ctrs < in_parms->dev_ctrs->valid_p_ctrs)) {
		out_parms->imp_rc = SC_READ_CTRS(0x03);
		goto exit;
	};

	if (((RIO_ALL_PORTS == in_parms->ptl.num_ports) && (in_parms->dev_ctrs->num_p_ctrs < NUM_PORTS(dev_info))) ||
		((RIO_ALL_PORTS != in_parms->ptl.num_ports) && (in_parms->dev_ctrs->num_p_ctrs < in_parms->ptl.num_ports))) {
		out_parms->imp_rc = SC_READ_CTRS(0x04);
		goto exit;
	};

	rc = DARrioGetPortList(dev_info, &in_parms->ptl, &good_ptl);
	if ((RIO_SUCCESS != rc) || (TSI721_MAX_PORTS != good_ptl.num_ports)) {
		rc = RIO_ERR_INVALID_PARAMETER;
		out_parms->imp_rc = SC_READ_CTRS(0x10);
		goto exit;
	};

	// There's only one port, and one set of counters...
	ctrs = &in_parms->dev_ctrs->p_ctrs[0].ctrs[0];
	for (cntr = 0; cntr < TSI721_NUM_PERF_CTRS; cntr++) {
		uint32_t cnt, split_cnt;
		if (tsi721_dev_ctrs[cntr].split && !tsi721_dev_ctrs[cntr].os) {
			continue;
		};

		rc = DARRegRead(dev_info, tsi721_dev_ctrs[cntr].os, &cnt);
		if (RIO_SUCCESS != rc) {
			out_parms->imp_rc = SC_READ_CTRS(0x20 + cntr);
			goto exit;
		};

		if (!tsi721_dev_ctrs[cntr].split) {
			ctrs[cntr].last_inc = cnt;
			ctrs[cntr].total += cnt;
			continue;
		};
		split_cnt = (cnt & TSI721_MWR_CNT_MW_TOT_CNT) >> 16;
		cnt &= TSI721_MWR_CNT_MW_OK_CNT;
		ctrs[cntr].last_inc = split_cnt;
		ctrs[cntr].total += split_cnt;
		ctrs[cntr + 1].last_inc = cnt;
		ctrs[cntr + 1].total += cnt;
	};

	rc = RIO_SUCCESS;
exit:
	return rc;
}

/* Routine to bind in all Tsi721 specific Device Specific Function routines.
*/

uint32_t bind_tsi721_DSF_support( void )
{
    IDT_DSF_DB_t idt_driver;
 
	IDT_DSF_init_driver( &idt_driver );

    idt_driver.dev_type = TSI721_RIO_DEVICE_ID;

    idt_driver.idt_pc_clr_errs           = idt_tsi721_pc_clr_errs;
    idt_driver.idt_pc_dev_reset_config   = idt_tsi721_pc_dev_reset_config;
    idt_driver.idt_pc_get_config         = idt_tsi721_pc_get_config;
    idt_driver.idt_pc_get_status         = idt_tsi721_pc_get_status;
    idt_driver.idt_pc_reset_link_partner = idt_tsi721_pc_reset_link_partner;
    idt_driver.idt_pc_reset_port         = idt_tsi721_pc_reset_port;
    idt_driver.idt_pc_secure_port        = idt_tsi721_pc_secure_port;
    idt_driver.idt_pc_set_config         = idt_tsi721_pc_set_config;
    idt_driver.idt_pc_probe              = default_idt_pc_probe;

/*
    idt_driver.idt_rt_initialize      = idt_tsi721_rt_initialize;
    idt_driver.idt_rt_probe           = idt_tsi721_rt_probe;
    idt_driver.idt_rt_probe_all       = idt_tsi721_rt_probe_all;
    idt_driver.idt_rt_set_all         = idt_tsi721_rt_set_all;
    idt_driver.idt_rt_set_changed     = idt_tsi721_rt_set_changed;
    idt_driver.idt_rt_alloc_mc_mask   = IDT_DSF_rt_alloc_mc_mask;
    idt_driver.idt_rt_dealloc_mc_mask = IDT_DSF_rt_dealloc_mc_mask;
    idt_driver.idt_rt_change_rte      = idt_tsi721_rt_change_rte;
    idt_driver.idt_rt_change_mc_mask  = idt_tsi721_rt_change_mc_mask;

*/
    idt_driver.idt_em_cfg_pw       = idt_tsi721_em_cfg_pw       ;
    idt_driver.idt_em_cfg_set      = idt_tsi721_em_cfg_set      ;
    idt_driver.idt_em_cfg_get      = idt_tsi721_em_cfg_get      ;
    idt_driver.idt_em_dev_rpt_ctl  = idt_tsi721_em_dev_rpt_ctl  ;
    idt_driver.idt_em_parse_pw     = idt_tsi721_em_parse_pw     ;
    idt_driver.idt_em_get_int_stat = idt_tsi721_em_get_int_stat ;
    idt_driver.idt_em_get_pw_stat  = idt_tsi721_em_get_pw_stat  ;
    idt_driver.idt_em_clr_events   = idt_tsi721_em_clr_events   ;
    idt_driver.idt_em_create_events= idt_tsi721_em_create_events;

    idt_driver.idt_sc_init_dev_ctrs= idt_tsi721_sc_init_dev_ctrs;
    idt_driver.idt_sc_read_ctrs    = idt_tsi721_sc_read_ctrs;

    IDT_DSF_bind_driver( &idt_driver, &Tsi721_driver_handle);

    return RIO_SUCCESS;
}

#ifdef __cplusplus
}
#endif
