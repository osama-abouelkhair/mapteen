#ifndef	__mapteen_h__
#define __mapteen_h__

#include "mapteen_pkt.h"
#include "mapteen_rtable.h"
#include <packet.h>
#include <agent.h>
#include <trace.h>
#include <timer-handler.h>
#include <random.h>
#include <classifier-port.h>

#define CURRENT_TIME Scheduler::instance().clock()
#define JITTER (Random::uniform()*0.2)

class MAPTEEN;

/* Timers */

class MAPTEEN_PktTimer : public TimerHandler {
public:
	MAPTEEN_PktTimer(MAPTEEN* agent) : TimerHandler(){
		agent_ = agent;
	}

protected:
	MAPTEEN* agent_;
	virtual void expire(Event* e);
};

/* Agent */

class MAPTEEN : public Agent {

	/* friends */
	friend class MAPTEEN_PktTimer;

	/* private members */
	nsaddr_t ra_addr_;
	//mapteen_state state_;
	mapteen_rtable rtable_;
	int accessible_var_;
	u_int8_t seq_num_;
	bool broadcast_;
	int isBS_;
	short int hop_index_;

protected:
	PortClassifier* dmux_; // For passing packets up to agents.
	Trace* logtarget_; // For logging.
	MAPTEEN_PktTimer pkt_timer_; // Timer for sending packets.

	inline nsaddr_t& ra_addr() { return ra_addr_; }
	//inline mapteen_state& state() { return state_; }
	inline int& accesible_var() { return accessible_var_; }
	inline bool& broadcast() { return broadcast_; }
    inline short int& hop_index() {return hop_index_;}

	void forward_data(Packet*);
	void recv_mapteen_pkt(Packet*);
	void send_mapteen_pkt();
	void forward_mapteen_setupreq(Packet*);
	void forward_mapteen_setuprreq(Packet*);
	void forward_mapteen_data(Packet*);
    void reply_mapteen_setup();
    void increment_hop_index();
    void received_rreq(Packet* p);
	void recv_data_pkt(Packet* p);
	void reset_mapteen_pkt_timer();

public:

	MAPTEEN(nsaddr_t);
	int command(int, const char*const*);
	void recv(Packet*, Handler*);

};

#endif
