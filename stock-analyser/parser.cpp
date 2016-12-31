/*
 * parser.cpp
 *
 *  Created on: DEC, 2016
 *  Author: Kiran Puranik
 */

#include "global.h"
#include "utility.h"
#include "parser.h"
#include "msg_format.h"

using namespace std;

/* Parser class Definition */
Parser::Parser(const char* file_name, const char* term_str)
{
	m_file_name.clear();
	m_init_status = false;
	m_term_seq.clear();
	if((file_name)&&(term_str)) {
		m_file_name = C2Str(file_name);
		m_term_seq = C2Str(term_str);
		if(Init_Parser(m_file_name, m_term_seq) == eStatus::SUCCESS) {
			m_init_status = true;
		}
	}
}

Parser::~Parser()
{
	if(m_msg_format)
		delete m_msg_format;
	m_msg_format = NULL;
}

std::vector<uint32_t> Parser::search_pattern_index(const char *msg, uint32_t msg_len, const char *pat, uint32_t pat_len)
{
	std::vector<uint32_t> pat_index;
	if((msg) && (msg_len != 0) && (pat) && (pat_len != 0)) {
		/* A loop to move through pattern */
		for (int i = 0; i <= (msg_len - pat_len); i++) {
			int j = 0;
			/* For current i checking pattern match */
			for (j = 0; j < (pat_len); j++)
				if (msg[i+j] != pat[j])
					break;

			if (j == pat_len)
				pat_index.push_back(i);
		}
	}
	return pat_index;
}

eStatus Parser::check_file_index_positions(const std::vector<uint32_t> &index, uint32_t chk_size)
{
	eStatus ret = eStatus::FAILED;
	if((!index.empty()) && (chk_size != 0)) {
		std::vector<uint32_t>::const_iterator it = index.begin();
		uint32_t curVal = 0;
		uint32_t prevVal = 0;
		prevVal = *it;
		it++;
		while(it < index.end()) {
			curVal = *it;
			if(chk_size < (curVal - prevVal)) {
				return ret;
			}
			it++;
			prevVal = curVal;
		}

		ret = eStatus::SUCCESS;
	}
	return ret;
}

eStatus Parser::fast_parse_and_format(std::string filename)
{
	eStatus ret = eStatus::FAILED;
	if((!filename.empty()) && (m_init_status)) {
		fstream file;
		uint32_t file_length = 0;
		char *mainBuf = nullptr;
		char parseBuf[MARKET_DATA_MAX_SIZE + EXTRA_PADDING];
		std::vector<uint32_t> pat_index;

		/* open file for reading */
		file.open(filename.c_str(), ios::in|ios::binary);
		if (file.fail()){
			return ret;
		}

		/* get file length for future use */
		file.seekg(0, file.end);
		file_length = file.tellg();
		file.seekg(0, file.beg);

		/*allocate buffer*/
		mainBuf = new char(file_length);
		if(mainBuf == nullptr) {
			return eStatus::FAILED;
		}
		/*read into mainBuf*/
		file.read(mainBuf, file_length);

		/* parse and format */
		pat_index = search_pattern_index(mainBuf, file_length, m_term_seq.c_str(), m_term_seq.length());
		/**
		 * pat_index cannot be empty, else the file is corrupted
		 * let's perform some integrity testing of file contents:
		 * 	. check pat_index positions are in size to fit in MARKET_DATA_MAX_SIZE
		 * 	. check pat_index cannot have zero(0) at the beginning
		 * 	. and definitely, pat_index cannot be empty, yes !
		 * */
		if(check_file_index_positions(pat_index, MARKET_DATA_MAX_SIZE+1) == eStatus::FAILED) {
			/* failed integrity test, cannot parse the file contents */
			return eStatus::FAILED;
		}
		if(!pat_index.empty()) {
			uint32_t cur_pos = 0;
			ret = eStatus::SUCCESS;
			for(uint32_t &i : pat_index)
			{
				memset(parseBuf, '\0', (MARKET_DATA_MAX_SIZE + EXTRA_PADDING));
				memcpy(parseBuf, (const char*)mainBuf[cur_pos], (i-cur_pos));
				/* identify the message type */
				/* and, add it to corresponding format list */
				switch(get_msg_type(parseBuf)) {
				case MSG_TYP_ORDR_ENTRY:
					ret = add_to_order_entry_list(parseBuf, (i-cur_pos));
					break;
				case MSG_TYP_ORDR_ACK:
					ret = add_to_order_ack_list(parseBuf, (i-cur_pos));
					break;
				case MSG_TYP_ORDR_FILL:
					ret = add_to_order_fill_list(parseBuf, (i-cur_pos));
					break;
				}
				/**
				 * If ret is FAILED, it means individual message is broken
				 * and inturn file seems tampered
				 * This file cannot be processed any more
				 * */
				if(ret == eStatus::FAILED)
					break;
				/* discard TERMINATION pattern */
				cur_pos += m_term_seq.length();
				/* increment position to next valid msg */
				cur_pos += i;
			}
		} //end-of-parsing
		/* some cleanup activities */
		delete[] mainBuf;
		file.close();
	}
	return ret;
}

#if 0
eStatus Parser::mem_efficient_parse_and_format(std::string filename)
{
	eStatus ret = eStatus::FAILED;
	if((!filename.empty()) && (m_init_status)) {
		fstream file;
		uint32_t file_length = 0;
		uint32_t buf_length = (MARKET_DATA_MAX_SIZE + EXTRA_PADDING + PARSER_BUF_SIZE);
		uint32_t parser_len = (buf_length < (MARKET_DATA_MAX_SIZE + EXTRA_PADDING))? buf_length:(MARKET_DATA_MAX_SIZE + EXTRA_PADDING);
		char mainBuf[buf_length];
		char parseBuf[parser_len];

		/* open file for reading */
		file.open(filename.c_str(), ios::in | ios::out | ios::binary);
		if (file.fail()){
			return ret;
		}
		/* get file length for future use */
		file_length = file.tellg();

		/* a big while loop to read entire file and process it */
		while(!file.eof()) {
			std::vector<uint32_t> pat_index;
			uint32_t read_len = 0;
			/* reset back all to default */
			memset(mainBuf, '\0', buf_length);

			/* start reading further */
			file.read(mainBuf, buf_length);
			read_len = file.gcount();
			/* below logic fetches individual messages from mainBuf */
			pat_index = search_pattern_index(mainBuf, read_len, m_term_seq.c_str(), m_term_seq.length());
			/* pat_index cannot be empty, else the file is corrupted */
			if(!pat_index.empty()) {
				for(uint32_t &i : pat_index) {
					//unfinished :(
				}
			}
		}
	}
	return ret;
}
#endif

uint32_t Parser::parse_and_fill_header(const RawMsg_t msg, Header &h)
{
	uint32_t 		cur_pos = 0;
	if(msg) {
		h.m_marker 			= (uint16_t)std::strtoul((const char*)msg[cur_pos], NULL, 0);
		cur_pos += sizeof(uint16_t);
		h.m_msg_type 		= (uint8_t) (msg[cur_pos]);
		cur_pos += sizeof(uint8_t);
		h.m_seq_id 			= (uint64_t)std::strtoll((const char*)msg[cur_pos], NULL, 0);
		cur_pos += sizeof(uint64_t);
		h.m_timestamp 		= (uint64_t)std::strtoll((const char*)msg[cur_pos], NULL, 0);
		cur_pos += sizeof(uint64_t);
		h.m_msg_direction 	= (uint8_t) (msg[cur_pos]);
		cur_pos += sizeof(uint8_t);
		h.m_msg_len 		= (uint16_t)std::strtoul((const char*)msg[cur_pos], NULL, 0);
		cur_pos += sizeof(uint16_t);
	}
	return cur_pos;
}

eStatus Parser::add_to_order_entry_list(const RawMsg_t msg, uint32_t len){
	eStatus ret = eStatus::FAILED;
	if((msg) && (len != 0) && (m_init_status)){
		Header hdr;
		OrderEntry oe;
		uint32_t cur_pos = 0;
		cur_pos = parse_and_fill_header(msg, hdr);
		if(cur_pos == 0)
			return ret;
		/*Fill Order Entry Message*/
		oe.m_oe_header = hdr;
		oe.m_oe_price 		= (uint64_t)std::strtoll((const char*)msg[cur_pos], NULL, 0);
		cur_pos += sizeof(uint64_t);
		oe.m_oe_qty 		= (uint32_t)std::strtoul((const char*)msg[cur_pos], NULL, 0);
		cur_pos += sizeof(uint32_t);
		memset(oe.m_oe_instrument, '\0', 10);
		memcpy(oe.m_oe_instrument, (const char*)msg[cur_pos], 10);
		cur_pos += 10;
		oe.m_oe_side 		= (uint8_t)(msg[cur_pos]);
		cur_pos += sizeof(uint8_t);
		oe.m_oe_client_id 	= (uint64_t)std::strtoll((const char*)msg[cur_pos], NULL, 0);
		cur_pos += sizeof(uint64_t);
		oe.m_oe_time_in_force = (uint8_t)(msg[cur_pos]);
		cur_pos += sizeof(uint8_t);
		memset(oe.m_oe_trader_tag, '0', 3);
		memcpy(oe.m_oe_trader_tag, (const char*)msg[cur_pos], 3);
		cur_pos += 3;
		oe.m_oe_firm_id 	= (uint8_t)(msg[cur_pos]);
		cur_pos += sizeof(uint8_t);
		oe.m_oe_firm 		= C2Str((const char*)msg[cur_pos]);
		cur_pos += oe.m_oe_firm.length();

		/* add to container */
		m_msg_format->m_OrderEntryList.push_back(oe);

		/* all done successfully */
		ret = eStatus::SUCCESS;
	}
	return ret;
}

eStatus Parser::add_to_order_ack_list(const RawMsg_t msg, uint32_t len){
	eStatus ret = eStatus::FAILED;
	if((msg) && (len != 0) && (m_init_status)){
		Header hdr;
		OrderAck oa;
		uint32_t cur_pos = 0;
		cur_pos = parse_and_fill_header(msg, hdr);
		if(cur_pos == 0)
			return ret;
		/*Fill Order Acknowledgment Messages*/
		oa.m_oa_header 			= hdr;
		oa.m_oa_order_id 		= (uint32_t)std::strtoul((const char*)msg[cur_pos], NULL, 0);
		cur_pos += sizeof(uint32_t);
		oa.m_oa_client_id 		= (uint64_t)std::strtoull((const char*)msg[cur_pos], NULL, 0);
		cur_pos += sizeof(uint64_t);
		oa.m_oa_order_status 	= (uint8_t)(msg[cur_pos]);
		cur_pos += sizeof(uint8_t);
		oa.m_oa_reject_code 	= (uint8_t)(msg[cur_pos]);
		cur_pos += sizeof(uint8_t);

		/* add to container */
		m_msg_format->m_OrderAckList.push_back(oa);

		/* all done successfully */
		ret = eStatus::SUCCESS;
	}
	return ret;
}

eStatus Parser::add_to_order_fill_list(const RawMsg_t msg, uint32_t len){
	eStatus ret = eStatus::FAILED;
	if((msg) && (len != 0) && (m_init_status)){
		Header hdr;
		OrderFill of;
		uint32_t cur_pos = 0;
		cur_pos = parse_and_fill_header(msg, hdr);
		if(cur_pos == 0)
			return ret;
		/*Fill Order Fill Messages*/
		of.m_of_header 			= hdr;
		of.m_of_order_id 		= (uint32_t)std::strtoul((const char*)msg[cur_pos], NULL, 0);
		cur_pos += sizeof(uint32_t);
		of.m_of_fill_price 		= (uint64_t)std::strtoull((const char*)msg[cur_pos], NULL, 0);
		cur_pos += sizeof(uint64_t);
		of.m_of_fill_qty 		= (uint32_t)std::strtoul((const char*)msg[cur_pos], NULL, 0);
		cur_pos += sizeof(uint32_t);
		of.m_of_no_of_contras 	= (uint8_t)(msg[cur_pos]);
		cur_pos += sizeof(uint8_t);
		/* fill order fill contras left */
		uint8_t tmpCount = of.m_of_no_of_contras;
		OrderFill_ContrasGrp cGrp;
		while((tmpCount != 0) && (cur_pos <= len)) {
			cGrp.m_ofcg_firm_id = (uint8_t)(msg[cur_pos]);
			cur_pos += sizeof(uint8_t);
			memset(cGrp.m_ofcg_trader_tag, '\0', 3);
			memcpy(cGrp.m_ofcg_trader_tag, (const char*)msg[cur_pos], 3);
			cur_pos += 3;
			cGrp.m_ofcg_qty	= (uint32_t)std::strtoul((const char*)msg[cur_pos], NULL, 0);
			cur_pos += sizeof(uint32_t);
			of.m_of_cgrps.push_back(cGrp);
			tmpCount--;
		}

		/* add to container */
		m_msg_format->m_OrderFillList.push_back(of);

		/* all done successfully */
		ret = eStatus::SUCCESS;
	}
	return ret;
}

eStatus Parser::Init_Parser(std::string file, std::string term_str)
{
	eStatus ret = eStatus::FAILED;
	if((file.length() != 0)  && (term_str.length() != 0))
	{
		m_file_name = file;
		m_init_status = true;
		m_term_seq = term_str;
		ret = fast_parse_and_format(file);
		if(ret != eStatus::SUCCESS) {
			m_file_name.clear();
			m_init_status = false;
			m_term_seq.clear();
		}
	}
	return ret;
}

eStatus Parser::get_order_entry_msgs(std::list<OrderEntry> &l)
{
	eStatus ret = eStatus::FAILED;
	if((m_init_status) && (m_msg_format->m_OrderEntryList.size() != 0)) {
		l = m_msg_format->m_OrderEntryList;
		ret = eStatus::SUCCESS;
	}
	return ret;
}

eStatus Parser::get_order_ack_msgs(std::list<OrderAck> &l)
{
	eStatus ret = eStatus::FAILED;
	if((m_init_status) && (m_msg_format->m_OrderAckList.size() != 0)) {
		l = m_msg_format->m_OrderAckList;
		ret = eStatus::SUCCESS;
	}
	return ret;
}

eStatus Parser::get_order_fill_msgs(std::list<OrderFill> &l)
{
	eStatus ret = eStatus::FAILED;
	if((m_init_status) && (m_msg_format->m_OrderFillList.size() != 0)) {
		l = m_msg_format->m_OrderFillList;
		ret = eStatus::SUCCESS;
	}
	return ret;
}

eStatus Parser::ReInit_Parser(std::string file, std::string term_str)
{
	eStatus ret = eStatus::FAILED;
	if((!file.empty()) && (!term_str.empty()) && (file != m_file_name)) {
		m_file_name = file;
		m_term_seq = term_str;
		m_init_status = false;
		ret = Init_Parser(m_file_name, m_term_seq);
		if(ret == eStatus::SUCCESS) {
			m_init_status = true;
		}
	}
	return ret;
}
