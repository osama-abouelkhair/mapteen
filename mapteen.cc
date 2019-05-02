#include "mapteen.h"
#include <random.h>
#include <cmu-trace.h>
#include <iostream>
#include <cmath>


int hdr_mapteen_pkt::offset_;
static class MAPTEENHeaderClass : public PacketHeaderClass {
public:
	MAPTEENHeaderClass() : PacketHeaderClass("PacketHeader/MAPTEEN",
							sizeof(hdr_mapteen_pkt)) {
		bind_offset(&hdr_mapteen_pkt::offset_);
	}
} class_rtProtoMAPTEEN_hdr;


static class MAPTEENClass : public TclClass {
public:
	MAPTEENClass() : TclClass("Agent/MAPTEEN") {}
	TclObject* create(int argc, const char*const* argv) {
		assert(argc == 5);
		return (new MAPTEEN((nsaddr_t)Address::instance().str2addr(argv[4])));
	}
} class_rtProtoMAPTEEN;

void
MAPTEEN_PktTimer::expire(Event* e) {
	agent_->send_mapteen_pkt();
	fprintf(stdout, "entered expire\n");
	//agent_->reset_mapteen_pkt_timer();
}

MAPTEEN::MAPTEEN(nsaddr_t id) : Agent(PT_MAPTEEN), pkt_timer_(this) {
	bind_bool("accessible_var_", &accessible_var_);
	bind_bool("isBS_", &isBS_);
	ra_addr_ = id;
	broadcast_ = false;
	hop_index_ = 0;
	isBS_ = false;
	rtable_.clear();
}

int
MAPTEEN::command(int argc, const char*const* argv) {
	if (argc == 2) {
		if (strcasecmp(argv[1], "start") == 0) {
			pkt_timer_.resched(0.0);
			fprintf(stdout, "entered start command\n");
			//agent_->send_mapteen_pkt();
			return TCL_OK;
		}
		else if (strcasecmp(argv[1], "print_rtable") == 0) {
			if (logtarget_ != 0) {
				if(isBS_){
					sprintf(logtarget_->pt_->buffer(), "P %f _%d_ Routing Table",
					CURRENT_TIME,
					ra_addr());
					logtarget_->pt_->dump();
					rtable_.print_BS(logtarget_);
				}else{
					sprintf(logtarget_->pt_->buffer(), "P %f _%d_ Routing Table",
					CURRENT_TIME,
					ra_addr());
					logtarget_->pt_->dump();
					rtable_.print(logtarget_);
				}

			}
			else {
				fprintf(stdout, "%f _%d_ If you want to print this routing table you must create a trace file in your tcl script",
				CURRENT_TIME,
				ra_addr());
			}
			return TCL_OK;
		}else if (strcasecmp(argv[1], "set_as_bs") == 0) {
			rtable_.init_BS();
			isBS_ = true;
			return TCL_OK;
		}
	}
	else if (argc == 3) {
		// Obtains corresponding dmux to carry packets to upper layers
		if (strcmp(argv[1], "port-dmux") == 0) {
			dmux_ = (PortClassifier*)TclObject::lookup(argv[2]);
			if (dmux_ == 0) {
				fprintf(stderr, "%s: %s lookup of %s failed\n",
				__FILE__,
				argv[1],
				argv[2]);
				return TCL_ERROR;
			}
		return TCL_OK;
	}
	// Obtains corresponding tracer
	else if (strcmp(argv[1], "log-target") == 0 ||
		strcmp(argv[1], "tracetarget") == 0) {
			logtarget_ = (Trace*)TclObject::lookup(argv[2]);
			if (logtarget_ == 0)return TCL_ERROR;
			return TCL_OK;
		}
	}
	// Pass the command to the base class
	return Agent::command(argc, argv);
}

void
MAPTEEN::recv(Packet* p, Handler* h) {
	struct hdr_cmn* ch = HDR_CMN(p);
	struct hdr_ip* ih = HDR_IP(p);

	if (ih->saddr() == ra_addr()) {
		// If there exists a loop, must drop the packet
		if (ch->num_forwards() > 0) {
			drop(p, DROP_RTR_ROUTE_LOOP);
			return;
		}
		// else if this is a packet I am originating, must add IP header
		else if (ch->num_forwards() == 0){
			ch->size() += IP_HDR_LEN;
		}
	}


		// If it is a mapteen packet, must process it
    if (ch->ptype() == PT_MAPTEEN){
        //if(hop_index() < 3){
            recv_mapteen_pkt(p);
        //}
    } 
    // Otherwise, must forward the packet (unless TTL has reached zero)
    else {
			fprintf(stdout, "received data packet\n");
			ih->ttl_--;
			if (ih->ttl_ == 0) {
				drop(p, DROP_RTR_TTL);
				fprintf(stdout, "packet dropped\n");
				return;
			}
			//if(isBS_){
			//	fprintf(stdout, "bs received data packet\n");
			//	recv_data_pkt(p);

			//}else{
				forward_data(p);
			//}
		}
	}

void 
MAPTEEN::recv_data_pkt(Packet* p){
		
		fprintf(stdout, "free data packet");
		Packet::free(p);
}

void
MAPTEEN::recv_mapteen_pkt(Packet* p) {
	struct hdr_ip* ih = HDR_IP(p);
	struct hdr_cmn* ch = HDR_CMN(p);
	struct hdr_mapteen_pkt* ph = HDR_MAPTEEN_PKT(p);

	// All routing messages are sent from and to port RT_PORT,
	// so we check it.
	assert(ih->sport() == RT_PORT);
	assert(ih->dport() == RT_PORT);

    double sourceDisX = God::instance()->getNodeXByID(ih->saddr());
    double destDisX = God::instance()->getNodeXByID(ra_addr());
    int dis = destDisX - sourceDisX;

    fprintf(stdout ,"node _%d_ received packet from node _%d_ ",
        ra_addr(), ih->saddr());
	fprintf(stdout ,"distance = _%d_\n", dis);

    if(ph->pkt_type() == SETUPREQ && !isBS_){
    struct hdr_mapteen_setupreq* sph = HDR_MAPTEEN_SETUPREQ(p);
        if(hop_index() < 3 && dis > 10){
			
            rtable_.set_bs(sph->pkt_src());
            rtable_.add_entry(hop_index(), ih->saddr());
            increment_hop_index();
            reply_mapteen_setup();
            if(broadcast() != true && ch->next_hop() == IP_BROADCAST){
			
                forward_mapteen_setupreq(p);
            
			}
        }else{
			if(broadcast() != true && ch->next_hop() == IP_BROADCAST){
			
                forward_mapteen_setupreq(p);
            
			}
		}
    }else if(ph->pkt_type() == SETUPRREQ){
		if(isBS_){
			received_rreq(p);
		}else {
			if(dis < -10){
				fprintf(stdout, "detect reply packet");
				forward_mapteen_setuprreq(p);
			}
		}
	}else if(ph->pkt_type() == DATA_PKT){
		if(dis < -10){
			fprintf(stdout, "detect data packet");
			forward_mapteen_data(p);
		}
	}
	else {
		fprintf(stdout, "free data packet");
		Packet::free(p);
	}
}

void
MAPTEEN::received_rreq(Packet* p){
	struct hdr_ip* ih = HDR_IP(p);
	struct hdr_cmn* ch = HDR_CMN(p);
	
	struct hdr_mapteen_setuprreq* rph = HDR_MAPTEEN_SETUPRREQ(p);
		fprintf(stdout, "bs received setup reply request from node _%d_ with hop node _%d_\n", rph->pkt_src(), ih->saddr());
        rtable_.add_BS_entry(rph->pkt_src(), ih->saddr(),
                            rph->x(), rph->y(), rph->z());
    
}


void
MAPTEEN::reply_mapteen_setup(){

    nsaddr_t bs = rtable_.bs_addr();
    fprintf(stdout, "base station address: %d\n", bs);

    Packet* p = allocpkt();
	struct hdr_cmn* ch = HDR_CMN(p);
	struct hdr_ip* ih = HDR_IP(p);
	struct hdr_mapteen_setuprreq* ph = HDR_MAPTEEN_SETUPRREQ(p);

	ch->ptype() = PT_MAPTEEN;
	ch->direction() = hdr_cmn::DOWN;
	ch->size() = MAPTEEN_RREQ_LEN;
	ch->error() = 0;
	ch->next_hop() = rtable_.lookup();
	fprintf(stdout ,"reply for setup request lookup address: %d\n", rtable_.lookup());
	ch->addr_type() = NS_AF_INET;

	ih->saddr() = ra_addr();
	fprintf(stdout ,"reply for setup request node address: %d\n", ra_addr());
	ih->daddr() = bs;

	fprintf(stdout ,"reply for setup request bs address: %d\n", bs);
	ih->sport() = RT_PORT;
	ih->dport() = RT_PORT;
    ih->ttl() = IP_DEF_TTL;

	ph->pkt_src() = ra_addr();
	ph->setuprreq_type() = SETUPRREQ;
	//ph->x() = God::instance()->getNodeXByID(node_addr);
	//ph->y() = God::instance()->getNodeYByID(node_addr);
	//ph->z() = God::instance()->getNodeZByID(node_addr);

    //fprintf(stdout, "scheduling the packet");
    Scheduler::instance().schedule(target_, p, JITTER);
    //fprintf(stdout, "the packet is scheduled");


}

void
MAPTEEN::send_mapteen_pkt() {
	if(isBS_){
		Packet* p = allocpkt();
		struct hdr_cmn* ch = HDR_CMN(p);
		struct hdr_ip* ih = HDR_IP(p);
		struct hdr_mapteen_setupreq* ph = HDR_MAPTEEN_SETUPREQ(p);

		ph->pkt_src() = ra_addr();
		ph->setupreq_type() = SETUPREQ;
	
		ch->ptype() = PT_MAPTEEN;
		ch->direction() = hdr_cmn::DOWN;
		ch->size() = MAPTEEN_REQ_LEN;
		ch->error() = 0;
		ch->next_hop() = IP_BROADCAST;
		ch->addr_type() = NS_AF_INET;

		ih->saddr() = ra_addr();
		ih->daddr() = IP_BROADCAST;
		ih->sport() = RT_PORT;
		ih->dport() = RT_PORT;
		ih->ttl() = IP_DEF_TTL;

		broadcast_ = true;

		Scheduler::instance().schedule(target_, p, 0.0);
	}
	/*Packet* p = allocpkt();
	struct hdr_cmn* ch = HDR_CMN(p);
	struct hdr_ip* ih = HDR_IP(p);
	struct hdr_mapteen_data* ph = HDR_MAPTEEN_DATA(p);

	ch->ptype() = PT_MAPTEEN;
	ch->direction() = hdr_cmn::DOWN;
	ch->size() = MAPTEEN_DATA_LEN;
	fprintf(stdout, "data packet length: %d\n", (int)MAPTEEN_DATA_LEN);
	ch->error() = 0;
	ch->next_hop() = rtable_.lookup();
	ch->addr_type() = NS_AF_INET;

	ih->saddr() = ra_addr();
	ih->daddr() = rtable_.bs_addr();
	ih->sport() = RT_PORT;
	ih->dport() = RT_PORT;
	ih->ttl() = IP_DEF_TTL;
	
	ph->pkt_src() = ra_addr();
	ph->data_type() = DATA_PKT;

	//broadcast_ = true;
	fprintf(stdout, "send data from node _%d_ to node _%d_ next hop node _%d_\n", ra_addr(), rtable_.bs_addr(), rtable_.lookup());
	Scheduler::instance().schedule(target_, p, JITTER);
	* */
}

void
MAPTEEN::forward_mapteen_setupreq(Packet* receivedPacket) {

    struct hdr_cmn* rp_ch	= NULL;
	struct hdr_mapteen_setupreq * rp_ph = NULL;
	struct hdr_ip* rp_ih = NULL;

	if(receivedPacket != NULL)
	{
		rp_ph = HDR_MAPTEEN_SETUPREQ(receivedPacket);
		rp_ch = HDR_CMN(receivedPacket);
		rp_ih	= HDR_IP(receivedPacket);
	
		fprintf(stdout, "forwarding setup req\n");
		//struct hdr_mapteen_setupreq srp_ph = HDR_MAPTEEN_SETUPREQ(receivedPacket);
        rp_ch->next_hop() = IP_BROADCAST;
		rp_ch->direction() = hdr_cmn::DOWN;
        rp_ih->daddr() = IP_BROADCAST;
        rp_ih->saddr() = ra_addr();
		
        //if(hop_index() >= 3){
            broadcast_ = true;
        //}
    


	Scheduler::instance().schedule(target_, receivedPacket, JITTER);
	}
}

void
MAPTEEN::forward_mapteen_setuprreq(Packet* receivedPacket) {

    struct hdr_cmn* rp_ch	= NULL;
	struct hdr_mapteen_setuprreq * rp_ph = NULL;
	struct hdr_ip* rp_ih = NULL;

	if(receivedPacket != NULL)
	{
		
		rp_ph = HDR_MAPTEEN_SETUPRREQ(receivedPacket);
		rp_ch = HDR_CMN(receivedPacket);
		rp_ih	= HDR_IP(receivedPacket);
		
		fprintf(stdout, "forwarding setup reply req\n");
		//struct hdr_mapteen_setuprreq srrp_ph = HDR_MAPTEEN_SETUPRREQ(receivedPacket);
        rp_ch->next_hop() = rtable_.lookup();
		rp_ch->direction() = hdr_cmn::DOWN;
        rp_ih->daddr() = rtable_.bs_addr();
        rp_ih->saddr() = ra_addr();
    }


	Scheduler::instance().schedule(target_, receivedPacket, JITTER);
	
}

void
MAPTEEN::forward_mapteen_data(Packet* receivedPacket) {

    struct hdr_cmn* rp_ch	= NULL;
	struct hdr_mapteen_data * rp_ph = NULL;
	struct hdr_ip* rp_ih = NULL;

	if(receivedPacket != NULL)
	{
		
		//rp_ph = HDR_MAPTEEN_DATA(receivedPacket);
		rp_ch = HDR_CMN(receivedPacket);
		rp_ih	= HDR_IP(receivedPacket);
		
		fprintf(stdout, "forwarding data reply from _%d_ to _%d_ \n", rp_ih->saddr(), rtable_.bs_addr());
		//struct hdr_mapteen_setuprreq srrp_ph = HDR_MAPTEEN_SETUPRREQ(receivedPacket);
        rp_ch->next_hop() = rtable_.lookup();
		rp_ch->direction() = hdr_cmn::DOWN;
        rp_ih->daddr() = rtable_.bs_addr();
        rp_ih->saddr() = ra_addr();
    }


	Scheduler::instance().schedule(target_, receivedPacket, JITTER);
	
}

void
MAPTEEN::reset_mapteen_pkt_timer() {
	//pkt_timer_.resched((double)5.0);
}

void
MAPTEEN::forward_data(Packet* p) {
	struct hdr_cmn* ch = HDR_CMN(p);
	struct hdr_ip* ih = HDR_IP(p);

	if(ch->direction() == hdr_cmn::UP){
		fprintf(stdout, "direction is up source node %d and dest node %d\n", ih->saddr(), ih->daddr());
	}else{
		fprintf(stdout, "direction is down\n");
	}

	if (ch->direction() == hdr_cmn::UP &&
		((u_int32_t)ih->daddr() == IP_BROADCAST || ih->daddr() == ra_addr())) {
			dmux_->recv(p, 0);
			//recv_data_pkt(p);
			return;
		}
		else {
			if(!isBS_) {
			ch->direction() = hdr_cmn::DOWN;
			ch->addr_type() = NS_AF_INET;

			if ((u_int32_t)ih->daddr() == IP_BROADCAST){
				ch->next_hop() = IP_BROADCAST;
				fprintf(stdout, "destination is broadcast\n");
			}
			else {
				
					nsaddr_t next_hop = rtable_.lookup();
					if (next_hop == IP_BROADCAST) {
						debug("%f: Agent %d can not forward a packet destined to %d\n",
						CURRENT_TIME,
						ra_addr(),
						ih->daddr());
						drop(p, DROP_RTR_NO_ROUTE);
						return;
					}
					else{
						ch->next_hop() = next_hop;
						ih->daddr() = rtable_.bs_addr();
						ih->saddr() = ra_addr();
						fprintf(stdout, "forwarding data packet from node _%d_ to hop node _%d_ bs _%d_\n", ih->saddr(), ch->next_hop(), ih->daddr());
					}
				}
			
			
			Scheduler::instance().schedule(target_, p, JITTER);
			}
		}
}

void
MAPTEEN::increment_hop_index(){
    hop_index_++;
}
