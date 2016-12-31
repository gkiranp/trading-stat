/*
 * msg_format.cpp
 *
 *  Created on: DEC 2016
 *  Author: Kiran Puranik (gkiranp@gmail.com)
 */

#include "global.h"
#include "msg_format.h"
#include "utility.h"

using namespace std;

Header::Header()
{}

Header::Header(	uint16_t marker, uint8_t msg_type, uint64_t seq_id,
		uint64_t timestamp,	uint8_t msg_direction, uint16_t msg_len ) 
{
	m_marker 		= marker;
	m_msg_type 		= msg_type;
	m_seq_id 		= seq_id;
	m_timestamp 	= timestamp;
	m_msg_direction = msg_direction;
	m_msg_len 		= msg_len;
}

const std::string Header::to_str () const 
{
    std::stringstream s;
    s << UInt2Str(m_marker) << "," << UInt2Str(m_msg_type) << "," << UInt2Str(m_seq_id) << ",";
    s << UInt2Str(m_timestamp) << "," << UInt2Str(m_msg_direction) << "," << UInt2Str(m_msg_len);
    return s.str();
}

OrderEntry::OrderEntry()
{}

OrderEntry::OrderEntry( Header oe_header, uint64_t oe_price, uint32_t oe_qty, char *oe_instrument, uint8_t oe_side,
						uint64_t oe_client_id,	uint8_t oe_time_in_force, char *oe_trader_tag, uint8_t oe_firm_id, std::string oe_firm )
{
	m_oe_header 		= oe_header;
	m_oe_price 			= oe_price;
	m_oe_qty 			= oe_qty;
	if(oe_instrument) {
		strncpy(m_oe_instrument, oe_instrument, 10);
	}
	m_oe_side 			= oe_side;
	m_oe_client_id 		= oe_client_id;
	m_oe_time_in_force 	= oe_time_in_force;
	if(oe_trader_tag) {
		strncpy(m_oe_trader_tag, oe_trader_tag, 3);
	}
	m_oe_firm_id 		= oe_firm_id;
	m_oe_firm 			= oe_firm;
}

const std::string OrderEntry::to_str () const 
{
	std::stringstream s;
    s << m_oe_header.to_str() << "," << UInt2Str(m_oe_price) << "," << UInt2Str(m_oe_qty) << ",";
    s << C2Str(m_oe_instrument) << "," << UInt2Str(m_oe_side) << "," << UInt2Str(m_oe_client_id);
	s << UInt2Str(m_oe_time_in_force) << "," << C2Str(m_oe_trader_tag) << "," << UInt2Str(m_oe_firm_id) << ",";
	s << m_oe_firm;
    return s.str();
}

OrderAck::OrderAck()
{}

OrderAck::OrderAck(	Header oa_header, uint32_t oa_order_id, uint64_t oa_client_id,
					uint8_t oa_order_status, uint8_t oa_reject_code)
{
	m_oa_header 		= oa_header ;
	m_oa_order_id 		= oa_order_id;
	m_oa_client_id 		= oa_client_id;
	m_oa_order_status 	= oa_order_status;
	m_oa_reject_code 	= oa_reject_code;
}

const std::string OrderAck::to_str () const
{
	std::stringstream s;
	s << m_oa_header.to_str() << "," << UInt2Str(m_oa_order_id) << "," << UInt2Str(m_oa_client_id) << ",";
	s << UInt2Str(m_oa_order_status) << "," << UInt2Str(m_oa_reject_code);
	return s.str();
}

OrderFill_ContrasGrp::OrderFill_ContrasGrp()
{}

OrderFill_ContrasGrp::OrderFill_ContrasGrp( uint8_t ofcg_firm_id,
							char *ofcg_trader_tag, uint32_t ofcg_qty)
{
	m_ofcg_firm_id		 = ofcg_firm_id;
	if(ofcg_trader_tag) {
		strncpy(m_ofcg_trader_tag, ofcg_trader_tag, 3);
	}
	m_ofcg_qty 			 = ofcg_qty;
}

OrderFill_ContrasGrp::~OrderFill_ContrasGrp()
{}

const std::string OrderFill_ContrasGrp::to_str (bool final_delim) const
{
	std::stringstream s;
	s << UInt2Str(m_ofcg_firm_id) << "," << C2Str(m_ofcg_trader_tag) << ",";
	s << UInt2Str(m_ofcg_qty);
	if(final_delim) {
		s << ",";
	}
	return s.str();
}

OrderFill::OrderFill()
{}

OrderFill::OrderFill( Header of_header, uint32_t of_order_id, uint64_t of_fill_price,
					  uint32_t of_fill_qty, uint8_t of_no_of_contras, OF_ContrasGrp_ptr of_cgrps)
{
	m_of_header 		= of_header;
	m_of_order_id 		= of_order_id;
	m_of_fill_price 	= of_fill_price;
	m_of_fill_qty 		= of_fill_qty;
	m_of_no_of_contras 	= of_no_of_contras;

	OrderFill_ContrasGrp *cg = new OrderFill_ContrasGrp();
	OF_ContrasGrp_ptr 	 cgPtr;
	cgPtr = (OF_ContrasGrp_ptr)of_cgrps;
	for(uint8_t i = 0; i < of_no_of_contras; i++) {
		cg->m_ofcg_firm_id = cgPtr->m_ofcg_firm_id;
		strncpy(cg->m_ofcg_trader_tag, cgPtr->m_ofcg_trader_tag, 3);
		cg->m_ofcg_qty = cgPtr->m_ofcg_qty;
		m_of_cgrps.push_back(*cg);
		cgPtr++;
	}
}

OrderFill::OrderFill( Header of_header, uint32_t of_order_id, uint64_t of_fill_price,
					  uint32_t of_fill_qty, uint8_t of_no_of_contras, vectOF_ContrasGrp of_cgrps)
{
	m_of_header 		= of_header;
	m_of_order_id 		= of_order_id;
	m_of_fill_price 	= of_fill_price;
	m_of_fill_qty 		= of_fill_qty;
	m_of_no_of_contras 	= of_no_of_contras;
	m_of_cgrps 			= of_cgrps;
}

OrderFill::~OrderFill()
{}

const std::string OrderFill::to_str () const
{
	std::stringstream s;
	s << m_of_header.to_str() << "," << UInt2Str(m_of_order_id) << "," << UInt2Str(m_of_fill_price) << ",";
	s << UInt2Str(m_of_fill_qty) << "," << UInt2Str(m_of_no_of_contras) << ",";

	for( vectOF_ContrasGrp::const_iterator it = m_of_cgrps.begin();
		 it != std::prev(m_of_cgrps.end()); ++it ){
		s << (*it).to_str(true);
	}
	/* I don't need a comma delimiter for last element !*/
	vectOF_ContrasGrp::const_iterator it = std::prev(m_of_cgrps.end());
	s << (*it).to_str(false);
	return s.str();
}

uint32_t OrderFill::get_contra_length(void)
{
	return (m_of_cgrps.size());
}

