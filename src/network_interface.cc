#include <iostream>

#include "arp_message.hh"
#include "exception.hh"
#include "network_interface.hh"

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface( string_view name,
                                    shared_ptr<OutputPort> port,
                                    const EthernetAddress& ethernet_address,
                                    const Address& ip_address )
  : name_( name )
  , port_( notnull( "OutputPort", move( port ) ) )
  , ethernet_address_( ethernet_address )
  , ip_address_( ip_address )
{
  cerr << "DEBUG: Network interface has Ethernet address " << to_string( ethernet_address ) << " and IP address "
       << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but
//! may also be another host if directly connected to the same network as the destination) Note: the Address type
//! can be converted to a uint32_t (raw 32-bit IP address) by using the Address::ipv4_numeric() method.
void NetworkInterface::send_datagram( const InternetDatagram& dgram, const Address& next_hop )
{
  uint32_t IPaddr = next_hop.ipv4_numeric();
  EthernetFrame frame;
  frame.header.src = ethernet_address_;

  if ( mapping.find( IPaddr ) != mapping.end() ) { // dest ethernet addr is already known
    frame.header.type = EthernetHeader::TYPE_IPv4;
    frame.header.dst = mapping[IPaddr].eaddr;
    frame.payload = serialize( dgram );
    transmit( frame );
  } else if ( ( datagrams_sent_.find( IPaddr ) == datagrams_sent_.end() )
              || datagrams_sent_[IPaddr].time < time ) { // make sure not flooding network
    ARPMessage myARP { .opcode = ARPMessage::OPCODE_REQUEST,
                       .sender_ethernet_address = ethernet_address_,
                       .sender_ip_address = ip_address_.ipv4_numeric(),
                       .target_ip_address = IPaddr };
    frame.payload = serialize( myARP );
    frame.header.type = EthernetHeader::TYPE_ARP;
    frame.header.dst = ETHERNET_BROADCAST;
    dgramwtime qdgram { time + static_cast<uint64_t>( 5000 ), dgram };
    datagrams_sent_[IPaddr] = qdgram; // queue datagram
    transmit( frame );
  }
}

//! \param[in] frame the incoming Ethernet frame
void NetworkInterface::recv_frame( const EthernetFrame& frame )
{
  if ( frame.header.dst != ETHERNET_BROADCAST
       && frame.header.dst != ethernet_address_ ) { // ignore any frames not destined for net interface
    return;
  } else if ( frame.header.type == EthernetHeader::TYPE_IPv4 ) {
    InternetDatagram dgram;
    parse( dgram, frame.payload ) ? datagrams_received_.push( dgram ) : void();
  } else {
    ARPMessage recARP;
    if ( !parse( recARP, frame.payload ) ) {
      return;
    }
    eaddrwtime eaddr { time + static_cast<uint64_t>( 30000 ), recARP.sender_ethernet_address };
    mapping[recARP.sender_ip_address] = eaddr; // learn ip to ethernet mappings

    if ( recARP.opcode == ARPMessage::OPCODE_REQUEST
         && recARP.target_ip_address == ip_address_.ipv4_numeric() ) { // request asking for our ip addr
      ARPMessage sendARP { .opcode = ARPMessage::OPCODE_REPLY,
                           .sender_ethernet_address = ethernet_address_,
                           .sender_ip_address = ip_address_.ipv4_numeric(),
                           .target_ethernet_address = recARP.sender_ethernet_address,
                           .target_ip_address = recARP.sender_ip_address };
      EthernetFrame replyframe { .payload = serialize( sendARP ) };
      replyframe.header.type = EthernetHeader::TYPE_ARP;
      replyframe.header.dst = recARP.sender_ethernet_address, replyframe.header.src = ethernet_address_;
      transmit( replyframe );
    } else if ( datagrams_sent_.find( recARP.sender_ip_address )
                != datagrams_sent_.end() ) { // reply is within our dg queue
      send_datagram( datagrams_sent_[recARP.sender_ip_address].dgram,
                     Address::from_ipv4_numeric( recARP.sender_ip_address ) );
      datagrams_sent_.erase( recARP.sender_ip_address );
    }
  }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
  time += ms_since_last_tick;
  if ( time >= 30000 && !mapping.empty() ) {
    for ( auto it = mapping.begin(); it != mapping.end(); ) {
      if ( it->second.time <= time ) { // check if 30 seconds have expired
        it = mapping.erase( it );      // Delete the element and advance the iterator
      } else {
        ++it;
      }
    }
  }
}
