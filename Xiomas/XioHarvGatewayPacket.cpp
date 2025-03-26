//:Source:568, "XioHarvGatewayPacket", 67cf1152
//
// File: XioHarvGatewayPacket.cpp
//
// Copyright (C) 2025 Xiomas Technologies, LLC, All rights reserved.
//
// Modification History:
// 10 Mar 2025, dbr -- Generated 12:20:34 by Genitor V4.50.621.2
//
//:---------------------------------------------------------------------------
//:Description
//
// This a C coded module. All functions are extern "C" defined
//
// These module handle the setup, access and validation of message headers used
// in communication with the Harvard Communications Gateway. The protocol is
// defined in the document "Xiomas Scanner / Harvard Gateway Communications Packet
// Protocol"
//
// All functions in this module are prefixed with "xhg"
//
//:---------------------------------------------------------------------------

//:ClassInc
#include "XioHarvGatewayPacket.h"

//:Include
#include <stdio.h>

//:Custom
//

#if 1	// enable debug output
#define DEBUG(fmt, ...)	printf(fmt, ##__VA_ARGS__)
#else
#define DEBUG(...)
#endif
//:End Custom

//:>                  +-----------------------------------+
//:>------------------|    Global Function Definitions    |-------------------
//:>                  +-----------------------------------+

//::2
// +---------------------+
// |  xhgCalculateLRC()  |
// +---------------------+

//:Description
//
// Calculate the LRC of a header
//
// The LRC is the complement of the modulo 256 sum of the bytes of the passed
// header.
//
// If filling the header the LRC field is zero when passed. The return LRC value
// is used to fill the field.
//
// If validating the header the passed LRC field is passed unchanged. The return
// RC value is zero if the header LRC is valid.
//
uint8_t xhgCalculateLRC(
	XioHarvardHeader* pHeader)			// header structure to fill
{
	uint8_t* pBuf = (uint8_t*)pHeader;
	uint32_t uIndex;
	uint8_t uLRC = 0;

	for(uIndex = 0; uIndex < sizeof(XioHarvardHeader); ++uIndex)
	{
	    uLRC += pBuf[uIndex];
	}
	return(~uLRC);
}

//::1
// +-------------------+
// |  xhgFillHeader()  |
// +-------------------+

//:Description
//
// Fill in the header with the passed values and set the LRC field
//
int xhgFillHeader(
	XioHarvardHeader* pHeader,			// header structure to fill
	EXHPacketIDs ePID,					// Packet ID
	EXHPacketSources eSrc,				// Packet Source
	uint8_t uFlags,						// bitmap of packet flags
	uint32_t uPayloadLength,			// Payload length
	uint32_t uBlockCount)				// packet block count
{
	memcpy(pHeader->m_aStart, XioHarPacketStart, 4);		// $PKT
	pHeader->m_uPacketID = ePID;		// type of packet (see enum EPacketIDs)
	pHeader->m_uSource = eSrc;			// source of packet (see enum EPacketSources)
	pHeader->m_uLRC = 0;				// Longitudinal redundancy check(LRC) compliment of sum of header bytes
	pHeader->m_uFlags = uFlags;			// bitmap of packet flags (see enum EPacketFlags)
	xhgSetValue32(pHeader->m_aPayloadLength, uPayloadLength);		// 32-bit length of payload (little endian)
	xhgSetValue32(pHeader->m_aBlockCount, uBlockCount);			// incrementing block count of packet channel

	pHeader->m_uLRC = 0;
	pHeader->m_uLRC = xhgCalculateLRC(pHeader);
	return(xhgGetPacketLength(pHeader));
}

//::3
// +----------------------+
// |  xhgGetBlockCount()  |
// +----------------------+

//:Description
//
// Get the Block Count field
//
uint32_t xhgGetBlockCount(
	XioHarvardHeader* pHeader)			// header structure to fill
{
	return(xhgGetValue32(pHeader->m_aBlockCount));
}

//::4
// +------------------------+
// |  xhgGetPacketLength()  |
// +------------------------+

//:Description
//
// Get the total length of a packet (header_length + payload_length)
//
uint32_t xhgGetPacketLength(
	XioHarvardHeader* pHeader)			// header structure to fill
{
	return(sizeof(XioHarvardHeader) + xhgGetPayloadLength(pHeader));
}

//::5
// +-------------------------+
// |  xhgGetPayloadLength()  |
// +-------------------------+

//:Description
//
// Get the length of a packet's payload
//
uint32_t xhgGetPayloadLength(
	XioHarvardHeader* pHeader)			// header structure to fill
{
	return(xhgGetValue32(pHeader->m_aPayloadLength));
}

//::9
// +-------------------+
// |  xhgGetValue32()  |
// +-------------------+

//:Description
//
// Get a 32bit value in local endianness from a 4byte littleendian field
//
uint32_t xhgGetValue32(
	uint8_t* aValueLE)					// littleendian value in 4 byte array field
{
	uint32_t uValue = 0;

	// extract littleendian data from field and create local endian 32bit value
	uValue  = ((uint32_t)aValueLE[0]) << 0;
	uValue |= ((uint32_t)aValueLE[1]) << 8;
	uValue |= ((uint32_t)aValueLE[2]) << 16;
	uValue |= ((uint32_t)aValueLE[3]) << 24;
	return(uValue);
}

//::6
// +----------------------+
// |  xhgSetBlockCount()  |
// +----------------------+

//:Description
//
// Set the Block Count field
//
void xhgSetBlockCount(
	XioHarvardHeader* pHeader,			// header structure to fill
	uint32_t uCount)					// block count
{
	xhgSetValue32(pHeader->m_aBlockCount, uCount);
}

//::7
// +-------------------------+
// |  xhgSetPayloadLength()  |
// +-------------------------+

//:Description
//
// Set the Payload Length field
//
void xhgSetPayloadLength(
	XioHarvardHeader* pHeader,			// header structure to fill
	uint32_t uLength)					// payload length
{
	xhgSetValue32(pHeader->m_aPayloadLength, uLength);
}

//::10
// +-------------------+
// |  xhgSetValue32()  |
// +-------------------+

//:Description
//
// Set a 32bit value in local endianness to a 4byte littleendian array
//
void xhgSetValue32(
	uint8_t* aValueLE,					// littleendian value in 4 byte array field
	uint32_t uValue)					// value to set in local enadianness
{
	aValueLE[0] = (uint8_t)((uValue >> 0) & 0xff);
	aValueLE[1] = (uint8_t)((uValue >> 8) & 0xff);
	aValueLE[2] = (uint8_t)((uValue >> 16) & 0xff);
	aValueLE[3] = (uint8_t)((uValue >> 24) & 0xff);
}

//::11
// +-------------------+
// |  xhgShowHeader()  |
// +-------------------+

//:Description
//
// Print the header field values
//
void xhgShowHeader(
	XioHarvardHeader* pHeader)			// header structure to fill
{
	printf("XioHarvard Header: %0.4s pid=%u src=%u lrc=%02x flags=%02x plen=%u blk=%u\n",
	       pHeader->m_aStart, pHeader->m_uPacketID, pHeader->m_uSource, pHeader->m_uLRC, pHeader->m_uFlags,
	       xhgGetPayloadLength(pHeader), xhgGetBlockCount(pHeader));
}

//::8
// +-----------------------+
// |  xhgValidateHeader()  |
// +-----------------------+

//:Description
//
// Validate the header fields. Checks that the PacketID and Source are valid values,
// the LRC and payload length are correct.
//
bool xhgValidateHeader(
	XioHarvardHeader* pHeader,			// header structure to fill
	uint32_t uPacketLength)				// total length of packet received
{
	bool bValid = true;
	uint8_t uCalcLRC = xhgCalculateLRC(pHeader);	// calculate LRC for header

	if(memcmp(pHeader->m_aStart, XioHarPacketStart, 4) != 0)
	{	// start mismatch
	    DEBUG("Packet Start mismatch; %02x:%02x:%02x;%02x\n",
	    	  pHeader->m_aStart[0], pHeader->m_aStart[1], pHeader->m_aStart[2], pHeader->m_aStart[3]);
	    bValid = false;
	}

	if((pHeader->m_uPacketID >= xhpidPacketLimit && pHeader->m_uPacketID < xhpidFireLayer)
	   || pHeader->m_uPacketID >= xhpidStreamLimit)
	{
	    DEBUG("Packet PID Invalid = %02x\n", pHeader->m_uPacketID);
	    bValid = false;
	}

	if((pHeader->m_uSource >= xhsrcOnboardLimit && pHeader->m_uSource < xhsrcHavardServer) || pHeader->m_uSource >= xhsrcRemoteLimit || pHeader->m_uSource == xhsrcRemoteLimit)
	{
	    DEBUG("Packet Source Invalid = %02x\n", pHeader->m_uSource);
	    bValid = false;
	}

	if(uCalcLRC != 0)
	{
	    DEBUG("Packet LRC Invalid: expect=%02x calc=%02x\n", pHeader->m_uLRC, uCalcLRC);
	    bValid = false;
	}

	if(pHeader->m_uFlags & ~xhfValidFlags)
	{
	    DEBUG("Packet Invalid Flag(s) set: flags=%02x\n", pHeader->m_uFlags);
	    bValid = false;
	}

	if((pHeader->m_uPacketID <= xhpidPacketLimit) && (xhgGetPayloadLength(pHeader) > XioHarMaxPayloadLength))
	{
	    DEBUG("Packet Payload too large = %u\n", xhgGetPayloadLength(pHeader));
	    bValid = false;
	}
	if(xhgGetPacketLength(pHeader) != uPacketLength)
	{
	    DEBUG("Packet Packet Length error: expected=%u header=%u\n", uPacketLength, xhgGetPacketLength(pHeader));
	    bValid = false;
	}
	return(bValid);
}
