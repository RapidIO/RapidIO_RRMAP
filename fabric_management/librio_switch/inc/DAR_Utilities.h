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
#ifndef __DAR_UTILITIES_H__
#define __DAR_UTILITIES_H__

#include "stdint.h"
#include "rio_ecosystem.h"
#include "DAR_DevDriver.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RIO_MAX_PKT_BYTES 276
#define RIO_MAX_PKT_PAYLOAD 256

/* Device Access Routine (DAR) Utility Routines
*
*  Multiple register read and read/modify/write utilities
*
*  Control symbol composition and parsing utilities related
*      to Error Management Extensions registers
*  Control symbols field value description strings
*
*  Packet composition and parsing utilities for related 
*      to Error Management Extensions registers
*  Packet field value description strings
*/

/* --->>>>>>>START Multi-register read/modify/write utilities <<<<<<---
*/
typedef struct DAR_read_entry_in_t_TAG {
    uint32_t offset;  /* Offset for the register to be read from
                    */
} DAR_read_entry_in_t;

typedef struct DAR_read_entry_out_t_TAG {
    uint32_t value_read;  /* Value read
                        */
    uint32_t rc;          /* Status of the read request
                        */
} DAR_read_entry_out_t;


/* Host Specific routine for performing a delay.
   Delays can be longer than specified, but must not be shorter.
*/
extern void (*WaitSec) ( uint32_t delay_nsec,
                         uint32_t delay_sec );

void DAR_WaitSec( uint32_t delay_nsec, uint32_t delay_sec );

/* Read num_entries worth of register offsets passed in as in_parms,
   storing the values read and return code in out_parms.
*/
uint32_t DAR_multi_reg_read( DAR_DEV_INFO_t       *dev_info, 
                           DAR_read_entry_in_t  *in_parms, 
                           DAR_read_entry_out_t *out_parms, 
                           uint32_t                num_entries);

typedef struct DAR_read_write_entry_in_t_TAG {
    uint32_t offset; /* offset of the register to be read from
                   */ 
    uint32_t mask  ; /* bit mask for bits to change
                   */
    uint32_t value_delta;
                     /* Value to be written.
                     *  value written is ((*offset)    &  mask) | 
                     *                   (value_delta & ^mask))
                     *  Note: No value is read if mask is 0.
                     *  Note: No value is written if mask is 0xFFFFFFFF.
                     */
} DAR_read_write_entry_in_t;

typedef struct DAR_read_write_entry_out_t_TAG {
    uint32_t read_value;  /* Value read
                        */
    uint32_t read_rc;     /* RC for read of register
                        */
    uint32_t write_value; /* Actual value written
                        */
            
    uint32_t write_rc;    /* RC for write of register
                        */
} DAR_read_write_entry_out_t;

/* Perform num_entries read-modify-write operations as described in in_parms.
   Store the results of the operations in out_parms.
*/
uint32_t DAR_multi_reg_acc ( DAR_DEV_INFO_t             *dev_info, 
                           DAR_read_write_entry_in_t  *in_parms,
                           DAR_read_write_entry_out_t *out_parms,
                           uint32_t                      num_entries);

/* --->>>>>>>  END Multi-register read and read/modify/write utilities <<<<<<---
   --->>>>>>>  START  Control symbol support routines <<<<<<---
*/
typedef enum { cs_invalid, cs_small, cs_large } rio_cs_size;

typedef enum 
{
    stype0_pa = 0,
    stype0_rty = 1,
    stype0_pna = 2,
    stype0_rsvd = 3,
    stype0_status = 4,
    stype0_VC_status = 5,
    stype0_lresp = 6,
    stype0_imp = 7
} stype0;

// Definitions for STYPE0_PNA parm1 field.
#define PNA_PKT_UNEXP_ACKID 1
#define PNA_CS_UNEXP_ACKID  2
#define PNA_NONMTC_STOPPED  3
#define PNA_PKT_BAD_CRC     4
#define PNA_DELIN_ERR       5
#define PNA_RETRY           6
#define PNA_NO_DESCRAM_SYNC 7
#define PNA_GENERAL_ERR    0x1F 

typedef enum
{
    stype1_sop  = 0,
    stype1_stomp= 1,
    stype1_eop  = 2,
    stype1_rfr  = 3,
    stype1_lreq = 4,
    stype1_mecs = 5,
    stype1_rsvd = 6,
    stype1_nop  = 7
} stype1;

// STYPE1_CMD_RSVD should be used for all STYPE1 CMD field values 
// with the exception of LREQ (Link Request), which has two command
// options.

#define STYPE1_CMD_RSVD           0
#define STYPE1_LREQ_CMD_RST_DEV   3
#define STYPE1_LREQ_CMD_PORT_STAT 4

typedef enum
{
    stype2_nop     = 0,
    stype2_vc_stat = 1,
    stype2_rsvd2   = 2,
    stype2_rsvd3   = 3,
    stype2_rsvd4   = 4,
    stype2_rsvd5   = 5,
    stype2_rsvd6   = 6,
    stype2_rsvd7   = 7
} stype2;

typedef struct CS_field_t_TAG
{
    rio_cs_size    cs_size;    /* cs_invalid means no fields are valid
                               */
    stype0         cs_t0;
    uint32_t         parm_0;
    uint32_t         parm_1;
    stype1         cs_t1;
    uint32_t         cs_t1_cmd;
    stype2         cs_t2;      /* Valid if cs_size is cs_large
                               */
    uint32_t         cs_t2_val;  /* Valid if cs_size is cs_large
                               */
    uint32_t         cs_crc;
    bool           cs_crc_correct;
                                  /* Ignored by CS_fields_to_bytes,
                                       set by CS_bytes_to_fields
                                  */
} CS_field_t;

typedef struct CS_bytes_t_TAG {
    rio_cs_size   cs_type_valid;  /* true if cs_type & cs_bytes are valid, 
                                        false if not
                                  */
    uint8_t         cs_bytes[8];    /* Bytes occur in network byte order i.e. 
                                        byte 0 is first to be transmitted.
                                  */
} CS_bytes_t;

/* Accept the control symbol field values, and compose the bytes which
   comprise the actual control symbol as latched in error registers or
   written to control symbol transmission registers.
*/
uint32_t CS_fields_to_bytes( CS_field_t *fields_in, CS_bytes_t *bytes_out); 

/* Accept 3, 4, 6 or 8 bytes of the control symbol, and parse the control
   symbol into the appropriate fields.
*/
uint32_t CS_bytes_to_fields( CS_bytes_t *bytes_in, CS_field_t *fields_out);

/* Strings which name the 8 types of STYPE0 control symbols
*/
char *get_stype0_descr( CS_field_t *fields_in ); 
  
/* Strings which describe the cause of a PNA
*/
char *get_stype0_PNA_cause_parm1( CS_field_t *fields_in ); 
  
/* Strings which describe the port status field of a LR
*/
char *get_stype0_LR_port_status_parm1( CS_field_t *fields_in ); 
  
/* Strings which name the 8 types of STYPE1 control symbols
*/
char *get_stype1_descr( CS_field_t *fields_in ); 
  
/* Strings which describe the 8 Link Request command values.
*/
char *get_stype1_lreq_cmd( CS_field_t *fields_in ); 
  
/* Strings which name the 8 types of STYPE2 control symbols
*/
char *get_stype2_descr( CS_field_t *fields_in ); 

/* --->>>>>>>  END  Control symbol support routines <<<<<<---
   --->>>>>>> START Packet format support types and structures <<<<<<---
*/

typedef enum {  pkt_raw,         /* Raw, uninitialized packet
                                 */
                pkt_nr,          /* NREAD packet
                                 */
                pkt_nr_inc,      /* ATOMIC Increment
                                 */
                pkt_nr_dec,      /* ATOMIC Decrement
                                 */
                pkt_nr_set,      /* ATOMIC Set
                                 */
                pkt_nr_clr,      /* ATOMIC Clear
                                 */
                pkt_nw,          /* NWRITE packet
                                 */
                pkt_nwr,         /* NWRITE_R packet
                                 */
                pkt_nw_swap,     /* ATOMIC Swap
                                 */
                pkt_nw_cmp_swap, /* ATOMIC compare and swap
                                 */
                pkt_nw_tst_swap, /* ATOMIC Test-and-swap
                                 */
                pkt_sw,          /* SWRITE packet
                                 */
                pkt_fc,          /* Flow Control Packet
                                 */
                pkt_mr,          /* Maintenance Read
                                 */
                pkt_mw,          /* Maintenance Write
                                 */
                pkt_mrr,         /* Maintenance Read Response,
                                 */
                pkt_mwr,         /* Maintenance Write Response
                                 */
                pkt_pw,          /* Port-Write
                                 */
                pkt_dstm,        /* Data Streaming (Type 9) packet
                                 */
                pkt_db,          /* Doorbell (type 10) packet
                                 */
                pkt_msg,         /* Message (Type 11) packet
                                 */
                pkt_resp,        /* Response (Type 13) packet, no data
                                 */
                pkt_resp_data,   /* Response (Type 13) packet with data
                                 */
                pkt_msg_resp,    /* Response (Type 13) packet for Message
                                    segment
                                 */
                pkt_last_type
} DAR_pkt_type;

typedef enum { pkt_done = 0, pkt_retry = 3, pkt_err = 7 } rio_pkt_status;

typedef enum { rio_addr_21, rio_addr_32, rio_addr_34, rio_addr_50,
               rio_addr_66 } rio_addr_size;

typedef enum { tt_small, tt_large, undef2, undef3 } rio_TT_code;

typedef enum { fc_flow_0A, fc_flow_0B, fc_flow_0C, fc_flow_0D, fc_flow_0E, 
               fc_flow_0Fplus,
               fc_flow_1A, fc_flow_2A, fc_flow_3A, fc_flow_4A, 
               fc_flow_5A, fc_flow_6A, fc_flow_7A, fc_flow_8A } rio_fc_flow_id;

typedef enum { fc_fam_000, fc_fam_001, fc_fam_010, fc_fam_011,
               fc_fam_100, fc_fam_101, fc_fam_110, fc_fam_111 } rio_fc_fam_t;

/* Structures used to store packet field values.
   Definitions are structured in the same layered manner as the RapidIO
       specification.
   Physical and Transport layer structures apply to every packet.
   Logical layer structures apply to the specific packet type(s).
*/

/* Packet physical layer fields.
*/
typedef struct DAR_pkt_phy_hdr_t_TAG
{
    uint32_t       pkt_vc;    /* Virtual channel the packet is associated with, 
                                  with a value between 0 and 8.
                                  VC 0 uses pkt_prio and crf to determine 
                                      packet priority.
                                  VC 1-8 does not have a packet priority.
                            */
    uint32_t       pkt_prio;  /* Packet priority, value 0-3
                            */
    bool         crf;       /* true if this is a critical request flow packet.
                                  CRF affects packet priority
                            */
    uint32_t       pkt_ackID; /* Packet ackID.  All composed packets have an 
                                   ackID of 0.  All ackIDS are assumed to have
                                   6 bits - it is up to the user to determine
                                   if large or small ackIDs are in use, and to
                                       adjust accordingly.
                            */
} DAR_pkt_phy_hdr_t;

typedef struct DAR_pkt_phy_mask_t_TAG
{
	uint8_t pkt_ackID_m;    // AckID field mask value. 6 bits.
	bool  pkt_vc_m;       // VC   field mask value. 1 bit
	uint8_t pkt_prio_m;     // PRIO field mask value.  2 bits.
	bool  pkt_crf_m;      // CRF  field mask value. 1 bit
	bool  pkt_ftype_m;    // FType field mask value. 4 bits

	uint16_t pkt_int_crc_m;    // Intermediate CRC field mask value.  Only used for packets > 80 bytes.
	uint16_t pkt_fin_crc_m;    // Final CRC field mask value.
} DAR_pkt_phy_mask_t;

/* Transport layer header fields
*/
typedef struct DAR_pkt_trans_hdr_t_TAG
{
    /* Transport layer fields
    */
    rio_TT_code tt_code;   /* TT code, as per enum above */
    uint32_t destID;    /* 8/16 bits, depending on tt_code value. */
    uint32_t srcID;     /* Both destID and srcID are the same size. */
    hc_t hopcount;  /* Only Valid for Maintenance packets. Max value 255 */
} DAR_pkt_trans_hdr_t;

typedef struct DAR_pkt_trans_mask_t_TAG
{
	uint8_t        tt_m;      // Mask value for TT     field.  2 bits.
	rio_TT_code  tt_code;   // TT code, determines how many bits of destID/srcID mask are used
	uint32_t       destID_m;  // Mask value for DestID field.  8, 16, or 32 bits
	uint32_t       srcID_m;   // Mask value for SrcID  field.  8, 16, or 32 bits
	hc_t        hc_m;      // Mask value for hopcount field.  8 bits.  Only used for Maintenance packets.
} DAR_pkt_trans_mask_t;


/* Packet fields for the following logical layer packet types:
   FTYPE 2 (NREAD & ATOMICs) 
   FTYPE 5 (NWRITE, NWRITE_R, ATOMICs) 
   FTYPE 6 (Streaming Write), 
   FTYPE 8 (Maintenance)
   FTYPE 13 (responses with and without data)
*/
typedef struct DAR_pkt_log_rw_hdr_t_TAG
{
    /* pkt_addr_size is only valid for nr, nw, nwr, sw, mr, mw
    */
    rio_addr_size  pkt_addr_size; 
    /* pkt_addr_size values affect which portion of addr[] are used.
       rio_addr_21 - Least significant 3 bytes of addr[0]
       rio_addr_32 - addr[0]
       rio_addr_34 - addr[0-1], addr[1] max value is 3
       rio_addr_50 - addr[0-1], addr[1] max value is 0x3_FFFF
       rio_addr_66 - addr[0-2], addr[2] max value is 3
    */
    uint32_t         addr[3]; 
    uint32_t         tid;     /* Transaction ID.  Valid for FTYPE 2, 5, 8 and 13
                            */
    rio_pkt_status status;  /* Only valid for response types
                               (FTYPE 8, FTYPE 13)
                            */
} DAR_pkt_log_rw_hdr_t;

// Used for FTYPE 2, 5 8, 10 and 13 (R/W and Msg)
// NOTE: Msg responses also need log_ms header filled in.
typedef struct DAR_pkt_log_rw_mask_t_TAG 
{
	uint8_t  ttype_m; // TransType Mask Value, 4 bits.  Used for FType 2, 5, 8, 10 and 13 (R/W and Msg)
	uint8_t  size_m;  // Read/write size mask, 4 bits
	bool   ptr_m;   // Read/Write pointer mask
	rio_addr_size  pkt_addr_size;   // pkt_addr_size is only valid for nr, nw, nwr, sw, mr, mw
                                    // pkt_addr_size values affect which portion of addr_m[] are used.
    uint32_t addr_m[3];  // Address   Mask Value. addr_m[0] is least significant, addr_m[2] is most significant.
	uint8_t  addr_l;     // Least significant byte of address, used for FTYPE 2, 5, 8 to ensure
	                   //    data is aligned correctly in the packet payload.
	uint8_t  tid_m;      // TransID   Mask value, 8 bits.  Used for FType 2, 5, 8, and 10
	uint8_t  status_m;   // Status    Mask value, 4 bits.  Used for FType 2, 5, 8, 10 and 13 (R/W and Msg)
} DAR_pkt_log_rw_mask_t;

/* FTYPE 7 (Flow Control) Packet Fields
*/
typedef struct DAR_pkt_log_fc_hdr_t_TAG
{
    /* Logical layer fields for flow control transactions (FType 7) */
    bool            fc_xon;        /* True if this is an XON, false for XOFF.
                                   */
    rio_fc_flow_id  fc_flow;       /* Flow ID for flow control
                                   */
    uint32_t          fc_destID;     /* Flow control destID
                                   */
    uint32_t          fc_srcID;      /* Flow control sourceID
                                   */
    bool            fc_soc_is_ep;  /* True if source of FC is an endpoint, 
                                          false if source is a switch
                                   */
    rio_fc_fam_t    fc_fam;        /* Value 0-7, used to control buffer 
                                          requests/releases
                                   */ 
} DAR_pkt_log_fc_hdr_t;

typedef struct DAR_pkt_log_fc_mask_t_TAG
{
    /* Logical layer fields for flow control transactions (FType 7) */
    bool   fc_xon_m;       // XON Mask value, 4 bits   
    bool   fc_soc_is_ep_m; // SRC is EP Mask value, 1 bits 
	uint8_t  fc_flow_m;      // Status Mask value, 4 bits 
	uint8_t  fc_fam_m;       // Fam    Mask value, 3 bits
} DAR_pkt_log_fc_mask_t;

/* FTYPE 9 (Data Streaming) Packet Fields
   Note: the fields used are controlled by the dstm_xh_seg value.
*/
typedef struct DAR_pkt_log_ds_hdr_t_TAG
{
    /* Logical layer fields for Data Streaming (FType 9) Request transactions
    */
    bool         dstm_start_seg;
    bool         dstm_end_seg;
    bool         dstm_xh_seg;       /* true if this is an "extended header" 
                                           segment.
                                    */
    uint32_t       dstm_COS;          /* Present for all Data Streaming packets
                                    */
    uint32_t       dstm_streamid;     /* Present if dstm_start_seg or dstm_xh_seg
                                    */
    bool         dstm_odd_data_amt; /* True if there is an odd number of 
                                           halfwords in the data
                                    */
    bool         dstm_pad_data_amt; /* True if there is a one byte pad in the 
                                           last halfword.
                                    */
    uint32_t       dstm_PDU_len;      /* Total length of the PDU. 
                                           Present IFF !dstm_start_seg & 
                                           dstm_end_seg
                                    */
    /* Logical layer fields for Data Streaming Extended Header request 
          transactions
    */
    uint32_t       dstm_xh_type;      /* Extended header type
                                    */
    uint32_t       dstm_xh_tm_op;     /* Type of extended header flow control 
                                          operation
                                    */
    uint32_t       dstm_xh_wildcard;  /* Optionally apply to all destIDs, 
                                          classes, or streamIDs.
                                    */
    uint32_t       dstm_xh_COS_mask;  /* Specify a mask for COS to select a 
                                          subset of COS for this operation
                                    */
    uint32_t       dstm_xh_parm1;     /* tm_op specific parameter 1
                                    */
    uint32_t       dstm_xh_parm2;     /* tm_op specific parameter 2
                                    */
} DAR_pkt_log_ds_hdr_t;

typedef struct DAR_pkt_log_ds_mask_t_TAG
{
    /* Logical layer fields for Data Streaming (FType 9) Request transactions
    */
    bool dstm_s_m;            // Mask value for S bit
    bool dstm_e_m;            // Mask value for E bit
    bool dstm_xh_m;           // Mask value for XH bit
    uint8_t dstm_COS_m;         // Mask value for COS field
    bool   dstm_strm_or_len;  // true if STRM/LEN    field should be included in the mask, false if not
    uint16_t dstm_strm_or_len_m; // Mask value for StreamID/Length field
    bool dstm_o_m;            // Mask value for O bit
    bool dstm_p_m;            // Mask value for P bit
	bool dtsm_xh_pkt;       // true if should use an extended header packet format, false if not.
	                        //    If dtsm_xh_pkt is false, none of the folllowing fields need to be filled in.
	uint8_t dstm_xh_type_m;   // Mask value for XH_TYPE field
	uint8_t dstm_xh_tm_op_m;  // Mask value for XH TM Op field
	uint8_t dstm_xh_wc_m;     // Mask value for XH TWildcard field
	uint8_t dstm_xh_COS_m;    // Mask value for XH COS mask field
	uint8_t dstm_xh_parm1_m;    // Mask value for XH COS mask field
	uint8_t dstm_xh_parm2_m;    // Mask value for XH COS mask field
} DAR_pkt_log_ds_mask_t;

/* FTYPE 11 (Message) Packet Fields
*/
typedef struct DAR_pkt_log_ms_hdr_t_TAG
{
    /* Logical Layer fields for Message Request (FType 11) transactions,
       and Message responses
    */
    uint32_t         msg_len;   /* # of messages in PDU
                                 Only valid for msg packets.
                              */
    uint32_t         mbid;      /* Mailbox ID.  0-63 if msg_len = 0, 
                                              0- 3 otherwise
                              */
    uint32_t         msgseg;    /* Segment of message. 0 if msg_len = 0,
                                     <=msg_len
                              */
    uint32_t         letter;    /* Letter ID.  Invalid if msg_len = 0, 
                                     0-3 otherwise.
                              */
    rio_pkt_status status;    /* Only valid for message response
                              */
} DAR_pkt_log_ms_hdr_t;

// NOTE: Msg responses need log_ms header filled in.
// However, FType 13 Status and TType are found in log_rw (DAR_pkt_log_rw_mask_t)
typedef struct DAR_pkt_log_ms_mask_t_TAG
{
    /* Logical Layer fields for Message Request (FType 11) transactions,
       and Message responses
    */
    uint8_t msg_len_m;  // Mask value for Len  field, 4 bits
	uint8_t msg_size_m; // Mask value for Size field, 4 bits
	bool  msg_xmbox;  // Determines size of Mbid.
	                  //    Required for requests and responses.
	                  //    If True,  MBID is 6 bits and MSGSEG is not used.
	                  //    If False, MBID is 2 bits and MSGSEG is used.
    uint8_t msg_mbid_m; // Mask value for mailbox/xmbox  field.
		              //    Required for requests and responses.
    uint8_t msg_seg_m;  // Mask value for message segment field, 4 bits
		              //    Required for requests and responses.
    uint8_t msg_letter_m; // Mask value for letter field, 2 bits
		                //    Required for requests and responses.
} DAR_pkt_log_ms_mask_t;

/* Structure containing all packet fields for all logical layer types.
*/
typedef struct DAR_pkt_fields_t_TAG 
{
    uint32_t       tot_bytes; /* Total size of packet in bytes.  
                                   If tot_bytes = -1, the structure is not 
                                   initialized.
                            */
    uint32_t       pad_bytes; /* Number of pad bytes (0's) at the end of the 
                                  packet.  Will be either 0, or 2.
                                  Ignored by DAR_pkt_fields_to_bytes, 
                                  computed by DAR_pkt_bytes_to_fields
                            */
    DAR_pkt_phy_hdr_t   phys;   /* Physical layer packet header fields
                                */
    DAR_pkt_trans_hdr_t trans;  /* Transport layer packet header fields
                                */

    /* Common Logical Layer Fields
    */
    DAR_pkt_type pkt_type;   /* Packet type, as per enum above
                             */
    uint32_t       pkt_bytes;  /* Amount of pkt_data[] which is valid, 
                                    starting at pkt_data[0].
                             */  
    uint8_t       *pkt_data;   /* Pointer to data, an array of at least
                                    276 bytes.
                             */
    DAR_pkt_log_rw_hdr_t log_rw; /* FTYPE 2, 5, 6, 8, 13
                                 */
    DAR_pkt_log_fc_hdr_t log_fc; /* FTYPE 7
                                 */
    DAR_pkt_log_ds_hdr_t log_ds; /* FTYPE 9
                                 */
    DAR_pkt_log_ms_hdr_t log_ms; /* FTYPE 11
                                 */
} DAR_pkt_fields_t; 

/* Structure containing all packet fields for all logical layer types.
*/
typedef struct DAR_pkt_mask_t_TAG 
{
    uint32_t       tot_bytes; /* Total size of packet in bytes.  
                                   If tot_bytes = -1, the structure is not 
                                   initialized.
                            */

    DAR_pkt_phy_mask_t   phys;   // Physical layer packet header fields
    DAR_pkt_trans_mask_t trans;  // Transport layer packet header fields

    /* Common Logical Layer Fields
    */
    DAR_pkt_type pkt_type;   // Packet type
    uint32_t       pkt_bytes;  // Amount of pkt_data[] which is valid, starting at pkt_data[0].
    uint8_t       *pkt_data;   // Mask value for data, an array of at least 276 bytes.
    DAR_pkt_log_rw_mask_t log_rw; // FTYPE 2, 5, 6, 8, 13 TTYPE 0
    DAR_pkt_log_fc_mask_t log_fc; // FTYPE 7
    DAR_pkt_log_ds_mask_t log_ds; // FTYPE 9
    DAR_pkt_log_ms_mask_t log_ms; // FTYPE 11, FTYPE 13 TTYPE 1
} DAR_pkt_mask_t; 

/* Structure used to store packets as a sequence of bytes
   Bytes are stored in transmission order.
*/
typedef struct DAR_pkt_bytes_t_TAG
{
    rio_addr_size pkt_addr_size; /* Address size of packet, determined by 
                                        system not packet fields
                                 */
    bool          pkt_has_crc;   /* True of the packet incorporates the CRC 
                                        codes, false otherwise
                                 */
    bool          pkt_padded;    /* True if the last 2 bytes of the packet 
				    are padding, false if not.
				 */
    uint32_t        num_chars;     /* Number of valid bytes in pkt_data, 
                                        -1 means nothing is valid
                                 */ 
    uint8_t         pkt_data[RIO_MAX_PKT_BYTES];
} DAR_pkt_bytes_t;

/* Error codes for packet composition and parsing routines
*/
#define DAR_UTIL_INVALID_TT     0x78000000  
#define DAR_UTIL_BAD_ADDRSIZE   0x78000001  
#define DAR_UTIL_INVALID_RDSIZE 0x78000002  
#define DAR_UTIL_BAD_DATA_SIZE  0x78000003  
#define DAR_UTIL_INVALID_MTC    0x78000004  
#define DAR_UTIL_BAD_MSG_DSIZE  0x78000005  
#define DAR_UTIL_BAD_RESP_DSIZE 0x78000006 
#define DAR_UTIL_UNKNOWN_TRANS  0x78000007  
#define DAR_UTIL_BAD_DS_DSIZE   0x78000008  
#define DAR_UTIL_UNKNOWN_uint32_t 0x78000009  
#define DAR_UTIL_UNKNOWN_FTYPE  0x78000010  
#define DAR_UTIL_0_MASK_VAL_ERR 0x78000011  

/* To compose a packet as a stream of bytes, pass in an initialized 
   rio_pkt_fields_t, and get out a rio_pkt_bytes_t.
*/
uint32_t DAR_pkt_fields_to_bytes ( DAR_pkt_fields_t *fields_in, 
                                 DAR_pkt_bytes_t  *bytes_out );

/* If the stream of bytes must be altered after the packet has been composed, 
 * use this routine to update the packet's CRC.
 *
 * Note that it is the responsibility of the calling routine to ensure that
 * the num_chars and pkt_padded fields are consistent after the stream of
 * bytes has been altered.
 */
uint32_t DAR_update_pkt_CRC ( DAR_pkt_bytes_t  *bytes_in );

/* To parse a packet, pass in a rio_pkt_bytes_t stream of bytes and get 
   back a rio_pkt_fields_t.
*/
uint32_t DAR_pkt_bytes_to_fields ( DAR_pkt_bytes_t  *bytes_in, 
                                 DAR_pkt_fields_t *fields_out );

/* Routines to return strings describing field values
   Returns string naming packet FTYPE.
*/
char *DAR_pkt_ftype_descr( DAR_pkt_fields_t *pkt_fields );  
                                                            
/* Returns string naming packet transaction type.
       Works for all FTYPEs.
   If the FTYPEs does not have a transaction type, 
       this routine returns NULL.
   If the transaction type value is invalid,
       this routine returns "UNKNOWN"
*/
char *DAR_pkt_trans_descr( DAR_pkt_fields_t *pkt_fields );  

/* Returns string for response packet (FTYPE 13) 
       status field (rio_pkt_status)
   If the packet passed in is not a response packet,
       the routine returns NULL.
   If the status field is invalid, this routine
       returns "UNKNOWN"
*/
char *DAR_pkt_resp_status_descr( DAR_pkt_fields_t *pkt_fields ); 

/* --->>>>>>> END Packet format support types and structures <<<<<<---
*/
 
#ifdef __cplusplus
}
#endif

#endif /* __DAR_UTILITIES_H__ */

