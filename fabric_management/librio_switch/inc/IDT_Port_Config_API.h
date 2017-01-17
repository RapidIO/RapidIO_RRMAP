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
#ifndef __IDT_PORT_CONFIG_API_H__
#define __IDT_PORT_CONFIG_API_H__

#include <stdbool.h>
#include <DAR_DevDriver.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Definitions of parameter structures for IDT Port Configuration routines.

   Generic structure which contains the parameters which describe the
   configuration of a port.
*/
#define IDT_PC_MAX_LANES                    4
#define IDT_PC_NO_ASSIGNED_LANE             0

#define IDT_PC_UNINIT_PARM                  0xFF

#define IDT_PC_SUCCESS                      0x000
#define IDT_PC_INVALID                      0x001

#define IDT_PC_PORT_UNAVAILABLE             0x100
#define IDT_PC_CORRUPTED                    0x103
#define IDT_PC_DEV_SPEC                     0x104
#define IDT_PC_PORT_ERR                     0x105

typedef enum idt_pc_pw_t_tag
{
    idt_pc_pw_1x = 0,   /* Single lane port
                        */
    idt_pc_pw_2x,       /* Two lane port
                        */
    idt_pc_pw_4x,       /* Four lane port
                        */
    idt_pc_pw_1x_l0,    /* Force multilane port to operate on lane 0
                        */
    idt_pc_pw_1x_l1,    /* Force multilane port to operate on lane 1
                        */
    idt_pc_pw_1x_l2,    /* Force multilane port to operate on lane 2
                        */
    idt_pc_pw_last      /* Last port width value */
} idt_pc_pw_t;

extern int         pw_to_lanes[]; // Converts idt_pc_pw_t to lane count
extern char       *pw_to_str[]  ; // Converts idt_pc_pw_t to string
extern idt_pc_pw_t lanes_to_pw[]; // Converts lane count to idt_pc_pw_t

#define PW_TO_LANES(x) (x>=idt_pc_pw_last?pw_to_lanes[idt_pc_pw_last]:pw_to_lanes[x])
#define PW_TO_STR(x) (x>=idt_pc_pw_last?pw_to_str[idt_pc_pw_last]:pw_to_str[x])
#define LANES_TO_PW(x) (x>4?idt_pc_pw_last:lanes_to_pw[x])

typedef enum idt_pc_ls_t_tag
{
    idt_pc_ls_1p25 = 0, /* 1.25 Gbaud lane speed.
                        */
    idt_pc_ls_2p5  = 1, /* 2.5  Gbaud lane speed.
                        */
    idt_pc_ls_3p125= 2, /* 3.125 Gbaud lane speed.
                        */
    idt_pc_ls_5p0  = 3, /* 5.0 Gbaud lane speed.
                        */
    idt_pc_ls_6p25 = 4, /* 6.25 Gbaud lane speed.
                        */
    idt_pc_ls_10p3 = 5, /* 10.3 Gbaud lane speed.
                        */
    idt_pc_ls_12p5 = 6, /* 12.5 Gbaud lane speed.
                        */
    idt_pc_ls_last      // last lane speed, not used...
} idt_pc_ls_t;

// Convert lane speed to a string
extern char *ls_to_str[];

#define LS_TO_STR(x) (x>=idt_pc_ls_last?ls_to_str[idt_pc_ls_last]:ls_to_str[x])

enum idt_pc_fc {
	idt_pc_fc_rx,
	idt_pc_fc_tx,
	idt_pc_fc_last
};

// Convert flow control to a string
extern char *fc_to_str[];
#define FC_TO_STR(x) (x>=idt_pc_fc_last?fc_to_str[idt_pc_fc_last]:fc_to_str[x])

enum idt_pc_idle_seq {
	idt_pc_is_one,
	idt_pc_is_two,
	idt_pc_is_three,
	idt_pc_is_last
};

// Convert idle sequenceto a string
extern char *is_to_str[];

#define ISEQ_TO_STR(x) (x>=idt_pc_is_last?is_to_str[idt_pc_is_last]:is_to_str[x])

typedef struct idt_pc_one_port_config_t_TAG
{
    uint8_t       pnum;           /* Port number.  Allows port configuration
                                   information to be declared out of order,
                                   as per quadrants/port numbering.
                                */
    bool        port_available; /* true if the port is available, 
                                   false if the port is not available
                                   If the port is not available, resources
                                     associated with the port may be powered
                                     down as long as they do not interfere
                                     with other ports.
                                   If the port is not available,
                                     only the "powerdown" field of this 
                                     structure is valid.    
                                */
    bool        powered_up;     /* true if the port is powered up
				 * false if the port is powered down.
				 */
    idt_pc_pw_t pw;             /* Port width
                                */   
    idt_pc_ls_t ls;             /* Lane speed
                                */
    enum idt_pc_fc 	fc; /* Type of flow control that is enabled.
			 */
    enum idt_pc_idle_seq iseq; /* Idle sequence configured
			 * IDLE2 means that IDLE2 or IDLE1 may be used.
			 */
    bool        xmitter_disable;   /* True if the port should be disabled, false if
				      the port should be enabled.  NOTE that if
				      a port is disabled, the transmitter is disabled
				      and the port will not be able to train.
				   Packets routed to a disabled port are dropped.
				   Corresponds to standard "port disable" bit.
				 */
    bool        port_lockout;   /* True if the port should be locked out, false if
				      the port should not be locked out.  NOTE that 
				      if a port is locked out, the transmitter and
				      receiver are enabled, so the port can achieve
				      PORT_OK, but packets cannot be exchanged.
				 */
	bool	nmtc_xfer_enable; /* True if the port should be able to send and receive
						 non-maintenance packets, false if non-maintenance packets
						 should not be exchanged by this port.  NOTE: To prevent all
						 packets from being exchanged on the port, set port_lockout. 
				   Corresponds to standard "output port enable" and "input port enable" bit. */
    bool        tx_lswap;       /* True if the transmit lanes are connected to
                                   a port in order of highest numbered lane to
                                   lowest numbered
                                */
    bool        tx_linvert[IDT_PC_MAX_LANES];
                                /* True if the tracks of a differential pair
                                   are inverted.                            
                                   Does not reflect lane swapping status.
                                */
    bool        rx_lswap;       /* Trued if the receive lanes are connected to
                                   a port in order of highest numbered lane to
                                   lowest numbered.
                                */
    bool        rx_linvert[IDT_PC_MAX_LANES];
                                /* True if the tracks of a differential pair
                                   are inverted.                            
                                   Does not reflect lane swapping status. 
                                */
} idt_pc_one_port_config_t;

/* Structure which captures the status of a port.
*/
typedef struct idt_pc_one_port_status_t_TAG
{
    uint8_t       pnum;           /* Port number.  Allows port configuration
                                     information to be declared out of order,
                                     as per quadrants/port numbering.
                                */
    bool        port_ok;        /* true if a link partner is present and
                                     control symbols can be exchanged
                                     successfully.
                                */
    idt_pc_pw_t pw;             /* Port width
                                */
    enum idt_pc_fc  fc;          /* Flow control algorithm for the link
				*/
    enum idt_pc_idle_seq iseq; /* Idle sequence being used
			 */
    bool        port_error;     /* true if a fatal error is present which
                                     prevents packet transmission.
                                   This is a combination of the standard 
                                     PORT_ERR indications, and the PORT_FAILED
				     indication when "Stop on Fail" is set
				     in the Port x Control CSR.
                                */
    bool        input_stopped;  /* true if the port has detected a transmission
                                     error or has retried a packet and is
                                     awaiting packet transmission to start
                                     again.
                                */
    bool        output_stopped; /* true if the port is attempting
                                     error recovery with the link partner.                                                                              
                                */
    uint8_t     num_lanes;      /* Number of lanes connected to the port.
				   0 means that the port is unavailable, powered 
				   down, or both.
                                */
    bool        first_lane;     /* Lane number of the first lane connected to
                                     the port.  It is assumed that sequentially
                                     numbered lanes are connected to a port.
                                */
    
} idt_pc_one_port_status_t;
    
typedef struct idt_pc_get_config_in_t_TAG
{
    struct DAR_ptl ptl; /* Return configuration for this port list. */
} idt_pc_get_config_in_t;

typedef struct idt_pc_set_config_in_t_TAG
{
    uint32_t      lrto;            // Link response timeout value for all ports.
                                 // Specified in hundreds of nanoseconds, range of 0 (disabled)
				 // up to 60,000,000 (6 seconds).
    uint32_t      log_rto;      // Logical layer response timeout, specified
                                // in hundreds of nanoseconds.  Range of 0
                                // (disabled) up to 60,000,000 (6 seconds)
    bool        oob_reg_acc;     /* If true, register access is not dependent
                                      upon RapidIO (i.e. JTAG, I2C).  It is
				      possible to reprogram the port used for
				      RapidIO connectivity.
                                 */
    uint8_t       reg_acc_port;    /* Register access port.
				      Valid when oob_reg_acc is false.
				      Must be filled in to protect against 
				      inadvertently disabling/ resetting the 
				      port(s) used for connectivity
				      to the switch.
				  */
    uint8_t       num_ports;  /* Number of ports which should be updated.
                               If RIO_ALL_PORTS was passed in,  
                                 only the first entry of pc must be valid.
                               All ports will be configured according to
                                 this entry.
                            */
    idt_pc_one_port_config_t pc[IDT_MAX_PORTS];
} idt_pc_set_config_in_t;

typedef struct idt_pc_set_config_out_t_TAG
{
    uint32_t      imp_rc;     /* Implementation specific return code information.
                            */
    uint32_t      lrto;        // Link response timeout value for all ports.
                             // Specified in hundreds of nanoseconds, range of 0 (disabled)
			     // up to 60,000,000 (6 seconds).
    uint32_t      log_rto;      // Logical layer response timeout, specified
                                // in hundreds of nanoseconds.  Range of 0
                                // (disabled) up to 60,000,000 (6 seconds)
    uint8_t       num_ports;  /* Number of ports which are now present.
                               If RIO_ALL_PORTS was passed in,  
                                 this reflects the actual number of ports
                                 present after the configuration was changed.
                            */
    idt_pc_one_port_config_t pc[IDT_MAX_PORTS];
                            /* Current configuration of the devices ports. 
                            */
} idt_pc_set_config_out_t;

typedef idt_pc_set_config_out_t idt_pc_get_config_out_t;

typedef struct idt_pc_get_status_in_t_TAG
{
    struct DAR_ptl ptl;
} idt_pc_get_status_in_t;

typedef struct idt_pc_get_status_out_t_TAG
{
    uint32_t      imp_rc;      /* Implementation specific return code information.
                             */
    uint8_t       num_ports;   /* Number of ports for which status was returned.
                                If RIO_ALL_PORTS was passed in  
                                  idt_pc_get_status_in_t.ptl.num_ports, this
                                  reflects the number of ports which have
                                  information present in pc.
                             */
    idt_pc_one_port_status_t ps[IDT_MAX_PORTS];    
} idt_pc_get_status_out_t;

// The RapidIO port used to access the switch will not be reset 
// using this routine unless oob_reg_acc is true.
//
// When out-of-band access methods are used (I2C or JTAG),
// Port 0 will not be reset unless oob_reg_acc is true.
//
// Resetting a port requires multiple register
// accesses.  When registers are accessed using RapidIO, 
// the port used for connectivity must be reset by using 
// idt_pc_reset_link_partner.

typedef struct idt_pc_reset_port_in_t_TAG
{
   /* NOTE:
    * If RIO_ALL_PORTS == port_num, reset_lp is true, and
    * preserve_config is false, this routine MAY reset the entire device.
    */

    uint8_t       port_num;        /* Port which should be reset on this
                                      device.  
                                    RIO_ALL_PORTS is an acceptable value
                                 */
    bool        oob_reg_acc;     /* If true, register access is not dependent
                                      upon RapidIO (i.e. JTAG, I2C).  It is
				      possible to reset the port used for 
				      RapidIO connectivity.
                                 */
    uint8_t       reg_acc_port;    /* Register access port.
				      Valid when oob_reg_acc is false.
				      Must be filled in to protect against 
				      inadvertently resetting the 
				      port(s) used for connectivity
				      to the switch.
				  */
    bool        reset_lp;        /* If true, reset the link partner just
                                      before resetting this port.
                                 */
    bool        preserve_config; /* If true, preserves port
                                      configuration state which may be
                                      destroyed by the reset.                                   
				    When false, the configuration of the 
				      port may be destroyed by the reset.  This
				      may be a more comprehensive reset than
				      what is done when preserve_config is true.
                                 */
} idt_pc_reset_port_in_t;

typedef struct idt_pc_reset_port_out_t_TAG
{
    uint32_t      imp_rc;         /* Implementation specific return code
                                       information.
                                */
} idt_pc_reset_port_out_t;

typedef struct idt_pc_reset_link_partner_in_t_TAG
{
    uint8_t       port_num;       /* Port whose link partner should be reset.
                                      Must be a valid port number. 
				   RIO_ALL_PORTS is not supported.
				   Port must have PORT_OK status for this 
				      routine to succeeed.
                                */
    bool        resync_ackids;  /* If true, attempts to resychronize ackIDs
                                     by clearing ackIDs to 0 on the local
                                     port after the reset.                                       
                                */
} idt_pc_reset_link_partner_in_t;

typedef struct  idt_pc_reset_link_partner_out_t_TAG
{
    uint32_t      imp_rc;         /* Implementation specific return code
                                       information.
                                */
} idt_pc_reset_link_partner_out_t;

typedef struct idt_pc_clr_errs_in_t_TAG  
{
    uint8_t       port_num;           /* Port on this device which should have its 
                                         input-err stop, output-err stop and port_err
					 error conditions cleared. 
                                       RIO_ALL_PORTS is an illegal value for
				         this field.
				       The port MUST have PORT_OK status for this routine
				         to be successful.  If the port does not have 
					 PORT_OK status, then the port may be reset to
					 clear errors.
                                    */
    bool        clr_lp_port_err;    /* idt_pc_clr_errs will always attempt to clear
				          input error-stop, output error-stop and
					  port error conditions on the local port.  
					  This may involve sending control symbols 
					  to the link partner.
				       If true, then this routine will attempt to clear
				          port_err conditions on the link partner.
					  Depending on the device, this may involve 
					  sending control symbols and packets to the 
					  link partner to establish ackID
					  synchronization.  It may involve resetting
					  the link partner. Maintenance requests may time out.
				       If false, then this routine will not reset the
				          link partner or send packets to the link 
					  partner.  AckID values are cleared to 0
					  on the local end of the link.     
					  This is a lower risk option option that can
					  be used if the capacity to clear port_err 
					  locally exists at both ends of the link.
				    */
    DAR_DEV_INFO_t  *lp_dev_info;  /* Device information for the link partner.
				    * If this pointer is NULL when clr_lp_port_err is true,
				    * error recovery is limited to resetting the 
				    * link partner and the local port.
                                    */
    uint8_t           num_lp_ports;  /* Number of entries in the lp_port_list.
				    * If this value is 0 when clr_lp_port_err is true, 
				    *   then ports on the link partner will be checked 
				    *   until the port_err condition is cleared.
				    *   Note that both available and unavailable
				    *   ports will be checked.  If more than one
				    *   port is supplied, response timeouts may
				    *   occur for maintenance packets.
				    */
    uint8_t           lp_port_list[IDT_MAX_PORTS]; 
                                   /* List of possible link partner port numbers 
				    *   that the local port could be connected to.  
				    */
} idt_pc_clr_errs_in_t;

typedef struct idt_pc_clr_errs_out_t_TAG
{
    uint32_t      imp_rc;    /* Implementation specific return code information.
                        */
} idt_pc_clr_errs_out_t;

typedef enum {
    idt_pc_rst_device = 0, /* Link reset request resets the device
                           */
    idt_pc_rst_port   = 1, /* Just reset the port the reset request was received on
                           */
    idt_pc_rst_int    = 2, /* Assert an interrupt if a reset request was received
                           */
    idt_pc_rst_pw     = 3, /* Send a port-write if a reset request was received
                           */
    idt_pc_rst_ignore = 4, /* Ignore reset requests received.
                           */
    idt_pc_rst_last        // Begin illegal parameter values
} idt_pc_rst_handling;

// Convert reset configuration to a string
extern char *rst_to_str[];

/* The secure_port routine may be used on any device.  Some parameters
       may not apply to all devices.
*/
typedef struct idt_pc_secure_port_in_t_TAG
{
	struct      DAR_ptl      ptl;   /* List of ports to configure. */
    bool        mtc_pkts_allowed;   /* If false, filter out maintenance packets
                                         if possible.
                                    */
    bool        MECS_participant;   /* If false, this port will not retransmit
                                         MECS events
                                    */
    bool        MECS_acceptance;    /* If false, this port will ignore MECS
                                    */
    idt_pc_rst_handling rst;        /* Configure reset handling for this port.
                                    */
} idt_pc_secure_port_in_t;

typedef struct idt_pc_secure_port_out_t_TAG
{
    uint32_t      imp_rc;              /* Implementation specific return code
                                          information.
                                     */
    bool        bc_mtc_pkts_allowed; /* Current configuration values for
                                          specified port.
                                     */
    bool        MECS_participant; 
    bool        MECS_acceptance;  
    idt_pc_rst_handling rst;  
} idt_pc_secure_port_out_t;

typedef struct idt_pc_dev_reset_config_in_t_TAG
{
    idt_pc_rst_handling rst;    /* Configure reset handling for the device.
                                */
} idt_pc_dev_reset_config_in_t;

typedef struct idt_pc_dev_reset_config_out_t_TAG
{
    uint32_t      imp_rc;         /* Implementation specific return code
                                     information
                                */
    idt_pc_rst_handling rst;    /* Current configuration value.
                                */
} idt_pc_dev_reset_config_out_t;

typedef struct idt_pc_probe_in_t_TAG
{
    uint8_t port;    /* Check this port for problems exchanging control symbols
                                */
} idt_pc_probe_in_t;

typedef enum idt_port_status_t_TAG {
    port_ok,
    port_degr,
    port_los,
    port_err
} idt_port_status_t;

typedef struct idt_pc_probe_out_t_TAG
{
    uint32_t            imp_rc;   /* Implementation specific return code
                                     information
                                */
    idt_port_status_t status;   /* Current configuration value.
                                */
} idt_pc_probe_out_t;

// Implementation specific return codes for Port Configuration routines

#define PC_GET_CONFIG_0       (DAR_FIRST_IMP_SPEC_ERROR+0x0100)
#define PC_SET_CONFIG_0       (DAR_FIRST_IMP_SPEC_ERROR+0x0200)
#define PC_GET_uint32_t_0       (DAR_FIRST_IMP_SPEC_ERROR+0x0300)
#define PC_RESET_PORT_0       (DAR_FIRST_IMP_SPEC_ERROR+0x0400)
#define PC_RESET_LP_0         (DAR_FIRST_IMP_SPEC_ERROR+0x0500)
#define PC_CLR_ERRS_0         (DAR_FIRST_IMP_SPEC_ERROR+0x0600)
#define PC_SECURE_PORT_0      (DAR_FIRST_IMP_SPEC_ERROR+0x0700)
#define PC_DEV_RESET_CONFIG_0 (DAR_FIRST_IMP_SPEC_ERROR+0x0800)
#define PC_PROBE_0            (DAR_FIRST_IMP_SPEC_ERROR+0x0900)

#define PC_FIRST_SUBROUTINE_0 (DAR_FIRST_IMP_SPEC_ERROR+0x1000)

/* The following functions are implemented to support the above structures
   Refer to the above structures for the implementation detail 
*/
/* This function returns the port's configuration
*/
#define PC_GET_CONFIG(x) (PC_GET_CONFIG_0+x)

uint32_t idt_pc_get_config(
    DAR_DEV_INFO_t           *dev_info,
    idt_pc_get_config_in_t   *in_parms,
    idt_pc_get_config_out_t  *out_parms
);

/* This function sets up the port width, lane speed, etc.
*/
#define PC_SET_CONFIG(x) (PC_SET_CONFIG_0+x)

uint32_t idt_pc_set_config(
    DAR_DEV_INFO_t           *dev_info,
    idt_pc_set_config_in_t   *in_parms,
    idt_pc_set_config_out_t  *out_parms
);

/* This function returns the status of port
*/
#define PC_GET_uint32_t(x) (PC_GET_uint32_t_0+x)

uint32_t idt_pc_get_status(
    DAR_DEV_INFO_t           *dev_info,
    idt_pc_get_status_in_t   *in_parms,
    idt_pc_get_status_out_t  *out_parms
);

/* This function resets the specified port as well as the link partner
*/
#define PC_RESET_PORT(x) (PC_RESET_PORT_0+x)

uint32_t idt_pc_reset_port(
    DAR_DEV_INFO_t           *dev_info,
    idt_pc_reset_port_in_t   *in_parms,
    idt_pc_reset_port_out_t  *out_parms
);

/* This function resets the link partner
*/
#define PC_RESET_LP(x) (PC_RESET_LP_0+x)

uint32_t idt_pc_reset_link_partner(
    DAR_DEV_INFO_t                   *dev_info,
    idt_pc_reset_link_partner_in_t   *in_parms,
    idt_pc_reset_link_partner_out_t  *out_parms
);

/* This function clear error flags
*/
#define PC_CLR_ERRS(x) (PC_CLR_ERRS_0+x)

uint32_t idt_pc_clr_errs(
    DAR_DEV_INFO_t         *dev_info,
    idt_pc_clr_errs_in_t   *in_parms,
    idt_pc_clr_errs_out_t  *out_parms
);

/* This function configures MECS and Maintenance Pkt Allowance
*/
#define PC_SECURE_PORT(x) (PC_SECURE_PORT_0+x)

uint32_t idt_pc_secure_port(
    DAR_DEV_INFO_t           *dev_info,
    idt_pc_secure_port_in_t  *in_parms,
    idt_pc_secure_port_out_t  *out_parms
);

/* This function configures device behavior when a reset request 
 * is received
 */
#define PC_DEV_RESET_CONFIG(x) (PC_DEV_RESET_CONFIG_0+x)
uint32_t idt_pc_dev_reset_config(
    DAR_DEV_INFO_t                 *dev_info,
    idt_pc_dev_reset_config_in_t   *in_parms,
    idt_pc_dev_reset_config_out_t  *out_parms
);

/* This function determines if packets can be exchanged on a port.
 */
#define PC_PROBE(x) (PC_PROBE_0+x)
uint32_t idt_pc_probe(
    DAR_DEV_INFO_t      *dev_info,
    idt_pc_probe_in_t   *in_parms,
    idt_pc_probe_out_t  *out_parms
);

#ifdef __cplusplus
}
#endif

#endif /* __IDT_PORT_CONFIG_API_H__ */
