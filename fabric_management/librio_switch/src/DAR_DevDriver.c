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
#include<DAR_DB.h>
#include<DAR_DB_Private.h>

#ifdef __cplusplus
extern "C" {
#endif

const struct DAR_ptl ptl_all_ports = PTL_ALL_PORTS;

/* Device Access Routine (DAR) Device Driver routines
  
   This file contains the implementation of all of the device driver
   routines for the DAR.  These routines all have the same form:
   - Validate the dev_info parameter passed in
   - Invoke the device driver routine bound into the DAR DB, based on the
     dev_info device handle information.
*/

uint32_t update_dev_info_regvals( DAR_DEV_INFO_t *dev_info, uint32_t offset, uint32_t reg_val ) {

  uint32_t rc = RIO_SUCCESS;

  if (dev_info->extFPtrForPort && RIO_SP_VLD(dev_info->extFPtrPortType)) {
     if ((offset >= RIO_SPX_CTL(dev_info->extFPtrForPort, dev_info->extFPtrPortType, 0                      )) && 
         (offset <= RIO_SPX_CTL(dev_info->extFPtrForPort, dev_info->extFPtrPortType, (NUM_PORTS(dev_info) - 1)))) {
        if ((0x1C == (offset & 0x1C) && !RIO_SP3_VLD(dev_info->extFPtrPortType)) ||
            (0x3C == (offset & 0x3C) && RIO_SP3_VLD(dev_info->extFPtrPortType))) {
	   uint8_t idx;

           idx = (offset - RIO_SPX_CTL(dev_info->extFPtrForPort, dev_info->extFPtrPortType, 0)) /
	 			RIO_SP_STEP(dev_info->extFPtrPortType);
           if (idx >= NUM_PORTS(dev_info)) { 
              rc = RIO_ERR_SW_FAILURE;
           } else {
              dev_info->ctl1_reg[idx] = reg_val;
           };
        };
     };
  };

  return rc;
};

uint32_t DARRegRead( DAR_DEV_INFO_t *dev_info, uint32_t offset, uint32_t *readdata )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) ) {
       rc = driver_db[DAR_DB_INDEX(dev_info)].ReadReg( dev_info,
                                                       offset,
                                                       readdata );
	   /* Note: update_dev_info_regvals, as the registers managed should not change
	    * after being written.  On some devices (i.e. Tsi57x), reading these registers
		* under certain conditions (port 0 powerdown) will not return the correct value.
		*/
   };
   return rc;
}



uint32_t DARRegWrite( DAR_DEV_INFO_t *dev_info, uint32_t offset, uint32_t writedata )
{
	uint32_t rc = DAR_DB_INVALID_HANDLE;

	if ( VALIDATE_DEV_INFO( dev_info ) ) {
		rc = driver_db[DAR_DB_INDEX(dev_info)].WriteReg( dev_info,
								offset,
								writedata );
	};
	if (RIO_SUCCESS == rc) {
		rc = update_dev_info_regvals( dev_info, offset, writedata );
	};
	return rc;
}


uint32_t DARrioGetNumLocalPorts( DAR_DEV_INFO_t *dev_info, uint32_t *numLocalPorts )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioGetNumLocalPorts( dev_info,
                                                               numLocalPorts );
   return rc;
}


uint32_t DARrioGetFeatures( DAR_DEV_INFO_t *dev_info, RIO_PE_FEAT_T *features )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioGetFeatures( dev_info,
                                                              features );
   return rc;
}


uint32_t DARrioGetSwitchPortInfo( DAR_DEV_INFO_t *dev_info,
                         RIO_SW_PORT_INF_T *portinfo  )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioGetSwitchPortInfo( dev_info,
                                                                    portinfo );
   return rc;
}


uint32_t DARrioGetExtFeaturesPtr( DAR_DEV_INFO_t *dev_info, uint32_t *extfptr )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioGetExtFeaturesPtr( dev_info,
                                                                    extfptr );
   else
       *extfptr = 0x00000000;
   return rc;
}


uint32_t DARrioGetNextExtFeaturesPtr( DAR_DEV_INFO_t *dev_info,
                                           uint32_t  currfptr,
                                           uint32_t *extfptr )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioGetNextExtFeaturesPtr(
                                                                     dev_info,
                                                                     currfptr,
                                                                     extfptr );
   else
       *extfptr = 0x00000000;
   return rc;
}


uint32_t DARrioGetSourceOps( DAR_DEV_INFO_t *dev_info, RIO_SRC_OPS_T *srcops )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioGetSourceOps( dev_info,
                                                               srcops );
   return rc;
}


uint32_t DARrioGetDestOps( DAR_DEV_INFO_t *dev_info, RIO_DST_OPS_T *dstops )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioGetDestOps( dev_info, dstops);
   return rc;
}


uint32_t DARrioGetAddressMode( DAR_DEV_INFO_t *dev_info, RIO_PE_ADDR_T *amode )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioGetAddressMode( dev_info,
                                                                 amode );
   return rc;
}


uint32_t DARrioGetBaseDeviceId( DAR_DEV_INFO_t *dev_info, uint32_t *deviceid )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioGetBaseDeviceId( dev_info,
                                                                  deviceid );
   return rc;
}


uint32_t DARrioSetBaseDeviceId( DAR_DEV_INFO_t *dev_info, uint32_t newdeviceid )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioSetBaseDeviceId( dev_info,
                                                                  newdeviceid);
   return rc;
}


uint32_t DARrioAcquireDeviceLock( DAR_DEV_INFO_t *dev_info,
                                       uint16_t  hostdeviceid,
                                       uint16_t *hostlockid )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioAcquireDeviceLock( dev_info,
                                                                hostdeviceid,
                                                                  hostlockid );
   return rc;
}


uint32_t DARrioReleaseDeviceLock( DAR_DEV_INFO_t *dev_info,
                                       uint16_t  hostdeviceid,
                                       uint16_t *hostlockid )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioReleaseDeviceLock( dev_info,
                                                                hostdeviceid,
                                                                  hostlockid );
   return rc;
}


uint32_t DARrioGetComponentTag( DAR_DEV_INFO_t *dev_info, uint32_t *componenttag )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioGetComponentTag( dev_info,
                                                              componenttag );
   return rc;
}


uint32_t DARrioSetComponentTag( DAR_DEV_INFO_t *dev_info, uint32_t componenttag )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioSetComponentTag( dev_info,
                                                              componenttag );
   return rc;
}

uint32_t DARrioGetAddrMode( DAR_DEV_INFO_t *dev_info, RIO_PE_ADDR_T *addr_mode )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioGetAddrMode( dev_info,
                                                              addr_mode );
   return rc;
}

uint32_t DARrioSetAddrMode( DAR_DEV_INFO_t *dev_info, RIO_PE_ADDR_T addr_mode )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioSetAddrMode( dev_info,
                                                              addr_mode );
   return rc;
}


uint32_t DARrioGetPortErrorStatus( DAR_DEV_INFO_t *dev_info,
                                         uint8_t  portnum,
                         RIO_SPX_ERR_STAT_T *err_status )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioGetPortErrorStatus(dev_info,
                                                                     portnum,
                                                                  err_status );
   return rc;
}

uint32_t DARrioLinkReqNResp ( DAR_DEV_INFO_t *dev_info, 
                                     uint8_t  portnum, 
                                    uint32_t *link_stat )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioLinkReqNResp(dev_info, portnum, link_stat );
   return rc;
}

uint32_t DARrioStdRouteAddEntry( DAR_DEV_INFO_t *dev_info,
                                      uint16_t  routedestid,
                                       uint8_t  routeportno  )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioStdRouteAddEntry( dev_info,
                                                                routedestid,
                                                                routeportno );
   return rc;
}


uint32_t DARrioStdRouteGetEntry( DAR_DEV_INFO_t *dev_info,
                                      uint16_t  routedestid,
                                       uint8_t *routeportno  )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioStdRouteGetEntry( dev_info,
                                                                routedestid,
                                                                routeportno );
   return rc;
}


uint32_t DARrioStdRouteInitAll( DAR_DEV_INFO_t *dev_info, uint8_t routeportno )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioStdRouteInitAll( dev_info,
                                                               routeportno );
   return rc;
}


uint32_t DARrioStdRouteRemoveEntry( DAR_DEV_INFO_t *dev_info, uint16_t routedestid )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO(dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioStdRouteRemoveEntry(dev_info,
                                                                 routedestid );
   return rc;
}


uint32_t DARrioStdRouteSetDefault( DAR_DEV_INFO_t *dev_info, uint8_t routeportno )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioStdRouteSetDefault( dev_info,
                                                                 routeportno );
   return rc;
}

uint32_t DARrioSetAssmblyInfo( DAR_DEV_INFO_t *dev_info,
                                     uint32_t  AsmblyVendID,
                                     uint16_t  AsmblyRev )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioSetAssmblyInfo( dev_info,
                                                             AsmblyVendID,
                                                                AsmblyRev );
   return rc;
}


uint32_t DARrioGetAssmblyInfo ( DAR_DEV_INFO_t *dev_info,
                                      uint32_t *AsmblyVendID,
                                      uint16_t *AsmblyRev )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioGetAssmblyInfo( dev_info,
                                                                 AsmblyVendID,
                                                                 AsmblyRev );
   return rc;
}

uint32_t DARrioGetPortList( DAR_DEV_INFO_t *dev_info, struct DAR_ptl *ptl_in, struct DAR_ptl *ptl_out )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ((!dev_info) || (!ptl_in) || (!ptl_out))
	   return RIO_ERR_NULL_PARM_PTR;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioGetPortList( dev_info, ptl_in, ptl_out );
   return rc;
}

uint32_t DARrioSetEnumBound( DAR_DEV_INFO_t *dev_info, struct DAR_ptl *ptl,
			int enum_bnd_val)
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

     if (!ptl)
	   return RIO_ERR_NULL_PARM_PTR;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioSetEnumBound( dev_info,
                                                          ptl, enum_bnd_val );
   return rc;
}

uint32_t DARrioGetDevResetInitStatus( DAR_DEV_INFO_t *dev_info )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioGetDevResetInitStatus(
                                                                  dev_info );
   return rc;
}

uint32_t DARrioPortEnable(
    DAR_DEV_INFO_t  *dev_info,
    struct DAR_ptl	*ptl,
    bool            port_ena,
    bool            port_lkout,
    bool            in_out_ena )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if (!ptl)
	   return RIO_ERR_NULL_PARM_PTR;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioPortEnable( dev_info,
                                                             ptl,
                                                             port_ena,
                                                             port_lkout,
                                                             in_out_ena );
   return rc;
}

uint32_t DARrioEmergencyLockout( 
    DAR_DEV_INFO_t *dev_info,
    uint8_t           port_no  )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioEmergencyLockout ( dev_info,
                                                                    port_no
                                                                  );
   return rc;
}

uint32_t DARrioDeviceRemoved( DAR_DEV_INFO_t *dev_info )
{
   uint32_t rc = DAR_DB_INVALID_HANDLE;

   if ( VALIDATE_DEV_INFO( dev_info ) )
       rc = driver_db[DAR_DB_INDEX(dev_info)].rioDeviceRemoved( dev_info );

   return rc;
}

#ifdef __cplusplus
}
#endif
