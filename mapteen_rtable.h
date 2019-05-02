#ifndef __mapteen_rtable_h__
#define __mapteen_rtable_h__

#include <trace.h>
#include <cmu-trace.h>
#include <map>
#include <vector>



struct node_map{
	nsaddr_t next_addr_;
	double x_;
	double y_;
	double z_;

	inline nsaddr_t& next_addr(){return next_addr_;}
	inline double& x(){return x_;}
	inline double& y(){return y_;}
	inline double& z(){return z_;}
};

class mapteen_rtable {

	nsaddr_t rt_ [3];
	nsaddr_t bs_addr_;
	int nn_;
	std::vector<node_map> BS_rt_;


public:
	void init_BS();
	mapteen_rtable();
	void print(Trace*);
	void print_BS(Trace*);
	void clear();
	void clear_BS();
	void rm_entry(nsaddr_t);
	void rm_BS_entry(nsaddr_t);
	void add_entry(nsaddr_t, nsaddr_t);
	void add_BS_entry(nsaddr_t, nsaddr_t,double,double,double);
	nsaddr_t lookup();
	nsaddr_t BS_lookup(nsaddr_t);
	u_int32_t size();
	u_int32_t BS_size();
	void set_bs(nsaddr_t);
	nsaddr_t bs_addr();
};

#endif
