#include <mapteen/mapteen.h>
#include <mapteen/mapteen_pkt.h>
#include "mapteen_rtable.h"


mapteen_rtable::mapteen_rtable(){
    bs_addr_ = -1;
}

void
mapteen_rtable::init_BS(){
	nn_ = God::instance()->nodes();
    //test initialization of base station routing table
    fprintf(stdout, "number of nodes from the base station routing table is %d\n", nn_);

	BS_rt_ = std::vector<node_map>(nn_);
	//for(int i = 0; i < nn_ ; i++){
     //   rt_[i] = new node_map();
    //}
}

void
mapteen_rtable::print(Trace* out) {
	sprintf(out->pt_->buffer(), "P\tnext1\tnext2\tnext3");
	out->pt_->dump();

	//old code using multimap for routing table
//	for (rtable_t::iterator it = rt_.begin(); it != rt_.end(); it++) {
//		sprintf(out->pt_->buffer(), "P\t%d\t%d",
//		(*it).first,
//		(*it).second);
//		out->pt_->dump();
//    }
    //for(int i = 0 ; i<3; i++){
        sprintf(out->pt_->buffer(), "P\t%d\t%d\t%d", rt_[0], rt_[1], rt_[2]);
        out->pt_->dump();
    //}

	fprintf(stdout, "P\tnext1\tnext2\tnext3\n");
	//out->pt_->dump();
	//for(int i = 0 ; i<3; i++){
		fprintf(stdout, "P\t%d\t%d\t%d\n", rt_[0], rt_[1], rt_[2]);
		//out->pt_->dump();
	//}
}

void
mapteen_rtable::print_BS(Trace* out) {
	sprintf(out->pt_->buffer(), "P\tnext\tx\ty\tz\n");
	out->pt_->dump();

	//old code using multimap for routing table
//	for (rtable_t::iterator it = rt_.begin(); it != rt_.end(); it++) {
//		sprintf(out->pt_->buffer(), "P\t%d\t%d",
//		(*it).first,
//		(*it).second);
//		out->pt_->dump();
//    }
    std::vector<node_map>::iterator buff_it;
    for(buff_it = BS_rt_.begin(); buff_it != BS_rt_.end(); buff_it++){
        sprintf(out->pt_->buffer(), "P\t%d\t%.2f\t%.2f\t%.2f\n",
            buff_it->next_addr(),
            buff_it->x(),
            buff_it->y(),
            buff_it->z());
        out->pt_->dump();
    }

	fprintf(stdout, "P\tnext\tx\ty\tz\n");
    std::vector<node_map>::iterator stdout_it;
    for(stdout_it = BS_rt_.begin(); stdout_it != BS_rt_.end(); stdout_it++){
        fprintf(stdout, "P\t%d\t%.2f\t%.2f\t%.2f\n",
            stdout_it->next_addr(),
            stdout_it->x(),
            stdout_it->y(),
            stdout_it->z());
    }

}


void
mapteen_rtable::clear() {
	//rt_.clear();
	rt_[0] = -1;
	rt_[1] = -1;
	rt_[2] = -1;
}

void
mapteen_rtable::clear_BS() {
	std::vector<node_map>::iterator it;
    for(it = BS_rt_.begin(); it != BS_rt_.end(); it+=1){
        it->next_addr() = -1;
	}

}


void
mapteen_rtable::rm_entry(nsaddr_t next) {
	for(int i = 0 ; i<3; i++){
        if(rt_[i] == next){
            rt_[i] = -1;
            return;
        }
	}
}

void
mapteen_rtable::rm_BS_entry(nsaddr_t dest) {
    BS_rt_[dest].next_addr() = -1;
}

void
mapteen_rtable::add_entry(int hop_index, nsaddr_t next) {
	//rt_[dest] = next;
	//rt_.insert(pair<nsaddr_t, nsaddr_t>(dest, next));
	if(hop_index >= 0 && hop_index <3)
        rt_[hop_index] = next;
}

void
mapteen_rtable::add_BS_entry(nsaddr_t dest_addr, nsaddr_t next,
                        double x, double y, double z) {

	BS_rt_[dest_addr].next_addr() = next;
	BS_rt_[dest_addr].x() = x;
	BS_rt_[dest_addr].y() = y;
	BS_rt_[dest_addr].z() = z;
}


nsaddr_t
mapteen_rtable::lookup() {
    return rt_[0];

//    pair<multimap<nsaddr_t, nsaddr_t>::iterator, multimap<nsaddr_t, nsaddr_t>::iterator> range;
//	range = rt_.equal_range(dest);
//	for (multimap<nsaddr_t, nsaddr_t>::iterator it2 = range.first, i = 0; it2 != range.second && i<3; ++it2, i++){
//                addresses [i] = (*it2).second;
//        }

//    vector<nsaddr_t> v;
//    transform(rt_.lower_bound(dest), rt_.upper_bound(dest),
//              back_inserter(v), __gnu_cxx::select2nd<pair<nsaddr_t, nsaddr_t> >());
//    // note: select2nd is an SGI extension.
//
//    for (vector<nsaddr_t>::const_iterator cit = v.begin(), i = 0; cit != v.end() && i < 3; ++cit, i++){
//            //addresses [i] = (*cit);
//            printf(stdout,"%d, \n", (*cit).second);
//         }
//
//    return addresses;
}

nsaddr_t
mapteen_rtable::BS_lookup(nsaddr_t dest_addr) {
    return BS_rt_[dest_addr].next_addr();
}

u_int32_t
mapteen_rtable::size() {
    int count = 0;
    for(int i = 0; i<3; i++){
        if(rt_[i] != -1)
            count++;
    }
    return count;
	//return rt_.size();
}

u_int32_t
mapteen_rtable::BS_size() {

    return nn_;
	//return rt_.size();
}


void
mapteen_rtable::set_bs(nsaddr_t addr){
    bs_addr_ = addr;
}

nsaddr_t
mapteen_rtable::bs_addr(){
    return bs_addr_;
}

