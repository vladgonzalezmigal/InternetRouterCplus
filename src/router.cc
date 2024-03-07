#include "router.hh"

#include <iostream>
#include <limits>

using namespace std;

// route_prefix: The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
// prefix_length: For this route to be applicable, how many high-order (most-significant) bits of
//    the route_prefix will need to match the corresponding bits of the datagram's destination address?
// next_hop: The IP address of the next hop. Will be empty if the network is directly attached to the router (in
//    which case, the next hop address should be the datagram's final destination).
// interface_num: The index of the interface to send the datagram out on.
void Router::add_route( const uint32_t route_prefix,
                        const uint8_t prefix_length,
                        const optional<Address> next_hop,
                        const size_t interface_num )
{
  cerr << "DEBUG: adding route " << Address::from_ipv4_numeric( route_prefix ).ip() << "/"
       << static_cast<int>( prefix_length ) << " => " << ( next_hop.has_value() ? next_hop->ip() : "(direct)" )
       << " on interface " << interface_num << "\n";

  myRoute route {
    .route_prefix = route_prefix, 
    .prefix_length = prefix_length,
    .next_hop = next_hop, 
    .interface_num = interface_num
  };
  route_table.push_back(route);
}

// Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
void Router::route()
{
  for (std::shared_ptr<NetworkInterface> interface : _interfaces){
    std::queue dgram_q = interface->datagrams_received();
    while (!dgram_q.empty()){ // route dgrams in interface
      if (dgram_q.front().header.ttl > 1){
        uint32_t dest_addr = dgram_q.front().header.dst;
        optional<myRoute> match;
        for (myRoute route : route_table){
          
        }
      }
      //  void send_datagram( const InternetDatagram& dgram, const Address& next_hop )
    }
  }
}
