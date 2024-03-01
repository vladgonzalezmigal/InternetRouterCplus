#include "tcp_sender.hh"
#include "tcp_config.hh"
#include <algorithm>
#include <cmath>
#include <iostream>
using namespace std;

uint64_t TCPSender::sequence_numbers_in_flight() const
{
  // newest thing in the queue's last byte - oldest thing in the queue's first byte
  return ( outstanding_segs.empty() ) ? 0
                                      : ( abs_seqno - outstanding_segs.front().seqno.unwrap( isn_, abs_seqno ) );
}

uint64_t TCPSender::consecutive_retransmissions() const
{
  // Your code here.
  return num_retransmissions;
}

void TCPSender::push( const TransmitFunction& transmit )
{
  uint16_t pretend_window = ( !window_size ) ? 1 : window_size;

  while ( true ) {
    TCPSenderMessage message;
    message.seqno = message.seqno.wrap( abs_seqno, isn_ );
    message.SYN = ( abs_seqno == 0 ), message.RST = input_.has_error();
    size_t bytes_to_read
      = std::min<size_t>( TCPConfig::MAX_PAYLOAD_SIZE,
                          std::max<size_t>( pretend_window, TCPSender::sequence_numbers_in_flight() )
                            - ( message.SYN + TCPSender::sequence_numbers_in_flight() ) );
    read( input_.reader(), bytes_to_read, message.payload );
    abs_seqno += message.sequence_length();
    message.FIN = ( ( std::max<size_t>( pretend_window, TCPSender::sequence_numbers_in_flight() )
                        - TCPSender::sequence_numbers_in_flight() - message.sequence_length()
                      > 0 )
                    && reader().is_finished() && ( abs_seqno == reader().bytes_popped() + 1 ) );
    abs_seqno += message.FIN;
    if ( message.sequence_length() ) {
      if ( !timer.has_value() ) {
        timer = cur_RTO;
      }
      outstanding_segs.push( message );
      transmit( message );
    } else {
      break;
    }
    // left_window -=
  }
}

TCPSenderMessage TCPSender::make_empty_message() const
{
  TCPSenderMessage message;
  message.RST = input_.has_error();
  message.seqno = message.seqno.wrap( abs_seqno, isn_ );
  return message;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  if ( msg.RST ) {
    input_.set_error();
  }
  if ( msg.ackno.has_value() ) {
    uint64_t ackno = msg.ackno->unwrap( isn_, abs_seqno );
    if ( ackno > abs_seqno ) { // check for impossible seqnos
      return;
    }
    while ( !outstanding_segs.empty() ) {
      if ( outstanding_segs.front().seqno.unwrap( isn_, ackno ) + outstanding_segs.front().sequence_length()
           <= ackno ) {
        if ( !outstanding_segs.empty() ) {
          cur_RTO = initial_RTO_ms_;
          timer.value() = initial_RTO_ms_;
        }
        num_retransmissions = 0;
        outstanding_segs.pop();

      } else {
        break;
      }
    }
    if ( outstanding_segs.empty() ) {
      timer.reset();
    }
  }
  window_size = ( msg.window_size );
}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{

  if ( ms_since_last_tick >= timer && timer.has_value() ) {
    if ( window_size > 0 ) {
      num_retransmissions += 1;
    }
    cur_RTO = initial_RTO_ms_ * std::pow( 2, num_retransmissions );
    timer.value() = cur_RTO;
    transmit( outstanding_segs.front() );
  } else if ( timer.has_value() ) {
    timer.value() -= ms_since_last_tick;
  }
}
