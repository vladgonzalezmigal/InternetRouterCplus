#include "reassembler.hh"
#include <iostream>

using namespace std;

#define min( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  cout << "at start buf is " << unassembled_buf << "with length " << unassembled_buf.length() << "\n" << endl;
  long unsigned int max_indx = cur_index + writer().available_capacity();
  if ( max_indx > cur_index + unassembled_buf.length() ) { // if need to read white space
    unassembled_buf += std::string( ( max_indx - unassembled_buf.length() ), ' ' );
    string_bmap += std::string( ( max_indx - string_bmap.length() ), '0' );
  }

  if ( first_index == cur_index ) {
    cur_index = first_index + min( data.length(), writer().available_capacity() );
    unassembled_buf = unassembled_buf.substr( cur_index );
    string_bmap = string_bmap.substr( cur_index );

    int count = 0;
    for ( char c : string_bmap ) {
      if ( c == '1' ) {
        data += unassembled_buf[count];
        count++;
      } else {
        break;
      }
    }
    unassembled_buf = unassembled_buf.substr( count );
    string_bmap = string_bmap.substr( count );
    output_.writer().push( data );
  }
  // add stuff to buffer
  else if ( first_index > cur_index && first_index < ( cur_index + writer().available_capacity() ) ) {
    long unsigned int start_inx = first_index - cur_index;
    if ( start_inx + data.length() > max_indx ) { // if the string is 2 long clip it
      data = data.substr( 0, max_indx - start_inx );
    }
    unassembled_buf = unassembled_buf.replace( start_inx, data.length(), data );
    string_bmap = string_bmap.replace( start_inx, data.length(), std::string( data.length(), '1' ) );
  }
  if ( is_last_substring ) {
    output_.writer().close();
  };
}

uint64_t Reassembler::bytes_pending() const
{
  int count = 0;
  for ( char c : string_bmap ) {
    if ( c == '1' ) {
      count += 1;
    }
  }
  return count;
}
