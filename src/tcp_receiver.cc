#include "tcp_receiver.hh"
#include <algorithm>

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  if (message.RST){
    reader().set_error();
    return;
  }
  if ( message.SYN ) {
    ISN = message.seqno;
  }
  if ( ISN.has_value() ) {
    reassembler_.insert( message.seqno.unwrap( ISN.value(), writer().bytes_pushed() ) + message.SYN - 1,
                         message.payload,
                         ( message.FIN ) );
  }
}

TCPReceiverMessage TCPReceiver::send() const
{
  TCPReceiverMessage message;
  if ( ISN.has_value() ) {
    message.ackno = message.ackno->wrap( writer().bytes_pushed() + 1 + writer().is_closed(), ISN.value() );
  }
  message.window_size = std::min( ( writer().available_capacity() ), static_cast<uint64_t>( UINT16_MAX ) );
  message.RST = reader().has_error();
  return message;
}
