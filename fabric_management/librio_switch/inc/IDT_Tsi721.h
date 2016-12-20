/***************************************************************************
** (C) Copyright 2011; Integrated Device Technology
** July 5, 2011 All Rights Reserved.
**
** This file contains the IDT TSI721 register definitions and bitfield masks. 
**  
** Disclaimer
** Integrated Device Technology, Inc. ("IDT") reserves the right to make changes
** to its products or specifications at any time, without notice, in order to
** improve design or performance. IDT does not assume responsibility for use of
** any circuitry described herein other than the circuitry embodied in an IDT
** product. Disclosure of the information herein does not convey a license or
** any other right, by implication or otherwise, in any patent, trademark, or
** other intellectual property right of IDT. IDT products may contain errata
** which can affect product performance to a minor or immaterial degree. Current
** characterized errata will be made available upon request. Items identified
** herein as "reserved" or "undefined" are reserved for future definition. IDT
** does not assume responsibility for conflicts or incompatibilities arising
** from the future definition of such items. IDT products have not been
** designed, tested, or manufactured for use in, and thus are not warranted for,
** applications where the failure, malfunction, or any inaccuracy in the
** application carries a risk of death, serious bodily injury, or damage to
** tangible property. Code examples provided herein by IDT are for illustrative
** purposes only and should not be relied upon for developing applications. Any
** use of such code examples shall be at the user's sole risk.
***************************************************************************/

#ifndef _IDT_TSI721_H_
#define _IDT_TSI721_H_

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************/
/* TSI721 : PCIe Register address offset definitions */
/*****************************************************/

#define TSI721_PCI_ID                                              (0x00000000)
#define TSI721_PCI_CSR                                             (0x00000004)
#define TSI721_PCI_CLASS                                           (0x00000008)
#define TSI721_PCI_MISC0                                           (0x0000000c)
#define TSI721_PCI_BAR0                                            (0x00000010)
#define TSI721_PCI_BAR1                                            (0x00000014)
#define TSI721_PCI_BAR2                                            (0x00000018)
#define TSI721_PCI_BAR3                                            (0x0000001c)
#define TSI721_PCI_BAR4                                            (0x00000020)
#define TSI721_PCI_BAR5                                            (0x00000024)
#define TSI721_PCI_CCISPTR                                         (0x00000028)
#define TSI721_PCI_SID                                             (0x0000002c)
#define TSI721_PCI_EROMBASE                                        (0x00000030)
#define TSI721_PCI_CAPPTR                                          (0x00000034)
#define TSI721_PCI_MISC1                                           (0x0000003c)
#define TSI721_PCIECAP                                             (0x00000040)
#define TSI721_PCIEDCAP                                            (0x00000044)
#define TSI721_PCIEDCTL                                            (0x00000048)
#define TSI721_PCIELCAP                                            (0x0000004c)
#define TSI721_PCIELCTL                                            (0x00000050)
#define TSI721_PCIEDCAP2                                           (0x00000064)
#define TSI721_PCIEDCTL2                                           (0x00000068)
#define TSI721_PCIELCAP2                                           (0x0000006c)
#define TSI721_PCIELCTL2                                           (0x00000070)
#define TSI721_MSIXCAP                                             (0x000000a0)
#define TSI721_MSIXTBL                                             (0x000000a4)
#define TSI721_MSIXPBA                                             (0x000000a8)
#define TSI721_PMCAP                                               (0x000000c0)
#define TSI721_PMCSR                                               (0x000000c4)
#define TSI721_MSICAP                                              (0x000000d0)
#define TSI721_MSIADDR                                             (0x000000d4)
#define TSI721_MSIUADDR                                            (0x000000d8)
#define TSI721_MSIMDATA                                            (0x000000dc)
#define TSI721_MSIMASK                                             (0x000000e0)
#define TSI721_MSIPENDING                                          (0x000000e4)
#define TSI721_SSIDSSVIDCAP                                        (0x000000f0)
#define TSI721_SSIDSSVID                                           (0x000000f4)
#define TSI721_ECFGADDR                                            (0x000000f8)
#define TSI721_ECFGDATA                                            (0x000000fc)
#define TSI721_AERCAP                                              (0x00000100)
#define TSI721_AERUES                                              (0x00000104)
#define TSI721_AERUEM                                              (0x00000108)
#define TSI721_AERUESV                                             (0x0000010c)
#define TSI721_AERCES                                              (0x00000110)
#define TSI721_AERCEM                                              (0x00000114)
#define TSI721_AERCTL                                              (0x00000118)
#define TSI721_AERHL1DW                                            (0x0000011c)
#define TSI721_AERHL2DW                                            (0x00000120)
#define TSI721_AERHL3DW                                            (0x00000124)
#define TSI721_AERHL4DW                                            (0x00000128)
#define TSI721_SNUMCAP                                             (0x00000180)
#define TSI721_SNUMLDW                                             (0x00000184)
#define TSI721_SNUMUDW                                             (0x00000188)
#define TSI721_EPCTL                                               (0x00000400)
#define TSI721_EPSTS                                               (0x00000404)
#define TSI721_SEDELAY                                             (0x0000040c)
#define TSI721_BARSETUP0                                           (0x00000440)
#define TSI721_BARSETUP1                                           (0x00000444)
#define TSI721_BARSETUP2                                           (0x00000448)
#define TSI721_BARSETUP3                                           (0x0000044c)
#define TSI721_BARSETUP4                                           (0x00000450)
#define TSI721_BARSETUP5                                           (0x00000454)
#define TSI721_IERRORCTL                                           (0x00000480)
#define TSI721_IERRORSTS0                                          (0x00000484)
#define TSI721_IERRORMSK0                                          (0x00000488)
#define TSI721_IERRORSEV0                                          (0x0000048c)
#define TSI721_IERRORTST0                                          (0x00000494)
#define TSI721_TOCTL                                               (0x000004b0)
#define TSI721_IFBTOCNT                                            (0x000004b4)
#define TSI721_EFBTOCNT                                            (0x000004b8)
#define TSI721_TOTSCTL                                             (0x000004bc)
#define TSI721_MECTL                                               (0x000004c0)
#define TSI721_SERDESCFG                                           (0x00000510)
#define TSI721_SERDESSTS0                                          (0x00000514)
#define TSI721_LANESTS0                                            (0x0000051c)
#define TSI721_LANESTS1                                            (0x00000520)
#define TSI721_LANESTS2                                            (0x00000524)
#define TSI721_PHYFSMT0                                            (0x00000528)
#define TSI721_PHYFSMT1                                            (0x0000052c)
#define TSI721_PHYLCFG0                                            (0x00000530)
#define TSI721_PHYLCFG1                                            (0x00000534)
#define TSI721_PHYLSTS0                                            (0x00000538)
#define TSI721_PHYLSTS1                                            (0x0000053c)
#define TSI721_PHYLSTATE0                                          (0x00000540)
#define TSI721_PHYLTSSMSTS0                                        (0x00000544)
#define TSI721_PHYLTSSMSTS1                                        (0x00000548)
#define TSI721_PHYCNT0                                             (0x0000054c)
#define TSI721_PHYCNT1                                             (0x00000550)
#define TSI721_PHYCNTCFG                                           (0x00000554)
#define TSI721_PHYRECEL                                            (0x00000558)
#define TSI721_PHYPRBS                                             (0x0000055c)
#define TSI721_DLCTL1                                              (0x00000600)
#define TSI721_DLCTL2                                              (0x00000604)
#define TSI721_DLCTL3                                              (0x00000608)
#define TSI721_DLSTS                                               (0x0000060c)
#define TSI721_DLRXSTS                                             (0x00000610)
#define TSI721_DLTXSTS                                             (0x00000614)
#define TSI721_DLCNT0                                              (0x00000618)
#define TSI721_DLCNT1                                              (0x0000061c)
#define TSI721_DLCNTCFG                                            (0x00000620)
#define TSI721_TLSTSE                                              (0x00000680)
#define TSI721_TLCTL                                               (0x00000684)
#define TSI721_TLCNT0                                              (0x00000688)
#define TSI721_TLCNT1                                              (0x0000068c)
#define TSI721_TLCNTCFG                                            (0x00000690)
#define TSI721_INTSTS                                              (0x000006a0)
#define TSI721_PMPC0                                               (0x00000700)
#define TSI721_PMPC1                                               (0x00000704)
#define TSI721_FCVC0PTCC                                           (0x00000800)
#define TSI721_FCVC0NPCC                                           (0x00000804)
#define TSI721_FCVC0CPCC                                           (0x00000808)
#define TSI721_FCVC0PTCL                                           (0x0000080c)
#define TSI721_FCVC0NPCL                                           (0x00000810)
#define TSI721_FCVC0CPCL                                           (0x00000814)
#define TSI721_FCVC0PTCA                                           (0x00000818)
#define TSI721_FCVC0NPCA                                           (0x0000081c)
#define TSI721_FCVC0CPCA                                           (0x00000820)
#define TSI721_FCVC0PTCR                                           (0x00000824)
#define TSI721_FCVC0NPCR                                           (0x00000828)
#define TSI721_FCVC0CPCR                                           (0x0000082c)
#define TSI721_EFBTC                                               (0x00000860)
#define TSI721_IFBCNT0                                             (0x000008b0)
#define TSI721_IFBCNT1                                             (0x000008b4)
#define TSI721_IFBCNTCFG                                           (0x000008b8)
#define TSI721_EFBCNT0                                             (0x000008c0)
#define TSI721_EFBCNT1                                             (0x000008c4)
#define TSI721_EFBCNTCFG                                           (0x000008c8)
#define TSI721_UEEM                                                (0x00000d90)
#define TSI721_CEEM                                                (0x00000d94)
#define TSI721_STMCTL                                              (0x00000e54)
#define TSI721_STMSTS                                              (0x00000e58)
#define TSI721_STMTCTL                                             (0x00000e5c)
#define TSI721_STMTSTS                                             (0x00000e60)
#define TSI721_STMECNT0                                            (0x00000e64)
#define TSI721_STMECNT1                                            (0x00000e68)
#define TSI721_STMECNT2                                            (0x00000e6c)
#define TSI721_STMECNT3                                            (0x00000e70)
#define TSI721_ALLCS                                               (0x00000e84)
#define TSI721_IFBVC0PTCFG                                         (0x00000e90)
#define TSI721_IFBVC0NPCFG                                         (0x00000e94)
#define TSI721_IFBVC0CPCFG                                         (0x00000e98)
#define TSI721_IFCSTS                                              (0x00000ea8)
#define TSI721_EFBVC0PTSTS                                         (0x00000ec0)
#define TSI721_EFBVC0NPSTS                                         (0x00000ec4)
#define TSI721_EFBVC0CPSTS                                         (0x00000ec8)
#define TSI721_EFBRBSTS                                            (0x00000ecc)

/*****************************************************/
/* TSI721 : PCIe Register Bit Masks and Reset Values */
/*           definitions for every register          */
/*****************************************************/

/* TSI721_PCI_ID : Register Bits Masks Definitions */
#define TSI721_PCI_ID_VID                                           (0x0000ffff)
#define TSI721_PCI_ID_DID                                           (0xffff0000)

/* TSI721_PCI_CSR : Register Bits Masks Definitions */
#define TSI721_PCI_CSR_IOAE                                         (0x00000001)
#define TSI721_PCI_CSR_MAE                                          (0x00000002)
#define TSI721_PCI_CSR_BME                                          (0x00000004)
#define TSI721_PCI_CSR_SCE                                          (0x00000008)
#define TSI721_PCI_CSR_MWI                                          (0x00000010)
#define TSI721_PCI_CSR_VGAS                                         (0x00000020)
#define TSI721_PCI_CSR_PERRE                                        (0x00000040)
#define TSI721_PCI_CSR_ADSTEP                                       (0x00000080)
#define TSI721_PCI_CSR_SERRE                                        (0x00000100)
#define TSI721_PCI_CSR_FB2BE                                        (0x00000200)
#define TSI721_PCI_CSR_INTXD                                        (0x00000400)
#define TSI721_PCI_CSR_INTS                                         (0x00080000)
#define TSI721_PCI_CSR_CAPL                                         (0x00100000)
#define TSI721_PCI_CSR_C66MHZ                                       (0x00200000)
#define TSI721_PCI_CSR_FB2B                                         (0x00800000)
#define TSI721_PCI_CSR_MDPED                                        (0x01000000)
#define TSI721_PCI_CSR_DEVT                                         (0x06000000)
#define TSI721_PCI_CSR_STAS                                         (0x08000000)
#define TSI721_PCI_CSR_RTAS                                         (0x10000000)
#define TSI721_PCI_CSR_RMAS                                         (0x20000000)
#define TSI721_PCI_CSR_SSE                                          (0x40000000)
#define TSI721_PCI_CSR_DPE                                          (0x80000000)

/* TSI721_PCI_CLASS : Register Bits Masks Definitions */
#define TSI721_PCI_CLASS_RID                                        (0x000000ff)
#define TSI721_PCI_CLASS_INTF                                       (0x0000ff00)
#define TSI721_PCI_CLASS_SUB                                        (0x00ff0000)
#define TSI721_PCI_CLASS_BASE                                       (0xff000000)

/* TSI721_PCI_MISC0 : Register Bits Masks Definitions */
#define TSI721_PCI_MISC0_CLS                                        (0x000000ff)
#define TSI721_PCI_MISC0_LTIMER                                     (0x0000ff00)
#define TSI721_PCI_MISC0_HDR                                        (0x00ff0000)
#define TSI721_PCI_MISC0_CCODE                                      (0x0f000000)
#define TSI721_PCI_MISC0_START                                      (0x40000000)
#define TSI721_PCI_MISC0_CAPABLE                                    (0x80000000)

/* TSI721_PCI_BAR0 : Register Bits Masks Definitions */
#define TSI721_PCI_BAR0_MEMSI                                       (0x00000001)
#define TSI721_PCI_BAR0_TYPE                                        (0x00000006)
#define TSI721_PCI_BAR0_PREF                                        (0x00000008)
#define TSI721_PCI_BAR0_BADDR                                       (0xfffffff0)

/* TSI721_PCI_BAR1 : Register Bits Masks Definitions */
#define TSI721_PCI_BAR1_MEMSI                                       (0x00000001)
#define TSI721_PCI_BAR1_TYPE                                        (0x00000006)
#define TSI721_PCI_BAR1_PREF                                        (0x00000008)
#define TSI721_PCI_BAR1_BADDR                                       (0xfffffff0)

/* TSI721_PCI_BAR2 : Register Bits Masks Definitions */
#define TSI721_PCI_BAR2_MEMSI                                       (0x00000001)
#define TSI721_PCI_BAR2_TYPE                                        (0x00000006)
#define TSI721_PCI_BAR2_PREF                                        (0x00000008)
#define TSI721_PCI_BAR2_BADDR                                       (0xfffffff0)

/* TSI721_PCI_BAR3 : Register Bits Masks Definitions */
#define TSI721_PCI_BAR3_MEMSI                                       (0x00000001)
#define TSI721_PCI_BAR3_TYPE                                        (0x00000006)
#define TSI721_PCI_BAR3_PREF                                        (0x00000008)
#define TSI721_PCI_BAR3_BADDR                                       (0xfffffff0)

/* TSI721_PCI_BAR4 : Register Bits Masks Definitions */
#define TSI721_PCI_BAR4_MEMSI                                       (0x00000001)
#define TSI721_PCI_BAR4_TYPE                                        (0x00000006)
#define TSI721_PCI_BAR4_PREF                                        (0x00000008)
#define TSI721_PCI_BAR4_BADDR                                       (0xfffffff0)

/* TSI721_PCI_BAR5 : Register Bits Masks Definitions */
#define TSI721_PCI_BAR5_MEMSI                                       (0x00000001)
#define TSI721_PCI_BAR5_TYPE                                        (0x00000006)
#define TSI721_PCI_BAR5_PREF                                        (0x00000008)
#define TSI721_PCI_BAR5_BADDR                                       (0xfffffff0)

/* TSI721_PCI_CCISPTR : Register Bits Masks Definitions */
#define TSI721_PCI_CCISPTR_CCISPTR                                  (0xffffffff)

/* TSI721_PCI_SID : Register Bits Masks Definitions */
#define TSI721_PCI_SID_SUBVID                                       (0x0000ffff)
#define TSI721_PCI_SID_SUBID                                        (0xffff0000)

/* TSI721_PCI_EROMBASE : Register Bits Masks Definitions */
#define TSI721_PCI_EROMBASE_EN                                      (0x00000001)
#define TSI721_PCI_EROMBASE_BADDR                                   (0xfffff800)

/* TSI721_PCI_CAPPTR : Register Bits Masks Definitions */
#define TSI721_PCI_CAPPTR_CAPPTR                                    (0x000000ff)

/* TSI721_PCI_MISC1 : Register Bits Masks Definitions */
#define TSI721_PCI_MISC1_INTRLINE                                   (0x000000ff)
#define TSI721_PCI_MISC1_INTRPIN                                    (0x0000ff00)
#define TSI721_PCI_MISC1_MINGNT                                     (0x00ff0000)
#define TSI721_PCI_MISC1_MAXLAT                                     (0xff000000)

/* TSI721_PCIECAP : Register Bits Masks Definitions */
#define TSI721_PCIECAP_CAPID                                        (0x000000ff)
#define TSI721_PCIECAP_NXTPTR                                       (0x0000ff00)
#define TSI721_PCIECAP_VER                                          (0x000f0000)
#define TSI721_PCIECAP_TYPE                                         (0x00f00000)
#define TSI721_PCIECAP_SLOT                                         (0x01000000)
#define TSI721_PCIECAP_IMN                                          (0x3e000000)

/* TSI721_PCIEDCAP : Register Bits Masks Definitions */
#define TSI721_PCIEDCAP_MPAYLOAD                                    (0x00000007)
#define TSI721_PCIEDCAP_PFS                                         (0x00000018)
#define TSI721_PCIEDCAP_ETAG                                        (0x00000020)
#define TSI721_PCIEDCAP_E0AL                                        (0x000001c0)
#define TSI721_PCIEDCAP_E1AL                                        (0x00000e00)
#define TSI721_PCIEDCAP_ABP                                         (0x00001000)
#define TSI721_PCIEDCAP_AIP                                         (0x00002000)
#define TSI721_PCIEDCAP_PIP                                         (0x00004000)
#define TSI721_PCIEDCAP_RBERR                                       (0x00008000)
#define TSI721_PCIEDCAP_CSPLV                                       (0x03fc0000)
#define TSI721_PCIEDCAP_CSPLS                                       (0x0c000000)
#define TSI721_PCIEDCAP_FLR                                         (0x10000000)

/* TSI721_PCIEDCTL : Register Bits Masks Definitions */
#define TSI721_PCIEDCTL_CEREN                                       (0x00000001)
#define TSI721_PCIEDCTL_NFEREN                                      (0x00000002)
#define TSI721_PCIEDCTL_FEREN                                       (0x00000004)
#define TSI721_PCIEDCTL_URREN                                       (0x00000008)
#define TSI721_PCIEDCTL_ERO                                         (0x00000010)
#define TSI721_PCIEDCTL_MPS                                         (0x000000e0)
#define TSI721_PCIEDCTL_ETFEN                                       (0x00000100)
#define TSI721_PCIEDCTL_PFEN                                        (0x00000200)
#define TSI721_PCIEDCTL_AUXPMEN                                     (0x00000400)
#define TSI721_PCIEDCTL_NOSNOOP                                     (0x00000800)
#define TSI721_PCIEDCTL_MRRS                                        (0x00007000)
#define TSI721_PCIEDCTL_IFLR                                        (0x00008000)
#define TSI721_PCIEDCTL_CED                                         (0x00010000)
#define TSI721_PCIEDCTL_NFED                                        (0x00020000)
#define TSI721_PCIEDCTL_FED                                         (0x00040000)
#define TSI721_PCIEDCTL_URD                                         (0x00080000)
#define TSI721_PCIEDCTL_AUXPD                                       (0x00100000)
#define TSI721_PCIEDCTL_TP                                          (0x00200000)

/* TSI721_PCIELCAP : Register Bits Masks Definitions */
#define TSI721_PCIELCAP_MAXLNKSPD                                   (0x0000000f)
#define TSI721_PCIELCAP_MAXLNKWDTH                                  (0x000003f0)
#define TSI721_PCIELCAP_ASPMS                                       (0x00000c00)
#define TSI721_PCIELCAP_L0SEL                                       (0x00007000)
#define TSI721_PCIELCAP_L1EL                                        (0x00038000)
#define TSI721_PCIELCAP_CPM                                         (0x00040000)
#define TSI721_PCIELCAP_SDERR                                       (0x00080000)
#define TSI721_PCIELCAP_DLLLA                                       (0x00100000)
#define TSI721_PCIELCAP_LBN                                         (0x00200000)
#define TSI721_PCIELCAP_PORTNUM                                     (0xff000000)

/* TSI721_PCIELCTL : Register Bits Masks Definitions */
#define TSI721_PCIELCTL_ASPM                                        (0x00000003)
#define TSI721_PCIELCTL_RCB                                         (0x00000008)
#define TSI721_PCIELCTL_LDIS                                        (0x00000010)
#define TSI721_PCIELCTL_LRET                                        (0x00000020)
#define TSI721_PCIELCTL_CCLK                                        (0x00000040)
#define TSI721_PCIELCTL_ESYNC                                       (0x00000080)
#define TSI721_PCIELCTL_CLKPWRMGT                                   (0x00000100)
#define TSI721_PCIELCTL_HAWD                                        (0x00000200)
#define TSI721_PCIELCTL_LBWINTEN                                    (0x00000400)
#define TSI721_PCIELCTL_LABWINTEN                                   (0x00000800)
#define TSI721_PCIELCTL_CLS                                         (0x000f0000)
#define TSI721_PCIELCTL_NLW                                         (0x03f00000)
#define TSI721_PCIELCTL_LTRAIN                                      (0x08000000)
#define TSI721_PCIELCTL_SCLK                                        (0x10000000)
#define TSI721_PCIELCTL_DLLLA                                       (0x20000000)
#define TSI721_PCIELCTL_LBWSTS                                      (0x40000000)
#define TSI721_PCIELCTL_LABWSTS                                     (0x80000000)

/* TSI721_PCIEDCAP2 : Register Bits Masks Definitions */
#define TSI721_PCIEDCAP2_CTRS                                       (0x0000000f)
#define TSI721_PCIEDCAP2_CTDS                                       (0x00000010)
#define TSI721_PCIEDCAP2_ARIFS                                      (0x00000020)
#define TSI721_PCIEDCAP2_ATOPRS                                     (0x00000040)
#define TSI721_PCIEDCAP2_ATOPC32S                                   (0x00000080)
#define TSI721_PCIEDCAP2_ATOPC64S                                   (0x00000100)
#define TSI721_PCIEDCAP2_CASC128S                                   (0x00000200)
#define TSI721_PCIEDCAP2_NROEP                                      (0x00000400)
#define TSI721_PCIEDCAP2_LTRMS                                      (0x00000800)
#define TSI721_PCIEDCAP2_TPHCS                                      (0x00003000)
#define TSI721_PCIEDCAP2_EFMTFS                                     (0x00100000)
#define TSI721_PCIEDCAP2_E2ETPS                                     (0x00200000)

/* TSI721_PCIEDCTL2 : Register Bits Masks Definitions */
#define TSI721_PCIEDCTL2_CTV                                        (0x0000000f)
#define TSI721_PCIEDCTL2_CTD                                        (0x00000010)
#define TSI721_PCIEDCTL2_ARIFEN                                     (0x00000020)
#define TSI721_PCIEDCTL2_ATOPRE                                     (0x00000040)
#define TSI721_PCIEDCTL2_ATOPEB                                     (0x00000080)
#define TSI721_PCIEDCTL2_IDORE                                      (0x00000100)
#define TSI721_PCIEDCTL2_IDOCE                                      (0x00000200)
#define TSI721_PCIEDCTL2_LTRME                                      (0x00000400)
#define TSI721_PCIEDCTL2_E2ETLPPB                                   (0x00008000)

/* TSI721_PCIELCAP2 : Register Bits Masks Definitions */

/* TSI721_PCIELCTL2 : Register Bits Masks Definitions */
#define TSI721_PCIELCTL2_TLS                                        (0x0000000f)
#define TSI721_PCIELCTL2_ECOMP                                      (0x00000010)
#define TSI721_PCIELCTL2_HASD                                       (0x00000020)
#define TSI721_PCIELCTL2_SDE                                        (0x00000040)
#define TSI721_PCIELCTL2_TM                                         (0x00000380)
#define TSI721_PCIELCTL2_EMC                                        (0x00000400)
#define TSI721_PCIELCTL2_CSOS                                       (0x00000800)
#define TSI721_PCIELCTL2_COMP_DE                                    (0x00001000)
#define TSI721_PCIELCTL2_CDE                                        (0x00010000)

/* TSI721_MSIXCAP : Register Bits Masks Definitions */
#define TSI721_MSIXCAP_CAPID                                        (0x000000ff)
#define TSI721_MSIXCAP_NXTPTR                                       (0x0000ff00)
#define TSI721_MSIXCAP_TBLSIZE                                      (0x07ff0000)
#define TSI721_MSIXCAP_MASK                                         (0x40000000)
#define TSI721_MSIXCAP_EN                                           (0x80000000)

/* TSI721_MSIXTBL : Register Bits Masks Definitions */
#define TSI721_MSIXTBL_BIR                                          (0x00000007)
#define TSI721_MSIXTBL_OFFSET                                       (0xfffffff8)

/* TSI721_MSIXPBA : Register Bits Masks Definitions */
#define TSI721_MSIXPBA_BIR                                          (0x00000007)
#define TSI721_MSIXPBA_OFFSET                                       (0xfffffff8)

/* TSI721_PMCAP : Register Bits Masks Definitions */
#define TSI721_PMCAP_CAPID                                          (0x000000ff)
#define TSI721_PMCAP_NXTPTR                                         (0x0000ff00)
#define TSI721_PMCAP_VER                                            (0x00070000)
#define TSI721_PMCAP_PMECLK                                         (0x00080000)
#define TSI721_PMCAP_DEVSP                                          (0x00200000)
#define TSI721_PMCAP_AUXI                                           (0x01c00000)
#define TSI721_PMCAP_D1                                             (0x02000000)
#define TSI721_PMCAP_D2                                             (0x04000000)
#define TSI721_PMCAP_PME                                            (0xf8000000)

/* TSI721_PMCSR : Register Bits Masks Definitions */
#define TSI721_PMCSR_PSTATE                                         (0x00000003)
#define TSI721_PMCSR_NOSOFTRST                                      (0x00000008)
#define TSI721_PMCSR_PMEE                                           (0x00000100)
#define TSI721_PMCSR_DSEL                                           (0x00001e00)
#define TSI721_PMCSR_DSCALE                                         (0x00006000)
#define TSI721_PMCSR_PMES                                           (0x00008000)
#define TSI721_PMCSR_B2B3                                           (0x00400000)
#define TSI721_PMCSR_BPCCE                                          (0x00800000)
#define TSI721_PMCSR_DATA                                           (0xff000000)

/* TSI721_MSICAP : Register Bits Masks Definitions */
#define TSI721_MSICAP_CAPID                                         (0x000000ff)
#define TSI721_MSICAP_NXTPTR                                        (0x0000ff00)
#define TSI721_MSICAP_EN                                            (0x00010000)
#define TSI721_MSICAP_MMC                                           (0x000e0000)
#define TSI721_MSICAP_MME                                           (0x00700000)
#define TSI721_MSICAP_A64                                           (0x00800000)
#define TSI721_MSICAP_MASKCAP                                       (0x01000000)

/* TSI721_MSIADDR : Register Bits Masks Definitions */
#define TSI721_MSIADDR_ADDR                                         (0xfffffffc)

/* TSI721_MSIUADDR : Register Bits Masks Definitions */
#define TSI721_MSIUADDR_UADDR                                       (0xffffffff)

/* TSI721_MSIMDATA : Register Bits Masks Definitions */
#define TSI721_MSIMDATA_MDATA                                       (0x0000ffff)

/* TSI721_MSIMASK : Register Bits Masks Definitions */
#define TSI721_MSIMASK_MASK                                         (0xffffffff)

/* TSI721_MSIPENDING : Register Bits Masks Definitions */
#define TSI721_MSIPENDING_PENDING                                   (0xffffffff)

/* TSI721_SSIDSSVIDCAP : Register Bits Masks Definitions */
#define TSI721_SSIDSSVIDCAP_CAPID                                   (0x000000ff)
#define TSI721_SSIDSSVIDCAP_NXTPTR                                  (0x0000ff00)

/* TSI721_SSIDSSVID : Register Bits Masks Definitions */
#define TSI721_SSIDSSVID_SSVID                                      (0x0000ffff)
#define TSI721_SSIDSSVID_SSID                                       (0xffff0000)

/* TSI721_ECFGADDR : Register Bits Masks Definitions */
#define TSI721_ECFGADDR_REG                                         (0x000000fc)
#define TSI721_ECFGADDR_EREG                                        (0x00000f00)

/* TSI721_ECFGDATA : Register Bits Masks Definitions */
#define TSI721_ECFGDATA_DATA                                        (0xffffffff)

/* TSI721_AERCAP : Register Bits Masks Definitions */
#define TSI721_AERCAP_CAPID                                         (0x0000ffff)
#define TSI721_AERCAP_CAPVER                                        (0x000f0000)
#define TSI721_AERCAP_NXTPTR                                        (0xfff00000)

/* TSI721_AERUES : Register Bits Masks Definitions */
#define TSI721_AERUES_UDEF                                          (0x00000001)
#define TSI721_AERUES_DLPERR                                        (0x00000010)
#define TSI721_AERUES_SDOENERR                                      (0x00000020)
#define TSI721_AERUES_POISONED                                      (0x00001000)
#define TSI721_AERUES_FCPERR                                        (0x00002000)
#define TSI721_AERUES_COMPTO                                        (0x00004000)
#define TSI721_AERUES_CABORT                                        (0x00008000)
#define TSI721_AERUES_UECOMP                                        (0x00010000)
#define TSI721_AERUES_RCVOVR                                        (0x00020000)
#define TSI721_AERUES_MALFORMED                                     (0x00040000)
#define TSI721_AERUES_ECRC                                          (0x00080000)
#define TSI721_AERUES_UR                                            (0x00100000)
#define TSI721_AERUES_ACSV                                          (0x00200000)
#define TSI721_AERUES_UIE                                           (0x00400000)
#define TSI721_AERUES_MCBLKTLP                                      (0x00800000)
#define TSI721_AERUES_ATOPEB                                        (0x01000000)
#define TSI721_AERUES_TLPPBE                                        (0x02000000)

/* TSI721_AERUEM : Register Bits Masks Definitions */
#define TSI721_AERUEM_UDEF                                          (0x00000001)
#define TSI721_AERUEM_DLPERR                                        (0x00000010)
#define TSI721_AERUEM_SDOENERR                                      (0x00000020)
#define TSI721_AERUEM_POISONED                                      (0x00001000)
#define TSI721_AERUEM_FCPERR                                        (0x00002000)
#define TSI721_AERUEM_COMPTO                                        (0x00004000)
#define TSI721_AERUEM_CABORT                                        (0x00008000)
#define TSI721_AERUEM_UECOMP                                        (0x00010000)
#define TSI721_AERUEM_RCVOVR                                        (0x00020000)
#define TSI721_AERUEM_MALFORMED                                     (0x00040000)
#define TSI721_AERUEM_ECRC                                          (0x00080000)
#define TSI721_AERUEM_UR                                            (0x00100000)
#define TSI721_AERUEM_ACSV                                          (0x00200000)
#define TSI721_AERUEM_UIE                                           (0x00400000)
#define TSI721_AERUEM_MCBLKTLP                                      (0x00800000)
#define TSI721_AERUEM_ATOPEB                                        (0x01000000)
#define TSI721_AERUEM_TLPPBE                                        (0x02000000)

/* TSI721_AERUESV : Register Bits Masks Definitions */
#define TSI721_AERUESV_UDEF                                         (0x00000001)
#define TSI721_AERUESV_DLPERR                                       (0x00000010)
#define TSI721_AERUESV_SDOENERR                                     (0x00000020)
#define TSI721_AERUESV_POISONED                                     (0x00001000)
#define TSI721_AERUESV_FCPERR                                       (0x00002000)
#define TSI721_AERUESV_COMPTO                                       (0x00004000)
#define TSI721_AERUESV_CABORT                                       (0x00008000)
#define TSI721_AERUESV_UECOMP                                       (0x00010000)
#define TSI721_AERUESV_RCVOVR                                       (0x00020000)
#define TSI721_AERUESV_MALFORMED                                    (0x00040000)
#define TSI721_AERUESV_ECRC                                         (0x00080000)
#define TSI721_AERUESV_UR                                           (0x00100000)
#define TSI721_AERUESV_ACSV                                         (0x00200000)
#define TSI721_AERUESV_UIE                                          (0x00400000)
#define TSI721_AERUESV_MCBLKTLP                                     (0x00800000)
#define TSI721_AERUESV_ATOPEB                                       (0x01000000)
#define TSI721_AERUESV_TLPPBE                                       (0x02000000)

/* TSI721_AERCES : Register Bits Masks Definitions */
#define TSI721_AERCES_RCVERR                                        (0x00000001)
#define TSI721_AERCES_BADTLP                                        (0x00000040)
#define TSI721_AERCES_BADDLLP                                       (0x00000080)
#define TSI721_AERCES_RPLYROVR                                      (0x00000100)
#define TSI721_AERCES_RPLYTO                                        (0x00001000)
#define TSI721_AERCES_ADVISORYNF                                    (0x00002000)
#define TSI721_AERCES_CIE                                           (0x00004000)
#define TSI721_AERCES_HLO                                           (0x00008000)

/* TSI721_AERCEM : Register Bits Masks Definitions */
#define TSI721_AERCEM_RCVERR                                        (0x00000001)
#define TSI721_AERCEM_BADTLP                                        (0x00000040)
#define TSI721_AERCEM_BADDLLP                                       (0x00000080)
#define TSI721_AERCEM_RPLYROVR                                      (0x00000100)
#define TSI721_AERCEM_RPLYTO                                        (0x00001000)
#define TSI721_AERCEM_ADVISORYNF                                    (0x00002000)
#define TSI721_AERCEM_CIE                                           (0x00004000)
#define TSI721_AERCEM_HLO                                           (0x00008000)

/* TSI721_AERCTL : Register Bits Masks Definitions */
#define TSI721_AERCTL_FEPTR                                         (0x0000001f)
#define TSI721_AERCTL_ECRCGC                                        (0x00000020)
#define TSI721_AERCTL_ECRCGE                                        (0x00000040)
#define TSI721_AERCTL_ECRCCC                                        (0x00000080)
#define TSI721_AERCTL_ECRCCE                                        (0x00000100)
#define TSI721_AERCTL_MHRC                                          (0x00000200)
#define TSI721_AERCTL_MHRE                                          (0x00000400)

/* TSI721_AERHL1DW : Register Bits Masks Definitions */
#define TSI721_AERHL1DW_HL                                          (0xffffffff)

/* TSI721_AERHL2DW : Register Bits Masks Definitions */
#define TSI721_AERHL2DW_HL                                          (0xffffffff)

/* TSI721_AERHL3DW : Register Bits Masks Definitions */
#define TSI721_AERHL3DW_HL                                          (0xffffffff)

/* TSI721_AERHL4DW : Register Bits Masks Definitions */
#define TSI721_AERHL4DW_HL                                          (0xffffffff)

/* TSI721_SNUMCAP : Register Bits Masks Definitions */
#define TSI721_SNUMCAP_CAPID                                        (0x0000ffff)
#define TSI721_SNUMCAP_CAPVER                                       (0x000f0000)
#define TSI721_SNUMCAP_NXTPTR                                       (0xfff00000)

/* TSI721_SNUMLDW : Register Bits Masks Definitions */
#define TSI721_SNUMLDW_SNUM                                         (0xffffffff)

/* TSI721_SNUMUDW : Register Bits Masks Definitions */
#define TSI721_SNUMUDW_SNUM                                         (0xffffffff)

/* TSI721_EPCTL : Register Bits Masks Definitions */
#define TSI721_EPCTL_REGUNLOCK                                      (0x00000001)
#define TSI721_EPCTL_IFBCTDIS                                       (0x00000002)
#define TSI721_EPCTL_EFBCTDIS                                       (0x00000004)

/* TSI721_EPSTS : Register Bits Masks Definitions */
#define TSI721_EPSTS_QUASIRSTSTS                                    (0x00000001)

/* TSI721_SEDELAY : Register Bits Masks Definitions */
#define TSI721_SEDELAY_SEDELAY                                      (0x0000ffff)

/* TSI721_BARSETUP0 : Register Bits Masks Definitions */
#define TSI721_BARSETUP0_MEMSI                                      (0x00000001)
#define TSI721_BARSETUP0_TYPE                                       (0x00000006)
#define TSI721_BARSETUP0_PREF                                       (0x00000008)
#define TSI721_BARSETUP0_SIZE                                       (0x000003f0)
#define TSI721_BARSETUP0_EN                                         (0x80000000)

/* TSI721_BARSETUP1 : Register Bits Masks Definitions */
#define TSI721_BARSETUP1_MEMSI                                      (0x00000001)
#define TSI721_BARSETUP1_TYPE                                       (0x00000006)
#define TSI721_BARSETUP1_PREF                                       (0x00000008)
#define TSI721_BARSETUP1_SIZE                                       (0x000003f0)
#define TSI721_BARSETUP1_EN                                         (0x80000000)

/* TSI721_BARSETUP2 : Register Bits Masks Definitions */
#define TSI721_BARSETUP2_MEMSI                                      (0x00000001)
#define TSI721_BARSETUP2_TYPE                                       (0x00000006)
#define TSI721_BARSETUP2_PREF                                       (0x00000008)
#define TSI721_BARSETUP2_SIZE                                       (0x000003f0)
#define TSI721_BARSETUP2_EN                                         (0x80000000)

/* TSI721_BARSETUP3 : Register Bits Masks Definitions */
#define TSI721_BARSETUP3_MEMSI                                      (0x00000001)
#define TSI721_BARSETUP3_TYPE                                       (0x00000006)
#define TSI721_BARSETUP3_PREF                                       (0x00000008)
#define TSI721_BARSETUP3_SIZE                                       (0x000003f0)
#define TSI721_BARSETUP3_EN                                         (0x80000000)

/* TSI721_BARSETUP4 : Register Bits Masks Definitions */
#define TSI721_BARSETUP4_MEMSI                                      (0x00000001)
#define TSI721_BARSETUP4_TYPE                                       (0x00000006)
#define TSI721_BARSETUP4_PREF                                       (0x00000008)
#define TSI721_BARSETUP4_SIZE                                       (0x000003f0)
#define TSI721_BARSETUP4_EN                                         (0x80000000)

/* TSI721_BARSETUP5 : Register Bits Masks Definitions */
#define TSI721_BARSETUP5_MEMSI                                      (0x00000001)
#define TSI721_BARSETUP5_TYPE                                       (0x00000006)
#define TSI721_BARSETUP5_PREF                                       (0x00000008)
#define TSI721_BARSETUP5_SIZE                                       (0x000003f0)
#define TSI721_BARSETUP5_EN                                         (0x80000000)

/* TSI721_IERRORCTL : Register Bits Masks Definitions */
#define TSI721_IERRORCTL_IERROREN                                   (0x00000001)

/* TSI721_IERRORSTS0 : Register Bits Masks Definitions */
#define TSI721_IERRORSTS0_IFBPTLPTO                                 (0x00000001)
#define TSI721_IERRORSTS0_IFBNPTLPTO                                (0x00000002)
#define TSI721_IERRORSTS0_IFBCPTLPTO                                (0x00000004)
#define TSI721_IERRORSTS0_EFBPTLPTO                                 (0x00000010)
#define TSI721_IERRORSTS0_EFBNPTLPTO                                (0x00000020)
#define TSI721_IERRORSTS0_EFBCPTLPTO                                (0x00000040)
#define TSI721_IERRORSTS0_IFBDATSBE                                 (0x00000080)
#define TSI721_IERRORSTS0_IFBDATDBE                                 (0x00000100)
#define TSI721_IERRORSTS0_IFBCTLSBE                                 (0x00000200)
#define TSI721_IERRORSTS0_IFBCTLDBE                                 (0x00000400)
#define TSI721_IERRORSTS0_EFBDATSBE                                 (0x00000800)
#define TSI721_IERRORSTS0_EFBDATDBE                                 (0x00001000)
#define TSI721_IERRORSTS0_EFBCTLSBE                                 (0x00002000)
#define TSI721_IERRORSTS0_EFBCTLDBE                                 (0x00004000)
#define TSI721_IERRORSTS0_E2EPE                                     (0x00008000)
#define TSI721_IERRORSTS0_RBCTLSBE                                  (0x00010000)
#define TSI721_IERRORSTS0_RBCTLDBE                                  (0x00020000)

/* TSI721_IERRORMSK0 : Register Bits Masks Definitions */
#define TSI721_IERRORMSK0_IFBPTLPTO                                 (0x00000001)
#define TSI721_IERRORMSK0_IFBNPTLPTO                                (0x00000002)
#define TSI721_IERRORMSK0_IFBCPTLPTO                                (0x00000004)
#define TSI721_IERRORMSK0_EFBPTLPTO                                 (0x00000010)
#define TSI721_IERRORMSK0_EFBNPTLPTO                                (0x00000020)
#define TSI721_IERRORMSK0_EFBCPTLPTO                                (0x00000040)
#define TSI721_IERRORMSK0_IFBDATSBE                                 (0x00000080)
#define TSI721_IERRORMSK0_IFBDATDBE                                 (0x00000100)
#define TSI721_IERRORMSK0_IFBCTLSBE                                 (0x00000200)
#define TSI721_IERRORMSK0_IFBCTLDBE                                 (0x00000400)
#define TSI721_IERRORMSK0_EFBDATSBE                                 (0x00000800)
#define TSI721_IERRORMSK0_EFBDATDBE                                 (0x00001000)
#define TSI721_IERRORMSK0_EFBCTLSBE                                 (0x00002000)
#define TSI721_IERRORMSK0_EFBCTLDBE                                 (0x00004000)
#define TSI721_IERRORMSK0_E2EPE                                     (0x00008000)
#define TSI721_IERRORMSK0_RBCTLSBE                                  (0x00010000)
#define TSI721_IERRORMSK0_RBCTLDBE                                  (0x00020000)

/* TSI721_IERRORSEV0 : Register Bits Masks Definitions */
#define TSI721_IERRORSEV0_IFBPTLPTO                                 (0x00000001)
#define TSI721_IERRORSEV0_IFBNPTLPTO                                (0x00000002)
#define TSI721_IERRORSEV0_IFBCPTLPTO                                (0x00000004)
#define TSI721_IERRORSEV0_EFBPTLPTO                                 (0x00000010)
#define TSI721_IERRORSEV0_EFBNPTLPTO                                (0x00000020)
#define TSI721_IERRORSEV0_EFBCPTLPTO                                (0x00000040)
#define TSI721_IERRORSEV0_IFBDATSBE                                 (0x00000080)
#define TSI721_IERRORSEV0_IFBDATDBE                                 (0x00000100)
#define TSI721_IERRORSEV0_IFBCTLSBE                                 (0x00000200)
#define TSI721_IERRORSEV0_IFBCTLDBE                                 (0x00000400)
#define TSI721_IERRORSEV0_EFBDATSBE                                 (0x00000800)
#define TSI721_IERRORSEV0_EFBDATDBE                                 (0x00001000)
#define TSI721_IERRORSEV0_EFBCTLSBE                                 (0x00002000)
#define TSI721_IERRORSEV0_EFBCTLDBE                                 (0x00004000)
#define TSI721_IERRORSEV0_E2EPE                                     (0x00008000)
#define TSI721_IERRORSEV0_RBCTLSBE                                  (0x00010000)
#define TSI721_IERRORSEV0_RBCTLDBE                                  (0x00020000)

/* TSI721_IERRORTST0 : Register Bits Masks Definitions */
#define TSI721_IERRORTST0_IFBPTLPTO                                 (0x00000001)
#define TSI721_IERRORTST0_IFBNPTLPTO                                (0x00000002)
#define TSI721_IERRORTST0_IFBCPTLPTO                                (0x00000004)
#define TSI721_IERRORTST0_EFBPTLPTO                                 (0x00000010)
#define TSI721_IERRORTST0_EFBNPTLPTO                                (0x00000020)
#define TSI721_IERRORTST0_EFBCPTLPTO                                (0x00000040)
#define TSI721_IERRORTST0_IFBDATSBE                                 (0x00000080)
#define TSI721_IERRORTST0_IFBDATDBE                                 (0x00000100)
#define TSI721_IERRORTST0_IFBCTLSBE                                 (0x00000200)
#define TSI721_IERRORTST0_IFBCTLDBE                                 (0x00000400)
#define TSI721_IERRORTST0_EFBDATSBE                                 (0x00000800)
#define TSI721_IERRORTST0_EFBDATDBE                                 (0x00001000)
#define TSI721_IERRORTST0_EFBCTLSBE                                 (0x00002000)
#define TSI721_IERRORTST0_EFBCTLDBE                                 (0x00004000)
#define TSI721_IERRORTST0_E2EPE                                     (0x00008000)
#define TSI721_IERRORTST0_RBCTLSBE                                  (0x00010000)
#define TSI721_IERRORTST0_RBCTLDBE                                  (0x00020000)

/* TSI721_TOCTL : Register Bits Masks Definitions */
#define TSI721_TOCTL_ETO                                            (0x00000001)

/* TSI721_IFBTOCNT : Register Bits Masks Definitions */
#define TSI721_IFBTOCNT_IFBPTTOC                                    (0x000000ff)
#define TSI721_IFBTOCNT_IFBNPTOC                                    (0x0000ff00)
#define TSI721_IFBTOCNT_IFBCPTOC                                    (0x00ff0000)

/* TSI721_EFBTOCNT : Register Bits Masks Definitions */
#define TSI721_EFBTOCNT_EFBPTOC                                     (0x000000ff)
#define TSI721_EFBTOCNT_EFBNPTOC                                    (0x0000ff00)
#define TSI721_EFBTOCNT_EFBCPTOC                                    (0x00ff0000)

/* TSI721_TOTSCTL : Register Bits Masks Definitions */
#define TSI721_TOTSCTL_TCOUNT                                       (0x7fc00000)

/* TSI721_MECTL : Register Bits Masks Definitions */
#define TSI721_MECTL_EIEN                                           (0x00000008)
#define TSI721_MECTL_IFBDATSBE                                      (0x00000100)
#define TSI721_MECTL_IFBCTLSBE                                      (0x00000200)
#define TSI721_MECTL_IFBDATDBE                                      (0x00000400)
#define TSI721_MECTL_IFBCTLDBE                                      (0x00000800)
#define TSI721_MECTL_EFBDATSBE                                      (0x00001000)
#define TSI721_MECTL_EFBCTLSBE                                      (0x00002000)
#define TSI721_MECTL_EFBDATDBE                                      (0x00004000)
#define TSI721_MECTL_EFBCTLDBE                                      (0x00008000)
#define TSI721_MECTL_RBCTLSBE                                       (0x00010000)
#define TSI721_MECTL_RBCTLDBE                                       (0x00020000)

/* TSI721_SERDESCFG : Register Bits Masks Definitions */
#define TSI721_SERDESCFG_RCVD_OVRD                                  (0x0000000f)
#define TSI721_SERDESCFG_FEID                                       (0x00000100)
#define TSI721_SERDESCFG_EIDD                                       (0x00000200)
#define TSI721_SERDESCFG_P1D                                        (0x00000400)
#define TSI721_SERDESCFG_P2D                                        (0x00000800)
#define TSI721_SERDESCFG_ILPBSEL                                    (0x00007000)
#define TSI721_SERDESCFG_LSE                                        (0x00010000)

/* TSI721_SERDESSTS0 : Register Bits Masks Definitions */
#define TSI721_SERDESSTS0_RCVD                                      (0x0000000f)
#define TSI721_SERDESSTS0_CURR_SPEED                                (0x00000010)
#define TSI721_SERDESSTS0_PLL_LOCK                                  (0x00000020)
#define TSI721_SERDESSTS0_P0_READY                                  (0x00000040)
#define TSI721_SERDESSTS0_CDR_LOCK                                  (0x00000f00)
#define TSI721_SERDESSTS0_EIDLE_DETST                               (0x0000f000)
#define TSI721_SERDESSTS0_EIDLE_DET                                 (0x000f0000)
#define TSI721_SERDESSTS0_EIOS_DET                                  (0x00f00000)
#define TSI721_SERDESSTS0_EIDLE_INF                                 (0x01000000)
#define TSI721_SERDESSTS0_EIDLE_DETS                                (0x1e000000)

/* TSI721_LANESTS0 : Register Bits Masks Definitions */
#define TSI721_LANESTS0_PDE                                         (0x0000000f)
#define TSI721_LANESTS0_E8B10B                                      (0x000f0000)

/* TSI721_LANESTS1 : Register Bits Masks Definitions */
#define TSI721_LANESTS1_UND                                         (0x0000000f)
#define TSI721_LANESTS1_OVR                                         (0x000f0000)

/* TSI721_LANESTS2 : Register Bits Masks Definitions */
#define TSI721_LANESTS2_L0DAP                                       (0x0000000f)
#define TSI721_LANESTS2_L1DAP                                       (0x000000f0)
#define TSI721_LANESTS2_L2DAP                                       (0x00000f00)
#define TSI721_LANESTS2_L3DAP                                       (0x0000f000)

/* TSI721_PHYFSMT0 : Register Bits Masks Definitions */
#define TSI721_PHYFSMT0_SSCD                                        (0x00000003)
#define TSI721_PHYFSMT0_USCD                                        (0x0000000c)
#define TSI721_PHYFSMT0_REL                                         (0x00000030)
#define TSI721_PHYFSMT0_EITXPDG1                                    (0x00000f00)
#define TSI721_PHYFSMT0_EIRXPDG1                                    (0x0003f000)
#define TSI721_PHYFSMT0_RXDETDELAY                                  (0x003c0000)
#define TSI721_PHYFSMT0_EITXPDG2                                    (0x03c00000)
#define TSI721_PHYFSMT0_EIRXPDG2                                    (0xfc000000)

/* TSI721_PHYFSMT1 : Register Bits Masks Definitions */
#define TSI721_PHYFSMT1_SOSIP                                       (0x000007ff)
#define TSI721_PHYFSMT1_NFTSNCC                                     (0x0007f800)
#define TSI721_PHYFSMT1_NFTSCC                                      (0x07f80000)
#define TSI721_PHYFSMT1_EIES_FTS                                    (0xf0000000)

/* TSI721_PHYLCFG0 : Register Bits Masks Definitions */
#define TSI721_PHYLCFG0_LNKNUM                                      (0x000000ff)
#define TSI721_PHYLCFG0_G1CME                                       (0x00000100)
#define TSI721_PHYLCFG0_SRMBLDIS                                    (0x00000400)
#define TSI721_PHYLCFG0_CLINKDIS                                    (0x00000800)
#define TSI721_PHYLCFG0_PCEC                                        (0x00001000)
#define TSI721_PHYLCFG0_SCLINKEN                                    (0x00002000)
#define TSI721_PHYLCFG0_ILSCC                                       (0x00004000)
#define TSI721_PHYLCFG0_ECFGAREC                                    (0x00040000)
#define TSI721_PHYLCFG0_TLW                                         (0x00380000)
#define TSI721_PHYLCFG0_SLANEREV                                    (0x00400000)
#define TSI721_PHYLCFG0_FLANEREV                                    (0x00800000)
#define TSI721_PHYLCFG0_RDETECT                                     (0x30000000)

/* TSI721_PHYLCFG1 : Register Bits Masks Definitions */
#define TSI721_PHYLCFG1_LNPOLOR                                     (0x0000000f)
#define TSI721_PHYLCFG1_LNPOLOREN                                   (0x00000100)
#define TSI721_PHYLCFG1_NFTS_TOC                                    (0x00ff0000)
#define TSI721_PHYLCFG1_TXEIDL                                      (0x03000000)
#define TSI721_PHYLCFG1_L0S_RXEIDL                                  (0x0c000000)
#define TSI721_PHYLCFG1_L0S_RXSKP                                   (0x30000000)
#define TSI721_PHYLCFG1_TX_FULL_SKP                                 (0x40000000)

/* TSI721_PHYLSTS0 : Register Bits Masks Definitions */
#define TSI721_PHYLSTS0_STPSDP                                      (0x00000001)
#define TSI721_PHYLSTS0_PADERR                                      (0x00000002)
#define TSI721_PHYLSTS0_SEOPERR                                     (0x00000004)
#define TSI721_PHYLSTS0_EOPPERR                                     (0x00000008)
#define TSI721_PHYLSTS0_DSOPERR                                     (0x00000010)
#define TSI721_PHYLSTS0_SOPEOPERR                                   (0x00000020)
#define TSI721_PHYLSTS0_SOPLERR                                     (0x00000040)
#define TSI721_PHYLSTS0_RECDET                                      (0x00000200)
#define TSI721_PHYLSTS0_RECCON                                      (0x00000400)
#define TSI721_PHYLSTS0_L0SREC                                      (0x00000800)
#define TSI721_PHYLSTS0_ILW                                         (0x0000c000)
#define TSI721_PHYLSTS0_LPWUC                                       (0x00010000)
#define TSI721_PHYLSTS0_LNPOLORSTS                                  (0xff000000)

/* TSI721_PHYLSTS1 : Register Bits Masks Definitions */
#define TSI721_PHYLSTS1_TRAINDE                                     (0x00000002)
#define TSI721_PHYLSTS1_RFTSOS                                      (0x00000004)
#define TSI721_PHYLSTS1_RIDLOS                                      (0x00000008)
#define TSI721_PHYLSTS1_RTS2OS                                      (0x00000010)
#define TSI721_PHYLSTS1_RTS1OS                                      (0x00000020)
#define TSI721_PHYLSTS1_RSKPOS                                      (0x00000040)
#define TSI721_PHYLSTS1_RXFRERR                                     (0x00010000)
#define TSI721_PHYLSTS1_RXDISPERR                                   (0x00020000)
#define TSI721_PHYLSTS1_RXUFERR                                     (0x00040000)
#define TSI721_PHYLSTS1_RXOFERR                                     (0x00080000)
#define TSI721_PHYLSTS1_RX8B10BERR                                  (0x00100000)

/* TSI721_PHYLSTATE0 : Register Bits Masks Definitions */
#define TSI721_PHYLSTATE0_LTSSMSTATE                                (0x0000001f)
#define TSI721_PHYLSTATE0_TXLSTATE                                  (0x000000e0)
#define TSI721_PHYLSTATE0_RXLSTATE                                  (0x00000700)
#define TSI721_PHYLSTATE0_SRMBLSTAT                                 (0x00000800)
#define TSI721_PHYLSTATE0_LPNFTS                                    (0x000ff000)
#define TSI721_PHYLSTATE0_LNKNUM                                    (0x1ff00000)
#define TSI721_PHYLSTATE0_LANEREV                                   (0x20000000)
#define TSI721_PHYLSTATE0_FLRET                                     (0x80000000)

/* TSI721_PHYLTSSMSTS0 : Register Bits Masks Definitions */
#define TSI721_PHYLTSSMSTS0_XMIT_EIOS                               (0x00000001)
#define TSI721_PHYLTSSMSTS0_TMOUT_1MS                               (0x00000002)
#define TSI721_PHYLTSSMSTS0_DQUIET                                  (0x00000004)
#define TSI721_PHYLTSSMSTS0_DACTIVE                                 (0x00000008)
#define TSI721_PHYLTSSMSTS0_PACTIVE                                 (0x00000010)
#define TSI721_PHYLTSSMSTS0_PCOMP                                   (0x00000020)
#define TSI721_PHYLTSSMSTS0_PCONFIG                                 (0x00000040)
#define TSI721_PHYLTSSMSTS0_CLWSTART                                (0x00000100)
#define TSI721_PHYLTSSMSTS0_CLWACCEPT                               (0x00000200)
#define TSI721_PHYLTSSMSTS0_CLNWAIT                                 (0x00000400)
#define TSI721_PHYLTSSMSTS0_CLNACCEPT                               (0x00000800)
#define TSI721_PHYLTSSMSTS0_CCOMPLETE                               (0x00001000)
#define TSI721_PHYLTSSMSTS0_CIDLE                                   (0x00002000)
#define TSI721_PHYLTSSMSTS0_TOUPCFG                                 (0x00008000)
#define TSI721_PHYLTSSMSTS0_RECRCVLOCK                              (0x00010000)
#define TSI721_PHYLTSSMSTS0_RECRCVCFG                               (0x00020000)
#define TSI721_PHYLTSSMSTS0_RECIDLE                                 (0x00040000)
#define TSI721_PHYLTSSMSTS0_RECSPEED                                (0x00080000)
#define TSI721_PHYLTSSMSTS0_L0                                      (0x00100000)
#define TSI721_PHYLTSSMSTS0_L0S                                     (0x00200000)
#define TSI721_PHYLTSSMSTS0_L1ENTRY                                 (0x00400000)
#define TSI721_PHYLTSSMSTS0_L1IDLE                                  (0x00800000)
#define TSI721_PHYLTSSMSTS0_L2IDLE                                  (0x01000000)
#define TSI721_PHYLTSSMSTS0_L2XMITWAKE                              (0x02000000)
#define TSI721_PHYLTSSMSTS0_DISABLED                                (0x04000000)
#define TSI721_PHYLTSSMSTS0_HOTRESET                                (0x08000000)
#define TSI721_PHYLTSSMSTS0_LBENTRY                                 (0x10000000)
#define TSI721_PHYLTSSMSTS0_LBACTIVE                                (0x20000000)
#define TSI721_PHYLTSSMSTS0_LBEXIT                                  (0x40000000)
#define TSI721_PHYLTSSMSTS0_IDT_TM                                  (0x80000000)

/* TSI721_PHYLTSSMSTS1 : Register Bits Masks Definitions */
#define TSI721_PHYLTSSMSTS1_TXACTIVE                                (0x00000001)
#define TSI721_PHYLTSSMSTS1_TXL0SENTRY                              (0x00000002)
#define TSI721_PHYLTSSMSTS1_TXL0SIDLE                               (0x00000004)
#define TSI721_PHYLTSSMSTS1_TXL0SFTS                                (0x00000008)
#define TSI721_PHYLTSSMSTS1_RXACTIVE                                (0x00000010)
#define TSI721_PHYLTSSMSTS1_RXL0SENTRY                              (0x00000020)
#define TSI721_PHYLTSSMSTS1_RXL0SIDLE                               (0x00000040)
#define TSI721_PHYLTSSMSTS1_RXL0SFTS                                (0x00000080)

/* TSI721_PHYCNT0 : Register Bits Masks Definitions */
#define TSI721_PHYCNT0_COUNT                                        (0xffffffff)

/* TSI721_PHYCNT1 : Register Bits Masks Definitions */
#define TSI721_PHYCNT1_COUNT                                        (0xffffffff)

/* TSI721_PHYCNTCFG : Register Bits Masks Definitions */
#define TSI721_PHYCNTCFG_PHYCNT0SEL                                 (0x0000007f)
#define TSI721_PHYCNTCFG_PHYCNT1SEL                                 (0x00007f00)
#define TSI721_PHYCNTCFG_LANESEL                                    (0x001f0000)

/* TSI721_PHYRECEL : Register Bits Masks Definitions */
#define TSI721_PHYRECEL_RCOUNT                                      (0x000000ff)
#define TSI721_PHYRECEL_LRET                                        (0x00000100)
#define TSI721_PHYRECEL_LDIS                                        (0x00000200)
#define TSI721_PHYRECEL_DDL                                         (0x00000400)
#define TSI721_PHYRECEL_DSKERR                                      (0x00000800)
#define TSI721_PHYRECEL_ILSC                                        (0x00001000)
#define TSI721_PHYRECEL_OLSC                                        (0x00004000)
#define TSI721_PHYRECEL_TSL0                                        (0x00008000)
#define TSI721_PHYRECEL_EIDLL0                                      (0x00010000)
#define TSI721_PHYRECEL_NFTSTO                                      (0x00020000)
#define TSI721_PHYRECEL_HOTRST                                      (0x00040000)
#define TSI721_PHYRECEL_CITO                                        (0x00080000)
#define TSI721_PHYRECEL_L1EXIT                                      (0x00100000)
#define TSI721_PHYRECEL_TM                                          (0x00200000)
#define TSI721_PHYRECEL_FC                                          (0x0f000000)
#define TSI721_PHYRECEL_ENLOG                                       (0x80000000)

/* TSI721_PHYPRBS : Register Bits Masks Definitions */
#define TSI721_PHYPRBS_SEED                                         (0x0000ffff)

/* TSI721_DLCTL1 : Register Bits Masks Definitions */
#define TSI721_DLCTL1_RPTIMEOUT                                     (0x00007fff)
#define TSI721_DLCTL1_RPTIMEOUTO                                    (0x00008000)
#define TSI721_DLCTL1_ANTIMEOUT                                     (0x7fff0000)
#define TSI721_DLCTL1_ANTIMEOUTO                                    (0x80000000)

/* TSI721_DLCTL2 : Register Bits Masks Definitions */
#define TSI721_DLCTL2_INITFCVALVC                                   (0x00003fff)
#define TSI721_DLCTL2_INITFCTOVC                                    (0x00004000)
#define TSI721_DLCTL2_DISCRCCHK                                     (0x80000000)

/* TSI721_DLCTL3 : Register Bits Masks Definitions */
#define TSI721_DLCTL3_DLLPRXTO                                      (0x0001ffff)
#define TSI721_DLCTL3_DLLPRXTE                                      (0x00100000)

/* TSI721_DLSTS : Register Bits Masks Definitions */
#define TSI721_DLSTS_DLFSM                                          (0x00000003)
#define TSI721_DLSTS_RXFERR                                         (0x00000010)
#define TSI721_DLSTS_RXPROTERR                                      (0x00000080)
#define TSI721_DLSTS_DLBUFOVRFL                                     (0x00000100)

/* TSI721_DLRXSTS : Register Bits Masks Definitions */
#define TSI721_DLRXSTS_TLP                                          (0x00000001)
#define TSI721_DLRXSTS_TLPCRCERR                                    (0x00000002)
#define TSI721_DLRXSTS_DLLP                                         (0x00000004)
#define TSI721_DLRXSTS_DLLPCRCERR                                   (0x00000008)
#define TSI721_DLRXSTS_NACKDLLP                                     (0x00000010)
#define TSI721_DLRXSTS_ACKDLLP                                      (0x00000020)
#define TSI721_DLRXSTS_IFCDLLP1                                     (0x00000040)
#define TSI721_DLRXSTS_IFCDLLP2                                     (0x00000100)
#define TSI721_DLRXSTS_UFCDLLP                                      (0x00000400)
#define TSI721_DLRXSTS_PMDLLP                                       (0x00001000)
#define TSI721_DLRXSTS_ROSEQ                                        (0x00002000)
#define TSI721_DLRXSTS_RBEDB                                        (0x00004000)
#define TSI721_DLRXSTS_RDUPTLP                                      (0x00008000)
#define TSI721_DLRXSTS_RTLPNULL                                     (0x00020000)
#define TSI721_DLRXSTS_DLLPRXTO                                     (0x00040000)
#define TSI721_DLRXSTS_RXVDDLLP                                     (0x00080000)
#define TSI721_DLRXSTS_RXUDLLP                                      (0x00100000)
#define TSI721_DLRXSTS_RXTLPLERR                                    (0x00200000)

/* TSI721_DLTXSTS : Register Bits Masks Definitions */
#define TSI721_DLTXSTS_TLP                                          (0x00000001)
#define TSI721_DLTXSTS_TXNRPTLP                                     (0x00000002)
#define TSI721_DLTXSTS_TXRPTLP                                      (0x00000004)
#define TSI721_DLTXSTS_TXTLPNULL                                    (0x00000008)
#define TSI721_DLTXSTS_DLLP                                         (0x00000040)
#define TSI721_DLTXSTS_NACKDLLP                                     (0x00000080)
#define TSI721_DLTXSTS_ACKDLLP                                      (0x00000100)
#define TSI721_DLTXSTS_IFCDLLP1                                     (0x00000200)
#define TSI721_DLTXSTS_IFCDLLP2                                     (0x00000800)
#define TSI721_DLTXSTS_UFCDLLP                                      (0x00002000)
#define TSI721_DLTXSTS_PMDLLP                                       (0x00008000)
#define TSI721_DLTXSTS_ANTIMOUT                                     (0x00010000)
#define TSI721_DLTXSTS_REPLAYTO                                     (0x00020000)
#define TSI721_DLTXSTS_REPLAYEVNT                                   (0x00040000)
#define TSI721_DLTXSTS_RPNUMRO                                      (0x00200000)
#define TSI721_DLTXSTS_VC0INITFCTO                                  (0x01000000)

/* TSI721_DLCNT0 : Register Bits Masks Definitions */
#define TSI721_DLCNT0_COUNT                                         (0xffffffff)

/* TSI721_DLCNT1 : Register Bits Masks Definitions */
#define TSI721_DLCNT1_COUNT                                         (0xffffffff)

/* TSI721_DLCNTCFG : Register Bits Masks Definitions */
#define TSI721_DLCNTCFG_DLCNT0SEL                                   (0x0000001f)
#define TSI721_DLCNTCFG_DLCNT1SEL                                   (0x00001f00)

/* TSI721_TLSTSE : Register Bits Masks Definitions */
#define TSI721_TLSTSE_RUR                                           (0x00000001)
#define TSI721_TLSTSE_MALFORMED                                     (0x00000004)
#define TSI721_TLSTSE_NULLIFIED                                     (0x00000008)
#define TSI721_TLSTSE_RO                                            (0x00000010)
#define TSI721_TLSTSE_RCVTLP                                        (0x00010000)
#define TSI721_TLSTSE_RTLPCPE                                       (0x00040000)
#define TSI721_TLSTSE_RTLPMPE                                       (0x00080000)
#define TSI721_TLSTSE_RTLPIOPE                                      (0x00100000)
#define TSI721_TLSTSE_IETLPME                                       (0x00200000)
#define TSI721_TLSTSE_RTLPME                                        (0x00400000)

/* TSI721_TLCTL : Register Bits Masks Definitions */
#define TSI721_TLCTL_FCUTIMER                                       (0x000003ff)
#define TSI721_TLCTL_FCUTIMERO                                      (0x00000400)
#define TSI721_TLCTL_SEQTAG                                         (0x00000800)

/* TSI721_TLCNT0 : Register Bits Masks Definitions */
#define TSI721_TLCNT0_COUNT                                         (0xffffffff)

/* TSI721_TLCNT1 : Register Bits Masks Definitions */
#define TSI721_TLCNT1_COUNT                                         (0xffffffff)

/* TSI721_TLCNTCFG : Register Bits Masks Definitions */
#define TSI721_TLCNTCFG_TLCNT0SEL                                   (0x0000001f)
#define TSI721_TLCNTCFG_TLCNT1SEL                                   (0x000003e0)
#define TSI721_TLCNTCFG_FUNC                                        (0x00070000)
#define TSI721_TLCNTCFG_DEV                                         (0x00f80000)
#define TSI721_TLCNTCFG_BUS                                         (0xff000000)

/* TSI721_INTSTS : Register Bits Masks Definitions */
#define TSI721_INTSTS_INTA                                          (0x00000001)
#define TSI721_INTSTS_INTB                                          (0x00000002)
#define TSI721_INTSTS_INTC                                          (0x00000004)
#define TSI721_INTSTS_INTD                                          (0x00000008)

/* TSI721_PMPC0 : Register Bits Masks Definitions */
#define TSI721_PMPC0_L0ET                                           (0x0fff0000)
#define TSI721_PMPC0_L0SASPMD                                       (0x10000000)
#define TSI721_PMPC0_L1ASPMD                                        (0x20000000)
#define TSI721_PMPC0_D3HOTL1D                                       (0x40000000)

/* TSI721_PMPC1 : Register Bits Masks Definitions */
#define TSI721_PMPC1_PMCS                                           (0x0000003f)
#define TSI721_PMPC1_EXITL1                                         (0x00000100)
#define TSI721_PMPC1_ENTRLTR                                        (0x00000200)
#define TSI721_PMPC1_ENTRLTA                                        (0x00000400)
#define TSI721_PMPC1_ENTRL1T                                        (0x00000800)
#define TSI721_PMPC1_ENTRL1C                                        (0x00001000)
#define TSI721_PMPC1_ENTRL1P                                        (0x00002000)
#define TSI721_PMPC1_EXITL0S                                        (0x00004000)
#define TSI721_PMPC1_ENTRL0S                                        (0x00008000)

/* TSI721_FCVC0PTCC : Register Bits Masks Definitions */
#define TSI721_FCVC0PTCC_DATAFCCC                                   (0x00000fff)
#define TSI721_FCVC0PTCC_HDRFCCC                                    (0x003fc000)

/* TSI721_FCVC0NPCC : Register Bits Masks Definitions */
#define TSI721_FCVC0NPCC_DATAFCCC                                   (0x00000fff)
#define TSI721_FCVC0NPCC_HDRFCCC                                    (0x003fc000)

/* TSI721_FCVC0CPCC : Register Bits Masks Definitions */
#define TSI721_FCVC0CPCC_DATAFCCC                                   (0x00000fff)
#define TSI721_FCVC0CPCC_HDRFCCC                                    (0x003fc000)

/* TSI721_FCVC0PTCL : Register Bits Masks Definitions */
#define TSI721_FCVC0PTCL_DATAFCCC                                   (0x00000fff)
#define TSI721_FCVC0PTCL_HDRFCCC                                    (0x003fc000)
#define TSI721_FCVC0PTCL_INFDAT                                     (0x20000000)
#define TSI721_FCVC0PTCL_INFHDR                                     (0x40000000)
#define TSI721_FCVC0PTCL_VALID                                      (0x80000000)

/* TSI721_FCVC0NPCL : Register Bits Masks Definitions */
#define TSI721_FCVC0NPCL_DATAFCCC                                   (0x00000fff)
#define TSI721_FCVC0NPCL_HDRFCCC                                    (0x003fc000)
#define TSI721_FCVC0NPCL_INFDAT                                     (0x20000000)
#define TSI721_FCVC0NPCL_INFHDR                                     (0x40000000)
#define TSI721_FCVC0NPCL_VALID                                      (0x80000000)

/* TSI721_FCVC0CPCL : Register Bits Masks Definitions */
#define TSI721_FCVC0CPCL_DATAFCCC                                   (0x00000fff)
#define TSI721_FCVC0CPCL_HDRFCCC                                    (0x003fc000)
#define TSI721_FCVC0CPCL_INFDAT                                     (0x20000000)
#define TSI721_FCVC0CPCL_INFHDR                                     (0x40000000)
#define TSI721_FCVC0CPCL_VALID                                      (0x80000000)

/* TSI721_FCVC0PTCA : Register Bits Masks Definitions */
#define TSI721_FCVC0PTCA_DATAFCCC                                   (0x00000fff)
#define TSI721_FCVC0PTCA_HDRFCCC                                    (0x003fc000)

/* TSI721_FCVC0NPCA : Register Bits Masks Definitions */
#define TSI721_FCVC0NPCA_DATAFCCC                                   (0x00000fff)
#define TSI721_FCVC0NPCA_HDRFCCC                                    (0x003fc000)

/* TSI721_FCVC0CPCA : Register Bits Masks Definitions */
#define TSI721_FCVC0CPCA_DATAFCCC                                   (0x00000fff)
#define TSI721_FCVC0CPCA_HDRFCCC                                    (0x003fc000)

/* TSI721_FCVC0PTCR : Register Bits Masks Definitions */
#define TSI721_FCVC0PTCR_DATAFCCC                                   (0x00000fff)
#define TSI721_FCVC0PTCR_HDRFCCC                                    (0x003fc000)

/* TSI721_FCVC0NPCR : Register Bits Masks Definitions */
#define TSI721_FCVC0NPCR_DATAFCCC                                   (0x00000fff)
#define TSI721_FCVC0NPCR_HDRFCCC                                    (0x003fc000)

/* TSI721_FCVC0CPCR : Register Bits Masks Definitions */
#define TSI721_FCVC0CPCR_DATAFCCC                                   (0x00000fff)
#define TSI721_FCVC0CPCR_HDRFCCC                                    (0x003fc000)

/* TSI721_EFBTC : Register Bits Masks Definitions */
#define TSI721_EFBTC_VC0IPTT                                        (0x00000001)
#define TSI721_EFBTC_VC0INPT                                        (0x00000002)
#define TSI721_EFBTC_VC0ICPT                                        (0x00000004)

/* TSI721_IFBCNT0 : Register Bits Masks Definitions */
#define TSI721_IFBCNT0_COUNT                                        (0xffffffff)

/* TSI721_IFBCNT1 : Register Bits Masks Definitions */
#define TSI721_IFBCNT1_COUNT                                        (0xffffffff)

/* TSI721_IFBCNTCFG : Register Bits Masks Definitions */
#define TSI721_IFBCNTCFG_IFBCNT0SEL                                 (0x0000001f)
#define TSI721_IFBCNTCFG_IFBCNT1SEL                                 (0x000003e0)

/* TSI721_EFBCNT0 : Register Bits Masks Definitions */
#define TSI721_EFBCNT0_COUNT                                        (0xffffffff)

/* TSI721_EFBCNT1 : Register Bits Masks Definitions */
#define TSI721_EFBCNT1_COUNT                                        (0xffffffff)

/* TSI721_EFBCNTCFG : Register Bits Masks Definitions */
#define TSI721_EFBCNTCFG_EFBCNT0SEL                                 (0x0000001f)
#define TSI721_EFBCNTCFG_EFBCNT1SEL                                 (0x000003e0)

/* TSI721_UEEM : Register Bits Masks Definitions */
#define TSI721_UEEM_DLPERR                                          (0x00000010)
#define TSI721_UEEM_POISONED                                        (0x00001000)
#define TSI721_UEEM_COMPTO                                          (0x00004000)
#define TSI721_UEEM_CABORT                                          (0x00008000)
#define TSI721_UEEM_UECOMP                                          (0x00010000)
#define TSI721_UEEM_RCVOVR                                          (0x00020000)
#define TSI721_UEEM_MALFORMED                                       (0x00040000)
#define TSI721_UEEM_ECRC                                            (0x00080000)
#define TSI721_UEEM_UR                                              (0x00100000)
#define TSI721_UEEM_UIE                                             (0x00400000)
#define TSI721_UEEM_ADVISORYNF                                      (0x80000000)

/* TSI721_CEEM : Register Bits Masks Definitions */
#define TSI721_CEEM_RCVERR                                          (0x00000001)
#define TSI721_CEEM_BADTLP                                          (0x00000040)
#define TSI721_CEEM_BADDLLP                                         (0x00000080)
#define TSI721_CEEM_RPLYROVR                                        (0x00000100)
#define TSI721_CEEM_RPLYTO                                          (0x00001000)
#define TSI721_CEEM_CIE                                             (0x00004000)
#define TSI721_CEEM_HLO                                             (0x00008000)

/* TSI721_STMCTL : Register Bits Masks Definitions */
#define TSI721_STMCTL_CMD                                           (0x00000007)
#define TSI721_STMCTL_SPEED                                         (0x00000008)

/* TSI721_STMSTS : Register Bits Masks Definitions */
#define TSI721_STMSTS_CC                                            (0x00000001)
#define TSI721_STMSTS_PSTATE                                        (0x00000006)

/* TSI721_STMTCTL : Register Bits Masks Definitions */
#define TSI721_STMTCTL_TSEL                                         (0x0000000f)
#define TSI721_STMTCTL_TSYNCP                                       (0x00000030)

/* TSI721_STMTSTS : Register Bits Masks Definitions */
#define TSI721_STMTSTS_SYNC                                         (0x0000000f)
#define TSI721_STMTSTS_TR                                           (0x80000000)

/* TSI721_STMECNT0 : Register Bits Masks Definitions */
#define TSI721_STMECNT0_COUNT                                       (0x000003ff)
#define TSI721_STMECNT0_OVR                                         (0x00000400)
#define TSI721_STMECNT0_ERR_DET                                     (0x00000800)
#define TSI721_STMECNT0_ERR_INJ                                     (0x00010000)

/* TSI721_STMECNT1 : Register Bits Masks Definitions */
#define TSI721_STMECNT1_COUNT                                       (0x000003ff)
#define TSI721_STMECNT1_OVR                                         (0x00000400)
#define TSI721_STMECNT1_ERR_DET                                     (0x00000800)
#define TSI721_STMECNT1_ERR_INJ                                     (0x00010000)

/* TSI721_STMECNT2 : Register Bits Masks Definitions */
#define TSI721_STMECNT2_COUNT                                       (0x000003ff)
#define TSI721_STMECNT2_OVR                                         (0x00000400)
#define TSI721_STMECNT2_ERR_DET                                     (0x00000800)
#define TSI721_STMECNT2_ERR_INJ                                     (0x00010000)

/* TSI721_STMECNT3 : Register Bits Masks Definitions */
#define TSI721_STMECNT3_COUNT                                       (0x000003ff)
#define TSI721_STMECNT3_OVR                                         (0x00000400)
#define TSI721_STMECNT3_ERR_DET                                     (0x00000800)
#define TSI721_STMECNT3_ERR_INJ                                     (0x00010000)

/* TSI721_ALLCS : Register Bits Masks Definitions */
#define TSI721_ALLCS_ALLME                                          (0x00000001)
#define TSI721_ALLCS_ALLS                                           (0x80000000)

/* TSI721_IFBVC0PTCFG : Register Bits Masks Definitions */
#define TSI721_IFBVC0PTCFG_PTHDR                                    (0x000000ff)
#define TSI721_IFBVC0PTCFG_PTDATA                                   (0x03ff0000)

/* TSI721_IFBVC0NPCFG : Register Bits Masks Definitions */
#define TSI721_IFBVC0NPCFG_NPHDR                                    (0x000000ff)
#define TSI721_IFBVC0NPCFG_NPDATA                                   (0x03ff0000)

/* TSI721_IFBVC0CPCFG : Register Bits Masks Definitions */
#define TSI721_IFBVC0CPCFG_CPHDR                                    (0x000000ff)
#define TSI721_IFBVC0CPCFG_CPDATA                                   (0x03ff0000)

/* TSI721_IFCSTS : Register Bits Masks Definitions */
#define TSI721_IFCSTS_VC0PTHO                                       (0x00000001)
#define TSI721_IFCSTS_VC0NPHO                                       (0x00000002)
#define TSI721_IFCSTS_VC0CPHO                                       (0x00000004)
#define TSI721_IFCSTS_VC0PTDO                                       (0x00000008)
#define TSI721_IFCSTS_VC0NPDO                                       (0x00000010)
#define TSI721_IFCSTS_VC0CPDO                                       (0x00000020)

/* TSI721_EFBVC0PTSTS : Register Bits Masks Definitions */
#define TSI721_EFBVC0PTSTS_HDR                                      (0x000000ff)
#define TSI721_EFBVC0PTSTS_DATA                                     (0x07ff0000)

/* TSI721_EFBVC0NPSTS : Register Bits Masks Definitions */
#define TSI721_EFBVC0NPSTS_HDR                                      (0x000000ff)
#define TSI721_EFBVC0NPSTS_DATA                                     (0x07ff0000)

/* TSI721_EFBVC0CPSTS : Register Bits Masks Definitions */
#define TSI721_EFBVC0CPSTS_HDR                                      (0x000000ff)
#define TSI721_EFBVC0CPSTS_DATA                                     (0x07ff0000)

/* TSI721_EFBRBSTS : Register Bits Masks Definitions */
#define TSI721_EFBRBSTS_FULL                                        (0x00000100)


/******************************************************/
/* TSI721 : S-RIO Register address offset definitions */
/******************************************************/

#define TSI721_MAX_PORTS 1
#define TSI721_MAX_LANES 4

#define TSI721_RIO_DEV_ID                                          (0x00000000)
#define TSI721_RIO_DEV_INFO                                        (0x00000004)
#define TSI721_RIO_ASBLY_ID                                        (0x00000008)
#define TSI721_RIO_ASBLY_INFO                                      (0x0000000c)
#define TSI721_RIO_PE_FEAT                                         (0x00000010)
#define TSI721_RIO_SRC_OP                                          (0x00000018)
#define TSI721_RIO_DEST_OP                                         (0x0000001c)
#define TSI721_RIO_SR_XADDR                                        (0x0000004c)
#define TSI721_RIO_BASE_ID                                         (0x00000060)
#define TSI721_RIO_HOST_BASE_ID_LOCK                               (0x00000068)
#define TSI721_RIO_COMP_TAG                                        (0x0000006c)
#define TSI721_RIO_SP_MB_HEAD                                      (0x00000100)
#define TSI721_RIO_SP_LT_CTL                                       (0x00000120)
#define TSI721_RIO_SR_RSP_TO                                       (0x00000124)
#define TSI721_RIO_SP_GEN_CTL                                      (0x0000013c)
#define TSI721_RIO_SP_LM_REQ                                       (0x00000140)
#define TSI721_RIO_SP_LM_RESP                                      (0x00000144)
#define TSI721_RIO_SP_ACKID_STAT                                   (0x00000148)
#define TSI721_RIO_SP_CTL2                                         (0x00000154)
#define TSI721_RIO_SP_ERR_STAT                                     (0x00000158)
#define TSI721_RIO_SP_CTL                                          (0x0000015c)
#define TSI721_RIO_ERR_RPT_BH                                      (0x00001000)
#define TSI721_RIO_ERR_DET                                         (0x00001008)
#define TSI721_RIO_ERR_EN                                          (0x0000100c)
#define TSI721_RIO_H_ADDR_CAPT                                     (0x00001010)
#define TSI721_RIO_ADDR_CAPT                                       (0x00001014)
#define TSI721_RIO_ID_CAPT                                         (0x00001018)
#define TSI721_RIO_CTRL_CAPT                                       (0x0000101c)
#define TSI721_RIO_PW_TGT_ID                                       (0x00001028)
#define TSI721_RIO_SP_ERR_DET                                      (0x00001040)
#define TSI721_RIO_SP_RATE_EN                                      (0x00001044)
#define TSI721_RIO_SP_ERR_ATTR_CAPT                                (0x00001048)
#define TSI721_RIO_SP_ERR_CAPT_0                                   (0x0000104c)
#define TSI721_RIO_SP_ERR_CAPT_1                                   (0x00001050)
#define TSI721_RIO_SP_ERR_CAPT_2                                   (0x00001054)
#define TSI721_RIO_SP_ERR_CAPT_3                                   (0x00001058)
#define TSI721_RIO_SP_ERR_RATE                                     (0x00001068)
#define TSI721_RIO_SP_ERR_THRESH                                   (0x0000106c)
#define TSI721_RIO_PER_LANE_BH                                     (0x00003000)
#define TSI721_RIO_LANEX_STAT0(X)                          (0x3010 + 0x020*(X))
#define TSI721_RIO_LANEX_STAT1(X)                          (0x3014 + 0x020*(X))
#define TSI721_RIO_PLM_BH                                          (0x00010000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL                             (0x00010080)
#define TSI721_RIO_PLM_SP_uint32_t                                   (0x00010090)
#define TSI721_RIO_PLM_SP_INT_ENABLE                               (0x00010094)
#define TSI721_RIO_PLM_SP_PW_ENABLE                                (0x00010098)
#define TSI721_RIO_PLM_SP_EVENT_GEN                                (0x0001009c)
#define TSI721_RIO_PLM_SP_ALL_INT_EN                               (0x000100a0)
#define TSI721_RIO_PLM_SP_ALL_PW_EN                                (0x000100a4)
#define TSI721_RIO_PLM_SP_DISCOVERY_TIMER                          (0x000100b4)
#define TSI721_RIO_PLM_SP_SILENCE_TIMER                            (0x000100b8)
#define TSI721_RIO_PLM_SP_VMIN_EXP                                 (0x000100bc)
#define TSI721_RIO_PLM_SP_POL_CTL                                  (0x000100c0)
#define TSI721_RIO_PLM_SP_DENIAL_CTL                               (0x000100c8)
#define TSI721_RIO_PLM_SP_RCVD_MECS                                (0x000100d0)
#define TSI721_RIO_PLM_SP_MECS_FWD                                 (0x000100d8)
#define TSI721_RIO_PLM_SP_LONG_CS_TX1                              (0x000100e0)
#define TSI721_RIO_PLM_SP_LONG_CS_TX2                              (0x000100e4)
#define TSI721_RIO_TLM_BH                                          (0x00010300)
#define TSI721_RIO_TLM_SP_CONTROL                                  (0x00010380)
#define TSI721_RIO_TLM_SP_uint32_t                                   (0x00010390)
#define TSI721_RIO_TLM_SP_INT_ENABLE                               (0x00010394)
#define TSI721_RIO_TLM_SP_PW_ENABLE                                (0x00010398)
#define TSI721_RIO_TLM_SP_EVENT_GEN                                (0x0001039c)
#define TSI721_RIO_TLM_SP_BRR_CTLX(X)                     (0x103a0 + 0x010*(X))
#define TSI721_RIO_TLM_SP_BRR_PATTERN_MATCHX(X)           (0x103a4 + 0x010*(X))
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL                         (0x000103e0)
#define TSI721_RIO_PBM_BH                                          (0x00010600)
#define TSI721_RIO_PBM_SP_CONTROL                                  (0x00010680)
#define TSI721_RIO_PBM_SP_uint32_t                                   (0x00010690)
#define TSI721_RIO_PBM_SP_INT_ENABLE                               (0x00010694)
#define TSI721_RIO_PBM_SP_PW_ENABLE                                (0x00010698)
#define TSI721_RIO_PBM_SP_EVENT_GEN                                (0x0001069c)
#define TSI721_RIO_PBM_SP_IG_WATERMARK0                            (0x000106b0)
#define TSI721_RIO_PBM_SP_IG_WATERMARK1                            (0x000106b4)
#define TSI721_RIO_PBM_SP_IG_WATERMARK2                            (0x000106b8)
#define TSI721_RIO_PBM_SP_IG_WATERMARK3                            (0x000106bc)
#define TSI721_RIO_EM_BH                                           (0x00010900)
#define TSI721_RIO_EM_INT_STAT                                     (0x00010910)
#define TSI721_RIO_EM_INT_ENABLE                                   (0x00010914)
#define TSI721_RIO_EM_INT_PORT_STAT                                (0x00010918)
#define TSI721_RIO_EM_PW_STAT                                      (0x00010920)
#define TSI721_RIO_EM_PW_ENABLE                                    (0x00010924)
#define TSI721_RIO_EM_PW_PORT_STAT                                 (0x00010928)
#define TSI721_RIO_EM_DEV_INT_EN                                   (0x00010930)
#define TSI721_RIO_EM_DEV_PW_EN                                    (0x00010934)
#define TSI721_RIO_EM_MECS_STAT                                    (0x0001093c)
#define TSI721_RIO_EM_MECS_INT_EN                                  (0x00010940)
#define TSI721_RIO_EM_MECS_CAP_EN                                  (0x00010944)
#define TSI721_RIO_EM_MECS_TRIG_EN                                 (0x00010948)
#define TSI721_RIO_EM_MECS_REQ                                     (0x0001094c)
#define TSI721_RIO_EM_MECS_PORT_STAT                               (0x00010950)
#define TSI721_RIO_EM_MECS_EVENT_GEN                               (0x0001095c)
#define TSI721_RIO_EM_RST_PORT_STAT                                (0x00010960)
#define TSI721_RIO_EM_RST_INT_EN                                   (0x00010968)
#define TSI721_RIO_EM_RST_PW_EN                                    (0x00010970)
#define TSI721_RIO_PW_BH                                           (0x00010a00)
#define TSI721_RIO_PW_CTL                                          (0x00010a04)
#define TSI721_RIO_PW_ROUTE                                        (0x00010a08)
#define TSI721_RIO_PW_RX_STAT                                      (0x00010a10)
#define TSI721_RIO_PW_RX_EVENT_GEN                                 (0x00010a14)
#define TSI721_RIO_PW_RX_CAPT                                      (0x00010a20)
#define TSI721_RIO_LLM_BH                                          (0x00010d00)
#define TSI721_RIO_MTC_WR_RESTRICT                                 (0x00010d10)
#define TSI721_RIO_MTC_PWR_RESTRICT                                (0x00010d14)
#define TSI721_RIO_MTC_RD_RESTRICT                                 (0x00010d18)
#define TSI721_RIO_WHITEBOARD                                      (0x00010d24)
#define TSI721_RIO_PRESCALAR_SRV_CLK                               (0x00010d30)
#define TSI721_RIO_REG_RST_CTL                                     (0x00010d34)
#define TSI721_RIO_LOCAL_ERR_DET                                   (0x00010d48)
#define TSI721_RIO_LOCAL_ERR_EN                                    (0x00010d4c)
#define TSI721_RIO_LOCAL_H_ADDR_CAPT                               (0x00010d50)
#define TSI721_RIO_LOCAL_ADDR_CAPT                                 (0x00010d54)
#define TSI721_RIO_LOCAL_ID_CAPT                                   (0x00010d58)
#define TSI721_RIO_LOCAL_CTRL_CAPT                                 (0x00010d5c)
#define TSI721_RIO_FABRIC_BH                                       (0x00010e00)
#define TSI721_RIO_PRBS_BH                                         (0x00012000)
#define TSI721_RIO_PRBS_LANEX_CTRL(X)                     (0x12004 + 0x010*(X))
#define TSI721_RIO_PRBS_LANEX_SEED(X)                     (0x12008 + 0x010*(X))
#define TSI721_RIO_PRBS_LANEX_ERR_COUNT(X)                (0x1200c + 0x010*(X))

/******************************************************/
/* TSI721 : S-RIO Register Bit Masks and Reset Values */
/*           definitions for every register           */
/******************************************************/

/* TSI721_RIO_DEV_ID : Register Bits Masks Definitions */
#define TSI721_RIO_DEV_IDENT_VEND                               (0x0000ffff)
#define TSI721_RIO_DEV_IDENT_DEVI                                   (0xffff0000)
#define TSI721_RIO_DEVICE_VENDOR (0x00000038)
//#define TSI721_RIO_DEVICE_ID     (0x000080AB)

/* TSI721_RIO_DEV_INFO : Register Bits Masks Definitions */
#define TSI721_RIO_DEV_INFO_DEV_REV                                (0xffffffff)

/* TSI721_RIO_ASBLY_ID : Register Bits Masks Definitions */
#define TSI721_RIO_ASBLY_ID_ASBLY_VEN_ID                           (0x0000ffff)
#define TSI721_RIO_ASBLY_ID_ASBLY_ID                               (0xffff0000)

/* TSI721_RIO_ASBLY_INFO : Register Bits Masks Definitions */
#define TSI721_RIO_ASSY_INF_EFB_PTR                         (0x0000ffff)
#define TSI721_RIO_ASSY_INF_ASSY_REV                            (0xffff0000)

/* TSI721_RIO_PE_FEAT : Register Bits Masks Definitions */
#define TSI721_RIO_PE_FEAT_EXT_AS                                  (0x00000007)
#define TSI721_RIO_PE_FEAT_EXT_FEA                                 (0x00000008)
#define TSI721_RIO_PE_FEAT_CTLS                                    (0x00000010)
#define TSI721_RIO_PE_FEAT_CRF                                     (0x00000020)
#define TSI721_RIO_PE_FEAT_FLOW_CTRL                               (0x00000080)
#define TSI721_RIO_PE_FEAT_SRTC                                    (0x00000100)
#define TSI721_RIO_PE_FEAT_ERTC                                    (0x00000200)
#define TSI721_RIO_PE_FEAT_MC                                      (0x00000400)
#define TSI721_RIO_PE_FEAT_FLOW_ARB                                (0x00000800)
#define TSI721_RIO_PE_FEAT_MULT_P                                  (0x08000000)
#define TSI721_RIO_PE_FEAT_SW                                      (0x10000000)
#define TSI721_RIO_PE_FEAT_PROC                                    (0x20000000)
#define TSI721_RIO_PE_FEAT_MEM                                     (0x40000000)
#define TSI721_RIO_PE_FEAT_BRDG                                    (0x80000000)

/* TSI721_RIO_SRC_OP : Register Bits Masks Definitions */
#define TSI721_RIO_SRC_OP_IMPLEMENT_DEF2                           (0x00000003)
#define TSI721_RIO_SRC_OP_PORT_WR                                  (0x00000004)
#define TSI721_RIO_SRC_OP_A_SWAP                                   (0x00000008)
#define TSI721_RIO_SRC_OP_A_CLEAR                                  (0x00000010)
#define TSI721_RIO_SRC_OP_A_SET                                    (0x00000020)
#define TSI721_RIO_SRC_OP_A_DEC                                    (0x00000040)
#define TSI721_RIO_SRC_OP_A_INC                                    (0x00000080)
#define TSI721_RIO_SRC_OP_ATSWAP                                   (0x00000100)
#define TSI721_RIO_SRC_OP_ACSWAP                                   (0x00000200)
#define TSI721_RIO_SRC_OP_DBELL                                    (0x00000400)
#define TSI721_RIO_SRC_OP_D_MSG                                    (0x00000800)
#define TSI721_RIO_SRC_OP_WR_RES                                   (0x00001000)
#define TSI721_RIO_SRC_OP_STRM_WR                                  (0x00002000)
#define TSI721_RIO_SRC_OP_WRITE                                    (0x00004000)
#define TSI721_RIO_SRC_OP_READ                                     (0x00008000)
#define TSI721_RIO_SRC_OP_IMPLEMENT_DEF                            (0x00030000)
#define TSI721_RIO_SRC_OP_DS                                       (0x00040000)
#define TSI721_RIO_SRC_OP_DS_TM                                    (0x00080000)
#define TSI721_RIO_SRC_OP_RIO_RSVD_11                              (0x00100000)
#define TSI721_RIO_SRC_OP_RIO_RSVD_10                              (0x00200000)
#define TSI721_RIO_SRC_OP_G_TLB_SYNC                               (0x00400000)
#define TSI721_RIO_SRC_OP_G_TLB_INVALIDATE                         (0x00800000)
#define TSI721_RIO_SRC_OP_G_IC_INVALIDATE                          (0x01000000)
#define TSI721_RIO_SRC_OP_G_IO_READ                                (0x02000000)
#define TSI721_RIO_SRC_OP_G_DC_FLUSH                               (0x04000000)
#define TSI721_RIO_SRC_OP_G_CASTOUT                                (0x08000000)
#define TSI721_RIO_SRC_OP_G_DC_INVALIDATE                          (0x10000000)
#define TSI721_RIO_SRC_OP_G_READ_OWN                               (0x20000000)
#define TSI721_RIO_SRC_OP_G_IREAD                                  (0x40000000)
#define TSI721_RIO_SRC_OP_G_READ                                   (0x80000000)

/* TSI721_RIO_DEST_OP : Register Bits Masks Definitions */
#define TSI721_RIO_DEST_OP_IMPLEMENT_DEF2                          (0x00000003)
#define TSI721_RIO_DEST_OP_PORT_WR                                 (0x00000004)
#define TSI721_RIO_DEST_OP_A_SWAP                                  (0x00000008)
#define TSI721_RIO_DEST_OP_A_CLEAR                                 (0x00000010)
#define TSI721_RIO_DEST_OP_A_SET                                   (0x00000020)
#define TSI721_RIO_DEST_OP_A_DEC                                   (0x00000040)
#define TSI721_RIO_DEST_OP_A_INC                                   (0x00000080)
#define TSI721_RIO_DEST_OP_ATSWAP                                  (0x00000100)
#define TSI721_RIO_DEST_OP_ACSWAP                                  (0x00000200)
#define TSI721_RIO_DEST_OP_DBELL                                   (0x00000400)
#define TSI721_RIO_DEST_OP_D_MSG                                   (0x00000800)
#define TSI721_RIO_DEST_OP_WR_RES                                  (0x00001000)
#define TSI721_RIO_DEST_OP_STRM_WR                                 (0x00002000)
#define TSI721_RIO_DEST_OP_WRITE                                   (0x00004000)
#define TSI721_RIO_DEST_OP_READ                                    (0x00008000)
#define TSI721_RIO_DEST_OP_IMPLEMENT_DEF                           (0x00030000)
#define TSI721_RIO_DEST_OP_DS                                      (0x00040000)
#define TSI721_RIO_DEST_OP_DS_TM                                   (0x00080000)
#define TSI721_RIO_DEST_OP_RIO_RSVD_11                             (0x00100000)
#define TSI721_RIO_DEST_OP_RIO_RSVD_10                             (0x00200000)
#define TSI721_RIO_DEST_OP_G_TLB_SYNC                              (0x00400000)
#define TSI721_RIO_DEST_OP_G_TLB_INVALIDATE                        (0x00800000)
#define TSI721_RIO_DEST_OP_G_IC_INVALIDATE                         (0x01000000)
#define TSI721_RIO_DEST_OP_G_IO_READ                               (0x02000000)
#define TSI721_RIO_DEST_OP_G_DC_FLUSH                              (0x04000000)
#define TSI721_RIO_DEST_OP_G_CASTOUT                               (0x08000000)
#define TSI721_RIO_DEST_OP_G_DC_INVALIDATE                         (0x10000000)
#define TSI721_RIO_DEST_OP_G_READ_OWN                              (0x20000000)
#define TSI721_RIO_DEST_OP_G_IREAD                                 (0x40000000)
#define TSI721_RIO_DEST_OP_G_READ                                  (0x80000000)

/* TSI721_RIO_SR_XADDR : Register Bits Masks Definitions */
#define TSI721_RIO_SR_XADDR_EA_CTL                                 (0x00000007)

/* TSI721_RIO_BASE_ID : Register Bits Masks Definitions */
#define TSI721_RIO_BASE_ID_LAR_BASE_ID                             (0x0000ffff)
#define TSI721_RIO_BASE_ID_BASE_ID                                 (0x00ff0000)

/* TSI721_RIO_HOST_BASE_ID_LOCK : Register Bits Masks Definitions */
#define TSI721_RIO_HOST_BASE_ID_LOCK_HOST_BASE_ID                  (0x0000ffff)

/* TSI721_RIO_COMP_TAG : Register Bits Masks Definitions */
#define TSI721_RIO_COMP_TAG_CTAG                                   (0xffffffff)

/* TSI721_RIO_SP_MB_HEAD : Register Bits Masks Definitions */
#define TSI721_RIO_SP_MB_HEAD_EF_ID                                (0x0000ffff)
#define TSI721_RIO_SP_MB_HEAD_EF_PTR                               (0xffff0000)

/* TSI721_RIO_SP_LT_CTL : Register Bits Masks Definitions */
#define TSI721_RIO_SP_LT_CTL_TVAL                                  (0xffffff00)

/* TSI721_RIO_SR_RSP_TO : Register Bits Masks Definitions */
#define TSI721_RIO_SR_RSP_TO_RSP_TO                                (0x00ffffff)

/* TSI721_RIO_SP_GEN_CTL : Register Bits Masks Definitions */
#define TSI721_RIO_SP_GEN_CTL_DISC                                 (0x20000000)
#define TSI721_RIO_SP_GEN_CTL_MAST_EN                              (0x40000000)
#define TSI721_RIO_SP_GEN_CTL_HOST                                 (0x80000000)

/* TSI721_RIO_SP_LM_REQ : Register Bits Masks Definitions */
#define TSI721_RIO_SP_LM_REQ_CMD                                   (0x00000007)

/* TSI721_RIO_SP_LM_RESP : Register Bits Masks Definitions */
#define TSI721_RIO_SP_LM_RESP_LINK_STAT                            (0x0000001f)
#define TSI721_RIO_SP_LM_RESP_ACK_ID_STAT                          (0x000007e0)
#define TSI721_RIO_SP_LM_RESP_RESP_VLD                             (0x80000000)

/* TSI721_RIO_SP_ACKID_STAT : Register Bits Masks Definitions */
#define TSI721_RIO_SP_ACKID_STAT_OUTB_ACKID                        (0x0000003f)
#define TSI721_RIO_SP_ACKID_STAT_OUTSTD_ACKID                      (0x00003f00)
#define TSI721_RIO_SP_ACKID_STAT_INB_ACKID                         (0x3f000000)
#define TSI721_RIO_SP_ACKID_STAT_CLR_OUTSTD_ACKID                  (0x80000000)

/* TSI721_RIO_SP_CTL2 : Register Bits Masks Definitions */
#define TSI721_RIO_SP_CTL2_RTEC_EN                                 (0x00000001)
#define TSI721_RIO_SP_CTL2_RTEC                                    (0x00000002)
#define TSI721_RIO_SP_CTL2_D_SCRM_DIS                              (0x00000004)
#define TSI721_RIO_SP_CTL2_INACT_EN                                (0x00000008)
#define TSI721_RIO_SP_CTL2_GB_6p25_EN                              (0x00010000)
#define TSI721_RIO_SP_CTL2_GB_6p25                                 (0x00020000)
#define TSI721_RIO_SP_CTL2_GB_5p0_EN                               (0x00040000)
#define TSI721_RIO_SP_CTL2_GB_5p0                                  (0x00080000)
#define TSI721_RIO_SP_CTL2_GB_3p125_EN                             (0x00100000)
#define TSI721_RIO_SP_CTL2_GB_3p125                                (0x00200000)
#define TSI721_RIO_SP_CTL2_GB_2p5_EN                               (0x00400000)
#define TSI721_RIO_SP_CTL2_GB_2p5                                  (0x00800000)
#define TSI721_RIO_SP_CTL2_GB_1p25_EN                              (0x01000000)
#define TSI721_RIO_SP_CTL2_GB_1p25                                 (0x02000000)
#define TSI721_RIO_SP_CTL2_BAUD_DISC                               (0x08000000)
#define TSI721_RIO_SP_CTL2_BAUD_SEL                                (0xf0000000)

/* TSI721_RIO_SP_ERR_STAT : Register Bits Masks Definitions */
#define TSI721_RIO_SP_ERR_STAT_PORT_UNIT                           (0x00000001)
#define TSI721_RIO_SP_ERR_STAT_PORT_OK                             (0x00000002)
#define TSI721_RIO_SP_ERR_STAT_PORT_ERR                            (0x00000004)
#define TSI721_RIO_SP_ERR_STAT_PORT_UNAVL                          (0x00000008)
#define TSI721_RIO_SP_ERR_STAT_PORT_W_P                            (0x00000010)
#define TSI721_RIO_SP_ERR_STAT_INPUT_ERR_STOP                      (0x00000100)
#define TSI721_RIO_SP_ERR_STAT_INPUT_ERR_ENCTR                     (0x00000200)
#define TSI721_RIO_SP_ERR_STAT_INPUT_RS                            (0x00000400)
#define TSI721_RIO_SP_ERR_STAT_OUTPUT_ERR_STOP                     (0x00010000)
#define TSI721_RIO_SP_ERR_STAT_OUTPUT_ERR_ENCTR                    (0x00020000)
#define TSI721_RIO_SP_ERR_STAT_OUTPUT_RS                           (0x00040000)
#define TSI721_RIO_SP_ERR_STAT_OUTPUT_R                            (0x00080000)
#define TSI721_RIO_SP_ERR_STAT_OUTPUT_RE                           (0x00100000)
#define TSI721_RIO_SP_ERR_STAT_OUTPUT_DEGR                         (0x01000000)
#define TSI721_RIO_SP_ERR_STAT_OUTPUT_FAIL                         (0x02000000)
#define TSI721_RIO_SP_ERR_STAT_OUTPUT_DROP                         (0x04000000)
#define TSI721_RIO_SP_ERR_STAT_TXFC                                (0x08000000)
#define TSI721_RIO_SP_ERR_STAT_IDLE_SEQ                            (0x20000000)
#define TSI721_RIO_SP_ERR_STAT_IDLE2_EN                            (0x40000000)
#define TSI721_RIO_SP_ERR_STAT_IDLE2                               (0x80000000)

/* TSI721_RIO_SP_CTL : Register Bits Masks Definitions */
#define TSI721_RIO_SP_CTL_PTYP                                     (0x00000001)
#define TSI721_RIO_SP_CTL_PORT_LOCKOUT                             (0x00000002)
#define TSI721_RIO_SP_CTL_DROP_EN                                  (0x00000004)
#define TSI721_RIO_SP_CTL_STOP_FAIL_EN                             (0x00000008)
#define TSI721_RIO_SP_CTL_PORT_WIDTH2                              (0x00003000)
#define TSI721_RIO_SP_CTL_OVER_PWIDTH2                             (0x0000c000)
#define TSI721_RIO_SP_CTL_FLOW_ARB                                 (0x00010000)
#define TSI721_RIO_SP_CTL_ENUM_B                                   (0x00020000)
#define TSI721_RIO_SP_CTL_FLOW_CTRL                                (0x00040000)
#define TSI721_RIO_SP_CTL_MULT_CS                                  (0x00080000)
#define TSI721_RIO_SP_CTL_ERR_DIS                                  (0x00100000)
#define TSI721_RIO_SP_CTL_INP_EN                                   (0x00200000)
#define TSI721_RIO_SP_CTL_OTP_EN                                   (0x00400000)
#define TSI721_RIO_SP_CTL_PORT_DIS                                 (0x00800000)
#define TSI721_RIO_SP_CTL_OVER_PWIDTH                              (0x07000000)
#define TSI721_RIO_SP_CTL_INIT_PWIDTH                              (0x38000000)
#define TSI721_RIO_SP_CTL_PORT_WIDTH                               (0xc0000000)

/* TSI721_RIO_ERR_RPT_BH : Register Bits Masks Definitions */
#define TSI721_RIO_ERR_RPT_BH_EF_ID                                (0x0000ffff)
#define TSI721_RIO_ERR_RPT_BH_EF_PTR                               (0xffff0000)

/* TSI721_RIO_ERR_DET : Register Bits Masks Definitions */
#define TSI721_RIO_ERR_DET_IMP                                     (0x00000001)

/* TSI721_RIO_ERR_EN : Register Bits Masks Definitions */
#define TSI721_RIO_ERR_EN_IMP_EN                                   (0x00000001)

/* TSI721_RIO_H_ADDR_CAPT : Register Bits Masks Definitions */
#define TSI721_RIO_H_ADDR_CAPT_ADDR                                (0xffffffff)

/* TSI721_RIO_ADDR_CAPT : Register Bits Masks Definitions */
#define TSI721_RIO_ADDR_CAPT_XAMSBS                                (0x00000003)
#define TSI721_RIO_ADDR_CAPT_ADDR                                  (0xfffffff8)

/* TSI721_RIO_ID_CAPT : Register Bits Masks Definitions */
#define TSI721_RIO_ID_CAPT_SRC_ID                                  (0x000000ff)
#define TSI721_RIO_ID_CAPT_MSB_SRC_ID                              (0x0000ff00)
#define TSI721_RIO_ID_CAPT_DEST_ID                                 (0x00ff0000)
#define TSI721_RIO_ID_CAPT_MSB_DEST_ID                             (0xff000000)

/* TSI721_RIO_CTRL_CAPT : Register Bits Masks Definitions */
#define TSI721_RIO_CTRL_CAPT_MESSAGE_INFO                          (0x00ff0000)
#define TSI721_RIO_CTRL_CAPT_TTYPE                                 (0x0f000000)
#define TSI721_RIO_CTRL_CAPT_FTYPE                                 (0xf0000000)

/* TSI721_RIO_PW_TGT_ID : Register Bits Masks Definitions */
#define TSI721_RIO_PW_TGT_ID_LRG_TRANS                             (0x00008000)
#define TSI721_RIO_PW_TGT_ID_PW_TGT_ID                             (0x00ff0000)
#define TSI721_RIO_PW_TGT_ID_MSB_PW_ID                             (0xff000000)
#define TSI721_RIO_PW_TGT_ID_DEV16_ID                              (0xffff0000)

/* TSI721_RIO_SP_ERR_DET : Register Bits Masks Definitions */
#define TSI721_RIO_SP_ERR_DET_LINK_TO                              (0x00000001)
#define TSI721_RIO_SP_ERR_DET_CS_ACK_ILL                           (0x00000002)
#define TSI721_RIO_SP_ERR_DET_DELIN_ERR                            (0x00000004)
#define TSI721_RIO_SP_ERR_DET_PROT_ERR                             (0x00000010)
#define TSI721_RIO_SP_ERR_DET_LR_ACKID_ILL                         (0x00000020)
#define TSI721_RIO_SP_ERR_DET_DSCRAM_LOS                           (0x00004000)
#define TSI721_RIO_SP_ERR_DET_PKT_ILL_SIZE                         (0x00020000)
#define TSI721_RIO_SP_ERR_DET_PKT_CRC_ERR                          (0x00040000)
#define TSI721_RIO_SP_ERR_DET_PKT_ILL_ACKID                        (0x00080000)
#define TSI721_RIO_SP_ERR_DET_CS_NOT_ACC                           (0x00100000)
#define TSI721_RIO_SP_ERR_DET_CS_ILL_ID                            (0x00200000)
#define TSI721_RIO_SP_ERR_DET_CS_CRC_ERR                           (0x00400000)
#define TSI721_RIO_SP_ERR_DET_IMP_SPEC                             (0x80000000)

/* TSI721_RIO_SP_RATE_EN : Register Bits Masks Definitions */
#define TSI721_RIO_SP_RATE_EN_LINK_TO_EN                           (0x00000001)
#define TSI721_RIO_SP_RATE_EN_CS_ACK_ILL_EN                        (0x00000002)
#define TSI721_RIO_SP_RATE_EN_DELIN_ERR_EN                         (0x00000004)
#define TSI721_RIO_SP_RATE_EN_PROT_ERR_EN                          (0x00000010)
#define TSI721_RIO_SP_RATE_EN_LR_ACKID_ILL_EN                      (0x00000020)
#define TSI721_RIO_SP_RATE_EN_DSCRAM_LOS_EN                        (0x00004000)
#define TSI721_RIO_SP_RATE_EN_PKT_ILL_SIZE_EN                      (0x00020000)
#define TSI721_RIO_SP_RATE_EN_PKT_CRC_ERR_EN                       (0x00040000)
#define TSI721_RIO_SP_RATE_EN_PKT_ILL_ACKID_EN                     (0x00080000)
#define TSI721_RIO_SP_RATE_EN_CS_NOT_ACC_EN                        (0x00100000)
#define TSI721_RIO_SP_RATE_EN_CS_ILL_ID_EN                         (0x00200000)
#define TSI721_RIO_SP_RATE_EN_CS_CRC_ERR_EN                        (0x00400000)
#define TSI721_RIO_SP_RATE_EN_IMP_SPEC_EN                          (0x80000000)

/* TSI721_RIO_SP_ERR_ATTR_CAPT : Register Bits Masks Definitions */
#define TSI721_RIO_SP_ERR_ATTR_CAPT_VAL_CAPT                       (0x00000001)
#define TSI721_RIO_SP_ERR_ATTR_CAPT_IMPL_DEP                       (0x00fffff0)
#define TSI721_RIO_SP_ERR_ATTR_CAPT_ERR_TYPE                       (0x1f000000)
#define TSI721_RIO_SP_ERR_ATTR_CAPT_INFO_TYPE                      (0xe0000000)

/* TSI721_RIO_SP_ERR_CAPT_0 : Register Bits Masks Definitions */
#define TSI721_RIO_SP_ERR_CAPT_0_CAPT_0                            (0xffffffff)

/* TSI721_RIO_SP_ERR_CAPT_1 : Register Bits Masks Definitions */
#define TSI721_RIO_SP_ERR_CAPT_1_CAPT_1                            (0xffffffff)

/* TSI721_RIO_SP_ERR_CAPT_2 : Register Bits Masks Definitions */
#define TSI721_RIO_SP_ERR_CAPT_2_CAPT_2                            (0xffffffff)

/* TSI721_RIO_SP_ERR_CAPT_3 : Register Bits Masks Definitions */
#define TSI721_RIO_SP_ERR_CAPT_3_CAPT_3                            (0xffffffff)

/* TSI721_RIO_SP_ERR_RATE : Register Bits Masks Definitions */
#define TSI721_RIO_SP_ERR_RATE_ERR_RATE_CNT                        (0x000000ff)
#define TSI721_RIO_SP_ERR_RATE_PEAK                                (0x0000ff00)
#define TSI721_RIO_SP_ERR_RATE_ERR_RR                              (0x00030000)
#define TSI721_RIO_SP_ERR_RATE_ERR_RB                              (0xff000000)

/* TSI721_RIO_SP_ERR_THRESH : Register Bits Masks Definitions */
#define TSI721_RIO_SP_ERR_THRESH_ERR_RDT                           (0x00ff0000)
#define TSI721_RIO_SP_ERR_THRESH_ERR_RFT                           (0xff000000)

/* TSI721_RIO_PER_LANE_BH : Register Bits Masks Definitions */
#define TSI721_RIO_PER_LANE_BH_EF_ID                               (0x0000ffff)
#define TSI721_RIO_PER_LANE_BH_EF_PTR                              (0xffff0000)

/* TSI721_RIO_LANEX_STAT0 : Register Bits Masks Definitions */
#define TSI721_RIO_LANEX_STAT0_STAT2_7                             (0x00000007)
#define TSI721_RIO_LANEX_STAT0_STAT1                               (0x00000008)
#define TSI721_RIO_LANEX_STAT0_CHG_TRN                             (0x00000040)
#define TSI721_RIO_LANEX_STAT0_CHG_SYNC                            (0x00000080)
#define TSI721_RIO_LANEX_STAT0_ERR_CNT                             (0x00000f00)
#define TSI721_RIO_LANEX_STAT0_RX_RDY                              (0x00001000)
#define TSI721_RIO_LANEX_STAT0_RX_SYNC                             (0x00002000)
#define TSI721_RIO_LANEX_STAT0_RX_TRN                              (0x00004000)
#define TSI721_RIO_LANEX_STAT0_RX_INV                              (0x00008000)
#define TSI721_RIO_LANEX_STAT0_RX_TYPE                             (0x00030000)
#define TSI721_RIO_LANEX_STAT0_TX_MODE                             (0x00040000)
#define TSI721_RIO_LANEX_STAT0_TX_TYPE                             (0x00080000)
#define TSI721_RIO_LANEX_STAT0_LANE_NUM                            (0x00f00000)
#define TSI721_RIO_LANEX_STAT0_PORT_NUM                            (0xff000000)

/* TSI721_RIO_LANEX_STAT1 : Register Bits Masks Definitions */
#define TSI721_RIO_LANEX_STAT1_LP_SCRM                             (0x00008000)
#define TSI721_RIO_LANEX_STAT1_LP_TAP_P1                           (0x00030000)
#define TSI721_RIO_LANEX_STAT1_LP_TAP_M1                           (0x000c0000)
#define TSI721_RIO_LANEX_STAT1_LP_LANE_NUM                         (0x00f00000)
#define TSI721_RIO_LANEX_STAT1_LP_WIDTH                            (0x07000000)
#define TSI721_RIO_LANEX_STAT1_LP_RX_TRN                           (0x08000000)
#define TSI721_RIO_LANEX_STAT1_IMPL_SPEC                           (0x10000000)
#define TSI721_RIO_LANEX_STAT1_CHG                                 (0x20000000)
#define TSI721_RIO_LANEX_STAT1_INFO_OK                             (0x40000000)
#define TSI721_RIO_LANEX_STAT1_IDLE2                               (0x80000000)

/* TSI721_RIO_PLM_BH : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_BH_BLK_TYPE                                 (0x00000fff)
#define TSI721_RIO_PLM_BH_BLK_REV                                  (0x0000f000)
#define TSI721_RIO_PLM_BH_NEXT_BLK_PTR                             (0xffff0000)

/* TSI721_RIO_PLM_SP_IMP_SPEC_CTL : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_DLT_THRESH                  (0x0000ffff)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SWAP_RX                     (0x00030000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SWAP_TX                     (0x000c0000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SELF_RST                    (0x00100000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_PORT_SELF_RST               (0x00200000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_RESET_REG                   (0x00400000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_LLB_EN                      (0x00800000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_TX_BYPASS                   (0x01000000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_FORCE_REINIT                (0x04000000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_DLB_EN                      (0x10000000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_USE_IDLE1                   (0x20000000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_USE_IDLE2                   (0x40000000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_PAYL_CAP                    (0x80000000)

#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SWAP_RX_NONE                (0x00000000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SWAP_RX_1032                (0x00010000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SWAP_RX_3210                (0x00020000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SWAP_RX_2301                (0x00030000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SWAP_TX_NONE                (0x00000000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SWAP_TX_1032                (0x00040000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SWAP_TX_3210                (0x00080000)
#define TSI721_RIO_PLM_SP_IMP_SPEC_CTL_SWAP_TX_2301                (0x000C0000)

/* TSI721_RIO_PLM_SP_uint32_t : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_SP_uint32_t_TLM_INT                           (0x00000400)
#define TSI721_RIO_PLM_SP_uint32_t_PBM_INT                           (0x00000800)
#define TSI721_RIO_PLM_SP_uint32_t_MECS                              (0x00001000)
#define TSI721_RIO_PLM_SP_uint32_t_TLM_PW                            (0x00004000)
#define TSI721_RIO_PLM_SP_uint32_t_PBM_PW                            (0x00008000)
#define TSI721_RIO_PLM_SP_uint32_t_RST_REQ                           (0x00010000)
#define TSI721_RIO_PLM_SP_uint32_t_OUTPUT_DEGR                       (0x01000000)
#define TSI721_RIO_PLM_SP_uint32_t_OUTPUT_FAIL                       (0x02000000)
#define TSI721_RIO_PLM_SP_uint32_t_PORT_ERR                          (0x04000000)
#define TSI721_RIO_PLM_SP_uint32_t_DLT                               (0x08000000)
#define TSI721_RIO_PLM_SP_uint32_t_LINK_INIT                         (0x10000000)
#define TSI721_RIO_PLM_SP_uint32_t_MAX_DENIAL                        (0x80000000)

/* TSI721_RIO_PLM_SP_INT_ENABLE : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_SP_INT_ENABLE_OUTPUT_DEGR                   (0x01000000)
#define TSI721_RIO_PLM_SP_INT_ENABLE_OUTPUT_FAIL                   (0x02000000)
#define TSI721_RIO_PLM_SP_INT_ENABLE_PORT_ERR                      (0x04000000)
#define TSI721_RIO_PLM_SP_INT_ENABLE_DLT                           (0x08000000)
#define TSI721_RIO_PLM_SP_INT_ENABLE_LINK_INIT                     (0x10000000)
#define TSI721_RIO_PLM_SP_INT_ENABLE_MAX_DENIAL                    (0x80000000)

/* TSI721_RIO_PLM_SP_PW_ENABLE : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_SP_PW_ENABLE_OUTPUT_DEGR                    (0x01000000)
#define TSI721_RIO_PLM_SP_PW_ENABLE_OUTPUT_FAIL                    (0x02000000)
#define TSI721_RIO_PLM_SP_PW_ENABLE_PORT_ERR                       (0x04000000)
#define TSI721_RIO_PLM_SP_PW_ENABLE_DLT                            (0x08000000)
#define TSI721_RIO_PLM_SP_PW_ENABLE_LINK_INIT                      (0x10000000)
#define TSI721_RIO_PLM_SP_PW_ENABLE_MAX_DENIAL                     (0x80000000)

/* TSI721_RIO_PLM_SP_EVENT_GEN : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_SP_EVENT_GEN_RST_REQ                        (0x00010000)
#define TSI721_RIO_PLM_SP_EVENT_GEN_OUTPUT_DEGR                    (0x01000000)
#define TSI721_RIO_PLM_SP_EVENT_GEN_OUTPUT_FAIL                    (0x02000000)
#define TSI721_RIO_PLM_SP_EVENT_GEN_PORT_ERR                       (0x04000000)
#define TSI721_RIO_PLM_SP_EVENT_GEN_DLT                            (0x08000000)
#define TSI721_RIO_PLM_SP_EVENT_GEN_LINK_INIT                      (0x10000000)
#define TSI721_RIO_PLM_SP_EVENT_GEN_MAX_DENIAL                     (0x80000000)

/* TSI721_RIO_PLM_SP_ALL_INT_EN : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_SP_ALL_INT_EN_IRQ_EN                        (0x00000001)

/* TSI721_RIO_PLM_SP_ALL_PW_EN : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_SP_ALL_PW_EN_PW_EN                          (0x00000001)

/* TSI721_RIO_PLM_SP_DISCOVERY_TIMER : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_SP_DISCOVERY_TIMER_DISCOVERY_TIMER          (0xf0000000)

/* TSI721_RIO_PLM_SP_SILENCE_TIMER : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_SP_SILENCE_TIMER_SILENCE_TIMER              (0xf0000000)

/* TSI721_RIO_PLM_SP_VMIN_EXP : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_SP_VMIN_EXP_MMAX                            (0x00000f00)
#define TSI721_RIO_PLM_SP_VMIN_EXP_IMAX                            (0x000f0000)
#define TSI721_RIO_PLM_SP_VMIN_EXP_VMIN_EXP                        (0x1f000000)

/* TSI721_RIO_PLM_SP_POL_CTL : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_SP_POL_CTL_RX0_POL                          (0x00000001)
#define TSI721_RIO_PLM_SP_POL_CTL_RX1_POL                          (0x00000002)
#define TSI721_RIO_PLM_SP_POL_CTL_RX2_POL                          (0x00000004)
#define TSI721_RIO_PLM_SP_POL_CTL_RX3_POL                          (0x00000008)
#define TSI721_RIO_PLM_SP_POL_CTL_TX0_POL                          (0x00010000)
#define TSI721_RIO_PLM_SP_POL_CTL_TX1_POL                          (0x00020000)
#define TSI721_RIO_PLM_SP_POL_CTL_TX2_POL                          (0x00040000)
#define TSI721_RIO_PLM_SP_POL_CTL_TX3_POL                          (0x00080000)

/* TSI721_RIO_PLM_SP_DENIAL_CTL : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_SP_DENIAL_CTL_DENIAL_THRESH                 (0x000000ff)
#define TSI721_RIO_PLM_SP_DENIAL_CTL_CNT_RTY                       (0x10000000)
#define TSI721_RIO_PLM_SP_DENIAL_CTL_CNT_PNA                       (0x20000000)

/* TSI721_RIO_PLM_SP_RCVD_MECS : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_SP_RCVD_MECS_CMD_STAT                       (0x000000ff)

/* TSI721_RIO_PLM_SP_MECS_FWD : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_SP_MECS_FWD_MULT_CS                         (0x00000001)
#define TSI721_RIO_PLM_SP_MECS_FWD_SUBSCRIPTION                    (0x000000fe)

/* TSI721_RIO_PLM_SP_LONG_CS_TX1 : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_SP_LONG_CS_TX1_CMD                          (0x00000007)
#define TSI721_RIO_PLM_SP_LONG_CS_TX1_STYPE_1                      (0x00000070)
#define TSI721_RIO_PLM_SP_LONG_CS_TX1_CS_EMB                       (0x00000100)
#define TSI721_RIO_PLM_SP_LONG_CS_TX1_PAR_1                        (0x0003f000)
#define TSI721_RIO_PLM_SP_LONG_CS_TX1_PAR_0                        (0x03f00000)
#define TSI721_RIO_PLM_SP_LONG_CS_TX1_STYPE_0                      (0x70000000)

#define TSI721_MAGIC_CS \
((((uint32_t)(stype0_pna     ) << 28) & TSI721_RIO_PLM_SP_LONG_CS_TX1_STYPE_0) | \
 (((uint32_t)(0x00000000     ) << 20) & TSI721_RIO_PLM_SP_LONG_CS_TX1_PAR_0  ) | \
 (((uint32_t)(PNA_GENERAL_ERR) << 12) & TSI721_RIO_PLM_SP_LONG_CS_TX1_PAR_1  ) | \
 (((uint32_t)(stype1_lreq    ) <<  4) & TSI721_RIO_PLM_SP_LONG_CS_TX1_STYPE_1) | \
 (((uint32_t)(STYPE1_LREQ_CMD_PORT_STAT)) & TSI721_RIO_PLM_SP_LONG_CS_TX1_CMD))   

/* TSI721_RIO_PLM_SP_LONG_CS_TX2 : Register Bits Masks Definitions */
#define TSI721_RIO_PLM_SP_LONG_CS_TX2_PARM                         (0x07ff0000)
#define TSI721_RIO_PLM_SP_LONG_CS_TX2_STYPE2                       (0x70000000)

/* TSI721_RIO_TLM_BH : Register Bits Masks Definitions */
#define TSI721_RIO_TLM_BH_BLK_TYPE                                 (0x00000fff)
#define TSI721_RIO_TLM_BH_BLK_REV                                  (0x0000f000)
#define TSI721_RIO_TLM_BH_NEXT_BLK_PTR                             (0xffff0000)

/* TSI721_RIO_TLM_SP_CONTROL : Register Bits Masks Definitions */
#define TSI721_RIO_TLM_SP_CONTROL_LENGTH                           (0x0000f000)
#define TSI721_RIO_TLM_SP_CONTROL_MTC_TGT_ID_DIS                   (0x00100000)
#define TSI721_RIO_TLM_SP_CONTROL_TGT_ID_DIS                       (0x00200000)
#define TSI721_RIO_TLM_SP_CONTROL_PORTGROUP_SELECT                 (0x40000000)

/* TSI721_RIO_TLM_SP_uint32_t : Register Bits Masks Definitions */
#define TSI721_RIO_TLM_SP_uint32_t_IG_BRR_FILTER                     (0x00100000)
#define TSI721_RIO_TLM_SP_uint32_t_IG_BAD_VC                         (0x80000000)

/* TSI721_RIO_TLM_SP_INT_ENABLE : Register Bits Masks Definitions */
#define TSI721_RIO_TLM_SP_INT_ENABLE_IG_BRR_FILTER                 (0x00100000)
#define TSI721_RIO_TLM_SP_INT_ENABLE_IG_BAD_VC                     (0x80000000)

/* TSI721_RIO_TLM_SP_PW_ENABLE : Register Bits Masks Definitions */
#define TSI721_RIO_TLM_SP_PW_ENABLE_IG_BRR_FILTER                  (0x00100000)
#define TSI721_RIO_TLM_SP_PW_ENABLE_IG_BAD_VC                      (0x80000000)

/* TSI721_RIO_TLM_SP_EVENT_GEN : Register Bits Masks Definitions */
#define TSI721_RIO_TLM_SP_EVENT_GEN_IG_BRR_FILTER                  (0x00100000)
#define TSI721_RIO_TLM_SP_EVENT_GEN_IG_BAD_VC                      (0x80000000)

/* TSI721_RIO_TLM_SP_BRR_CTLX : Register Bits Masks Definitions */
#define TSI721_RIO_TLM_SP_BRR_CTLX_PRIVATE                         (0x01000000)
#define TSI721_RIO_TLM_SP_BRR_CTLX_ROUTE_MR_TO_LLM                 (0x04000000)
#define TSI721_RIO_TLM_SP_BRR_CTLX_ENABLE                          (0x80000000)

/* TSI721_RIO_TLM_SP_BRR_PATTERN_MATCHX : Register Bits Masks Definitions */
#define TSI721_RIO_TLM_SP_BRR_PATTERN_MATCHX_MATCH                 (0x0000ffff)
#define TSI721_RIO_TLM_SP_BRR_PATTERN_MATCHX_PATTERN               (0xffff0000)

/* TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL : Register Bits Masks Definitions */
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F15_IMPLEMENTATION      (0x00000002)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F14_RSVD                (0x00000004)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F13_OTHER               (0x00000008)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F13_RESPONSE_DATA       (0x00000010)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F13_RESPONSE            (0x00000020)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F12_RSVD                (0x00000040)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F11_MESSAGE             (0x00000080)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F10_DOORBELL            (0x00000100)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F9_DATA_STREAMING       (0x00000200)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F8_OTHER                (0x00000400)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F8_PWR                  (0x00000800)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F8_MWR                  (0x00001000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F8_MRR                  (0x00002000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F8_MW                   (0x00004000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F8_MR                   (0x00008000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F7_FLOW                 (0x00010000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F6_STREAMING_WRITE      (0x00020000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F5_OTHER                (0x00040000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F5_ATOMIC               (0x00080000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F5_NWRITE_R             (0x00100000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F5_NWRITE               (0x00200000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F5_GSM                  (0x00400000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F4_RSVD                 (0x00800000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F3_RSVD                 (0x01000000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F2_ATOMIC               (0x02000000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F2_NREAD                (0x04000000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F2_GSM                  (0x08000000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F1_ALL                  (0x10000000)
#define TSI721_RIO_TLM_SP_FTYPE_FILTER_CTL_F0_IMPLEMENTATION       (0x40000000)

/* TSI721_RIO_PBM_BH : Register Bits Masks Definitions */
#define TSI721_RIO_PBM_BH_BLK_TYPE                                 (0x00000fff)
#define TSI721_RIO_PBM_BH_BLK_REV                                  (0x0000f000)
#define TSI721_RIO_PBM_BH_NEXT_BLK_PTR                             (0xffff0000)

/* TSI721_RIO_PBM_SP_CONTROL : Register Bits Masks Definitions */
#define TSI721_RIO_PBM_SP_CONTROL_EG_REORDER_STICK                 (0x00000007)
#define TSI721_RIO_PBM_SP_CONTROL_EG_REORDER_MODE                  (0x00000030)
#define TSI721_RIO_PBM_SP_CONTROL_IG_BACKPRESSURE_ON_FATAL         (0x00010000)

/* TSI721_RIO_PBM_SP_uint32_t : Register Bits Masks Definitions */
#define TSI721_RIO_PBM_SP_uint32_t_EG_BABBLE_PACKET                  (0x00000001)
#define TSI721_RIO_PBM_SP_uint32_t_EG_BAD_CHANNEL                    (0x00000002)
#define TSI721_RIO_PBM_SP_uint32_t_EG_CRQ_OVERFLOW                   (0x00000008)
#define TSI721_RIO_PBM_SP_uint32_t_EG_DATA_OVERFLOW                  (0x00000010)
#define TSI721_RIO_PBM_SP_uint32_t_EG_DNFL_FATAL                     (0x00000020)
#define TSI721_RIO_PBM_SP_uint32_t_EG_DNFL_COR                       (0x00000040)
#define TSI721_RIO_PBM_SP_uint32_t_EG_DOH_FATAL                      (0x00000080)
#define TSI721_RIO_PBM_SP_uint32_t_EG_DOH_COR                        (0x00000100)
#define TSI721_RIO_PBM_SP_uint32_t_EG_DATA_UNCOR                     (0x00000800)
#define TSI721_RIO_PBM_SP_uint32_t_EG_DATA_COR                       (0x00001000)
#define TSI721_RIO_PBM_SP_uint32_t_EG_EMPTY                          (0x00008000)
#define TSI721_RIO_PBM_SP_uint32_t_IG_EMPTY                          (0x00010000)
#define TSI721_RIO_PBM_SP_uint32_t_IG_DNFL_FATAL                     (0x00400000)
#define TSI721_RIO_PBM_SP_uint32_t_IG_DNFL_COR                       (0x00800000)
#define TSI721_RIO_PBM_SP_uint32_t_IG_DOH_FATAL                      (0x01000000)
#define TSI721_RIO_PBM_SP_uint32_t_IG_DOH_COR                        (0x02000000)
#define TSI721_RIO_PBM_SP_uint32_t_IG_TFL_FATAL                      (0x04000000)
#define TSI721_RIO_PBM_SP_uint32_t_IG_TFL_COR                        (0x08000000)
#define TSI721_RIO_PBM_SP_uint32_t_IG_TAG_FATAL                      (0x10000000)
#define TSI721_RIO_PBM_SP_uint32_t_IG_TAG_COR                        (0x20000000)
#define TSI721_RIO_PBM_SP_uint32_t_IG_DATA_UNCOR                     (0x40000000)
#define TSI721_RIO_PBM_SP_uint32_t_IG_DATA_COR                       (0x80000000)

/* TSI721_RIO_PBM_SP_INT_ENABLE : Register Bits Masks Definitions */
#define TSI721_RIO_PBM_SP_INT_ENABLE_EG_BABBLE_PACKET              (0x00000001)
#define TSI721_RIO_PBM_SP_INT_ENABLE_EG_BAD_CHANNEL                (0x00000002)
#define TSI721_RIO_PBM_SP_INT_ENABLE_EG_CRQ_OVERFLOW               (0x00000008)
#define TSI721_RIO_PBM_SP_INT_ENABLE_EG_DATA_OVERFLOW              (0x00000010)
#define TSI721_RIO_PBM_SP_INT_ENABLE_EG_DNFL_FATAL                 (0x00000020)
#define TSI721_RIO_PBM_SP_INT_ENABLE_EG_DNFL_COR                   (0x00000040)
#define TSI721_RIO_PBM_SP_INT_ENABLE_EG_DOH_FATAL                  (0x00000080)
#define TSI721_RIO_PBM_SP_INT_ENABLE_EG_DOH_COR                    (0x00000100)
#define TSI721_RIO_PBM_SP_INT_ENABLE_EG_DATA_UNCOR                 (0x00000800)
#define TSI721_RIO_PBM_SP_INT_ENABLE_EG_DATA_COR                   (0x00001000)
#define TSI721_RIO_PBM_SP_INT_ENABLE_IG_DNFL_FATAL                 (0x00400000)
#define TSI721_RIO_PBM_SP_INT_ENABLE_IG_DNFL_COR                   (0x00800000)
#define TSI721_RIO_PBM_SP_INT_ENABLE_IG_DOH_FATAL                  (0x01000000)
#define TSI721_RIO_PBM_SP_INT_ENABLE_IG_DOH_COR                    (0x02000000)
#define TSI721_RIO_PBM_SP_INT_ENABLE_IG_TFL_FATAL                  (0x04000000)
#define TSI721_RIO_PBM_SP_INT_ENABLE_IG_TFL_COR                    (0x08000000)
#define TSI721_RIO_PBM_SP_INT_ENABLE_IG_TAG_FATAL                  (0x10000000)
#define TSI721_RIO_PBM_SP_INT_ENABLE_IG_TAG_COR                    (0x20000000)
#define TSI721_RIO_PBM_SP_INT_ENABLE_IG_DATA_UNCOR                 (0x40000000)
#define TSI721_RIO_PBM_SP_INT_ENABLE_IG_DATA_COR                   (0x80000000)

/* TSI721_RIO_PBM_SP_PW_ENABLE : Register Bits Masks Definitions */
#define TSI721_RIO_PBM_SP_PW_ENABLE_EG_BABBLE_PACKET               (0x00000001)
#define TSI721_RIO_PBM_SP_PW_ENABLE_EG_BAD_CHANNEL                 (0x00000002)
#define TSI721_RIO_PBM_SP_PW_ENABLE_EG_CRQ_OVERFLOW                (0x00000008)
#define TSI721_RIO_PBM_SP_PW_ENABLE_EG_DATA_OVERFLOW               (0x00000010)
#define TSI721_RIO_PBM_SP_PW_ENABLE_EG_DNFL_FATAL                  (0x00000020)
#define TSI721_RIO_PBM_SP_PW_ENABLE_EG_DNFL_COR                    (0x00000040)
#define TSI721_RIO_PBM_SP_PW_ENABLE_EG_DOH_FATAL                   (0x00000080)
#define TSI721_RIO_PBM_SP_PW_ENABLE_EG_DOH_COR                     (0x00000100)
#define TSI721_RIO_PBM_SP_PW_ENABLE_EG_DATA_UNCOR                  (0x00000800)
#define TSI721_RIO_PBM_SP_PW_ENABLE_EG_DATA_COR                    (0x00001000)
#define TSI721_RIO_PBM_SP_PW_ENABLE_IG_DNFL_FATAL                  (0x00400000)
#define TSI721_RIO_PBM_SP_PW_ENABLE_IG_DNFL_COR                    (0x00800000)
#define TSI721_RIO_PBM_SP_PW_ENABLE_IG_DOH_FATAL                   (0x01000000)
#define TSI721_RIO_PBM_SP_PW_ENABLE_IG_DOH_COR                     (0x02000000)
#define TSI721_RIO_PBM_SP_PW_ENABLE_IG_TFL_FATAL                   (0x04000000)
#define TSI721_RIO_PBM_SP_PW_ENABLE_IG_TFL_COR                     (0x08000000)
#define TSI721_RIO_PBM_SP_PW_ENABLE_IG_TAG_FATAL                   (0x10000000)
#define TSI721_RIO_PBM_SP_PW_ENABLE_IG_TAG_COR                     (0x20000000)
#define TSI721_RIO_PBM_SP_PW_ENABLE_IG_DATA_UNCOR                  (0x40000000)
#define TSI721_RIO_PBM_SP_PW_ENABLE_IG_DATA_COR                    (0x80000000)

/* TSI721_RIO_PBM_SP_EVENT_GEN : Register Bits Masks Definitions */
#define TSI721_RIO_PBM_SP_EVENT_GEN_EG_BABBLE_PACKET               (0x00000001)
#define TSI721_RIO_PBM_SP_EVENT_GEN_EG_BAD_CHANNEL                 (0x00000002)
#define TSI721_RIO_PBM_SP_EVENT_GEN_EG_CRQ_OVERFLOW                (0x00000008)
#define TSI721_RIO_PBM_SP_EVENT_GEN_EG_DATA_OVERFLOW               (0x00000010)
#define TSI721_RIO_PBM_SP_EVENT_GEN_EG_DNFL_FATAL                  (0x00000020)
#define TSI721_RIO_PBM_SP_EVENT_GEN_EG_DNFL_COR                    (0x00000040)
#define TSI721_RIO_PBM_SP_EVENT_GEN_EG_DOH_FATAL                   (0x00000080)
#define TSI721_RIO_PBM_SP_EVENT_GEN_EG_DOH_COR                     (0x00000100)
#define TSI721_RIO_PBM_SP_EVENT_GEN_EG_DATA_UNCOR                  (0x00000800)
#define TSI721_RIO_PBM_SP_EVENT_GEN_EG_DATA_COR                    (0x00001000)
#define TSI721_RIO_PBM_SP_EVENT_GEN_IG_DNFL_FATAL                  (0x00400000)
#define TSI721_RIO_PBM_SP_EVENT_GEN_IG_DNFL_COR                    (0x00800000)
#define TSI721_RIO_PBM_SP_EVENT_GEN_IG_DOH_FATAL                   (0x01000000)
#define TSI721_RIO_PBM_SP_EVENT_GEN_IG_DOH_COR                     (0x02000000)
#define TSI721_RIO_PBM_SP_EVENT_GEN_IG_TFL_FATAL                   (0x04000000)
#define TSI721_RIO_PBM_SP_EVENT_GEN_IG_TFL_COR                     (0x08000000)
#define TSI721_RIO_PBM_SP_EVENT_GEN_IG_TAG_FATAL                   (0x10000000)
#define TSI721_RIO_PBM_SP_EVENT_GEN_IG_TAG_COR                     (0x20000000)
#define TSI721_RIO_PBM_SP_EVENT_GEN_IG_DATA_UNCOR                  (0x40000000)
#define TSI721_RIO_PBM_SP_EVENT_GEN_IG_DATA_COR                    (0x80000000)

/* TSI721_RIO_PBM_SP_IG_WATERMARK0 : Register Bits Masks Definitions */
#define TSI721_RIO_PBM_SP_IG_WATERMARK0_PRIO0_WM                   (0x000003ff)
#define TSI721_RIO_PBM_SP_IG_WATERMARK0_PRIO0CRF_WM                (0x03ff0000)

/* TSI721_RIO_PBM_SP_IG_WATERMARK1 : Register Bits Masks Definitions */
#define TSI721_RIO_PBM_SP_IG_WATERMARK1_PRIO1_WM                   (0x000003ff)
#define TSI721_RIO_PBM_SP_IG_WATERMARK1_PRIO1CRF_WM                (0x03ff0000)

/* TSI721_RIO_PBM_SP_IG_WATERMARK2 : Register Bits Masks Definitions */
#define TSI721_RIO_PBM_SP_IG_WATERMARK2_PRIO2_WM                   (0x000003ff)
#define TSI721_RIO_PBM_SP_IG_WATERMARK2_PRIO2CRF_WM                (0x03ff0000)

/* TSI721_RIO_PBM_SP_IG_WATERMARK3 : Register Bits Masks Definitions */
#define TSI721_RIO_PBM_SP_IG_WATERMARK3_PRIO3_WM                   (0x000003ff)
#define TSI721_RIO_PBM_SP_IG_WATERMARK3_PRIO3CRF_WM                (0x03ff0000)

/* TSI721_RIO_EM_BH : Register Bits Masks Definitions */
#define TSI721_RIO_EM_BH_BLK_TYPE                                  (0x00000fff)
#define TSI721_RIO_EM_BH_BLK_REV                                   (0x0000f000)
#define TSI721_RIO_EM_BH_NEXT_BLK_PTR                              (0xffff0000)

/* TSI721_RIO_EM_INT_STAT : Register Bits Masks Definitions */
#define TSI721_RIO_EM_INT_STAT_LOCALOG                             (0x00000100)
#define TSI721_RIO_EM_INT_STAT_IG_DATA_UNCOR                       (0x00000400)
#define TSI721_RIO_EM_INT_STAT_IG_DATA_COR                         (0x00000800)
#define TSI721_RIO_EM_INT_STAT_PW_RX                               (0x00010000)
#define TSI721_RIO_EM_INT_STAT_MECS                                (0x04000000)
#define TSI721_RIO_EM_INT_STAT_RCS                                 (0x08000000)
#define TSI721_RIO_EM_INT_STAT_LOG                                 (0x10000000)
#define TSI721_RIO_EM_INT_STAT_PORT                                (0x20000000)

/* TSI721_RIO_EM_INT_ENABLE : Register Bits Masks Definitions */
#define TSI721_RIO_EM_INT_ENABLE_LOCALOG                           (0x00000100)
#define TSI721_RIO_EM_INT_ENABLE_IG_DATA_UNCOR                     (0x00000400)
#define TSI721_RIO_EM_INT_ENABLE_IG_DATA_COR                       (0x00000800)
#define TSI721_RIO_EM_INT_ENABLE_PW_RX                             (0x00010000)
#define TSI721_RIO_EM_INT_ENABLE_MECS                              (0x04000000)
#define TSI721_RIO_EM_INT_ENABLE_LOG                               (0x10000000)

/* TSI721_RIO_EM_INT_PORT_STAT : Register Bits Masks Definitions */
#define TSI721_RIO_EM_INT_PORT_STAT_IRQ_PENDING                    (0x00000001)

/* TSI721_RIO_EM_PW_STAT : Register Bits Masks Definitions */
#define TSI721_RIO_EM_PW_STAT_LOCALOG                              (0x00000100)
#define TSI721_RIO_EM_PW_STAT_IG_DATA_UNCOR                        (0x00000400)
#define TSI721_RIO_EM_PW_STAT_IG_DATA_COR                          (0x00000800)
#define TSI721_RIO_EM_PW_STAT_RCS                                  (0x08000000)
#define TSI721_RIO_EM_PW_STAT_LOG                                  (0x10000000)
#define TSI721_RIO_EM_PW_STAT_PORT                                 (0x20000000)

/* TSI721_RIO_EM_PW_ENABLE : Register Bits Masks Definitions */
#define TSI721_RIO_EM_PW_ENABLE_LOCALOG                            (0x00000100)
#define TSI721_RIO_EM_PW_ENABLE_IG_DATA_UNCOR                      (0x00000400)
#define TSI721_RIO_EM_PW_ENABLE_IG_DATA_COR                        (0x00000800)
#define TSI721_RIO_EM_PW_ENABLE_LOG                                (0x10000000)

/* TSI721_RIO_EM_PW_PORT_STAT : Register Bits Masks Definitions */
#define TSI721_RIO_EM_PW_PORT_STAT_PW_PENDING                      (0x00000001)

/* TSI721_RIO_EM_DEV_INT_EN : Register Bits Masks Definitions */
#define TSI721_RIO_EM_DEV_INT_EN_INT_EN                            (0x00000001)

/* TSI721_RIO_EM_DEV_PW_EN : Register Bits Masks Definitions */
#define TSI721_RIO_EM_DEV_PW_EN_PW_EN                              (0x00000001)

/* TSI721_RIO_EM_MECS_STAT : Register Bits Masks Definitions */
#define TSI721_RIO_EM_MECS_STAT_CMD_STAT                           (0x000000ff)

/* TSI721_RIO_EM_MECS_INT_EN : Register Bits Masks Definitions */
#define TSI721_RIO_EM_MECS_INT_EN_CMD_EN                           (0x000000ff)

/* TSI721_RIO_EM_MECS_CAP_EN : Register Bits Masks Definitions */
#define TSI721_RIO_EM_MECS_CAP_EN_CMD_EN                           (0x000000ff)

/* TSI721_RIO_EM_MECS_TRIG_EN : Register Bits Masks Definitions */
#define TSI721_RIO_EM_MECS_TRIG_EN_CMD_EN                          (0x000000ff)
#define TSI721_RIO_EM_MECS_TRIG_EN_CMD_STAT                        (0x0000ff00)

/* TSI721_RIO_EM_MECS_REQ : Register Bits Masks Definitions */
#define TSI721_RIO_EM_MECS_REQ_CMD                                 (0x000000ff)
#define TSI721_RIO_EM_MECS_REQ_SEND                                (0x00000100)

/* TSI721_RIO_EM_MECS_PORT_STAT : Register Bits Masks Definitions */
#define TSI721_RIO_EM_MECS_PORT_STAT_PORT                          (0x00000001)

/* TSI721_RIO_EM_MECS_EVENT_GEN : Register Bits Masks Definitions */
#define TSI721_RIO_EM_MECS_EVENT_GEN_CMD_STAT                      (0x000000ff)
#define TSI721_RIO_EM_MECS_EVENT_GEN_IG_DATA_UNCOR                 (0x00000400)
#define TSI721_RIO_EM_MECS_EVENT_GEN_IG_DATA_COR                   (0x00000800)

/* TSI721_RIO_EM_RST_PORT_STAT : Register Bits Masks Definitions */
#define TSI721_RIO_EM_RST_PORT_STAT_RST_REQ                        (0x00000001)

/* TSI721_RIO_EM_RST_INT_EN : Register Bits Masks Definitions */
#define TSI721_RIO_EM_RST_INT_EN_RST_INT_EN                        (0x00000001)

/* TSI721_RIO_EM_RST_PW_EN : Register Bits Masks Definitions */
#define TSI721_RIO_EM_RST_PW_EN_RST_PW_EN                          (0x00000001)

/* TSI721_RIO_PW_BH : Register Bits Masks Definitions */
#define TSI721_RIO_PW_BH_BLK_TYPE                                  (0x00000fff)
#define TSI721_RIO_PW_BH_BLK_REV                                   (0x0000f000)
#define TSI721_RIO_PW_BH_NEXT_BLK_PTR                              (0xffff0000)

/* TSI721_RIO_PW_CTL : Register Bits Masks Definitions */
#define TSI721_RIO_PW_CTL_PWC_MODE                                 (0x01000000)
#define TSI721_RIO_PW_CTL_PW_TIMER                                 (0xf0000000)
#define TSI721_RIO_PW_CTL_PW_TIMER_103us                           (0x10000000)
#define TSI721_RIO_PW_CTL_PW_TIMER_205us                           (0x20000000)
#define TSI721_RIO_PW_CTL_PW_TIMER_410us                           (0x40000000)
#define TSI721_RIO_PW_CTL_PW_TIMER_820us                           (0x80000000)

/* TSI721_RIO_PW_ROUTE : Register Bits Masks Definitions */
#define TSI721_RIO_PW_ROUTE_PORT                                   (0x00000001)

/* TSI721_RIO_PW_RX_STAT : Register Bits Masks Definitions */
#define TSI721_RIO_PW_RX_STAT_PW_VAL                               (0x00000001)
#define TSI721_RIO_PW_RX_STAT_PW_DISC                              (0x00000002)
#define TSI721_RIO_PW_RX_STAT_PW_TRUNC                             (0x00000004)
#define TSI721_RIO_PW_RX_STAT_PW_SHORT                             (0x00000008)
#define TSI721_RIO_PW_RX_STAT_WDPTR                                (0x00000100)
#define TSI721_RIO_PW_RX_STAT_WR_SIZE                              (0x0000f000)

/* TSI721_RIO_PW_RX_EVENT_GEN : Register Bits Masks Definitions */
#define TSI721_RIO_PW_RX_EVENT_GEN_PW_VAL                          (0x00000001)
#define TSI721_RIO_PW_RX_EVENT_GEN_PW_DISC                         (0x00000002)

/* TSI721_RIO_PW_RX_CAPT : Register Bits Masks Definitions */
#define TSI721_RIO_PW_RX_CAPT_PW_CAPT                              (0xffffffff)

/* TSI721_RIO_LLM_BH : Register Bits Masks Definitions */
#define TSI721_RIO_LLM_BH_BLK_TYPE                                 (0x00000fff)
#define TSI721_RIO_LLM_BH_BLK_REV                                  (0x0000f000)
#define TSI721_RIO_LLM_BH_NEXT_BLK_PTR                             (0xffff0000)

/* TSI721_RIO_MTC_WR_RESTRICT : Register Bits Masks Definitions */
#define TSI721_RIO_MTC_WR_RESTRICT_WR_DIS                          (0x00000001)

/* TSI721_RIO_MTC_PWR_RESTRICT : Register Bits Masks Definitions */
#define TSI721_RIO_MTC_PWR_RESTRICT_PWR_DIS                        (0x00000001)

/* TSI721_RIO_MTC_RD_RESTRICT : Register Bits Masks Definitions */
#define TSI721_RIO_MTC_RD_RESTRICT_RD_DIS                          (0x00000001)

/* TSI721_RIO_WHITEBOARD : Register Bits Masks Definitions */
#define TSI721_RIO_WHITEBOARD_SCRATCH                              (0xffffffff)

/* TSI721_RIO_PRESCALAR_SRV_CLK : Register Bits Masks Definitions */
#define TSI721_RIO_PRESCALAR_SRV_CLK_PRESCALAR_SRV_CLK             (0x000000ff)

/* TSI721_RIO_REG_RST_CTL : Register Bits Masks Definitions */
#define TSI721_RIO_REG_RST_CTL_CLEAR_STICKY                        (0x00000001)

/* TSI721_RIO_LOCAL_ERR_DET : Register Bits Masks Definitions */
#define TSI721_RIO_LOCAL_ERR_DET_ILL_TYPE                          (0x00400000)
#define TSI721_RIO_LOCAL_ERR_DET_ILL_ID                            (0x04000000)

/* TSI721_RIO_LOCAL_ERR_EN : Register Bits Masks Definitions */
#define TSI721_RIO_LOCAL_ERR_EN_ILL_TYPE_EN                        (0x00400000)
#define TSI721_RIO_LOCAL_ERR_EN_ILL_ID_EN                          (0x04000000)

/* TSI721_RIO_LOCAL_H_ADDR_CAPT : Register Bits Masks Definitions */
#define TSI721_RIO_LOCAL_H_ADDR_CAPT_ADDR                          (0xffffffff)

/* TSI721_RIO_LOCAL_ADDR_CAPT : Register Bits Masks Definitions */
#define TSI721_RIO_LOCAL_ADDR_CAPT_XAMSBS                          (0x00000003)
#define TSI721_RIO_LOCAL_ADDR_CAPT_ADDR                            (0xfffffff8)

/* TSI721_RIO_LOCAL_ID_CAPT : Register Bits Masks Definitions */
#define TSI721_RIO_LOCAL_ID_CAPT_SRC_ID                            (0x000000ff)
#define TSI721_RIO_LOCAL_ID_CAPT_MSB_SRC_ID                        (0x0000ff00)
#define TSI721_RIO_LOCAL_ID_CAPT_DEST_ID                           (0x00ff0000)
#define TSI721_RIO_LOCAL_ID_CAPT_MSB_DEST_ID                       (0xff000000)

/* TSI721_RIO_LOCAL_CTRL_CAPT : Register Bits Masks Definitions */
#define TSI721_RIO_LOCAL_CTRL_CAPT_MESSAGE_INFO                    (0x00ff0000)
#define TSI721_RIO_LOCAL_CTRL_CAPT_TTYPE                           (0x0f000000)
#define TSI721_RIO_LOCAL_CTRL_CAPT_FTYPE                           (0xf0000000)

/* TSI721_RIO_FABRIC_BH : Register Bits Masks Definitions */
#define TSI721_RIO_FABRIC_BH_BLK_TYPE                              (0x00000fff)
#define TSI721_RIO_FABRIC_BH_BLK_REV                               (0x0000f000)
#define TSI721_RIO_FABRIC_BH_NEXT_BLK_PTR                          (0xffff0000)

/* TSI721_RIO_PRBS_BH : Register Bits Masks Definitions */
#define TSI721_RIO_PRBS_BH_BLK_TYPE                                (0x00000fff)
#define TSI721_RIO_PRBS_BH_BLK_REV                                 (0x0000f000)
#define TSI721_RIO_PRBS_BH_NEXT_BLK_PTR                            (0xffff0000)

/* TSI721_RIO_PRBS_LANEX_CTRL : Register Bits Masks Definitions */
#define TSI721_RIO_PRBS_LANEX_CTRL_TRANSMIT                        (0x01000000)
#define TSI721_RIO_PRBS_LANEX_CTRL_ENABLE                          (0x02000000)
#define TSI721_RIO_PRBS_LANEX_CTRL_TRAIN                           (0x04000000)
#define TSI721_RIO_PRBS_LANEX_CTRL_UNI                             (0x08000000)
#define TSI721_RIO_PRBS_LANEX_CTRL_PATTERN                         (0xf0000000)

/* TSI721_RIO_PRBS_LANEX_SEED : Register Bits Masks Definitions */
#define TSI721_RIO_PRBS_LANEX_SEED_SEED                            (0xffffffff)

/* TSI721_RIO_PRBS_LANEX_ERR_COUNT : Register Bits Masks Definitions */
#define TSI721_RIO_PRBS_LANEX_ERR_COUNT_COUNT                      (0xffffffff)


/**********************************************************/
/* TSI721 : Mapping Register address offset definitions   */
/**********************************************************/

#define TSI721_IDQ_CTLX(X)                               (0x20000 + 0x1000*(X))
#define TSI721_IDQ_STSX(X)                               (0x20004 + 0x1000*(X))
#define TSI721_IDQ_MASKX(X)                              (0x20008 + 0x1000*(X))
#define TSI721_IDQ_RPX(X)                                (0x2000c + 0x1000*(X))
#define TSI721_IDQ_WPX(X)                                (0x20010 + 0x1000*(X))
#define TSI721_IDQ_BASELX(X)                             (0x20014 + 0x1000*(X))
#define TSI721_IDQ_BASEUX(X)                             (0x20018 + 0x1000*(X))
#define TSI721_IDQ_SIZEX(X)                              (0x2001c + 0x1000*(X))
#define TSI721_SR_CHXINT(X)                              (0x20040 + 0x1000*(X))
#define TSI721_SR_CHXINTE(X)                             (0x20044 + 0x1000*(X))
#define TSI721_SR_CHXINTSET(X)                           (0x20048 + 0x1000*(X))
#define TSI721_ODB_CNTX(X)                               (0x20100 + 0x1000*(X))
#define TSI721_ODB_LOG_DAT0X(X)                          (0x20104 + 0x1000*(X))
#define TSI721_ODB_LOG_DAT1X(X)                          (0x20108 + 0x1000*(X))
#define TSI721_ODB_LOG_DAT2X(X)                          (0x2010c + 0x1000*(X))
#define TSI721_ODB_LOG_DAT3X(X)                          (0x20120 + 0x1000*(X))
#define TSI721_ODBXLOGSTS(X)                             (0x20124 + 0x1000*(X))
#define TSI721_IBWIN_LBX(X)                               (0x29000 + 0x020*(X))
#define TSI721_IBWIN_UBX(X)                               (0x29004 + 0x020*(X))
#define TSI721_IBWIN_SZX(X)                               (0x29008 + 0x020*(X))
#define TSI721_IBWIN_TLAX(X)                              (0x2900c + 0x020*(X))
#define TSI721_IBWIN_TUAX(X)                              (0x29010 + 0x020*(X))
#define TSI721_SR2PC_GEN_INTE                                      (0x00029800)
#define TSI721_SR2PC_PWE                                           (0x00029804)
#define TSI721_SR2PC_GEN_INT                                       (0x00029808)
#define TSI721_SR2PC_GEN_INTSET                                    (0x0002980c)
#define TSI721_SR2PC_CORR_ECC_LOG                                  (0x00029810)
#define TSI721_SR2PC_UNCORR_ECC_LOG                                (0x00029814)
#define TSI721_SR2PC_PCIE_PS                                       (0x00029820)
#define TSI721_LOGBUF_STS                                          (0x00029824)
#define TSI721_DEV_INTE                                            (0x00029840)
#define TSI721_DEV_INT                                             (0x00029844)
#define TSI721_DEV_CHAN_INTE                                       (0x0002984c)
#define TSI721_DEV_CHAN_INT                                        (0x00029850)
#define TSI721_INT_MOD                                             (0x00029858)
#define TSI721_RXPKT_SMSG_CNT                                      (0x00029900)
#define TSI721_RXRSP_BDMA_CNT                                      (0x00029904)
#define TSI721_RXPKT_BRG_CNT                                       (0x00029908)
#define TSI721_TXTLP_SMSG_CNT                                      (0x0002990c)
#define TSI721_TXTLP_BDMA_CNT                                      (0x00029910)
#define TSI721_TXTLP_BRG_CNT                                       (0x00029914)
#define TSI721_BRG_PKT_ERR_CNT                                     (0x0002991c)
#define TSI721_MWR_CNT                                             (0x00029a00)
#define TSI721_NWR_CNT                                             (0x00029a04)
#define TSI721_MWR_LOG_DAT0                                        (0x00029a08)
#define TSI721_MWR_LOG_DAT1                                        (0x00029a0c)
#define TSI721_MWR_LOG_DAT2                                        (0x00029a10)
#define TSI721_MWR_LOG_DAT3                                        (0x00029a14)
#define TSI721_NWR_LOG_DAT0                                        (0x00029a18)
#define TSI721_NWR_LOG_DAT1                                        (0x00029a1c)
#define TSI721_NWR_LOG_DAT2                                        (0x00029a20)
#define TSI721_NWR_LOG_DAT3                                        (0x00029a24)
#define TSI721_MSIX_PBAL                                           (0x0002a000)
#define TSI721_MSIX_PBAM                                           (0x0002a004)
#define TSI721_MSIX_PBAU                                           (0x0002a008)
#define TSI721_MSIX_TAB_ADDRLX(X)                         (0x2c000 + 0x010*(X))
#define TSI721_MSIX_TAB_ADDRUX(X)                         (0x2c004 + 0x010*(X))
#define TSI721_MSIX_TAB_DATAX(X)                          (0x2c008 + 0x010*(X))
#define TSI721_MSIX_TAB_MSKX(X)                           (0x2c00c + 0x010*(X))
#define TSI721_OBWINLBX(X)                                (0x40000 + 0x020*(X))
#define TSI721_OBWINUBX(X)                                (0x40004 + 0x020*(X))
#define TSI721_OBWINSZX(X)                                (0x40008 + 0x020*(X))
#define TSI721_ZONE_SEL                                            (0x00041300)
#define TSI721_LUT_DATA0                                           (0x00041304)
#define TSI721_LUT_DATA1                                           (0x00041308)
#define TSI721_LUT_DATA2                                           (0x0004130c)
#define TSI721_PC2SR_INTE                                          (0x00041310)
#define TSI721_PC2SR_INT                                           (0x00041314)
#define TSI721_PC2SR_INTSET                                        (0x00041318)
#define TSI721_PC2SR_ECC_LOG                                       (0x0004131c)
#define TSI721_CPL_SMSG_CNT                                        (0x00041404)
#define TSI721_CPL_BDMA_CNT                                        (0x00041408)
#define TSI721_RXTLP_BRG_CNT                                       (0x0004140c)
#define TSI721_TXPKT_SMSG_CNT                                      (0x00041410)
#define TSI721_TXPKT_BDMA_CNT                                      (0x00041414)
#define TSI721_TXPKT_BRG_CNT                                       (0x00041418)
#define TSI721_BRG_TLP_ERR_CNT                                     (0x0004141c)


/********************************************************/
/* TSI721 : Mapping Register Bit Masks and Reset Values */
/*           definitions for every register             */
/********************************************************/

/* TSI721_IDQ_CTLX : Register Bits Masks Definitions */
#define TSI721_IDQ_CTLX_INIT                                        (0x00000001)
#define TSI721_IDQ_CTLX_SUSPEND                                     (0x00000002)

/* TSI721_IDQ_STSX : Register Bits Masks Definitions */
#define TSI721_IDQ_STSX_RUN                                         (0x00200000)

/* TSI721_IDQ_MASKX : Register Bits Masks Definitions */
#define TSI721_IDQ_MASKX_PATTERN                                    (0x0000ffff)
#define TSI721_IDQ_MASKX_MASK                                       (0xffff0000)

/* TSI721_IDQ_RPX : Register Bits Masks Definitions */
#define TSI721_IDQ_RPX_RD_PTR                                       (0x0007ffff)

/* TSI721_IDQ_WPX : Register Bits Masks Definitions */
#define TSI721_IDQ_WPX_WR_PTR                                       (0x0007ffff)

/* TSI721_IDQ_BASELX : Register Bits Masks Definitions */
#define TSI721_IDQ_BASELX_ADD                                       (0xffffffc0)

/* TSI721_IDQ_BASEUX : Register Bits Masks Definitions */
#define TSI721_IDQ_BASEUX_ADD                                       (0xffffffff)

/* TSI721_IDQ_SIZEX : Register Bits Masks Definitions */
#define TSI721_IDQ_SIZEX_SIZE                                       (0x0000000f)

/* TSI721_SR_CHXINT : Register Bits Masks Definitions */
#define TSI721_SR_CHXINT_ODB_ERR                                    (0x00000001)
#define TSI721_SR_CHXINT_ODB_RETRY                                  (0x00000002)
#define TSI721_SR_CHXINT_ODB_TO                                     (0x00000004)
#define TSI721_SR_CHXINT_SUSPENDED                                  (0x00000008)
#define TSI721_SR_CHXINT_IDBQ_RCV                                   (0x00000010)
#define TSI721_SR_CHXINT_ODB_OK                                     (0x00000020)

/* TSI721_SR_CHXINTE : Register Bits Masks Definitions */
#define TSI721_SR_CHXINTE_ODB_ERR_EN                                (0x00000001)
#define TSI721_SR_CHXINTE_ODB_RETRY_EN                              (0x00000002)
#define TSI721_SR_CHXINTE_ODB_TO_EN                                 (0x00000004)
#define TSI721_SR_CHXINTE_SUSPENDED_EN                              (0x00000008)
#define TSI721_SR_CHXINTE_IDBQ_RCV_EN                               (0x00000010)
#define TSI721_SR_CHXINTE_ODB_OK_EN                                 (0x00000020)

/* TSI721_SR_CHXINTSET : Register Bits Masks Definitions */
#define TSI721_SR_CHXINTSET_ODB_ERR_SET                             (0x00000001)
#define TSI721_SR_CHXINTSET_ODB_RETRY_SET                           (0x00000002)
#define TSI721_SR_CHXINTSET_ODB_TO_SET                              (0x00000004)
#define TSI721_SR_CHXINTSET_SUSPENDED_SET                           (0x00000008)
#define TSI721_SR_CHXINTSET_IDBQ_RCV_SET                            (0x00000010)
#define TSI721_SR_CHXINTSET_ODB_OK_SET                              (0x00000020)

/* TSI721_ODB_CNTX : Register Bits Masks Definitions */
#define TSI721_ODB_CNTX_ODB_OK_CNT                                  (0x0000ffff)
#define TSI721_ODB_CNTX_ODB_TOT_CNT                                 (0xffff0000)

/* TSI721_ODB_LOG_DAT0X : Register Bits Masks Definitions */
#define TSI721_ODB_LOG_DAT0X_DATA                                   (0xffffffff)

/* TSI721_ODB_LOG_DAT1X : Register Bits Masks Definitions */
#define TSI721_ODB_LOG_DAT1X_DATA                                   (0xffffffff)

/* TSI721_ODB_LOG_DAT2X : Register Bits Masks Definitions */
#define TSI721_ODB_LOG_DAT2X_DATA                                   (0xffffffff)

/* TSI721_ODB_LOG_DAT3X : Register Bits Masks Definitions */
#define TSI721_ODB_LOG_DAT3X_DATA                                   (0xffffffff)

/* TSI721_ODBXLOGSTS : Register Bits Masks Definitions */
#define TSI721_ODBXLOGSTS_LOG_BUF_ERR                               (0x00000003)

/* TSI721_IBWIN_LBX : Register Bits Masks Definitions */
#define TSI721_IBWIN_LBX_WIN_EN                                     (0x00000001)
#define TSI721_IBWIN_LBX_ADD                                        (0xfffff000)

/* TSI721_IBWIN_UBX : Register Bits Masks Definitions */
#define TSI721_IBWIN_UBX_ADD                                        (0xffffffff)

/* TSI721_IBWIN_SZX : Register Bits Masks Definitions */
#define TSI721_IBWIN_SZX_SIZE                                       (0x00001f00)
#define TSI721_IBWIN_SZX_ADD                                        (0x03000000)

/* TSI721_IBWIN_TLAX : Register Bits Masks Definitions */
#define TSI721_IBWIN_TLAX_ADD                                       (0xfffff000)

/* TSI721_IBWIN_TUAX : Register Bits Masks Definitions */
#define TSI721_IBWIN_TUAX_ADD                                       (0xffffffff)

/* TSI721_SR2PC_GEN_INTE : Register Bits Masks Definitions */
#define TSI721_SR2PC_GEN_INTE_MW_RSP_ERR_EN                         (0x00000001)
#define TSI721_SR2PC_GEN_INTE_NW_RSP_ERR_EN                         (0x00000002)
#define TSI721_SR2PC_GEN_INTE_MW_RSP_TO_EN                          (0x00000004)
#define TSI721_SR2PC_GEN_INTE_NW_RSP_TO_EN                          (0x00000008)
#define TSI721_SR2PC_GEN_INTE_DB_MISS_EN                            (0x00000010)
#define TSI721_SR2PC_GEN_INTE_ECC_CORR_EN                           (0x00000020)
#define TSI721_SR2PC_GEN_INTE_ECC_UNCORR_EN                         (0x00000040)
#define TSI721_SR2PC_GEN_INTE_DL_DOWN_EN                            (0x00000100)
#define TSI721_SR2PC_GEN_INTE_MW_RSP_OK_EN                          (0x00000200)
#define TSI721_SR2PC_GEN_INTE_NW_RSP_OK_EN                          (0x00000400)
#define TSI721_SR2PC_GEN_INTE_UNS_RSP_EN                            (0x00800000)
#define TSI721_SR2PC_GEN_INTE_RSP_TO_EN                             (0x01000000)
#define TSI721_SR2PC_GEN_INTE_ILL_TARGET_EN                         (0x04000000)
#define TSI721_SR2PC_GEN_INTE_ILL_DEC_EN                            (0x08000000)
#define TSI721_SR2PC_GEN_INTE_ERR_RSP_EN                            (0x80000000)

/* TSI721_SR2PC_PWE : Register Bits Masks Definitions */
#define TSI721_SR2PC_PWE_DL_DOWN_EN                                 (0x00000100)
#define TSI721_SR2PC_PWE_UNS_RSP_EN                                 (0x00800000)
#define TSI721_SR2PC_PWE_RSP_TO_EN                                  (0x01000000)
#define TSI721_SR2PC_PWE_ILL_TARGET_EN                              (0x04000000)
#define TSI721_SR2PC_PWE_ILL_DEC_EN                                 (0x08000000)
#define TSI721_SR2PC_PWE_ERR_RSP_EN                                 (0x80000000)

/* TSI721_SR2PC_GEN_INT : Register Bits Masks Definitions */
#define TSI721_SR2PC_GEN_INT_MW_RSP_ERR                             (0x00000001)
#define TSI721_SR2PC_GEN_INT_NW_RSP_ERR                             (0x00000002)
#define TSI721_SR2PC_GEN_INT_MW_RSP_TO                              (0x00000004)
#define TSI721_SR2PC_GEN_INT_NW_RSP_TO                              (0x00000008)
#define TSI721_SR2PC_GEN_INT_DB_MISS                                (0x00000010)
#define TSI721_SR2PC_GEN_INT_ECC_CORR                               (0x00000020)
#define TSI721_SR2PC_GEN_INT_ECC_UNCORR                             (0x00000040)
#define TSI721_SR2PC_GEN_INT_DL_DOWN                                (0x00000100)
#define TSI721_SR2PC_GEN_INT_NW_RSP_OK                              (0x00000200)
#define TSI721_SR2PC_GEN_INT_MW_RSP_OK                              (0x00000400)
#define TSI721_SR2PC_GEN_INT_UNS_RSP                                (0x00800000)
#define TSI721_SR2PC_GEN_INT_RSP_TO                                 (0x01000000)
#define TSI721_SR2PC_GEN_INT_ILL_TARGET                             (0x04000000)
#define TSI721_SR2PC_GEN_INT_ILL_DEC                                (0x08000000)
#define TSI721_SR2PC_GEN_INT_ERR_RSP                                (0x80000000)

/* TSI721_SR2PC_GEN_INTSET : Register Bits Masks Definitions */
#define TSI721_SR2PC_GEN_INTSET_MW_RSP_ERR_SET                      (0x00000001)
#define TSI721_SR2PC_GEN_INTSET_NW_RSP_ERR_SET                      (0x00000002)
#define TSI721_SR2PC_GEN_INTSET_MW_RSP_TO_SET                       (0x00000004)
#define TSI721_SR2PC_GEN_INTSET_NW_RSP_TO_SET                       (0x00000008)
#define TSI721_SR2PC_GEN_INTSET_DB_MISS_SET                         (0x00000010)
#define TSI721_SR2PC_GEN_INTSET_ECC_CORR_SET                        (0x00000020)
#define TSI721_SR2PC_GEN_INTSET_ECC_UNCORR_SET                      (0x00000040)
#define TSI721_SR2PC_GEN_INTSET_NW_RSP_OK_SET                       (0x00000200)
#define TSI721_SR2PC_GEN_INTSET_MW_RSP_OK_SET                       (0x00000400)
#define TSI721_SR2PC_GEN_INTSET_UNS_RSP_SET                         (0x00800000)
#define TSI721_SR2PC_GEN_INTSET_RSP_TO_SET                          (0x01000000)
#define TSI721_SR2PC_GEN_INTSET_ILL_TARGET_SET                      (0x04000000)
#define TSI721_SR2PC_GEN_INTSET_ILL_DEC_SET                         (0x08000000)
#define TSI721_SR2PC_GEN_INTSET_ERR_RSP_SET                         (0x80000000)

/* TSI721_SR2PC_CORR_ECC_LOG : Register Bits Masks Definitions */
#define TSI721_SR2PC_CORR_ECC_LOG_ECC_CORR_MEM                      (0x0007ffff)

/* TSI721_SR2PC_UNCORR_ECC_LOG : Register Bits Masks Definitions */
#define TSI721_SR2PC_UNCORR_ECC_LOG_ECC_UNCORR_MEM                  (0x0007ffff)

/* TSI721_SR2PC_PCIE_PS : Register Bits Masks Definitions */
#define TSI721_SR2PC_PCIE_PS_DSTATE                                 (0x00000003)

/* TSI721_LOGBUF_STS : Register Bits Masks Definitions */
#define TSI721_LOGBUF_STS_NWR_LOG_BUF_ERR                           (0x00000001)
#define TSI721_LOGBUF_STS_MWR_LOG_BUF_ERR                           (0x00000002)

/* TSI721_DEV_INTE : Register Bits Masks Definitions */
#define TSI721_DEV_INTE_INT_PC2SR_EN                                (0x00000004)
#define TSI721_DEV_INTE_INT_I2C_EN                                  (0x00000010)
#define TSI721_DEV_INTE_INT_SRIO_EN                                 (0x00000020)
#define TSI721_DEV_INTE_INT_SR2PC_NONCH_EN                          (0x00000100)
#define TSI721_DEV_INTE_INT_SR2PC_CH_EN                             (0x00000200)
#define TSI721_DEV_INTE_INT_SMSG_NONCH_EN                           (0x00000400)
#define TSI721_DEV_INTE_INT_SMSG_CH_EN                              (0x00000800)
#define TSI721_DEV_INTE_INT_BDMA_NONCH_EN                           (0x00001000)
#define TSI721_DEV_INTE_INT_BDMA_CH_EN                              (0x00002000)

/* TSI721_DEV_INT : Register Bits Masks Definitions */
#define TSI721_DEV_INT_INT_PC2SR                                    (0x00000004)
#define TSI721_DEV_INT_INT_I2C                                      (0x00000010)
#define TSI721_DEV_INT_INT_SRIO                                     (0x00000020)
#define TSI721_DEV_INT_INT_SR2PC_NONCH                              (0x00000100)
#define TSI721_DEV_INT_INT_SR2PC_CH                                 (0x00000200)
#define TSI721_DEV_INT_INT_SMSG_NONCH                               (0x00000400)
#define TSI721_DEV_INT_INT_SMSG_CH                                  (0x00000800)
#define TSI721_DEV_INT_INT_BDMA_NONCH                               (0x00001000)
#define TSI721_DEV_INT_INT_BDMA_CH                                  (0x00002000)

/* TSI721_DEV_CHAN_INTE : Register Bits Masks Definitions */
#define TSI721_DEV_CHAN_INTE_INT_BDMA_CHAN_EN                       (0x000000ff)
#define TSI721_DEV_CHAN_INTE_INT_OBMSG_CHAN_EN                      (0x0000ff00)
#define TSI721_DEV_CHAN_INTE_INT_IBMSG_CHAN_EN                      (0x00ff0000)
#define TSI721_DEV_CHAN_INTE_INT_SR2PC_CHAN_EN                      (0xff000000)

/* TSI721_DEV_CHAN_INT : Register Bits Masks Definitions */
#define TSI721_DEV_CHAN_INT_INT_BDMA_CHAN                           (0x000000ff)
#define TSI721_DEV_CHAN_INT_INT_OBMSG_CHAN                          (0x0000ff00)
#define TSI721_DEV_CHAN_INT_INT_IBMSG_CHAN                          (0x00ff0000)
#define TSI721_DEV_CHAN_INT_INT_SR2PC_CHAN                          (0xff000000)

/* TSI721_INT_MOD : Register Bits Masks Definitions */
#define TSI721_INT_MOD_INT_MOD                                      (0xffffffff)

/* TSI721_RXPKT_SMSG_CNT : Register Bits Masks Definitions */
#define TSI721_RXPKT_SMSG_CNT_RXPKT_SMSG_CNT                        (0xffffffff)

/* TSI721_RXRSP_BDMA_CNT : Register Bits Masks Definitions */
#define TSI721_RXRSP_BDMA_CNT_RXRSP_BDMA_CNT                        (0xffffffff)

/* TSI721_RXPKT_BRG_CNT : Register Bits Masks Definitions */
#define TSI721_RXPKT_BRG_CNT_RXPKT_BRG_CNT                          (0xffffffff)

/* TSI721_TXTLP_SMSG_CNT : Register Bits Masks Definitions */
#define TSI721_TXTLP_SMSG_CNT_TXTLP_SMSG_CNT                        (0xffffffff)

/* TSI721_TXTLP_BDMA_CNT : Register Bits Masks Definitions */
#define TSI721_TXTLP_BDMA_CNT_TXTLP_BDMA_CNT                        (0xffffffff)

/* TSI721_TXTLP_BRG_CNT : Register Bits Masks Definitions */
#define TSI721_TXTLP_BRG_CNT_TXTLP_BRG_CNT                          (0xffffffff)

/* TSI721_BRG_PKT_ERR_CNT : Register Bits Masks Definitions */
#define TSI721_BRG_PKT_ERR_CNT_BRG_PKT_ERR_CNT                      (0xffffffff)

/* TSI721_MWR_CNT : Register Bits Masks Definitions */
#define TSI721_MWR_CNT_MW_OK_CNT                                    (0x0000ffff)
#define TSI721_MWR_CNT_MW_TOT_CNT                                   (0xffff0000)

/* TSI721_NWR_CNT : Register Bits Masks Definitions */
#define TSI721_NWR_CNT_NW_OK_CNT                                    (0x0000ffff)
#define TSI721_NWR_CNT_NW_TOT_CNT                                   (0xffff0000)

/* TSI721_MWR_LOG_DAT0 : Register Bits Masks Definitions */
#define TSI721_MWR_LOG_DAT0_DATA                                    (0xffffffff)

/* TSI721_MWR_LOG_DAT1 : Register Bits Masks Definitions */
#define TSI721_MWR_LOG_DAT1_DATA                                    (0xffffffff)

/* TSI721_MWR_LOG_DAT2 : Register Bits Masks Definitions */
#define TSI721_MWR_LOG_DAT2_DATA                                    (0xffffffff)

/* TSI721_MWR_LOG_DAT3 : Register Bits Masks Definitions */
#define TSI721_MWR_LOG_DAT3_DATA                                    (0xffffffff)

/* TSI721_NWR_LOG_DAT0 : Register Bits Masks Definitions */
#define TSI721_NWR_LOG_DAT0_DATA                                    (0xffffffff)

/* TSI721_NWR_LOG_DAT1 : Register Bits Masks Definitions */
#define TSI721_NWR_LOG_DAT1_DATA                                    (0xffffffff)

/* TSI721_NWR_LOG_DAT2 : Register Bits Masks Definitions */
#define TSI721_NWR_LOG_DAT2_DATA                                    (0xffffffff)

/* TSI721_NWR_LOG_DAT3 : Register Bits Masks Definitions */
#define TSI721_NWR_LOG_DAT3_DATA                                    (0xffffffff)

/* TSI721_MSIX_PBAL : Register Bits Masks Definitions */
#define TSI721_MSIX_PBAL_PENDING                                    (0xffffffff)

/* TSI721_MSIX_PBAM : Register Bits Masks Definitions */
#define TSI721_MSIX_PBAM_PENDING                                    (0xffffffff)

/* TSI721_MSIX_PBAU : Register Bits Masks Definitions */
#define TSI721_MSIX_PBAU_PENDING                                    (0x0000003f)

/* TSI721_MSIX_TAB_ADDRLX : Register Bits Masks Definitions */
#define TSI721_MSIX_TAB_ADDRLX_ADDR                                 (0xffffffff)

/* TSI721_MSIX_TAB_ADDRUX : Register Bits Masks Definitions */
#define TSI721_MSIX_TAB_ADDRUX_ADDR                                 (0xffffffff)

/* TSI721_MSIX_TAB_DATAX : Register Bits Masks Definitions */
#define TSI721_MSIX_TAB_DATAX_DATA                                  (0xffffffff)

/* TSI721_MSIX_TAB_MSKX : Register Bits Masks Definitions */
#define TSI721_MSIX_TAB_MSKX_MASK                                   (0x00000001)

/* TSI721_OBWINLBX : Register Bits Masks Definitions */
#define TSI721_OBWINLBX_WIN_EN                                      (0x00000001)
#define TSI721_OBWINLBX_ADD                                         (0xffff8000)

/* TSI721_OBWINUBX : Register Bits Masks Definitions */
#define TSI721_OBWINUBX_ADD                                         (0xffffffff)

/* TSI721_OBWINSZX : Register Bits Masks Definitions */
#define TSI721_OBWINSZX_SIZE                                        (0x00001f00)

/* TSI721_ZONE_SEL : Register Bits Masks Definitions */
#define TSI721_ZONE_SEL_ZONE_SEL                                    (0x00000007)
#define TSI721_ZONE_SEL_WIN_SEL                                     (0x00000038)
#define TSI721_ZONE_SEL_ZONE_GO                                     (0x00010000)
#define TSI721_ZONE_SEL_RD_WRB                                      (0x00020000)

/* TSI721_LUT_DATA0 : Register Bits Masks Definitions */
#define TSI721_LUT_DATA0_WR_TYPE                                    (0x0000000f)
#define TSI721_LUT_DATA0_WR_CRF                                     (0x00000010)
#define TSI721_LUT_DATA0_RD_CRF                                     (0x00000020)
#define TSI721_LUT_DATA0_RD_TYPE                                    (0x00000f00)
#define TSI721_LUT_DATA0_ADD                                        (0xfffff000)

/* TSI721_LUT_DATA1 : Register Bits Masks Definitions */
#define TSI721_LUT_DATA1_ADD                                        (0xffffffff)

/* TSI721_LUT_DATA2 : Register Bits Masks Definitions */
#define TSI721_LUT_DATA2_DEVICEID                                   (0x0000ffff)
#define TSI721_LUT_DATA2_TT                                         (0x00030000)
#define TSI721_LUT_DATA2_ADD                                        (0x000c0000)
#define TSI721_LUT_DATA2_HOP_CNT                                    (0xff000000)

/* TSI721_PC2SR_INTE : Register Bits Masks Definitions */
#define TSI721_PC2SR_INTE_ECC_CORR_EN                               (0x00000002)
#define TSI721_PC2SR_INTE_ECC_UNCORR_EN                             (0x00000004)

/* TSI721_PC2SR_INT : Register Bits Masks Definitions */
#define TSI721_PC2SR_INT_ECC_CORR                                   (0x00000002)
#define TSI721_PC2SR_INT_ECC_UNCORR                                 (0x00000004)

/* TSI721_PC2SR_INTSET : Register Bits Masks Definitions */
#define TSI721_PC2SR_INTSET_ECC_CORR_SET                            (0x00000002)
#define TSI721_PC2SR_INTSET_ECC_UNCORR_SET                          (0x00000004)

/* TSI721_PC2SR_ECC_LOG : Register Bits Masks Definitions */
#define TSI721_PC2SR_ECC_LOG_ECC_CORR_MEM                           (0x00003fff)
#define TSI721_PC2SR_ECC_LOG_ECC_UNCORR_MEM                         (0x3fff0000)

/* TSI721_CPL_SMSG_CNT : Register Bits Masks Definitions */
#define TSI721_CPL_SMSG_CNT_CPL_SMSG_CNT                            (0xffffffff)

/* TSI721_CPL_BDMA_CNT : Register Bits Masks Definitions */
#define TSI721_CPL_BDMA_CNT_CPL_BDMA_CNT                            (0xffffffff)

/* TSI721_RXTLP_BRG_CNT : Register Bits Masks Definitions */
#define TSI721_RXTLP_BRG_CNT_RXTLP_BRG_CNT                          (0xffffffff)

/* TSI721_TXPKT_SMSG_CNT : Register Bits Masks Definitions */
#define TSI721_TXPKT_SMSG_CNT_TXPKT_SMSG_CNT                        (0xffffffff)

/* TSI721_TXPKT_BDMA_CNT : Register Bits Masks Definitions */
#define TSI721_TXPKT_BDMA_CNT_TXPKT_BDMA_CNT                        (0xffffffff)

/* TSI721_TXPKT_BRG_CNT : Register Bits Masks Definitions */
#define TSI721_TXPKT_BRG_CNT_TXPKT_BRG_CNT                          (0xffffffff)

/* TSI721_BRG_TLP_ERR_CNT : Register Bits Masks Definitions */
#define TSI721_BRG_TLP_ERR_CNT_BRG_TLP_ERR_CNT                      (0xffffffff)


/***********************************************************/
/* TSI721 : Messaging Register address offset definitions  */
/***********************************************************/

#define TSI721_RQRPTO                                              (0x00060010)
#define TSI721_IB_DEVID                                            (0x00060020)
#define TSI721_OBDMACXDWRCNT(X)                          (0x61000 + 0x1000*(X))
#define TSI721_OBDMACXDRDCNT(X)                          (0x61004 + 0x1000*(X))
#define TSI721_OBDMACXCTL(X)                             (0x61008 + 0x1000*(X))
#define TSI721_OBDMACXINT(X)                             (0x6100c + 0x1000*(X))
#define TSI721_OBDMACXINTSET(X)                          (0x61010 + 0x1000*(X))
#define TSI721_OBDMACXSTS(X)                             (0x61014 + 0x1000*(X))
#define TSI721_OBDMACXINTE(X)                            (0x61018 + 0x1000*(X))
#define TSI721_OBDMACXPWE(X)                             (0x6101c + 0x1000*(X))
#define TSI721_OBDMACXDPTRL(X)                           (0x61020 + 0x1000*(X))
#define TSI721_OBDMACXDPTRH(X)                           (0x61024 + 0x1000*(X))
#define TSI721_OBDMACXDSBL(X)                            (0x61040 + 0x1000*(X))
#define TSI721_OBDMACXDSBH(X)                            (0x61044 + 0x1000*(X))
#define TSI721_OBDMACXDSSZ(X)                            (0x61048 + 0x1000*(X))
#define TSI721_OBDMACXDSRP(X)                            (0x6104c + 0x1000*(X))
#define TSI721_OBDMACXDSWP(X)                            (0x61050 + 0x1000*(X))
#define TSI721_IBDMACXFQBL(X)                            (0x61200 + 0x1000*(X))
#define TSI721_IBDMACXFQBH(X)                            (0x61204 + 0x1000*(X))
#define TSI721_IBDMACXFQSZ(X)                            (0x61208 + 0x1000*(X))
#define TSI721_IBDMACXFQRP(X)                            (0x6120c + 0x1000*(X))
#define TSI721_IBDMACXFQWP(X)                            (0x61210 + 0x1000*(X))
#define TSI721_IBDMACXFQTH(X)                            (0x61214 + 0x1000*(X))
#define TSI721_IBDMACXCTL(X)                             (0x61240 + 0x1000*(X))
#define TSI721_IBDMACXSTS(X)                             (0x61244 + 0x1000*(X))
#define TSI721_IBDMACXINT(X)                             (0x61248 + 0x1000*(X))
#define TSI721_IBDMACXINTSET(X)                          (0x6124c + 0x1000*(X))
#define TSI721_IBDMACXINTE(X)                            (0x61250 + 0x1000*(X))
#define TSI721_IBDMACXPWE(X)                             (0x61254 + 0x1000*(X))
#define TSI721_IBDMACXDQBL(X)                            (0x61300 + 0x1000*(X))
#define TSI721_IBDMACXDQBH(X)                            (0x61304 + 0x1000*(X))
#define TSI721_IBDMACXDQRP(X)                            (0x61308 + 0x1000*(X))
#define TSI721_IBDMACXDQWP(X)                            (0x6130c + 0x1000*(X))
#define TSI721_IBDMACXDQSZ(X)                            (0x61314 + 0x1000*(X))
#define TSI721_SMSG_INTE                                           (0x0006a000)
#define TSI721_SMSG_PWE                                            (0x0006a004)
#define TSI721_SMSG_INT                                            (0x0006a008)
#define TSI721_SMSG_PW                                             (0x0006a00c)
#define TSI721_SMSG_INTSET                                         (0x0006a010)
#define TSI721_SMSG_ECC_LOG                                        (0x0006a014)
#define TSI721_RETRY_GEN_CNT                                       (0x0006a100)
#define TSI721_RETRY_RX_CNT                                        (0x0006a104)
#define TSI721_SMSG_ECC_CORRXLOG(X)                       (0x6a300 + 0x004*(X))
#define TSI721_SMSG_ECC_UNCORRXLOG(X)                     (0x6a340 + 0x004*(X))


/***********************************************************/
/* TSI721 : Messaging Register Bit Masks and Reset Values  */
/*           definitions for every register                */
/***********************************************************/

/* TSI721_RQRPTO : Register Bits Masks Definitions */
#define TSI721_RQRPTO_REQ_RSP_TO                                    (0x00ffffff)

/* TSI721_IB_DEVID : Register Bits Masks Definitions */
#define TSI721_IB_DEVID_DEVID                                       (0x0000ffff)

/* TSI721_OBDMACXDWRCNT : Register Bits Masks Definitions */
#define TSI721_OBDMACXDWRCNT_DWRCNT                                 (0xffffffff)

/* TSI721_OBDMACXDRDCNT : Register Bits Masks Definitions */
#define TSI721_OBDMACXDRDCNT_DRDCNT                                 (0xffffffff)

/* TSI721_OBDMACXCTL : Register Bits Masks Definitions */
#define TSI721_OBDMACXCTL_INIT                                      (0x00000001)
#define TSI721_OBDMACXCTL_SUSPEND                                   (0x00000002)
#define TSI721_OBDMACXCTL_RETRY_THR                                 (0x00000004)

/* TSI721_OBDMACXINT : Register Bits Masks Definitions */
#define TSI721_OBDMACXINT_IOF_DONE                                  (0x00000001)
#define TSI721_OBDMACXINT_ERROR                                     (0x00000002)
#define TSI721_OBDMACXINT_SUSPENDED                                 (0x00000004)
#define TSI721_OBDMACXINT_DONE                                      (0x00000008)
#define TSI721_OBDMACXINT_ST_FULL                                   (0x00000010)

/* TSI721_OBDMACXINTSET : Register Bits Masks Definitions */
#define TSI721_OBDMACXINTSET_IOF_DONE_SET                           (0x00000001)
#define TSI721_OBDMACXINTSET_ERROR_SET                              (0x00000002)
#define TSI721_OBDMACXINTSET_SUSPENDED_SET                          (0x00000004)
#define TSI721_OBDMACXINTSET_DONE_SET                               (0x00000008)
#define TSI721_OBDMACXINTSET_ST_FULL_SET                            (0x00000010)

/* TSI721_OBDMACXSTS : Register Bits Masks Definitions */
#define TSI721_OBDMACXSTS_CS                                        (0x001f0000)
#define TSI721_OBDMACXSTS_RUN                                       (0x00200000)
#define TSI721_OBDMACXSTS_ABORT                                     (0x00400000)

/* TSI721_OBDMACXINTE : Register Bits Masks Definitions */
#define TSI721_OBDMACXINTE_IOF_DONE_EN                              (0x00000001)
#define TSI721_OBDMACXINTE_ERROR_EN                                 (0x00000002)
#define TSI721_OBDMACXINTE_SUSPENDED_EN                             (0x00000004)
#define TSI721_OBDMACXINTE_DONE_EN                                  (0x00000008)
#define TSI721_OBDMACXINTE_ST_FULL_EN                               (0x00000010)

/* TSI721_OBDMACXPWE : Register Bits Masks Definitions */
#define TSI721_OBDMACXPWE_ERROR_EN                                  (0x00000002)

/* TSI721_OBDMACXDPTRL : Register Bits Masks Definitions */
#define TSI721_OBDMACXDPTRL_DPTRL                                   (0xfffffff0)

/* TSI721_OBDMACXDPTRH : Register Bits Masks Definitions */
#define TSI721_OBDMACXDPTRH_DPTRH                                   (0xffffffff)

/* TSI721_OBDMACXDSBL : Register Bits Masks Definitions */
#define TSI721_OBDMACXDSBL_ADD                                      (0xffffffc0)

/* TSI721_OBDMACXDSBH : Register Bits Masks Definitions */
#define TSI721_OBDMACXDSBH_ADD                                      (0xffffffff)

/* TSI721_OBDMACXDSSZ : Register Bits Masks Definitions */
#define TSI721_OBDMACXDSSZ_SIZE                                     (0x0000000f)

/* TSI721_OBDMACXDSRP : Register Bits Masks Definitions */
#define TSI721_OBDMACXDSRP_RD_PTR                                   (0x0007ffff)

/* TSI721_OBDMACXDSWP : Register Bits Masks Definitions */
#define TSI721_OBDMACXDSWP_WR_PTR                                   (0x0007ffff)

/* TSI721_IBDMACXFQBL : Register Bits Masks Definitions */
#define TSI721_IBDMACXFQBL_ADD                                      (0xffffffc0)

/* TSI721_IBDMACXFQBH : Register Bits Masks Definitions */
#define TSI721_IBDMACXFQBH_ADD                                      (0xffffffff)

/* TSI721_IBDMACXFQSZ : Register Bits Masks Definitions */
#define TSI721_IBDMACXFQSZ_SIZE                                     (0x0000000f)

/* TSI721_IBDMACXFQRP : Register Bits Masks Definitions */
#define TSI721_IBDMACXFQRP_RD_PTR                                   (0x0007ffff)

/* TSI721_IBDMACXFQWP : Register Bits Masks Definitions */
#define TSI721_IBDMACXFQWP_WR_PTR                                   (0x0007ffff)

/* TSI721_IBDMACXFQTH : Register Bits Masks Definitions */
#define TSI721_IBDMACXFQTH_IBFQ_TH                                  (0x0007ffff)

/* TSI721_IBDMACXCTL : Register Bits Masks Definitions */
#define TSI721_IBDMACXCTL_INIT                                      (0x00000001)
#define TSI721_IBDMACXCTL_SUSPEND                                   (0x00000002)

/* TSI721_IBDMACXSTS : Register Bits Masks Definitions */
#define TSI721_IBDMACXSTS_CS                                        (0x001f0000)
#define TSI721_IBDMACXSTS_RUN                                       (0x00200000)
#define TSI721_IBDMACXSTS_ABORT                                     (0x00400000)

/* TSI721_IBDMACXINT : Register Bits Masks Definitions */
#define TSI721_IBDMACXINT_DQ_RCV                                    (0x00000001)
#define TSI721_IBDMACXINT_FQ_LOW                                    (0x00000002)
#define TSI721_IBDMACXINT_PC_ERROR                                  (0x00000004)
#define TSI721_IBDMACXINT_SUSPENDED                                 (0x00000008)
#define TSI721_IBDMACXINT_SRTO                                      (0x00001000)
#define TSI721_IBDMACXINT_MASK (TSI721_IBDMACXINT_DQ_RCV | \
				TSI721_IBDMACXINT_FQ_LOW | \
				TSI721_IBDMACXINT_PC_ERROR | \
				TSI721_IBDMACXINT_SUSPENDED | \
				TSI721_IBDMACXINT_SRTO)

/* TSI721_IBDMACXINTSET : Register Bits Masks Definitions */
#define TSI721_IBDMACXINTSET_DQ_RCV_SET                             (0x00000001)
#define TSI721_IBDMACXINTSET_FQ_LOW_SET                             (0x00000002)
#define TSI721_IBDMACXINTSET_PC_ERROR_SET                           (0x00000004)
#define TSI721_IBDMACXINTSET_SUSPENDED_SET                          (0x00000008)
#define TSI721_IBDMACXINTSET_SRTO_SET                               (0x00001000)

/* TSI721_IBDMACXINTE : Register Bits Masks Definitions */
#define TSI721_IBDMACXINTE_DQ_RCV_EN                                (0x00000001)
#define TSI721_IBDMACXINTE_FQ_LOW_EN                                (0x00000002)
#define TSI721_IBDMACXINTE_PC_ERROR_EN                              (0x00000004)
#define TSI721_IBDMACXINTE_SUSPENDED_EN                             (0x00000008)
#define TSI721_IBDMACXINTE_SRTO_EN                                  (0x00001000)

/* TSI721_IBDMACXPWE : Register Bits Masks Definitions */
#define TSI721_IBDMACXPWE_SRTO_EN                                   (0x00001000)

/* TSI721_IBDMACXDQBL : Register Bits Masks Definitions */
#define TSI721_IBDMACXDQBL_ADD                                      (0xffffffc0)

/* TSI721_IBDMACXDQBH : Register Bits Masks Definitions */
#define TSI721_IBDMACXDQBH_ADD                                      (0xffffffff)

/* TSI721_IBDMACXDQRP : Register Bits Masks Definitions */
#define TSI721_IBDMACXDQRP_RD_PTR                                   (0x0007ffff)

/* TSI721_IBDMACXDQWP : Register Bits Masks Definitions */
#define TSI721_IBDMACXDQWP_WR_PTR                                   (0x0007ffff)

/* TSI721_IBDMACXDQSZ : Register Bits Masks Definitions */
#define TSI721_IBDMACXDQSZ_SIZE                                     (0x0000000f)

/* TSI721_SMSG_INTE : Register Bits Masks Definitions */
#define TSI721_SMSG_INTE_ECC_CORR_CH_EN                             (0x000000ff)
#define TSI721_SMSG_INTE_ECC_UNCORR_CH_EN                           (0x0000ff00)
#define TSI721_SMSG_INTE_ECC_CORR_EN                                (0x00020000)
#define TSI721_SMSG_INTE_ECC_UNCORR_EN                              (0x00040000)
#define TSI721_SMSG_INTE_UNS_RSP_EN                                 (0x00800000)

/* TSI721_SMSG_PWE : Register Bits Masks Definitions */
#define TSI721_SMSG_PWE_OBDMA_PW_EN                                 (0x000000ff)
#define TSI721_SMSG_PWE_IBDMA_PW_EN                                 (0x0000ff00)
#define TSI721_SMSG_PWE_UNS_RSP_EN                                  (0x00800000)

/* TSI721_SMSG_INT : Register Bits Masks Definitions */
#define TSI721_SMSG_INT_ECC_CORR_CH                                 (0x000000ff)
#define TSI721_SMSG_INT_ECC_UNCORR_CH                               (0x0000ff00)
#define TSI721_SMSG_INT_ECC_CORR                                    (0x00020000)
#define TSI721_SMSG_INT_ECC_UNCORR                                  (0x00040000)
#define TSI721_SMSG_INT_UNS_RSP                                     (0x00800000)

/* TSI721_SMSG_PW : Register Bits Masks Definitions */
#define TSI721_SMSG_PW_OBDMA_PW                                     (0x000000ff)
#define TSI721_SMSG_PW_IBDMA_PW                                     (0x0000ff00)

/* TSI721_SMSG_INTSET : Register Bits Masks Definitions */
#define TSI721_SMSG_INTSET_ECC_CORR_CH_SET                          (0x000000ff)
#define TSI721_SMSG_INTSET_ECC_UNCORR_CH_SET                        (0x0000ff00)
#define TSI721_SMSG_INTSET_ECC_CORR_SET                             (0x00020000)
#define TSI721_SMSG_INTSET_ECC_UNCORR_SET                           (0x00040000)
#define TSI721_SMSG_INTSET_UNS_RSP_SET                              (0x00800000)

/* TSI721_SMSG_ECC_LOG : Register Bits Masks Definitions */
#define TSI721_SMSG_ECC_LOG_ECC_CORR_MEM                            (0x00000007)
#define TSI721_SMSG_ECC_LOG_ECC_UNCORR_MEM                          (0x00070000)

/* TSI721_RETRY_GEN_CNT : Register Bits Masks Definitions */
#define TSI721_RETRY_GEN_CNT_RETRY_GEN_CNT                          (0xffffffff)

/* TSI721_RETRY_RX_CNT : Register Bits Masks Definitions */
#define TSI721_RETRY_RX_CNT_RETRY_RX_CNT                            (0xffffffff)

/* TSI721_SMSG_ECC_CORRXLOG : Register Bits Masks Definitions */
#define TSI721_SMSG_ECC_CORRXLOG_ECC_CORR_MEM                       (0x000000ff)

/* TSI721_SMSG_ECC_UNCORRXLOG : Register Bits Masks Definitions */
#define TSI721_SMSG_ECC_UNCORRXLOG_ECC_UNCORR_MEM                   (0x000000ff)


/**********************************************************/
/* TSI721 : Block DMA Register address offset definitions */
/**********************************************************/

#define TSI721_DMACXDWRCNT(X)                            (0x51000 + 0x1000*(X))
#define TSI721_DMACXDRDCNT(X)                            (0x51004 + 0x1000*(X))
#define TSI721_DMACXCTL(X)                               (0x51008 + 0x1000*(X))
#define TSI721_DMACXINT(X)                               (0x5100c + 0x1000*(X))
#define TSI721_DMACXINTSET(X)                            (0x51010 + 0x1000*(X))
#define TSI721_DMACXSTS(X)                               (0x51014 + 0x1000*(X))
#define TSI721_DMACXINTE(X)                              (0x51018 + 0x1000*(X))
#define TSI721_DMACXDPTRL(X)                             (0x51024 + 0x1000*(X))
#define TSI721_DMACXDPTRH(X)                             (0x51028 + 0x1000*(X))
#define TSI721_DMACXDSBL(X)                              (0x5102c + 0x1000*(X))
#define TSI721_DMACXDSBH(X)                              (0x51030 + 0x1000*(X))
#define TSI721_DMACXDSSZ(X)                              (0x51034 + 0x1000*(X))
#define TSI721_DMACXDSRP(X)                              (0x51038 + 0x1000*(X))
#define TSI721_DMACXDSWP(X)                              (0x5103c + 0x1000*(X))
#define TSI721_BDMA_INTE                                           (0x0005f000)
#define TSI721_BDMA_INT                                            (0x0005f004)
#define TSI721_BDMA_INTSET                                         (0x0005f008)
#define TSI721_BDMA_ECC_LOG                                        (0x0005f00c)
#define TSI721_BDMA_ECC_CORRXLOG(X)                       (0x5f300 + 0x004*(X))
#define TSI721_BDMA_ECC_UNCORRXLOG(X)                     (0x5f340 + 0x004*(X))


/**********************************************************/
/* TSI721 : Block DMA Register Bit Masks and Reset Values */
/*           definitions for every register               */
/**********************************************************/

/* TSI721_DMACXDWRCNT : Register Bits Masks Definitions */
#define TSI721_DMACXDWRCNT_DWRCNT                                   (0xffffffff)

/* TSI721_DMACXDRDCNT : Register Bits Masks Definitions */
#define TSI721_DMACXDRDCNT_DRDCNT                                   (0xffffffff)

/* TSI721_DMACXCTL : Register Bits Masks Definitions */
#define TSI721_DMACXCTL_INIT                                        (0x00000001)
#define TSI721_DMACXCTL_SUSPEND                                     (0x00000002)

/* TSI721_DMACXINT : Register Bits Masks Definitions */
#define TSI721_DMACXINT_IOF_DONE                                    (0x00000001)
#define TSI721_DMACXINT_ERROR                                       (0x00000002)
#define TSI721_DMACXINT_SUSPENDED                                   (0x00000004)
#define TSI721_DMACXINT_DONE                                        (0x00000008)
#define TSI721_DMACXINT_ST_FULL                                     (0x00000010)

/* TSI721_DMACXINTSET : Register Bits Masks Definitions */
#define TSI721_DMACXINTSET_IOF_DONE_SET                             (0x00000001)
#define TSI721_DMACXINTSET_ERROR_SET                                (0x00000002)
#define TSI721_DMACXINTSET_SUSPENDED_SET                            (0x00000004)
#define TSI721_DMACXINTSET_DONE_SET                                 (0x00000008)
#define TSI721_DMACXINTSET_ST_FULL_SET                              (0x00000010)

/* TSI721_DMACXSTS : Register Bits Masks Definitions */
#define TSI721_DMACXSTS_CS                                          (0x001f0000)
#define TSI721_DMACXSTS_RUN                                         (0x00200000)
#define TSI721_DMACXSTS_ABORT                                       (0x00400000)

/* TSI721_DMACXINTE : Register Bits Masks Definitions */
#define TSI721_DMACXINTE_IOF_DONE_EN                                (0x00000001)
#define TSI721_DMACXINTE_ERROR_EN                                   (0x00000002)
#define TSI721_DMACXINTE_SUSPENDED_EN                               (0x00000004)
#define TSI721_DMACXINTE_DONE_EN                                    (0x00000008)
#define TSI721_DMACXINTE_ST_FULL_EN                                 (0x00000010)

/* TSI721_DMACXDPTRL : Register Bits Masks Definitions */
#define TSI721_DMACXDPTRL_DPTRL                                     (0xffffffe0)

/* TSI721_DMACXDPTRH : Register Bits Masks Definitions */
#define TSI721_DMACXDPTRH_DPTRH                                     (0xffffffff)

/* TSI721_DMACXDSBL : Register Bits Masks Definitions */
#define TSI721_DMACXDSBL_ADD                                        (0xffffffc0)

/* TSI721_DMACXDSBH : Register Bits Masks Definitions */
#define TSI721_DMACXDSBH_ADD                                        (0xffffffff)

/* TSI721_DMACXDSSZ : Register Bits Masks Definitions */
#define TSI721_DMACXDSSZ_SIZE                                       (0x0000000f)

/* TSI721_DMACXDSRP : Register Bits Masks Definitions */
#define TSI721_DMACXDSRP_RD_PTR                                     (0x0007ffff)

/* TSI721_DMACXDSWP : Register Bits Masks Definitions */
#define TSI721_DMACXDSWP_WR_PTR                                     (0x0007ffff)

/* TSI721_BDMA_INTE : Register Bits Masks Definitions */
#define TSI721_BDMA_INTE_ECC_CORR_CH_EN                             (0x000000ff)
#define TSI721_BDMA_INTE_ECC_UNCORR_CH_EN                           (0x0000ff00)
#define TSI721_BDMA_INTE_ECC_CORR_EN                                (0x00020000)
#define TSI721_BDMA_INTE_ECC_UNCORR_EN                              (0x00040000)

/* TSI721_BDMA_INT : Register Bits Masks Definitions */
#define TSI721_BDMA_INT_ECC_CORR_CH                                 (0x000000ff)
#define TSI721_BDMA_INT_ECC_UNCORR_CH                               (0x0000ff00)
#define TSI721_BDMA_INT_ECC_CORR                                    (0x00020000)
#define TSI721_BDMA_INT_ECC_UNCORR                                  (0x00040000)

/* TSI721_BDMA_INTSET : Register Bits Masks Definitions */
#define TSI721_BDMA_INTSET_ECC_CORR_CH_SET                          (0x000000ff)
#define TSI721_BDMA_INTSET_ECC_UNCORR_CH_SET                        (0x0000ff00)
#define TSI721_BDMA_INTSET_ECC_CORR_SET                             (0x00020000)
#define TSI721_BDMA_INTSET_ECC_UNCORR_SET                           (0x00040000)

/* TSI721_BDMA_ECC_LOG : Register Bits Masks Definitions */
#define TSI721_BDMA_ECC_LOG_ECC_CORR_MEM                            (0x000000ff)
#define TSI721_BDMA_ECC_LOG_ECC_UNCORR_MEM                          (0x00ff0000)

/* TSI721_BDMA_ECC_CORRXLOG : Register Bits Masks Definitions */
#define TSI721_BDMA_ECC_CORRXLOG_ECC_CORR_MEM                       (0x0001ffff)

/* TSI721_BDMA_ECC_UNCORRXLOG : Register Bits Masks Definitions */
#define TSI721_BDMA_ECC_UNCORRXLOG_ECC_UNCORR_MEM                   (0x0001ffff)


/******************************************************/
/* TSI721 : I2C Register address offset definitions   */
/******************************************************/

#define TSI721_I2C_DEVID                                          (0x00049100)
#define TSI721_I2C_RESET                                          (0x00049104)
#define TSI721_I2C_MST_CFG                                        (0x00049108)
#define TSI721_I2C_MST_CNTRL                                      (0x0004910c)
#define TSI721_I2C_MST_RDATA                                      (0x00049110)
#define TSI721_I2C_MST_TDATA                                      (0x00049114)
#define TSI721_I2C_ACC_STAT                                       (0x00049118)
#define TSI721_I2C_INT_STAT                                       (0x0004911c)
#define TSI721_I2C_INT_ENABLE                                     (0x00049120)
#define TSI721_I2C_INT_SET                                        (0x00049124)
#define TSI721_I2C_SLV_CFG                                        (0x0004912c)
#define TSI721_I2C_BOOT_CNTRL                                     (0x00049140)
#define TSI721_EXI2C_REG_WADDR                                    (0x00049200)
#define TSI721_EXI2C_REG_WDATA                                    (0x00049204)
#define TSI721_EXI2C_REG_RADDR                                    (0x00049210)
#define TSI721_EXI2C_REG_RDATA                                    (0x00049214)
#define TSI721_EXI2C_ACC_STAT                                     (0x00049220)
#define TSI721_EXI2C_ACC_CNTRL                                    (0x00049224)
#define TSI721_EXI2C_STAT                                         (0x00049280)
#define TSI721_EXI2C_STAT_ENABLE                                  (0x00049284)
#define TSI721_EXI2C_MBOX_OUT                                     (0x00049290)
#define TSI721_EXI2C_MBOX_IN                                      (0x00049294)
#define TSI721_I2C_EVENT                                          (0x00049300) 
#define TSI721_I2C_SNAP_EVENT                                     (0x00049304) 
#define TSI721_I2C_NEW_EVENT                                      (0x00049308)
#define TSI721_I2C_EVENT_ENB                                      (0x0004930c)
#define TSI721_I2C_DIVIDER                                        (0x00049320)
#define TSI721_I2C_START_SETUP_HOLD                               (0x00049340)
#define TSI721_I2C_STOP_IDLE                                      (0x00049344)
#define TSI721_I2C_SDA_SETUP_HOLD                                 (0x00049348)
#define TSI721_I2C_SCL_PERIOD                                     (0x0004934c)
#define TSI721_I2C_SCL_MIN_PERIOD                                 (0x00049350)
#define TSI721_I2C_SCL_ARB_TIMEOUT                                (0x00049354)
#define TSI721_I2C_BYTE_TRAN_TIMEOUT                              (0x00049358)
#define TSI721_I2C_BOOT_DIAG_TIMER                                (0x0004935c)
#define TSI721_I2C_BOOT_DIAG_PROGRESS                             (0x000493b8)
#define TSI721_I2C_BOOT_DIAG_CFG                                  (0x000493bc)


/*****************************************************/
/* TSI721 : I2C Register Bit Masks and Reset Values  */
/*           definitions for every register          */
/*****************************************************/

/* TSI721_I2C_DEVID : Register Bits Masks Definitions */
#define TSI721_I2C_DEVID_REV                                        (0x0000000f)

/* TSI721_I2C_RESET : Register Bits Masks Definitions */
#define TSI721_I2C_RESET_SRESET                                     (0x80000000)

/* TSI721_I2C_MST_CFG : Register Bits Masks Definitions */
#define TSI721_I2C_MST_CFG_DEV_ADDR                                 (0x0000007f)
#define TSI721_I2C_MST_CFG_PA_SIZE                                  (0x00030000)
#define TSI721_I2C_MST_CFG_DORDER                                   (0x00800000)

/* TSI721_I2C_MST_CNTRL : Register Bits Masks Definitions */
#define TSI721_I2C_MST_CNTRL_PADDR                                  (0x0000ffff)
#define TSI721_I2C_MST_CNTRL_SIZE                                   (0x07000000)
#define TSI721_I2C_MST_CNTRL_WRITE                                  (0x40000000)
#define TSI721_I2C_MST_CNTRL_START                                  (0x80000000)

/* TSI721_I2C_MST_RDATA : Register Bits Masks Definitions */
#define TSI721_I2C_MST_RDATA_RBYTE0                                 (0x000000ff)
#define TSI721_I2C_MST_RDATA_RBYTE1                                 (0x0000ff00)
#define TSI721_I2C_MST_RDATA_RBYTE2                                 (0x00ff0000)
#define TSI721_I2C_MST_RDATA_RBYTE3                                 (0xff000000)

/* TSI721_I2C_MST_TDATA : Register Bits Masks Definitions */
#define TSI721_I2C_MST_TDATA_TBYTE0                                 (0x000000ff)
#define TSI721_I2C_MST_TDATA_TBYTE1                                 (0x0000ff00)
#define TSI721_I2C_MST_TDATA_TBYTE2                                 (0x00ff0000)
#define TSI721_I2C_MST_TDATA_TBYTE3                                 (0xff000000)

/* TSI721_I2C_ACC_STAT : Register Bits Masks Definitions */
#define TSI721_I2C_ACC_STAT_MST_NBYTES                              (0x0000000f)
#define TSI721_I2C_ACC_STAT_MST_AN                                  (0x00000100)
#define TSI721_I2C_ACC_STAT_MST_PHASE                               (0x00000e00)
#define TSI721_I2C_ACC_STAT_MST_ACTIVE                              (0x00008000)
#define TSI721_I2C_ACC_STAT_SLV_PA                                  (0x00ff0000)
#define TSI721_I2C_ACC_STAT_SLV_AN                                  (0x01000000)
#define TSI721_I2C_ACC_STAT_SLV_PHASE                               (0x06000000)
#define TSI721_I2C_ACC_STAT_SLV_WAIT                                (0x08000000)
#define TSI721_I2C_ACC_STAT_BUS_ACTIVE                              (0x40000000)
#define TSI721_I2C_ACC_STAT_SLV_ACTIVE                              (0x80000000)

/* TSI721_I2C_INT_STAT : Register Bits Masks Definitions */
#define TSI721_I2C_INT_STAT_MA_OK                                   (0x00000001)
#define TSI721_I2C_INT_STAT_MA_ATMO                                 (0x00000002)
#define TSI721_I2C_INT_STAT_MA_NACK                                 (0x00000004)
#define TSI721_I2C_INT_STAT_MA_TMO                                  (0x00000008)
#define TSI721_I2C_INT_STAT_MA_COL                                  (0x00000010)
#define TSI721_I2C_INT_STAT_MA_DIAG                                 (0x00000080)
#define TSI721_I2C_INT_STAT_SA_OK                                   (0x00000100)
#define TSI721_I2C_INT_STAT_SA_READ                                 (0x00000200)
#define TSI721_I2C_INT_STAT_SA_WRITE                                (0x00000400)
#define TSI721_I2C_INT_STAT_SA_FAIL                                 (0x00000800)
#define TSI721_I2C_INT_STAT_BL_OK                                   (0x00010000)
#define TSI721_I2C_INT_STAT_BL_FAIL                                 (0x00020000)
#define TSI721_I2C_INT_STAT_IMB_FULL                                (0x01000000)
#define TSI721_I2C_INT_STAT_OMB_EMPTY                               (0x02000000)

/* TSI721_I2C_INT_ENABLE : Register Bits Masks Definitions */
#define TSI721_I2C_INT_ENABLE_MA_OK                                 (0x00000001)
#define TSI721_I2C_INT_ENABLE_MA_ATMO                               (0x00000002)
#define TSI721_I2C_INT_ENABLE_MA_NACK                               (0x00000004)
#define TSI721_I2C_INT_ENABLE_MA_TMO                                (0x00000008)
#define TSI721_I2C_INT_ENABLE_MA_COL                                (0x00000010)
#define TSI721_I2C_INT_ENABLE_MA_DIAG                               (0x00000080)
#define TSI721_I2C_INT_ENABLE_SA_OK                                 (0x00000100)
#define TSI721_I2C_INT_ENABLE_SA_READ                               (0x00000200)
#define TSI721_I2C_INT_ENABLE_SA_WRITE                              (0x00000400)
#define TSI721_I2C_INT_ENABLE_SA_FAIL                               (0x00000800)
#define TSI721_I2C_INT_ENABLE_BL_OK                                 (0x00010000)
#define TSI721_I2C_INT_ENABLE_BL_FAIL                               (0x00020000)
#define TSI721_I2C_INT_ENABLE_IMB_FULL                              (0x01000000)
#define TSI721_I2C_INT_ENABLE_OMB_EMPTY                             (0x02000000)

/* TSI721_I2C_INT_SET : Register Bits Masks Definitions */
#define TSI721_I2C_INT_SET_MA_OK                                    (0x00000001)
#define TSI721_I2C_INT_SET_MA_ATMO                                  (0x00000002)
#define TSI721_I2C_INT_SET_MA_NACK                                  (0x00000004)
#define TSI721_I2C_INT_SET_MA_TMO                                   (0x00000008)
#define TSI721_I2C_INT_SET_MA_COL                                   (0x00000010)
#define TSI721_I2C_INT_SET_MA_DIAG                                  (0x00000080)
#define TSI721_I2C_INT_SET_SA_OK                                    (0x00000100)
#define TSI721_I2C_INT_SET_SA_READ                                  (0x00000200)
#define TSI721_I2C_INT_SET_SA_WRITE                                 (0x00000400)
#define TSI721_I2C_INT_SET_SA_FAIL                                  (0x00000800)
#define TSI721_I2C_INT_SET_BL_OK                                    (0x00010000)
#define TSI721_I2C_INT_SET_BL_FAIL                                  (0x00020000)
#define TSI721_I2C_INT_SET_IMB_FULL                                 (0x01000000)
#define TSI721_I2C_INT_SET_OMB_EMPTY                                (0x02000000)

/* TSI721_I2C_SLV_CFG : Register Bits Masks Definitions */
#define TSI721_I2C_SLV_CFG_SLV_ADDR                                 (0x0000007f)
#define TSI721_I2C_SLV_CFG_SLV_UNLK                                 (0x01000000)
#define TSI721_I2C_SLV_CFG_SLV_EN                                   (0x10000000)
#define TSI721_I2C_SLV_CFG_ALRT_EN                                  (0x20000000)
#define TSI721_I2C_SLV_CFG_WR_EN                                    (0x40000000)
#define TSI721_I2C_SLV_CFG_RD_EN                                    (0x80000000)

/* TSI721_I2C_BOOT_CNTRL : Register Bits Masks Definitions */
#define TSI721_I2C_BOOT_CNTRL_PADDR                                 (0x00001fff)
#define TSI721_I2C_BOOT_CNTRL_PAGE_MODE                             (0x0000e000)
#define TSI721_I2C_BOOT_CNTRL_BOOT_ADDR                             (0x007f0000)
#define TSI721_I2C_BOOT_CNTRL_BUNLK                                 (0x10000000)
#define TSI721_I2C_BOOT_CNTRL_BINC                                  (0x20000000)
#define TSI721_I2C_BOOT_CNTRL_PSIZE                                 (0x40000000)
#define TSI721_I2C_BOOT_CNTRL_CHAIN                                 (0x80000000)

/* TSI721_EXI2C_REG_WADDR : Register Bits Masks Definitions */
#define TSI721_EXI2C_REG_WADDR_ADDR                                 (0xfffffffc)

/* TSI721_EXI2C_REG_WDATA : Register Bits Masks Definitions */
#define TSI721_EXI2C_REG_WDATA_WDATA                                (0xffffffff)

/* TSI721_EXI2C_REG_RADDR : Register Bits Masks Definitions */
#define TSI721_EXI2C_REG_RADDR_ADDR                                 (0xfffffffc)

/* TSI721_EXI2C_REG_RDATA : Register Bits Masks Definitions */
#define TSI721_EXI2C_REG_RDATA_RDATA                                (0xffffffff)

/* TSI721_EXI2C_ACC_STAT : Register Bits Masks Definitions */
#define TSI721_EXI2C_ACC_STAT_ALERT_FLAG                            (0x00000001)
#define TSI721_EXI2C_ACC_STAT_IMB_FLAG                              (0x00000004)
#define TSI721_EXI2C_ACC_STAT_OMB_FLAG                              (0x00000008)
#define TSI721_EXI2C_ACC_STAT_ACC_OK                                (0x00000080)

/* TSI721_EXI2C_ACC_CNTRL : Register Bits Masks Definitions */
#define TSI721_EXI2C_ACC_CNTRL_WINC                                 (0x00000004)
#define TSI721_EXI2C_ACC_CNTRL_RINC                                 (0x00000008)
#define TSI721_EXI2C_ACC_CNTRL_WSIZE                                (0x00000030)
#define TSI721_EXI2C_ACC_CNTRL_RSIZE                                (0x000000c0)

/* TSI721_EXI2C_STAT : Register Bits Masks Definitions */
#define TSI721_EXI2C_STAT_BDMA_NONCH                                (0x00000001)
#define TSI721_EXI2C_STAT_SR2PC_NONCH                               (0x00000002)
#define TSI721_EXI2C_STAT_PC2SR                                     (0x00000004)
#define TSI721_EXI2C_STAT_SMSG_NONCH                                (0x00000008)
#define TSI721_EXI2C_STAT_DL_DOWN                                   (0x00000010)
#define TSI721_EXI2C_STAT_SRIO_MAC                                  (0x00000020)
#define TSI721_EXI2C_STAT_ECC_UNCORR                                (0x00000040)
#define TSI721_EXI2C_STAT_I2C                                       (0x02000000)
#define TSI721_EXI2C_STAT_IMBR                                      (0x04000000)
#define TSI721_EXI2C_STAT_OMBW                                      (0x08000000)
#define TSI721_EXI2C_STAT_SW_STAT0                                  (0x10000000)
#define TSI721_EXI2C_STAT_SW_STAT1                                  (0x20000000)
#define TSI721_EXI2C_STAT_SW_STAT2                                  (0x40000000)
#define TSI721_EXI2C_STAT_RESET                                     (0x80000000)

/* TSI721_EXI2C_STAT_ENABLE : Register Bits Masks Definitions */
#define TSI721_EXI2C_STAT_ENABLE_BDMA_NONCH                         (0x00000001)
#define TSI721_EXI2C_STAT_ENABLE_SR2PC_NONCH                        (0x00000002)
#define TSI721_EXI2C_STAT_ENABLE_PC2SR                              (0x00000004)
#define TSI721_EXI2C_STAT_ENABLE_SMSG_NONCH                         (0x00000008)
#define TSI721_EXI2C_STAT_ENABLE_DL_DOWN                            (0x00000010)
#define TSI721_EXI2C_STAT_ENABLE_SRIO_MAC                           (0x00000020)
#define TSI721_EXI2C_STAT_ENABLE_ECC_UNCORR                         (0x00000040)
#define TSI721_EXI2C_STAT_ENABLE_I2C                                (0x02000000)
#define TSI721_EXI2C_STAT_ENABLE_IMBR                               (0x04000000)
#define TSI721_EXI2C_STAT_ENABLE_OMBW                               (0x08000000)
#define TSI721_EXI2C_STAT_ENABLE_SW_STAT0                           (0x10000000)
#define TSI721_EXI2C_STAT_ENABLE_SW_STAT1                           (0x20000000)
#define TSI721_EXI2C_STAT_ENABLE_SW_STAT2                           (0x40000000)
#define TSI721_EXI2C_STAT_ENABLE_RESET                              (0x80000000)

/* TSI721_EXI2C_MBOX_OUT : Register Bits Masks Definitions */
#define TSI721_EXI2C_MBOX_OUT_DATA                                  (0xffffffff)

/* TSI721_EXI2C_MBOX_IN : Register Bits Masks Definitions */
#define TSI721_EXI2C_MBOX_IN_DATA                                   (0xffffffff)

/* TSI721_I2C_X : Register Bits Masks Definitions */
#define TSI721_I2C_X_MARBTO                                         (0x00000001)
#define TSI721_I2C_X_MSCLTO                                         (0x00000002)
#define TSI721_I2C_X_MBTTO                                          (0x00000004)
#define TSI721_I2C_X_MTRTO                                          (0x00000008)
#define TSI721_I2C_X_MCOL                                           (0x00000010)
#define TSI721_I2C_X_MNACK                                          (0x00000020)
#define TSI721_I2C_X_BLOK                                           (0x00000100)
#define TSI721_I2C_X_BLNOD                                          (0x00000200)
#define TSI721_I2C_X_BLSZ                                           (0x00000400)
#define TSI721_I2C_X_BLERR                                          (0x00000800)
#define TSI721_I2C_X_BLTO                                           (0x00001000)
#define TSI721_I2C_X_MTD                                            (0x00004000)
#define TSI721_I2C_X_SSCLTO                                         (0x00020000)
#define TSI721_I2C_X_SBTTO                                          (0x00040000)
#define TSI721_I2C_X_STRTO                                          (0x00080000)
#define TSI721_I2C_X_SCOL                                           (0x00100000)
#define TSI721_I2C_X_OMBR                                           (0x00400000)
#define TSI721_I2C_X_IMBW                                           (0x00800000)
#define TSI721_I2C_X_DCMDD                                          (0x01000000)
#define TSI721_I2C_X_DHIST                                          (0x02000000)
#define TSI721_I2C_X_DTIMER                                         (0x04000000)
#define TSI721_I2C_X_SD                                             (0x10000000)
#define TSI721_I2C_X_SDR                                            (0x20000000)
#define TSI721_I2C_X_SDW                                            (0x40000000)

/* TSI721_I2C_NEW_EVENT : Register Bits Masks Definitions */
#define TSI721_I2C_NEW_EVENT_MARBTO                                 (0x00000001)
#define TSI721_I2C_NEW_EVENT_MSCLTO                                 (0x00000002)
#define TSI721_I2C_NEW_EVENT_MBTTO                                  (0x00000004)
#define TSI721_I2C_NEW_EVENT_MTRTO                                  (0x00000008)
#define TSI721_I2C_NEW_EVENT_MCOL                                   (0x00000010)
#define TSI721_I2C_NEW_EVENT_MNACK                                  (0x00000020)
#define TSI721_I2C_NEW_EVENT_BLOK                                   (0x00000100)
#define TSI721_I2C_NEW_EVENT_BLNOD                                  (0x00000200)
#define TSI721_I2C_NEW_EVENT_BLSZ                                   (0x00000400)
#define TSI721_I2C_NEW_EVENT_BLERR                                  (0x00000800)
#define TSI721_I2C_NEW_EVENT_BLTO                                   (0x00001000)
#define TSI721_I2C_NEW_EVENT_MTD                                    (0x00004000)
#define TSI721_I2C_NEW_EVENT_SSCLTO                                 (0x00020000)
#define TSI721_I2C_NEW_EVENT_SBTTO                                  (0x00040000)
#define TSI721_I2C_NEW_EVENT_STRTO                                  (0x00080000)
#define TSI721_I2C_NEW_EVENT_SCOL                                   (0x00100000)
#define TSI721_I2C_NEW_EVENT_OMBR                                   (0x00400000)
#define TSI721_I2C_NEW_EVENT_IMBW                                   (0x00800000)
#define TSI721_I2C_NEW_EVENT_DCMDD                                  (0x01000000)
#define TSI721_I2C_NEW_EVENT_DHIST                                  (0x02000000)
#define TSI721_I2C_NEW_EVENT_DTIMER                                 (0x04000000)
#define TSI721_I2C_NEW_EVENT_SD                                     (0x10000000)
#define TSI721_I2C_NEW_EVENT_SDR                                    (0x20000000)
#define TSI721_I2C_NEW_EVENT_SDW                                    (0x40000000)

/* TSI721_I2C_EVENT_ENB : Register Bits Masks Definitions */
#define TSI721_I2C_EVENT_ENB_MARBTO                                 (0x00000001)
#define TSI721_I2C_EVENT_ENB_MSCLTO                                 (0x00000002)
#define TSI721_I2C_EVENT_ENB_MBTTO                                  (0x00000004)
#define TSI721_I2C_EVENT_ENB_MTRTO                                  (0x00000008)
#define TSI721_I2C_EVENT_ENB_MCOL                                   (0x00000010)
#define TSI721_I2C_EVENT_ENB_MNACK                                  (0x00000020)
#define TSI721_I2C_EVENT_ENB_BLOK                                   (0x00000100)
#define TSI721_I2C_EVENT_ENB_BLNOD                                  (0x00000200)
#define TSI721_I2C_EVENT_ENB_BLSZ                                   (0x00000400)
#define TSI721_I2C_EVENT_ENB_BLERR                                  (0x00000800)
#define TSI721_I2C_EVENT_ENB_BLTO                                   (0x00001000)
#define TSI721_I2C_EVENT_ENB_MTD                                    (0x00004000)
#define TSI721_I2C_EVENT_ENB_SSCLTO                                 (0x00020000)
#define TSI721_I2C_EVENT_ENB_SBTTO                                  (0x00040000)
#define TSI721_I2C_EVENT_ENB_STRTO                                  (0x00080000)
#define TSI721_I2C_EVENT_ENB_SCOL                                   (0x00100000)
#define TSI721_I2C_EVENT_ENB_OMBR                                   (0x00400000)
#define TSI721_I2C_EVENT_ENB_IMBW                                   (0x00800000)
#define TSI721_I2C_EVENT_ENB_DCMDD                                  (0x01000000)
#define TSI721_I2C_EVENT_ENB_DHIST                                  (0x02000000)
#define TSI721_I2C_EVENT_ENB_DTIMER                                 (0x04000000)
#define TSI721_I2C_EVENT_ENB_SD                                     (0x10000000)
#define TSI721_I2C_EVENT_ENB_SDR                                    (0x20000000)
#define TSI721_I2C_EVENT_ENB_SDW                                    (0x40000000)

/* TSI721_I2C_DIVIDER : Register Bits Masks Definitions */
#define TSI721_I2C_DIVIDER_MSDIV                                    (0x00000fff)
#define TSI721_I2C_DIVIDER_USDIV                                    (0x0fff0000)

/* TSI721_I2C_START_SETUP_HOLD : Register Bits Masks Definitions */
#define TSI721_I2C_START_SETUP_HOLD_START_HOLD                      (0x0000ffff)
#define TSI721_I2C_START_SETUP_HOLD_START_SETUP                     (0xffff0000)

/* TSI721_I2C_STOP_IDLE : Register Bits Masks Definitions */
#define TSI721_I2C_STOP_IDLE_IDLE_DET                               (0x0000ffff)
#define TSI721_I2C_STOP_IDLE_STOP_SETUP                             (0xffff0000)

/* TSI721_I2C_SDA_SETUP_HOLD : Register Bits Masks Definitions */
#define TSI721_I2C_SDA_SETUP_HOLD_SDA_HOLD                          (0x0000ffff)
#define TSI721_I2C_SDA_SETUP_HOLD_SDA_SETUP                         (0xffff0000)

/* TSI721_I2C_SCL_PERIOD : Register Bits Masks Definitions */
#define TSI721_I2C_SCL_PERIOD_SCL_LOW                               (0x0000ffff)
#define TSI721_I2C_SCL_PERIOD_SCL_HIGH                              (0xffff0000)

/* TSI721_I2C_SCL_MIN_PERIOD : Register Bits Masks Definitions */
#define TSI721_I2C_SCL_MIN_PERIOD_SCL_MINL                          (0x0000ffff)
#define TSI721_I2C_SCL_MIN_PERIOD_SCL_MINH                          (0xffff0000)

/* TSI721_I2C_SCL_ARB_TIMEOUT : Register Bits Masks Definitions */
#define TSI721_I2C_SCL_ARB_TIMEOUT_ARB_TO                           (0x0000ffff)
#define TSI721_I2C_SCL_ARB_TIMEOUT_SCL_TO                           (0xffff0000)

/* TSI721_I2C_BYTE_TRAN_TIMEOUT : Register Bits Masks Definitions */
#define TSI721_I2C_BYTE_TRAN_TIMEOUT_TRAN_TO                        (0x0000ffff)
#define TSI721_I2C_BYTE_TRAN_TIMEOUT_BYTE_TO                        (0xffff0000)

/* TSI721_I2C_BOOT_DIAG_TIMER : Register Bits Masks Definitions */
#define TSI721_I2C_BOOT_DIAG_TIMER_COUNT                            (0x0000ffff)
#define TSI721_I2C_BOOT_DIAG_TIMER_FREERUN                          (0x80000000)

/* TSI721_I2C_BOOT_DIAG_PROGRESS : Register Bits Masks Definitions */
#define TSI721_I2C_BOOT_DIAG_PROGRESS_PADDR                         (0x0000ffff)
#define TSI721_I2C_BOOT_DIAG_PROGRESS_REGCNT                        (0xffff0000)

/* TSI721_I2C_BOOT_DIAG_CFG : Register Bits Masks Definitions */
#define TSI721_I2C_BOOT_DIAG_CFG_BOOT_ADDR                          (0x0000007f)
#define TSI721_I2C_BOOT_DIAG_CFG_PINC                               (0x10000000)
#define TSI721_I2C_BOOT_DIAG_CFG_PASIZE                             (0x20000000)
#define TSI721_I2C_BOOT_DIAG_CFG_BDIS                               (0x40000000)
#define TSI721_I2C_BOOT_DIAG_CFG_BOOTING                            (0x80000000)


/********************************************************/
/* TSI721 : GPIO Register address offset definitions    */
/********************************************************/

#define TSI721_GPIO0_DATA                                         (0x0004a000)
#define TSI721_GPIO0_CNTRL                                        (0x0004a004)

/********************************************************/
/* TSI721 : GPIO Register Bit Masks and Reset Values    */
/*           definitions for every register             */
/********************************************************/

/* TSI721_GPIO0_DATA : Register Bits Masks Definitions */
#define TSI721_GPIO0_DATA_GPIO_DATA_OUT                             (0x0000ffff)
#define TSI721_GPIO0_DATA_GPIO_DATA_IN                              (0xffff0000)

/* TSI721_GPIO0_CNTRL : Register Bits Masks Definitions */
#define TSI721_GPIO0_CNTRL_GPIO_CFG                                 (0x0000ffff)
#define TSI721_GPIO0_CNTRL_GPIO_DIR                                 (0xffff0000)


/*******************************************************/
/* TSI721 : SerDes Register address offset definitions */
/*******************************************************/

#define TSI721_PCIE_SERDES_BASE                                     (0x0004c000)
#define TSI721_SRIO_SERDES_BASE                                     (0x0004e000)

/* SerDes register offsets are from base offsets shown above */

#define TSI721_SERDES_LANEX_LANEn_DIG_TX_OVRD_IN(X)        (0x1000 + 0x400*(X))
#define TSI721_SERDES_LANEX_LANEn_DIG_RX_OVRD_IN(X)        (0x100c + 0x400*(X))

#define TSI721_SRIO_SERDES_LANEX_DIG_TX_OVRD_IN(x)  (TSI721_SRIO_SERDES_BASE + \
		                                     TSI721_SERDES_LANEX_LANEn_DIG_TX_OVRD_IN(x))
#define TSI721_SRIO_SERDES_LANEX_DIG_RX_OVRD_IN(x)  (TSI721_SRIO_SERDES_BASE + \
		                                     TSI721_SERDES_LANEX_LANEn_DIG_RX_OVRD_IN(x))

/*******************************************************/
/* TSI721 : SerDes Register Bit Masks and Reset Values */
/*           definitions for every register            */
/*******************************************************/

/* TSI721_SERDES_LANEX_LANEn_DIG_TX_OVRD_IN : Register Bits Masks Definitions */

#define TSI721_SERDES_LANEX_LANEn_DIG_TX_OVRD_IN_HALF_RATE          (0x00000001)
#define TSI721_SERDES_LANEX_LANEn_DIG_TX_OVRD_IN_LOOPBK_EN          (0x00000002)
#define TSI721_SERDES_LANEX_LANEn_DIG_TX_OVRD_IN_INVERT             (0x00000004)
#define TSI721_SERDES_LANEX_LANEn_DIG_TX_OVRD_IN_DATA_EN            (0x00000010)
#define TSI721_SERDES_LANEX_LANEn_DIG_TX_OVRD_IN_TX_EN              (0x00000020)
#define TSI721_SERDES_LANEX_LANEn_DIG_TX_OVRD_IN_CM_EN              (0x00000040)
#define TSI721_SERDES_LANEX_LANEn_DIG_TX_OVRD_IN_EN_L               (0x00000200)

/* TSI721_SERDES_LANEX_LANEn_DIG_RX_OVRD_IN : Register Bits Masks Definitions */
#define TSI721_SERDES_LANEX_LANEn_DIG_RX_OVRD_IN_INVERT             (0x00000001)
#define TSI721_SERDES_LANEX_LANEn_DIG_RX_OVRD_IN_PLL_EN             (0x00000004)
#define TSI721_SERDES_LANEX_LANEn_DIG_RX_OVRD_IN_DATA_EN            (0x00000008)
#define TSI721_SERDES_LANEX_LANEn_DIG_RX_OVRD_IN_ALIGN_EN           (0x00000010)
#define TSI721_SERDES_LANEX_LANEn_DIG_RX_OVRD_IN_TERM_EN            (0x00000040)
#define TSI721_SERDES_LANEX_LANEn_DIG_RX_OVRD_IN_EN                 (0x00000400)


/**********************************************************/
/* TSI721 : Top-level Register address offset definitions */
/**********************************************************/

#define TSI721_DEVSTAT                                             (0x00048000)
#define TSI721_DEVCTL                                              (0x00048004)
#define TSI721_CLK_GATE                                            (0x00048008)
#define TSI721_JTAG_ID                                             (0x0004800c)
#define TSI721_PC_TX_CTL                                           (0x00048200)
#define TSI721_PC_TX_CTL_2                                         (0x00048220)
#define TSI721_SR_TX_CTLX(X)                              (0x48800 + 0x020*(X))

/**********************************************************/
/* TSI721 : Top-level Register Bit Masks and Reset Values */
/*           definitions for every register               */
/**********************************************************/

/* TSI721_DEVSTAT : Register Bits Masks Definitions */
#define TSI721_DEVSTAT_PCRDY                                        (0x00000002)
#define TSI721_DEVSTAT_STRAP_RATE                                   (0x00000e00)
#define TSI721_DEVSTAT_CLKSEL                                       (0x00003000)
#define TSI721_DEVSTAT_I2C_DISABLE                                  (0x00010000)
#define TSI721_DEVSTAT_I2C_SA                                       (0x001e0000)
#define TSI721_DEVSTAT_I2C_SEL                                      (0x01000000)
#define TSI721_DEVSTAT_SR_BOOT                                      (0x02000000)
#define TSI721_DEVSTAT_I2C_MA                                       (0x04000000)
#define TSI721_DEVSTAT_CLKMOD                                       (0x08000000)
#define TSI721_DEVSTAT_SP_SWAP_RX                                   (0x10000000)
#define TSI721_DEVSTAT_SP_SWAP_TX                                   (0x20000000)
#define TSI721_DEVSTAT_SP_HOST                                      (0x40000000)
#define TSI721_DEVSTAT_SP_DEVID                                     (0x80000000)

/* TSI721_DEVCTL : Register Bits Masks Definitions */
#define TSI721_DEVCTL_FRST                                          (0x00000001)
#define TSI721_DEVCTL_PCBOOT_CMPL                                   (0x00000002)
#define TSI721_DEVCTL_SRBOOT_CMPL                                   (0x00000004)
#define TSI721_DEVCTL_MECS_O                                        (0x00000010)
#define TSI721_DEVCTL_SR_RST_MODE                                   (0x000f0000)

#define TSI721_DEVCTL_SR_RST_MODE_HOT_RST                           (0x00000000)
#define TSI721_DEVCTL_SR_RST_MODE_SRIO_ONLY                         (0x00010000)

/* TSI721_CLK_GATE : Register Bits Masks Definitions */
#define TSI721_CLK_GATE_CLK_GATE_BDMACH                             (0x000000ff)
#define TSI721_CLK_GATE_CLK_GATE_SMSGCH                             (0x0000ff00)

/* TSI721_JTAG_ID : Register Bits Masks Definitions */
#define TSI721_JTAG_ID_MANU_ID                                      (0x00000ffe)
#define TSI721_JTAG_ID_PART                                         (0x0ffff000)
#define TSI721_JTAG_ID_VERSION                                      (0xf0000000)

/* TSI721_PC_TX_CTL : Register Bits Masks Definitions */
#define TSI721_PC_TX_CTL_TX_AMP_FULL                                (0x0000003f)
#define TSI721_PC_TX_CTL_TX_COEF60_G2                               (0x00001f00)
#define TSI721_PC_TX_CTL_TX_COEF35_G2                               (0x001f0000)

/* TSI721_PC_TX_CTL_2 : Register Bits Masks Definitions */
#define TSI721_PC_TX_CTL_2_TX_AMP_LOW                               (0x0000003f)
#define TSI721_PC_TX_CTL_2_TX_COEF35_G1                             (0x001f0000)

/* TSI721_SR_TX_CTLX : Register Bits Masks Definitions */
#define TSI721_SR_TX_CTLX_TX_AMP                                    (0x0000003f)
#define TSI721_SR_TX_CTLX_TX_COEF                                   (0x00001f00)
#define TSI721_SR_TX_CTLX_LB_EN                                     (0x80000000)

/**********************************************************/
/* TSI721 : 5G Training Work Around Register definitions  */
/**********************************************************/

#define TSI721_5G_WA_REG0(X)          (0x0004F054+(0x400*X))
#define TSI721_5G_WA_REG1(X)          (0x0004F04C+(0x400*X))
#define TSI721_NUM_WA_REGS            4
#define TSI721_WA_VAL_5G              0x0000006F
#define TSI721_WA_VAL_3G              0x0000000F

/******************************************************/
/* TSI721 : MESSAGE DESCRIPTOR BIT DEFINITIONS        */
/******************************************************/

#ifndef RIO_MAX_MSG_SIZE
#define RIO_MAX_MSG_SIZE                (0x1000)
#endif
#define TSI721_MSG_BUFFER_SIZE          RIO_MAX_MSG_SIZE
#define TSI721_MSG_MAX_SIZE             RIO_MAX_MSG_SIZE
#define TSI721_IMSG_MAXCH               8
#define TSI721_IMSGD_MIN_RING_SIZE      32

#define TSI721_OMSGD_MIN_RING_SIZE      32

/******************************************************/
/* Inbound Message Descriptors                        */
/******************************************************/

#define TSI721_IMD_DEVID        0x0000ffff
#define TSI721_IMD_CRF          0x00010000
#define TSI721_IMD_PRIO         0x00060000
#define TSI721_IMD_TT           0x00180000
#define TSI721_IMD_DTYPE        0xe0000000
#define TSI721_IMD_BCOUNT       0x00000ff8
#define TSI721_IMD_SSIZE        0x0000f000
#define TSI721_IMD_LETER        0x00030000
#define TSI721_IMD_XMBOX        0x003c0000
#define TSI721_IMD_MBOX         0x00c00000
#define TSI721_IMD_CS           0x78000000
#define TSI721_IMD_HO           0x80000000

/******************************************************/
/* Outbound Message Descriptors                       */
/******************************************************/

#define TSI721_OMD_DEVID        0x0000ffff
#define TSI721_OMD_CRF          0x00010000
#define TSI721_OMD_PRIO         0x00060000
#define TSI721_OMD_IOF          0x08000000
#define TSI721_OMD_DTYPE        0xe0000000
#define TSI721_OMD_RSRVD        0x17f80000

#define TSI721_OMD_BCOUNT       0x00000ff8
#define TSI721_OMD_SSIZE        0x0000f000
#define TSI721_OMD_LETTER       0x00030000
#define TSI721_OMD_XMBOX        0x003c0000
#define TSI721_OMD_MBOX         0x00c00000
#define TSI721_OMD_TT           0x0c000000

#ifdef __cplusplus
}
#endif

#endif /* _IDT_TSI721_H_  */
