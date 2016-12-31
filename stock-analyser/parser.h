/*
 * parser.h
 *
 *  Created on: DEC, 2016
 *  Author: Kiran Puranik
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "global.h"
#include "msg_format.h"

using namespace std;

enum class eStatus {SUCCESS, FAILED, INVALID, NO_MEM};

#define INVALID_HDL 	0
typedef uint32_t 	Handler_t;

typedef char* 		RawMsg_t;

#define PARSER_BUF_SIZE 			50
#define MARKET_DATA_MAX_SIZE 		300 //22, 300, 22, 105
#define EXTRA_PADDING 				10

class Parser
{
private:
	/**
	 * This is a formatting structure
	 * Only and only for this Parser class
	 */
	struct Format {
		std::list<OrderEntry> 	m_OrderEntryList;
		std::list<OrderAck> 	m_OrderAckList;
		std::list<OrderFill> 	m_OrderFillList;
	};

	std::string 	m_file_name;
	bool 			m_init_status;
	std::string 	m_term_seq;
	Format 			*m_msg_format;

public:
	Parser(const char* file_name, const char* term_str);
	~Parser();

private:
	eStatus Init_Parser(std::string file, std::string term_str);
	eStatus parse_the_file(std::string filename);
	eStatus fast_parse_and_format(std::string filename);
#if 0 //unfinished job :(
	eStatus mem_efficient_parse_and_format(std::string filename);
#endif
	std::vector<uint32_t> search_pattern_index(const char *msg, uint32_t msg_len, const char *pat, uint32_t pat_len);
	eStatus check_file_index_positions(const std::vector<uint32_t> &index, uint32_t chk_size);
	uint32_t parse_and_fill_header(const RawMsg_t msg, Header &h);
	eStatus add_to_order_entry_list(const RawMsg_t msg, uint32_t len);
	eStatus add_to_order_ack_list(const RawMsg_t msg, uint32_t len);
	eStatus add_to_order_fill_list(const RawMsg_t msg, uint32_t len);

public:
	eStatus ReInit_Parser(std::string file, std::string term_str);
	eStatus get_order_entry_msgs(std::list<OrderEntry> &l);
	eStatus get_order_ack_msgs(std::list<OrderAck> &l);
	eStatus get_order_fill_msgs(std::list<OrderFill> &l);
};

#endif /* PARSER_H_ */
