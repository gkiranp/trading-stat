/*
 * utility.h
 *
 *  Created on: DEC, 2016
 *  Author: Kiran Puranik
 */

#include "msg_format.h"


#ifndef UTILITY_H_
#define UTILITY_H_

using namespace std;

std::string UInt2Str(uint64_t val);
std::string UInt2Str(uint32_t val);
std::string UInt2Str(uint16_t val);
std::string UInt2Str(uint8_t  val);
std::string C2Str(const char *cstr);

/* Check whether file exists */
bool exists (const char* name);

/* Msg type can be determined by msg_type byte in header */
#define MSG_TYPE_LOC 	2
uint8_t get_msg_type(const char *msg_header);

/* Find highest trader ID */
uint32_t get_highest_trader_id(const std::list<OrderFill> &ofPackets);
uint64_t get_client_id_for_orderid(const std::list<OrderAck> &oaPacket, uint32_t order_id);
uint64_t get_highest_gfd_volume_trader_id(const std::list<OrderEntry> &ofPackets);

#endif /* UTILITY_H_ */
