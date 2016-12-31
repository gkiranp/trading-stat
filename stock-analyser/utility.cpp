/*
 * utility.cpp
 *
 *  Created on: DEC, 2016
 *  Author: Kiran Puranik
 */

#include "global.h"
#include "utility.h"
#include "msg_format.h"

namespace conv
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

using namespace std;

std::string UInt2Str(uint64_t val) { return conv::to_string(val); }
std::string UInt2Str(uint32_t val) { return conv::to_string(val); }
std::string UInt2Str(uint16_t val) { return conv::to_string(val); }
std::string UInt2Str(uint8_t  val) { return conv::to_string(val); }
std::string C2Str(const char *cstr){std::string s(cstr) ; return s;}

bool exists (const char* name)
{
	ifstream f(name);
	return f.good();
}

uint8_t get_msg_type(const char *msg_header)
{
	uint8_t ret = INVALID_ARG;
	if(msg_header) {
		switch(msg_header[MSG_TYPE_LOC]) {
		case MSG_TYP_ORDR_ENTRY:
			return MSG_TYP_ORDR_ENTRY;
			break;

		case MSG_TYP_ORDR_ACK:
			return MSG_TYP_ORDR_ACK;
			break;

		case MSG_TYP_ORDR_FILL:
			return MSG_TYP_ORDR_FILL;
			break;
		}
	}
	return ret;
}

uint32_t get_highest_trader_id(const std::list<OrderFill> &ofPackets)
{
	uint32_t ordrID = 0;
	std::list<OrderFill>::const_iterator it = ofPackets.begin();
	uint32_t largest_number = (*it).m_of_no_of_contras;
	for(it = ofPackets.begin() ; it != ofPackets.end() ; it++) {
		if((*it).m_of_no_of_contras > largest_number) {
			largest_number = (*it).m_of_no_of_contras;
			ordrID = (*it).m_of_order_id;
		}
	}
	return ordrID;
}

uint64_t get_client_id_for_orderid(const std::list<OrderAck> &oaPacket, uint32_t order_id)
{
	uint64_t cId = 0;
	std::list<OrderAck>::const_iterator it = oaPacket.begin();
	for(it = oaPacket.begin() ; it != oaPacket.end() ; it++) {
		if(order_id == (*it).m_oa_order_id) {
			cId = (*it).m_oa_client_id;
		}
	}
	return cId;
}

uint64_t get_highest_gfd_volume_trader_id(const std::list<OrderEntry> &ofPackets)
{
	uint64_t cId = 0;
	std::list<OrderEntry>::const_iterator it = ofPackets.begin();
	uint32_t largest_qty = 0;
	for(it = ofPackets.begin() ; it != ofPackets.end() ; it++) {
		if((*it).m_oe_time_in_force == OE_TIF_TYP_GFD) {
			largest_qty = (*it).m_oe_qty; //found first
			break;
		}
	}
	for(it = ofPackets.begin() ; it != ofPackets.end() ; it++) {
		if((*it).m_oe_time_in_force == OE_TIF_TYP_GFD) {
			if((*it).m_oe_qty > largest_qty) {
				largest_qty = (*it).m_oe_qty;
				cId = (*it).m_oe_client_id;
			}
		}
	}

	return cId;
}
