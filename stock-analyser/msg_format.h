/*
 * msg_format.h
 *
 *  Created on: DEC 2016
 *  Author: Kiran Puranik (gkiranp@gmail.com)
 */

#ifndef MSG_FORMAT_H
#define MSG_FORMAT_H

using namespace std;

/* MSG Types */
#define MSG_TYP_ORDR_ENTRY 		1
#define MSG_TYP_ORDR_ACK 		2
#define MSG_TYP_ORDR_FILL 		3

/* MSG DIRECTION */
#define MSG_DIR_TO_XCHANGE 		0
#define MSG_DIR_FROM_XCHANGE 	1

/* Order Entry Message Sides */
#define OE_SIDE_BUY 			1
#define OE_SIDE_SELL			2

/* Order Entry Message Time In Force */
#define OE_TIF_TYP_IOC 			1
#define OE_TIF_TYP_GFD 			2

/* Order Ack Message Status */
#define OA_STS_TYP_GOOD 		1
#define OA_STS_TYP_REJECT 		2

/* Order Ack Message Reject Codes */
#define OA_RC_TYP_NONE 				0
#define OA_RC_TYP_INVALID_PRODUCT 	1
#define OA_RC_TYP_INVALID_PRICE 	2
#define OA_RC_TYP_INVALID_QTY 		3

#pragma pack(1)
/* MSG Header */
struct Header {
	uint16_t 	m_marker;
	uint8_t 	m_msg_type;
	uint64_t 	m_seq_id;
	uint64_t 	m_timestamp;
	uint8_t 	m_msg_direction;
	uint16_t 	m_msg_len;
	
	/* default CTR */
	Header();

	/* CTR */
	Header(		uint16_t 	marker,
				uint8_t 	msg_type,
				uint64_t 	seq_id,
				uint64_t 	timestamp,
				uint8_t 	msg_direction,
				uint16_t 	msg_len
		  );
	
	/* API converts members into std::string */
	const std::string to_str() const;
};

/* Order Entry Message */
struct OrderEntry {
	Header 		m_oe_header;
	uint64_t 	m_oe_price;
	uint32_t 	m_oe_qty;
	char 		m_oe_instrument[10];
	uint8_t 	m_oe_side;
	uint64_t 	m_oe_client_id;
	uint8_t 	m_oe_time_in_force;
	char 		m_oe_trader_tag[3];
	uint8_t 	m_oe_firm_id;
	std::string m_oe_firm;
	
	/* CTR */
	OrderEntry();
	OrderEntry(		Header 		oe_header,
					uint64_t 	oe_price,
					uint32_t 	oe_qty,
					char 		*oe_instrument,
					uint8_t 	oe_side,
					uint64_t 	oe_client_id,
					uint8_t 	oe_time_in_force,
					char 		*oe_trader_tag,
					uint8_t 	oe_firm_id,
					std::string oe_firm
			  );
	
	/* API converts members into std::string */
	const std::string to_str() const;
};

/* Order Acknowledgment Messages */
struct OrderAck {
	Header 		m_oa_header;
	uint32_t 	m_oa_order_id;
	uint64_t 	m_oa_client_id;
	uint8_t 	m_oa_order_status;
	uint8_t 	m_oa_reject_code;
	
	/* CTR */
	OrderAck();
	OrderAck(	Header 		oa_header,
				uint32_t 	oa_order_id,
				uint64_t 	oa_client_id,
				uint8_t 	oa_order_status,
				uint8_t 	oa_reject_code
			);
			
	/* API converts members into std::string */
	const std::string to_str() const;
};

/* repeating counter party groups */
struct OrderFill_ContrasGrp {
	uint8_t 	m_ofcg_firm_id;
	char 		m_ofcg_trader_tag[3];
	uint32_t 	m_ofcg_qty;
	
	/* default CTR */
	OrderFill_ContrasGrp();
	/* CTR */
	OrderFill_ContrasGrp(	uint8_t 	ofcg_firm_id,
							char 		*ofcg_trader_tag,
							uint32_t 	ofcg_qty
						);
	/* DTR */
	~OrderFill_ContrasGrp();

	/* API converts members into std::string */
	const std::string to_str(bool final_delim = false) const;
};
typedef OrderFill_ContrasGrp* OF_ContrasGrp_ptr;
typedef std::vector<OrderFill_ContrasGrp> vectOF_ContrasGrp;

/* Order Fill Messages */
struct OrderFill {
	Header 		m_of_header;
	uint32_t 	m_of_order_id;
	uint64_t 	m_of_fill_price;
	uint32_t 	m_of_fill_qty;
	uint8_t 	m_of_no_of_contras;
	vectOF_ContrasGrp m_of_cgrps;
	
	/* CTR */
	OrderFill();
	OrderFill(	Header 		of_header,
				uint32_t 	of_order_id,
				uint64_t 	of_fill_price,
				uint32_t 	of_fill_qty,
				uint8_t 	of_no_of_contras,
				OF_ContrasGrp_ptr of_cgrps
			);

	/* Another CTR */
	OrderFill(	Header 		of_header,
				uint32_t 	of_order_id,
				uint64_t 	of_fill_price,
				uint32_t 	of_fill_qty,
				uint8_t 	of_no_of_contras,
				vectOF_ContrasGrp of_cgrps
			  );

	/* DTR */
	~OrderFill();

	/* API converts members into std::string */
	const std::string to_str() const;

	/* API to get length of Contra trader */
	uint32_t get_contra_length(void);
};
#pragma pop(1)

#endif
