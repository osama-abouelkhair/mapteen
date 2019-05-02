#ifndef __mapteen_pkt_h__
#define __mapteen_pkt_h__

#include <packet.h>

#define HDR_MAPTEEN_PKT(p) 			((struct hdr_mapteen_pkt*)hdr_mapteen_pkt::access(p))
#define HDR_MAPTEEN_SETUPREQ(p) 	((struct hdr_mapteen_setupreq*)hdr_mapteen_pkt::access(p))
#define HDR_MAPTEEN_SETUPRREQ(p) 	((struct hdr_mapteen_setuprreq*)hdr_mapteen_pkt::access(p))
#define HDR_MAPTEEN_DATA(p) 	((struct hdr_mapteen_data*)hdr_mapteen_pkt::access(p))


#define SETUPREQ    0x01
#define SETUPRREQ   0x02
#define DATA_PKT     0x04
//#define DATARREQ    0x08

#define MAPTEEN_REQ_LEN			(IP_HDR_LEN + sizeof(struct hdr_mapteen_setupreq))
#define MAPTEEN_RREQ_LEN		(IP_HDR_LEN + sizeof(struct hdr_mapteen_setuprreq))
#define MAPTEEN_DATA_LEN		(IP_HDR_LEN + sizeof(struct hdr_mapteen_data))

struct hdr_mapteen_pkt {

	u_int8_t pkt_type_; //type of the packet

    inline u_int8_t& pkt_type(){return pkt_type_;}

	static int offset_;
	inline static int& offset(){ return offset_;}
	inline static hdr_mapteen_pkt* access(const Packet* p){
		return (hdr_mapteen_pkt*) p->access(offset_);
	}
};

struct hdr_mapteen_setupreq {

	u_int8_t setupreq_type_; //type of the packet
	nsaddr_t pkt_src_;	//node which originated this packet

    inline u_int8_t& setupreq_type(){return setupreq_type_;}
	inline nsaddr_t& pkt_src(){ return pkt_src_;}

	//inline int& pkt_len(){
        //int size = 0;
        //size = sizeof(uint8_t) + sizeof(nsaddr_t);
        //return size;
    //}

};

struct hdr_mapteen_setuprreq {

    u_int8_t setuprreq_type_; //type of the packet
	nsaddr_t pkt_src_;	//node which originated this packet
	double x_;
	double y_;
	double z_;

    inline u_int8_t& setuprreq_type(){return setuprreq_type_;}
   	inline nsaddr_t& pkt_src(){ return pkt_src_;}
   	inline double& x(){return x_;}
   	inline double& y(){return y_;}
   	inline double& z(){return z_;}

   	//inline int& pkt_len(){
        //int size = 0;
        //size = sizeof(uint8_t) + sizeof(nsaddr_t) + 3*sizeof(double);
        //return size;
    //}


};


struct hdr_mapteen_data {
	
    u_int8_t data_type_; //type of the packet
	nsaddr_t pkt_src_;	//node which originated this packet

    inline u_int8_t& data_type(){return data_type_;}
   	inline nsaddr_t& pkt_src(){ return pkt_src_;}

   	//inline int& pkt_len(){
		//int size = 512;
        //return size;
    //}


};

#endif

