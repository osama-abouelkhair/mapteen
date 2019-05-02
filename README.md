# mapteen
 MAPTEEN is a wireless sensor network protocol This routing protocol allows multiple routes for the information to reach a certain destination.


The sensor network is organized into clusters and the
routing algorithm allows a single hop from one cluster to
another all the way to the Base Stations (BS).
 The communication between the nodes and the BS is
carried out using the IEEE 802.15.4 protocol and the 2.4
GHz band (ISM band). The IEEE WPAN 802.15.4
(Wireless Personal Area Networks) is the media access
control protocol layer used in our simulation model. This
protocol is used for sending data between all the nodes
and the base station.
The MAPTEEN protocol has 3 different packet types:
* Setup packet: sent by the BS in the setup phase. It
contains the BS address.
* Setup reply packet: sent by nodes that received setup
packet. It contains the node address and coordinates.
* Data packet: contains the source node address and the
data (to the BS).
Routing Table:
* Each node will have a small routing table with
several next hop addresses.
* The network will determine the routing table of each
node during the setup phase.
* The base station will have a routing table that
contains the addresses of all nodes and their
coordinates.
