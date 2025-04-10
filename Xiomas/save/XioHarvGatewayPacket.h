//:Header:568, "XioHarvGatewayPacket", 67cf1152
//
// File: XioHarvGatewayPacket.h
//
// Copyright (C) 2025 Xiomas Technologies, LLC, All rights reserved.
//
// Modification History:
// 10 Mar 2025, dbr -- Generated 12:20:34 by Genitor V4.50.621.2
//
//:---------------------------------------------------------------------------

#if !defined(XIOHARVGATEWAYPACKET_INCLUDED)
#define XIOHARVGATEWAYPACKET_INCLUDED

//:StdInc
#include "gtorsys.h"

//:Include
#include <glob.h>
#include <stdlib.h>
#include <stdint.h>
//
// #include <string.h>
//
#include <stdbool.h>

//:Custom
//
#ifndef EXTERN_C
# if __cplusplus
#  define EXTERN_C	extern "C"
# else
#  define EXTERN_C
# endif
#endif

#define XioHarPacketStart	"$PKT"		// start of packet value
#define XioHarMaxPayloadLength	(511 - sizeof(XioHarvardHeader))	// maximum size of packet (UDP) payload
//------------------------------------------
// Xiomas Harvard Packet header structure
//------------------------------------------
struct SXioHarvardHeader
{
	uint8_t	m_aStart[4];		// start flag = $PKT
	uint8_t m_uPacketID;		// type of packet (see enum EPacketIDs)
	uint8_t m_uSource;			// source of packet (see enum EPacketSources)
	uint8_t m_uLRC;				// Longitudinal redundancy check(LRC) compliment of sum of header bytes
	uint8_t m_uFlags;			// bitmap of packet flags (see enum EPacketFlags)
	uint8_t m_aPayloadLength[4];	// 32-bit length of payload (little endian)
	uint8_t m_aBlockCount[4];	// incrementing block count of packet channel
	//
	// Payload follows
};
typedef struct SXioHarvardHeader	XioHarvardHeader;

//------------------------------------------
// Xiomas Harvard Packet IDs
//------------------------------------------
enum EXHPacketIDs
{
	// 0-127 single block packets sent using UDP
	xhpidReserved = 0,		// reserved value do not use
	xhpidGWControl = 1,		// Gateway control message (ack, nak, pause, continue)
	xhpidXioCommand	= 2,	// Xiomas Command packet
	xhpidXioStatus	= 3,	// Xiomas Status packet
	// 4 - 127 reserved
	xhpidPacketLimit,		// Limit of valid packet ID values

	// 128-255 large stream messages sent using TCP
	xhpidFireLayer	= 0x81,	// fire layer imagery frame
	xipidMWBand		= 0x82,	// MW IR band imagery frame
	xipidLWBand		= 0x83,	// LW IR band imagery frame
	// 0x84 - 0xff reserved
	xhpidStreamLimit		// Limit of valid stream packet ID values
};
typedef enum EXHPacketIDs	EXHPacketIDs;

//------------------------------------------
// Xiomas Harvard Packet Sources
//------------------------------------------
enum EXHPacketSources
{
	// Onboard System ID's
	xhsrcReserved	= 0,		// reserved - do not use
	xhsrcGateway	= 1,		// Packet sent from gateway (control messages)
	xhsrcScanner	= 2,		// Xiomas Scanner (status messages)
	xhsrcOPU		= 3,		// Xiomas/Sidus OPU imagery messages
	// 4 - 7 reserved
	xhsrcOnboardLimit,			// limit of onboard systems

	// Ground Systems
	xhsrcHavardServer	= 8,	// Harvard ground stationg server
	xhsrcXiomasContol	= 9,	// Xiomas control system (command messages)
	// 10 - 15 reserved
	xhsrcRemoteLimit,			// limit of remote ground station systems
};
typedef enum EXHPacketSources	EXHPacketSources;

//------------------------------------------
// Xiomas Harvard Packet Flags
//
// bit values (can be or'd together to form bitmap
//------------------------------------------
enum EXHPacketFlags
{
	// flags sent with data packets
	xhfQoS		= 0x01,			// Quality of Service Flag
								// The Gateway attempts to guaranty delivery of the packet.
	// flags sent with control packets
	xhfNCTS		= 0x10,			// Not CTS -- Hold off sending on this socket
	xhfCTS		= 0x20,			// CTS -- Enable sending on this socket
	xhfRTS		= 0x40,			// RTS -- Request to Send query

	// oll other bits are reserved and are set to zero
};
typedef enum EXHPacketFlags	EXHPacketFlags;
#define xhfValidFlags	(xhfQoS | xhfNCTS | xhfCTS | xhfRTS)	// bitmap of valid flags

//------------------------------------------
// Xiomas Harvard Pacuint8_ket (header + payload)
//------------------------------------------
struct SXioHarvardPacket
{
	XioHarvardHeader	m_Header;		// packet header
	uint8_t				m_aPayload[XioHarMaxPayloadLength];		// packet payload data
};
typedef struct SXioHarvardPacket	XioHarvardPacket;
//:End Custom

//:>                  +------------------------------------+
//:>------------------|    Global Function Declarations    |------------------
//:>                  +------------------------------------+

//::2
// +---------------------+
// |  xhgCalculateLRC()  |
// +---------------------+

EXTERN_C uint8_t xhgCalculateLRC(
	XioHarvardHeader* pHeader);			// header structure to fill

//::1
// +-------------------+
// |  xhgFillHeader()  |
// +-------------------+

EXTERN_C int xhgFillHeader(
	XioHarvardHeader* pHeader,			// header structure to fill
	EXHPacketIDs ePID,					// Packet ID
	EXHPacketSources eSrc,				// Packet Source
	uint8_t uFlags,						// bitmap of packet flags
	uint32_t uPayloadLength,			// Payload length
	uint32_t uBlockCount);				// packet block count

//::3
// +----------------------+
// |  xhgGetBlockCount()  |
// +----------------------+

EXTERN_C uint32_t xhgGetBlockCount(
	XioHarvardHeader* pHeader);			// header structure to fill

//::4
// +------------------------+
// |  xhgGetPacketLength()  |
// +------------------------+

EXTERN_C uint32_t xhgGetPacketLength(
	XioHarvardHeader* pHeader);			// header structure to fill

//::5
// +-------------------------+
// |  xhgGetPayloadLength()  |
// +-------------------------+

EXTERN_C uint32_t xhgGetPayloadLength(
	XioHarvardHeader* pHeader);			// header structure to fill

//::9
// +-------------------+
// |  xhgGetValue32()  |
// +-------------------+

EXTERN_C uint32_t xhgGetValue32(
	uint8_t* aValueLE);					// littleendian value in 4 byte array field

//::6
// +----------------------+
// |  xhgSetBlockCount()  |
// +----------------------+

EXTERN_C void xhgSetBlockCount(
	XioHarvardHeader* pHeader,			// header structure to fill
	uint32_t uCount);					// block count

//::7
// +-------------------------+
// |  xhgSetPayloadLength()  |
// +-------------------------+

EXTERN_C void xhgSetPayloadLength(
	XioHarvardHeader* pHeader,			// header structure to fill
	uint32_t uLength);					// payload length

//::10
// +-------------------+
// |  xhgSetValue32()  |
// +-------------------+

EXTERN_C void xhgSetValue32(
	uint8_t* aValueLE,					// littleendian value in 4 byte array field
	uint32_t uValue);					// value to set in local enadianness

//::11
// +-------------------+
// |  xhgShowHeader()  |
// +-------------------+

EXTERN_C void xhgShowHeader(
	XioHarvardHeader* pHeader);			// header structure to fill

//::8
// +-----------------------+
// |  xhgValidateHeader()  |
// +-----------------------+

EXTERN_C bool xhgValidateHeader(
	XioHarvardHeader* pHeader,			// header structure to fill
	uint32_t uPacketLength);			// total length of packet received
#endif  								// XIOHARVGATEWAYPACKET_INCLUDED

