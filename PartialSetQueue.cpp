#include "PartialSetQueue.h"

using namespace DRAMSim;
PartialSetQueue::PartialSetQueue() {
	size_t bank_num;
	if (queuingStructure == PerRank) {
		bank_num = 1;
	} else if (queuingStructure==PerRankPerBank) {
		bank_num = NUM_BANKS;
	} else {
		ERROR("== Error - Unknown queuing structure");
		exit(0);
	}
    for (int i = 0; i < NUM_RANKS; ++i) {
        PartialBusPacket2D rank_queue;
        for (int j = 0; j < bank_num; ++j) {
            PartialBusPacket1D bank_queue;
            rank_queue.push_back(bank_queue);
        }
        m_queues.push_back(rank_queue);
    }
}

void PartialSetQueue::push(PartialBusPacket* partial_packet) {
    BusPacket* bus_packet = partial_packet->getBusPacket();

	unsigned rank = bus_packet->rank;
	unsigned bank = bus_packet->bank;
	PartialBusPacket1D queue;
	if (queuingStructure == PerRank) {
	    queue = m_queues[rank][0];
    } else if (queuingStructure == PerRankPerBank) {
        queue = m_queues[rank][bank];
    }
    for (vector<PartialBusPacket*>::iterator it = queue.begin(); it != queue.end(); ++it) {
        // 删除相同地址的packet
        if ((*it)->getBusPacket()->physicalAddress == bus_packet->physicalAddress) {
            queue.erase(it);
            break;
        }
    }
    queue.push_back(partial_packet);
    if (queue.size() > CMD_QUEUE_DEPTH) {
        ERROR("== Error - Enqueued more than allowed in command queue");
        ERROR("Need to call .hasRoomFor(int numberToEnqueue, unsigned rank, unsigned bank) first");
        exit(0);
    }
}
