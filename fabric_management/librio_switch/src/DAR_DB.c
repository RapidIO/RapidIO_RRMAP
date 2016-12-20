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
/* Device Access Routine (DAR) Database (DB) Implementation

   The DAR DB implementation includes
   - Database variables and pointers to procedures
   - Stub and default implementations for device driver routines
   - Routines for DAR DB initialization and binding driver routines
*/

#include <stdbool.h>

#include "string_util.h"
#include "DAR_DB.h"
#include "DAR_DB_Private.h"
#include "DAR_Utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DAR_DB_MAX_INDEX (DAR_DB_MAX_DRIVERS-1)

DAR_DB_Driver_t driver_db[DAR_DB_MAX_DRIVERS];
static int first_free_driver;


/* For the local SRIO register access */
uint32_t (*ReadReg ) ( DAR_DEV_INFO_t *dev_info, uint32_t offset,
                                               uint32_t *readdata );
uint32_t (*WriteReg) ( DAR_DEV_INFO_t *dev_info, uint32_t  offset,
                                               uint32_t  writedata );

static void update_db_h( DAR_DEV_INFO_t *dev_info, int32_t idx )
{
    dev_info->db_h = ( VENDOR_ID( dev_info ) << 16 ) + idx ;
}


static uint32_t DARDB_ReadRegStub ( DAR_DEV_INFO_t *dev_info,
                                          uint32_t  offset  ,
                                          uint32_t *readdata )
{
    if ((NULL == dev_info) || (NULL == readdata))
       return RIO_ERR_NULL_PARM_PTR;

    if (offset > 0xFFFFFF)
       return RIO_ERR_INVALID_PARAMETER;

    *readdata = 0xFFFFFFFF;
    return RIO_STUBBED;
}


static uint32_t DARDB_WriteRegStub ( DAR_DEV_INFO_t *dev_info,
                                           uint32_t  offset  ,
                                           uint32_t  writedata )
{
    if (NULL == dev_info)
       return RIO_ERR_NULL_PARM_PTR;

    if ((offset > 0xFFFFFF) || (writedata != 0))
       return RIO_ERR_INVALID_PARAMETER;

    return RIO_STUBBED;
}


uint32_t DARDB_ReadReg( DAR_DEV_INFO_t *dev_info,
                                     uint32_t  offset,
                                     uint32_t *readdata )
{
    return ReadReg( dev_info, offset, readdata );
}


uint32_t DARDB_WriteReg( DAR_DEV_INFO_t *dev_info,
                                      uint32_t  offset,
                                      uint32_t  writedata )
{
    return WriteReg( dev_info, offset, writedata );
}

/* Standard HAL routines for system bringup, all of which use
   standard RapidIO registers.  The default routines will function
   correctly for devices which are compliant to the RapidIO defined
   register definitions.
*/
static uint32_t DARDB_rioGetNumLocalPorts( DAR_DEV_INFO_t *dev_info,
                                                 uint32_t *numLocalPorts )
{
    uint32_t rc = DARRegRead( dev_info, RIO_SW_PORT_INF, numLocalPorts );
	if (RIO_SUCCESS == rc) 
       *numLocalPorts = RIO_AVAIL_PORTS(*numLocalPorts);
	else
	   *numLocalPorts = 0;
	return rc;
}


static uint32_t DARDB_rioGetFeatures( DAR_DEV_INFO_t *dev_info,
                                            RIO_PE_FEAT_T *features )
{
    return DARRegRead( dev_info, RIO_PE_FEAT, features );
}


static uint32_t DARDB_rioGetSwitchPortInfo(      DAR_DEV_INFO_t *dev_info,
                                         RIO_SW_PORT_INF_T *portinfo  )
{
    uint32_t rc =  DARRegRead( dev_info, RIO_SW_PORT_INF, portinfo );

    if ( RIO_SUCCESS == rc )
        /* If this is not a switch or a multiport-endpoint, portinfo
           should be 0.  Fake the existance of the switch port info register
           by supplying 1 for the number of ports, and  0 as the port that
           we're connected to.
          
           Otherwise, leave portinfo alone.
        */
        if (   !(dev_info->features &
                (RIO_PE_FEAT_SW | RIO_PE_FEAT_MULTIP))
            &&  (! *portinfo ) )
              *portinfo = 0x00000100 ;

    return rc;
}


static uint32_t DARDB_rioGetExtFeaturesPtr( DAR_DEV_INFO_t *dev_info,
                                                  uint32_t *extfptr )
{
    uint32_t rc = RIO_ERR_FEATURE_NOT_SUPPORTED;

    if ( dev_info->features & RIO_PE_FEAT_EFB_VALID )
    {
        rc = RIO_SUCCESS;
        *extfptr = dev_info->assyInfo & RIO_ASSY_INF_EFB_PTR;
    };

    return rc;
}


static uint32_t DARDB_rioGetNextExtFeaturesPtr( DAR_DEV_INFO_t *dev_info,
                                                      uint32_t  currfptr,
                                                      uint32_t *extfptr )
{
    uint32_t rc = RIO_ERR_FEATURE_NOT_SUPPORTED;

    if ( currfptr & ( RIO_ASSY_INF_EFB_PTR >> 16) )
    {
        rc = DARRegRead( dev_info, currfptr, extfptr );
        *extfptr = (*extfptr & RIO_ASSY_INF_EFB_PTR) >> 16;
    };

    return rc;
}


static uint32_t DARDB_rioGetSourceOps( DAR_DEV_INFO_t *dev_info,
                                     RIO_SRC_OPS_T *srcops )
{
    return DARRegRead( dev_info, RIO_SRC_OPS, srcops );
}


static uint32_t DARDB_rioGetDestOps( DAR_DEV_INFO_t *dev_info,
                                     RIO_DST_OPS_T *dstops )
{
    return DARRegRead( dev_info, RIO_DST_OPS, dstops );
}


static uint32_t DARDB_rioGetAddressMode( DAR_DEV_INFO_t *dev_info,
                                        RIO_PE_ADDR_T *amode )
{
    uint32_t rc = RIO_ERR_FEATURE_NOT_SUPPORTED;

    if ( dev_info->features & RIO_PE_FEAT_EXT_ADDR )
        rc = DARRegRead( dev_info, RIO_PE_LL_CTL, amode ) ;

    return rc;
}


static uint32_t DARDB_rioGetBaseDeviceId( DAR_DEV_INFO_t *dev_info,
                                                uint32_t *deviceid )
{
    uint32_t rc = RIO_ERR_FEATURE_NOT_SUPPORTED;

    if ( dev_info->features & (RIO_PE_FEAT_PROC |
                               RIO_PE_FEAT_MEM  |
                               RIO_PE_FEAT_BRDG ) )
        rc = DARRegRead( dev_info, RIO_DEVID, deviceid ) ;

    return rc;
}


static uint32_t DARDB_rioSetBaseDeviceID( DAR_DEV_INFO_t *dev_info,
                                                       uint32_t  newdeviceid )
{
    uint32_t rc = RIO_ERR_FEATURE_NOT_SUPPORTED;

    if ( dev_info->features & (RIO_PE_FEAT_PROC |
                               RIO_PE_FEAT_MEM  |
                               RIO_PE_FEAT_BRDG) )
        rc = DARRegWrite( dev_info, RIO_DEVID, newdeviceid ) ;

    return rc;
}


static uint32_t DARDB_rioAcquireDeviceLock( DAR_DEV_INFO_t *dev_info,
                                                  uint16_t  hostdeviceid,
                                                  uint16_t *hostlockid )
{
    uint32_t regVal;

    uint32_t rc = DARRegRead( dev_info, RIO_HOST_LOCK,
                                     &regVal );

    if ((hostdeviceid == RIO_HOST_LOCK_DEVID) ||
        ( !hostlockid                                          )) {
        return RIO_ERR_INVALID_PARAMETER;
    };

    *hostlockid = hostdeviceid;

    // Only do the write if the lock is not already taken...
    if ((RIO_SUCCESS == rc) && ((uint16_t)(regVal) != hostdeviceid)) {
       rc = DARRegWrite( dev_info, RIO_HOST_LOCK,
                                       hostdeviceid );
       if (RIO_SUCCESS == rc)
       {
           rc = DARRegRead(dev_info, RIO_HOST_LOCK, &regVal);
           if ( RIO_SUCCESS == rc )
           {
               regVal &= RIO_HOST_LOCK_DEVID;
               if (regVal != hostdeviceid)
                   rc = RIO_ERR_LOCK;
   
               *hostlockid = (uint16_t)(regVal);
           }
       }
    }

    return rc;
}


static uint32_t DARDB_rioReleaseDeviceLock( DAR_DEV_INFO_t *dev_info,
                                                  uint16_t  hostdeviceid,
                                                  uint16_t *hostlockid )
{
    uint32_t regVal;
    uint32_t rc = DARRegWrite( dev_info,
                             RIO_HOST_LOCK,
                             hostdeviceid );

    if ( RIO_SUCCESS == rc )
    {
        rc = DARRegRead( dev_info, RIO_HOST_LOCK, &regVal ) ;

        if ( RIO_SUCCESS == rc )
        {
            *hostlockid = (uint16_t)(regVal & RIO_HOST_LOCK_DEVID);
            if ( 0xFFFF != *hostlockid )
                rc = RIO_ERR_LOCK ;
        }
    }

    return rc;
}


static uint32_t DARDB_rioGetComponentTag( DAR_DEV_INFO_t *dev_info,
                                                uint32_t *componenttag )
{
    return DARRegRead( dev_info, RIO_COMPTAG, componenttag );
}


static uint32_t DARDB_rioSetComponentTag( DAR_DEV_INFO_t *dev_info,
                                                uint32_t  componenttag )
{
    return DARRegWrite( dev_info, RIO_COMPTAG, componenttag );
}

static uint32_t DARDB_rioGetAddrMode( DAR_DEV_INFO_t *dev_info,
                                                RIO_PE_ADDR_T  *addr_mode )
{
    return DARRegRead( dev_info, RIO_PE_LL_CTL, addr_mode );
}


static uint32_t DARDB_rioSetAddrMode( DAR_DEV_INFO_t *dev_info,
                                                RIO_PE_ADDR_T  addr_mode )
{
    return DARRegWrite( dev_info, RIO_PE_LL_CTL, addr_mode );
}


static uint32_t DARDB_rioGetPortErrorStatus( DAR_DEV_INFO_t *dev_info,
                                                   uint16_t  portnum,
                                    RIO_SPX_ERR_STAT_T *err_status )
{
    uint32_t rc = RIO_ERR_FEATURE_NOT_SUPPORTED;

    if ( dev_info->extFPtrForPort )
    {
        if ( ( !portnum ) ||
             (portnum < RIO_AVAIL_PORTS(dev_info->swPortInfo) ) )
            rc = DARRegRead( dev_info,
                             RIO_SPX_ERR_STAT( dev_info->extFPtrForPort,
				dev_info->extFPtrPortType, portnum),
                             err_status );
        else
            rc = RIO_ERR_INVALID_PARAMETER ;
    }

    return rc;
}

static uint32_t DARDB_rioLinkReqNResp( DAR_DEV_INFO_t *dev_info, 
                                              uint8_t  portnum, 
                               RIO_SPX_LM_RESP_STAT_T *link_stat )
{
    uint32_t rc = RIO_ERR_FEATURE_NOT_SUPPORTED;
	uint8_t attempts;
	uint32_t err_n_stat;

    if ( dev_info->extFPtrForPort )
    {
        if ( ( !portnum ) ||
             (portnum < RIO_AVAIL_PORTS(dev_info->swPortInfo) ) ) {
			rc = DARRegRead( dev_info,
				 RIO_SPX_ERR_STAT( dev_info->extFPtrForPort,
					dev_info->extFPtrPortType, portnum),
							 &err_n_stat );
			 if (RIO_SUCCESS != rc)
				 goto exit;
			 if (!RIO_PORT_OK(err_n_stat)) {
				rc = RIO_ERR_PORT_OK_NOT_DETECTED;
				goto exit;
			}

			 rc = DARRegWrite( dev_info,
                               RIO_SPX_LM_REQ( dev_info->extFPtrForPort,
                                         dev_info->extFPtrPortType, portnum),
                               RIO_SPX_LM_REQ_CMD_LR_IS );
			 if (RIO_SUCCESS != rc)
				 goto exit;
		   /* Note that typically a link-response will be received faster than another
		    * maintenance packet can be issued.  Nevertheless, the routine polls 10 times
			* to check for a valid response, where 10 is a small number assumed to give 
			* enough time for even the slowest device to respond.
            */
		   for (attempts = 0; attempts < 10; attempts++) {
				rc = DARRegRead( dev_info,
			  	RIO_SPX_LM_RESP( dev_info->extFPtrForPort,
						dev_info->extFPtrPortType, 
						portnum),
								 link_stat );

				if (RIO_SUCCESS != rc) 
					goto exit;

			    if (RIO_SPX_LM_RESP_IS_VALID(*link_stat))
					goto exit;
          }
		   rc = RIO_ERR_NOT_EXPECTED_RETURN_VALUE;
        } else {
            rc = RIO_ERR_INVALID_PARAMETER ;
		}
    }
exit:
    return rc;
}

static uint32_t DARDB_rioStdRouteAddEntry( DAR_DEV_INFO_t *dev_info,
                                                uint16_t  routedestid,
                                                 uint8_t  routeportno  )
{
    uint32_t rc = RIO_ERR_NO_SWITCH;

    if ( dev_info->features & RIO_PE_FEAT_SW )
    {
        rc = DARRegWrite( dev_info,
                          RIO_DEVID_RTE,
                          routedestid ) ;
        if ( RIO_SUCCESS == rc )
            rc = DARRegWrite( dev_info,
                              RIO_RTE,
                              routeportno ) ;
    }

    return rc;
}


static uint32_t DARDB_rioStdRouteGetEntry( DAR_DEV_INFO_t *dev_info,
                                                 uint16_t  routedestid,
                                                  uint8_t *routeportno )
{
    uint32_t rc = RIO_ERR_NO_SWITCH;
    uint32_t regVal;

    if ( dev_info->features & RIO_PE_FEAT_SW )
    {
        rc = DARRegWrite( dev_info,
                          RIO_DEVID_RTE,
                          routedestid ) ;
        if ( RIO_SUCCESS == rc )
        {
            rc = DARRegRead( dev_info,
                             RIO_RTE,
                            &regVal ) ;
            *routeportno = (uint8_t)( regVal & RIO_RTE_PORT );
        }
    }

    return rc;
}


/* Standard HAL routines for system bringup, none of which use RapidIO
   standard functions/values
  
   Device drivers MUST implement their own versions of these routines.
*/
static uint32_t DARDB_rioStdRouteInitAll( DAR_DEV_INFO_t *dev_info,
                                                 uint8_t  routeportno )
{
    uint32_t rc = RIO_ERR_NO_SWITCH;
    int32_t num_dests = ( dev_info->swRtInfo & RIO_SW_RT_TBL_LIM_MAX_DESTID );
    int32_t dest_idx;

    if ( (dev_info->swRtInfo) && (dev_info->features & RIO_PE_FEAT_SW) )
    {
        rc = DARrioStdRouteSetDefault( dev_info, routeportno ) ;
        for ( dest_idx = 0; ( (dest_idx <= num_dests) && (RIO_SUCCESS == rc) );
              dest_idx++ )
            rc = DARrioStdRouteAddEntry( dev_info, dest_idx, routeportno ) ;
    }

    return rc;
}

static uint32_t DARDB_rioStdRouteRemoveEntry( DAR_DEV_INFO_t *dev_info,
                                                    uint16_t  routedestid )
{
    uint32_t rc = RIO_ERR_NO_SWITCH;

    if ( dev_info->features & RIO_PE_FEAT_SW )
    {
        rc = DARRegWrite( dev_info,
                          RIO_DEVID_RTE,
                          routedestid ) ;
        if ( RIO_SUCCESS == rc )
            rc = DARRegWrite( dev_info,
                              RIO_RTE,
                              RIO_ALL_PORTS ) ;
    }

    return rc;
}


static uint32_t DARDB_rioStdRouteSetDefault( DAR_DEV_INFO_t *dev_info,
                                                    uint8_t  routeportno )
{
    uint32_t rc = RIO_ERR_NO_SWITCH;

    if ( dev_info->features & RIO_PE_FEAT_SW )
        rc = DARRegWrite( dev_info, RIO_DFLT_RTE, routeportno ) ;

    return rc;
}

static uint32_t DARDB_rioSetAssmblyInfo( DAR_DEV_INFO_t *dev_info, 
                                               uint32_t  AsmblyVendID, 
                                               uint16_t  AsmblyRev    )
{
    if (NULL == dev_info)
       return RIO_ERR_NULL_PARM_PTR;

    if (AsmblyVendID || AsmblyRev)
       return RIO_ERR_FEATURE_NOT_SUPPORTED;

    return RIO_ERR_FEATURE_NOT_SUPPORTED;
}


static uint32_t DARDB_rioGetAssmblyInfo ( DAR_DEV_INFO_t *dev_info, 
		                                uint32_t *AsmblyVendID,
				                uint16_t *AsmblyRev)
{
    uint32_t rc = DARRegRead( dev_info, RIO_ASSY_ID, AsmblyVendID );
    if (RIO_SUCCESS == rc) {
       uint32_t temp;
       rc = DARRegRead( dev_info, RIO_ASSY_INF, &temp );
       if (RIO_SUCCESS == rc)  {
          temp = (temp & RIO_ASSY_INF_ASSY_REV) >> 16;
          *AsmblyRev = (uint16_t)(temp);
       };
    };

    return rc;
}

uint32_t DARDB_rioGetPortListDefault ( DAR_DEV_INFO_t  *dev_info ,
									struct DAR_ptl	*ptl_in,
									struct DAR_ptl	*ptl_out )
{
   uint8_t idx;
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;
   bool dup_port_num[DAR_MAX_PORTS] = {false};

   ptl_out->num_ports = 0;

   if ((ptl_in->num_ports > NUM_PORTS(dev_info)) && (ptl_in->num_ports != RIO_ALL_PORTS))
	   goto exit;
   if ((ptl_in->num_ports > DAR_MAX_PORTS) && (ptl_in->num_ports != RIO_ALL_PORTS))
	   goto exit;

   if (!(ptl_in->num_ports))
	   goto exit;

   if (RIO_ALL_PORTS == ptl_in->num_ports) {
      ptl_out->num_ports = NUM_PORTS(dev_info);
      for (idx = 0; idx < NUM_PORTS(dev_info); idx++)
         ptl_out->pnums[idx] = idx;
   } else {
      ptl_out->num_ports = ptl_in->num_ports;
      for (idx = 0; idx < ptl_in->num_ports; idx++) {
         ptl_out->pnums[idx] = ptl_in->pnums[idx];
	     if ((ptl_out->pnums[idx] >= NUM_PORTS(dev_info)) ||
	        (dup_port_num[ptl_out->pnums[idx]]          )) {
			ptl_out->num_ports = 0;
			rc = RIO_ERR_PORT_ILLEGAL(idx);
	        goto exit;
	     };
	     dup_port_num[ptl_out->pnums[idx]] = true;
      };
   };
   rc = RIO_SUCCESS;

exit:
   return rc;
};

static uint32_t DARDB_rioSetEnumBound( DAR_DEV_INFO_t *dev_info,
                                     struct DAR_ptl *ptl,
					int enum_bnd_val )
{
    uint32_t rc = RIO_ERR_FEATURE_NOT_SUPPORTED;
    uint32_t currCSR, tempCSR;
	struct DAR_ptl good_ptl;
	uint8_t idx;
    enum_bnd_val = !!enum_bnd_val;
	
    if ( dev_info->extFPtrForPort )
    {
		rc = DARrioGetPortList( dev_info, ptl, &good_ptl);
		if (rc != RIO_SUCCESS)
			goto exit;

        for ( idx = 0; idx < good_ptl.num_ports; idx++) 
        {
            rc = DARRegRead( dev_info,
                             RIO_SPX_CTL( dev_info->extFPtrForPort,
						dev_info->extFPtrPortType, 
                                                     good_ptl.pnums[idx] ),
                            &currCSR ) ;
            if ( RIO_SUCCESS == rc )
            {
		if (enum_bnd_val) {
                	tempCSR = currCSR | RIO_SPX_CTL_ENUM_B;
		} else {
                	tempCSR = currCSR & ~RIO_SPX_CTL_ENUM_B;
		};
		if (tempCSR == currCSR)
			continue;

                rc = DARRegWrite( dev_info,
                                  RIO_SPX_CTL(
                                                      dev_info->extFPtrForPort,
						dev_info->extFPtrPortType, 
                                                      good_ptl.pnums[idx] ),
                                  tempCSR ) ;
                if ( RIO_SUCCESS == rc )
                {
                    rc = DARRegRead( dev_info,
                                     RIO_SPX_CTL(
                                                      dev_info->extFPtrForPort,
						dev_info->extFPtrPortType, 
                                                      good_ptl.pnums[idx] ),
                                    &tempCSR ) ;
                    if ( tempCSR != currCSR )
                        rc = RIO_ERR_FEATURE_NOT_SUPPORTED ;
                }
            }
        }
    }
exit:
    return rc;
}


static uint32_t DARDB_rioPortEnable( DAR_DEV_INFO_t *dev_info,
                                   struct DAR_ptl *ptl, 
                                            bool    port_ena,
                                            bool    port_lkout,
                                            bool    in_out_ena )
{
    uint32_t rc = RIO_SUCCESS;
    uint32_t port_n_ctrl1_addr;
    uint32_t port_n_ctrl1_reg;
	struct DAR_ptl good_ptl;
	uint8_t idx;

    /* Check whether 'portno' is assigned to a valid port value or not
    */
    rc = DARrioGetPortList( dev_info, ptl, &good_ptl);
	if (rc != RIO_SUCCESS)
		goto exit;

	for (idx = 0; idx < good_ptl.num_ports; idx++) {
		port_n_ctrl1_addr = 
			RIO_SPX_CTL( dev_info->extFPtrForPort,
						dev_info->extFPtrPortType, 
									good_ptl.pnums[idx] ) ; 
		rc = DARRegRead( dev_info, port_n_ctrl1_addr, &port_n_ctrl1_reg ) ;
		if ( RIO_SUCCESS != rc )
			goto exit;

		if ( port_ena )
			port_n_ctrl1_reg &= ~RIO_SPX_CTL_PORT_DIS ;
		else
			port_n_ctrl1_reg |= RIO_SPX_CTL_PORT_DIS ;

		if ( port_lkout )
		   port_n_ctrl1_reg |=  RIO_SPX_CTL_LOCKOUT;
		else
		   port_n_ctrl1_reg &= ~RIO_SPX_CTL_LOCKOUT;

		if ( in_out_ena )
			port_n_ctrl1_reg |= RIO_SPX_CTL_INP_EN | RIO_SPX_CTL_OTP_EN ;
		else
			port_n_ctrl1_reg &= ~(RIO_SPX_CTL_INP_EN | RIO_SPX_CTL_OTP_EN);

		rc = DARRegWrite( dev_info, port_n_ctrl1_addr, port_n_ctrl1_reg );
	};
exit:
    return rc;
}

static uint32_t DARDB_rioEmergencyLockout( DAR_DEV_INFO_t *dev_info, uint8_t port_num )
{
   uint32_t rc = RIO_ERR_INVALID_PARAMETER;

   if (dev_info->extFPtrForPort && RIO_SP_VLD(dev_info->extFPtrPortType) &&
      (port_num < NUM_PORTS(dev_info))) {
      rc = DARRegWrite( dev_info, 
		        RIO_SPX_CTL(dev_info->extFPtrForPort,
						dev_info->extFPtrPortType, 
				port_num), 
		        dev_info->ctl1_reg[port_num] | RIO_SPX_CTL_LOCKOUT );
   };
   return rc;
} 

static uint32_t DARDB_rioDeviceSupportedStub( DAR_DEV_INFO_t *dev_info )
{
    if (NULL == dev_info)
       return RIO_ERR_NULL_PARM_PTR;

    return DAR_DB_NO_DRIVER;
}


/* DARDB_rioDeviceSupportedDefault updates the database handle field,
   db_h, in dev_info to reflect the passed in index.
  
   The dev_info->devID field must be valid when this routine is called.

   Default driver for a device is the DARDB_ routines defined above.
   This routine is the last driver bound in to the DAR DB, so it will
   allow otherwise 'unsupported' devices to get minimal support.
  
   This routine should also be called directly by all devices to ensure that
   their DAR_DEV_INFO_t fields are correctly filled in.  Device specific
   fixups can be added after a call to this routine.
*/
uint32_t DARDB_rioDeviceSupportedDefault( DAR_DEV_INFO_t *dev_info )
{
    uint32_t rc;

    /* The reading devID should be unnecessary,
       but it should not hurt to do so here.
    */
    rc = ReadReg( dev_info, RIO_DEV_IDENT, &dev_info->devID ) ;
    if ( RIO_SUCCESS != rc )
        return rc;

    rc = ReadReg( dev_info, RIO_DEV_INF, &dev_info->devInfo ) ;
    if ( RIO_SUCCESS != rc )
        return rc;

    rc = ReadReg( dev_info, RIO_ASSY_INF, &dev_info->assyInfo ) ;
    if ( RIO_SUCCESS != rc )
        return rc;

    rc = ReadReg( dev_info, RIO_PE_FEAT, &dev_info->features ) ;
    if ( RIO_SUCCESS != rc )
        return rc;

    /* swPortInfo can be supported by endpoints.
       May as well read the register...
    */
    rc = ReadReg( dev_info, RIO_SW_PORT_INF, &dev_info->swPortInfo ) ;
    if ( RIO_SUCCESS != rc )
        return rc;

    if ( !dev_info->swPortInfo )
        dev_info->swPortInfo = 0x00000100 ; /* Default for endpoints is 1 port
                                            */

    if ( dev_info->features & RIO_PE_FEAT_SW )
    {
        rc = ReadReg( dev_info, RIO_SW_RT_TBL_LIM, &dev_info->swRtInfo ) ;
        if ( RIO_SUCCESS != rc )
            return rc;
    }

    rc = ReadReg( dev_info, RIO_SRC_OPS, &dev_info->srcOps ) ;
    if ( RIO_SUCCESS != rc )
        return rc;

    rc = ReadReg( dev_info, RIO_DST_OPS, &dev_info->dstOps ) ;
    if ( RIO_SUCCESS != rc )
        return rc;

    if (dev_info->features & RIO_PE_FEAT_MC)
    {
	rc = ReadReg(dev_info, RIO_SW_MC_INF, &dev_info->swMcastInfo);
        if ( RIO_SUCCESS != rc )
            return rc;
    }

    if ( dev_info->features & RIO_PE_FEAT_EFB_VALID )
    {
        uint32_t curr_ext_feat;
        uint32_t prev_addr;

        rc = DARDB_rioGetExtFeaturesPtr( dev_info, &prev_addr ) ;
        while ( ( RIO_SUCCESS == rc ) && prev_addr )
        {
            rc = ReadReg(dev_info, prev_addr, &curr_ext_feat);
            if (RIO_SUCCESS != rc)
                return rc;

            switch( curr_ext_feat & RIO_EFB_T )
            {
                case RIO_EFB_T_SP_EP           :
                case RIO_EFB_T_SP_EP_SAER      :
                case RIO_EFB_T_SP_NOEP      :
                case RIO_EFB_T_SP_NOEP_SAER :
	            dev_info->extFPtrPortType = curr_ext_feat & 
			                         RIO_EFB_T;
                    dev_info->extFPtrForPort = prev_addr;
                    break;

                case RIO_EFB_T_EMHS :
                    dev_info->extFPtrForErr = prev_addr;
                    break;

                case RIO_EFB_T_HS:
                    dev_info->extFPtrForHS = prev_addr;
                    break;

                case RIO_EFB_T_LANE :
                    dev_info->extFPtrForLane = prev_addr;
                    break;

                case RIO_EFB_T_VC:
                    dev_info->extFPtrForLane = prev_addr;
                    break;

                case RIO_EFB_T_V0Q:
                    dev_info->extFPtrForVOQ = prev_addr;
                    break;

                case RIO_EFB_T_RT:
                    dev_info->extFPtrForRT = prev_addr;
                    break;

                case RIO_EFB_T_TS:
                    dev_info->extFPtrForTS = prev_addr;
                    break;

                case RIO_EFB_T_MISC:
                    dev_info->extFPtrForMISC = prev_addr;
                    break;

                default:
					if (0xFFFFFFFF == curr_ext_feat) {
						// Register access has failed.
						rc = RIO_ERR_ACCESS;
						goto DARDB_rioDeviceSupportedDefault_fail;
					};
                    break;
            }
            prev_addr = curr_ext_feat >> 16 ;
        }
    }
DARDB_rioDeviceSupportedDefault_fail:
    return rc;
}


static uint32_t DARDB_rioGetDevResetInitStatus( DAR_DEV_INFO_t *dev_info )
{
    if (NULL == dev_info)
       return RIO_ERR_NULL_PARM_PTR;

    return RIO_SUCCESS;
}


uint32_t DAR_Find_Driver_for_Device( bool    dev_info_devID_valid,
                           DAR_DEV_INFO_t *dev_info )
{
    int32_t driverIdx;
    uint32_t rc = RIO_SUCCESS;

    SAFE_STRNCPY(dev_info->name, "UNKNOWN", sizeof(dev_info->name));

    /* If dev_info_devID_valid is true, we are using a static devID instead
         of probing.  Otherwise, we are probing a SRIO device to get a devID
    */
    if ( !dev_info_devID_valid )
    {
        rc = ReadReg( dev_info, RIO_DEV_IDENT, &dev_info->devID ) ;
        if ( RIO_SUCCESS != rc )
			goto exit;
    };

    rc = DAR_DB_NO_DRIVER ;

    /* Go through drivers until one acknowledges that it can support
       the targetted device.
      
       Worst case, expect that the default driver will be used to support
       the device.
    */
    for ( driverIdx = 0;
         ( (driverIdx < DAR_DB_MAX_DRIVERS) &&
           (DAR_DB_NO_DRIVER == rc        ) );
          driverIdx++ )
    {
        update_db_h( dev_info, driverIdx ) ;
        rc = driver_db[driverIdx].rioDeviceSupported( dev_info ) ;
    }

	if (RIO_SUCCESS == rc) {
		// NOTE: All register manipulations must be done with DARReadReg and DARWriteReg,
		// or the application must manage dev_info->ctl1_reg themselves manually.  Otherwise, the
		// EmergencyLockout call may have unintended consequences.
    
		if ( dev_info->extFPtrForPort ) {
		   uint32_t ctl1;
		   uint8_t  idx;
		   for (idx = 0; idx < NUM_PORTS(dev_info); idx++) {
			  rc = ReadReg(dev_info,
				RIO_SPX_CTL(dev_info->extFPtrForPort, 
					dev_info->extFPtrPortType, idx), &ctl1);
			  if ( RIO_SUCCESS != rc )
				 goto exit;
			   dev_info->ctl1_reg[idx] = ctl1;
			}; 
		}; 
	};
exit:
    return rc;
}


static uint32_t DARDB_rioDeviceRemoved( DAR_DEV_INFO_t *dev_info )
{
    uint32_t rc = RIO_SUCCESS;

    if ( NULL != dev_info->privateData )
        rc = DAR_DB_DRIVER_INFO ;

    return rc;
}

/* Initialize driver_db with defined max number of driver */

void init_DAR_driver( DAR_DB_Driver_t *DAR_info )
{
    DAR_info->ReadReg                  = DARDB_ReadReg ;
    DAR_info->WriteReg                 = DARDB_WriteReg ;
    DAR_info->WaitSec                  = DAR_WaitSec ;

    DAR_info->rioGetNumLocalPorts      = DARDB_rioGetNumLocalPorts ;
    DAR_info->rioGetFeatures           = DARDB_rioGetFeatures ;
    DAR_info->rioGetSwitchPortInfo     = DARDB_rioGetSwitchPortInfo ;
    DAR_info->rioGetExtFeaturesPtr     = DARDB_rioGetExtFeaturesPtr ;
    DAR_info->rioGetNextExtFeaturesPtr = DARDB_rioGetNextExtFeaturesPtr ;
    DAR_info->rioGetSourceOps          = DARDB_rioGetSourceOps ;
    DAR_info->rioGetDestOps            = DARDB_rioGetDestOps ;
    DAR_info->rioGetAddressMode        = DARDB_rioGetAddressMode ;
    DAR_info->rioGetBaseDeviceId       = DARDB_rioGetBaseDeviceId ;
    DAR_info->rioSetBaseDeviceId       = DARDB_rioSetBaseDeviceID ;
    DAR_info->rioAcquireDeviceLock     = DARDB_rioAcquireDeviceLock ;
    DAR_info->rioReleaseDeviceLock     = DARDB_rioReleaseDeviceLock ;
    DAR_info->rioGetComponentTag       = DARDB_rioGetComponentTag ;
    DAR_info->rioSetComponentTag       = DARDB_rioSetComponentTag ;
    DAR_info->rioGetAddrMode           = DARDB_rioGetAddrMode ;
    DAR_info->rioSetAddrMode           = DARDB_rioSetAddrMode ;
    DAR_info->rioGetPortErrorStatus    = DARDB_rioGetPortErrorStatus ;
    DAR_info->rioLinkReqNResp          = DARDB_rioLinkReqNResp;

    DAR_info->rioStdRouteAddEntry      = DARDB_rioStdRouteAddEntry ;
    DAR_info->rioStdRouteGetEntry      = DARDB_rioStdRouteGetEntry ;

    DAR_info->rioStdRouteInitAll       = DARDB_rioStdRouteInitAll ;
    DAR_info->rioStdRouteRemoveEntry   = DARDB_rioStdRouteRemoveEntry ;
    DAR_info->rioStdRouteSetDefault    = DARDB_rioStdRouteSetDefault ;
    DAR_info->rioSetAssmblyInfo        = DARDB_rioSetAssmblyInfo ;
    DAR_info->rioGetAssmblyInfo        = DARDB_rioGetAssmblyInfo ;
    DAR_info->rioGetPortList           = DARDB_rioGetPortListDefault;
    DAR_info->rioSetEnumBound          = DARDB_rioSetEnumBound ;
    DAR_info->rioGetDevResetInitStatus = DARDB_rioGetDevResetInitStatus ;

    DAR_info->rioPortEnable            = DARDB_rioPortEnable;
    DAR_info->rioEmergencyLockout      = DARDB_rioEmergencyLockout;

    DAR_info->rioDeviceSupported       = DARDB_rioDeviceSupportedStub ;
    DAR_info->rioDeviceRemoved         = DARDB_rioDeviceRemoved ;
}

uint32_t DARDB_init()
{
    int32_t idx;

    ReadReg            = DARDB_ReadRegStub ;
    WriteReg           = DARDB_WriteRegStub ;

    first_free_driver = 0 ; /* Nothing bound yet. */

    for ( idx = 0; idx < DAR_DB_MAX_DRIVERS; idx++ )
    {
		init_DAR_driver( &driver_db[idx] );
    }

    driver_db[DAR_DB_MAX_INDEX].rioDeviceSupported
                                              = DARDB_rioDeviceSupportedDefault;
    driver_db[DAR_DB_MAX_INDEX].db_h          = DAR_DB_MAX_INDEX ;

    return RIO_SUCCESS;
}


void DARDB_Init_Device_Info( DAR_DEV_INFO_t *dev_info )
{
    dev_info->db_h           = 0 ;
    dev_info->privateData    = 0 ;
    dev_info->accessInfo     = 0 ;
    dev_info->assyInfo       = 0 ;
    dev_info->extFPtrForPort = 0 ;
    dev_info->extFPtrPortType= 0 ;
    dev_info->extFPtrForLane = 0 ;
    dev_info->extFPtrForErr  = 0 ;
    dev_info->extFPtrForHS   = 0 ;
    dev_info->extFPtrForVC   = 0 ;
    dev_info->extFPtrForVOQ  = 0 ;
    dev_info->extFPtrForRT   = 0 ;
    dev_info->extFPtrForTS   = 0 ;
    dev_info->extFPtrForMISC = 0 ;

}


uint32_t DARDB_Init_Driver_Info( uint32_t VendorID, DAR_DB_Driver_t *DAR_info )
{
    uint32_t rc = DAR_DB_NO_HANDLES;

    /* Preserve the last index for the default device driver */
    if ( first_free_driver < DAR_DB_MAX_INDEX )
    {
        DAR_info->db_h = ( (VendorID & 0x0000FFFF) << 16 ) + first_free_driver ;
        first_free_driver++ ;
        rc = RIO_SUCCESS ;
    }

    init_DAR_driver( DAR_info );
    return rc;
}


uint32_t DARDB_Bind_Driver( DAR_DB_Driver_t *dev_info )
{

    driver_db[DAR_DB_INDEX(dev_info)] = *dev_info; 

    return RIO_SUCCESS;
}

#ifdef __cplusplus
}
#endif
