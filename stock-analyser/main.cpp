/*
 * main.cpp
 *
 *  Created on: DEC 2016
 *  Author: Kiran Puranik (gkiranp@gmail.com)
 */

#include "global.h"
#include "msg_format.h"
#include "utility.h"
#include "parser.h"

#define TERM_STR "DBDBDBDB"

void help(char **argv)
{
	std::string s(argv[0]);
	std::cout << "Usage :" << std::endl;
	std::cout << "  %s file_name" << s << std::endl;
}

int main(int argc, char** argv)
{
	if(argc != 2) {
		std::cout << "Invalid argument list, please pass proper arguments." << std::endl;
		help(argv);
		return -1;
	}
	char * file_name = argv[1];
	if(!exists(file_name)) {
		std::cout << "Invalid file passed; file doesn't exists." << std::endl;
		help(argv);
		return -1;
	}
	Parser *pars = new Parser(file_name, TERM_STR);
	std::list<OrderEntry> 	OrderEntryPackets;
	std::list<OrderAck> 	OrderAckPackets;
	std::list<OrderFill> 	OrderFillPackets;

	if(pars->get_order_entry_msgs(OrderEntryPackets) != eStatus::SUCCESS) {
		std::cout << "Stream parsing encountered an ERROR !" << std::endl;
		return -1;
	}
	if(pars->get_order_ack_msgs(OrderAckPackets) != eStatus::SUCCESS) {
		std::cout << "Stream parsing encountered an ERROR !" << std::endl;
		return -1;
	}
	if(pars->get_order_fill_msgs(OrderFillPackets) != eStatus::SUCCESS) {
		std::cout << "Stream parsing encountered an ERROR !" << std::endl;
		return -1;
	}

	/****************************************************************
	 * Let's see what all we can extract from this source by running,
	****************************************************************/
	/* How many packets of information are there in stream */
	std::cout << "%d" << (OrderEntryPackets.size() + OrderAckPackets.size() + OrderFillPackets.size()) << std::endl;

	/* How many message exists for message type */
	std::cout << "%d,%d,%d" << OrderEntryPackets.size() << OrderAckPackets.size() << OrderFillPackets.size() << std::endl;

	/* Who was the most active trader */
	uint32_t order_id = get_highest_trader_id(OrderFillPackets);
	uint64_t client_id = get_client_id_for_orderid(OrderAckPackets, order_id);
	std::cout << "%ld" << client_id << std::endl;

	/* Who provided most liquidity */
	client_id = 0;
	client_id = get_highest_gfd_volume_trader_id(OrderEntryPackets);
	std::cout << "%ld" << client_id << std::endl;

	/* Volume of trades per instruments */


	/* All successfully ended :) */
	return 0;
}



