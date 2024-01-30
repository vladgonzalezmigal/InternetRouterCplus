#include "wrapping_integers.hh"
#include <iostream>

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  return Wrap32 { zero_point + n };
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  Wrap32 check_seq_no = Wrap32::wrap( checkpoint, zero_point ); // get checkpoint seq_no
  uint32_t sub_dif = check_seq_no.raw_value_ - raw_value_;
  uint32_t sum_dif = raw_value_ - check_seq_no.raw_value_;
  if ( sub_dif < sum_dif && ( sub_dif <= checkpoint ) ) {
    return checkpoint - static_cast<uint64_t>( sub_dif );
  }
  return checkpoint + static_cast<uint64_t>( sum_dif );
}
