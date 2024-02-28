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
  EthernetHeader frameHeader;
  frameHeader.src = ethernet_address_;
  if (mapping.find(IPaddr) != mapping.end() ) { // dest ethernet addr is already known
    frameHeader.type = EthernetHeader::TYPE_IPv4;
    frameHeader.dst = mapping[IPaddr].eaddr;
    frame.header = frameHeader;
    frame.payload = serialize(dgram);
    transmit(frame);

  // pop queue
  } else if (last_arp != next_hop || time > 5000 ) { // broadcoast an ARP request // do modulo division for time
    ARPMessage myARP; 
    myARP.opcode = ARPMessage::OPCODE_REQUEST;
    myARP.sender_ip_address = ip_address_.ipv4_numeric(),myARP.sender_ethernet_address = ethernet_address_;
    myARP.target_ip_address = IPaddr;
    frameHeader.dst = ETHERNET_BROADCAST;
    frame.payload = serialize(myARP);
    frameHeader.type = EthernetHeader::TYPE_ARP;
    frame.header = frameHeader;
    datagrams_sent_[IPaddr] = dgram;   // queue datagram 
    last_arp = next_hop;
    time = 0;
    transmit(frame);
    }
  
  // don't flood ARP 
  

}

//! \param[in] frame the incoming Ethernet frame
void NetworkInterface::recv_frame( const EthernetFrame& frame )
{
  // Your code here.
  if (frame.header.dst == ETHERNET_BROADCAST || frame.header.dst == ethernet_address_){
    if (frame.header.type == EthernetHeader::TYPE_IPv4){
      InternetDatagram mydg;
      bool success = parse(mydg, frame.payload);
      if (success){
        datagrams_received_.push(mydg);
      }
    } else {
      // parse 
      ARPMessage inboundARP;
      bool success = parse(inboundARP, frame.payload);
      if (success){
        eaddrtime myInstance{time, inboundARP.sender_ethernet_address};
        mapping[inboundARP.sender_ip_address] = myInstance;
        if (inboundARP.opcode == ARPMessage::OPCODE_REPLY ){ // what happens if I get the reply for the 2nd one b4 the first, should I do nothing as I am waiting for the second one
          if (datagrams_sent_.find(inboundARP.sender_ip_address) != datagrams_sent_.end()){
            send_datagram(datagrams_sent_[inboundARP.sender_ip_address], Address::from_ipv4_numeric(inboundARP.sender_ip_address) );
          }
        }        
        if (inboundARP.opcode == ARPMessage::OPCODE_REQUEST && inboundARP.target_ip_address == ip_address_.ipv4_numeric()){
          EthernetFrame outframe;
          EthernetHeader frameHeader;
          ARPMessage outboundARP;
          outboundARP.opcode = ARPMessage::OPCODE_REPLY;
          outboundARP.sender_ip_address = ip_address_.ipv4_numeric(),outboundARP.sender_ethernet_address = ethernet_address_;
          outboundARP.target_ip_address = inboundARP.sender_ip_address, outboundARP.target_ethernet_address = inboundARP.sender_ethernet_address;
          outframe.payload = serialize(outboundARP);
          frameHeader.type = EthernetHeader::TYPE_ARP;
          frameHeader.dst = inboundARP.sender_ethernet_address;
          frameHeader.src = ethernet_address_;
          outframe.header = frameHeader;
          transmit(outframe);
        }
      }
    }
  }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
  time += ms_since_last_tick;
  if (time >= 30000 && !mapping.empty()){
    for (auto it = mapping.begin(); it != mapping.end();) {
        if (it->second.time + 30000 <= time ) {
            it = mapping.erase(it); // Delete the element and advance the iterator
        } else {
            ++it; // Move to the next element
        }
    }
  }
}
