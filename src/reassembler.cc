#include "reassembler.hh"
#include <iostream>

using namespace std;

#define min( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{ 
  max_index = cur_index + writer().available_capacity(); 
  if ( max_index > cur_index + unassembled_buf.length() ) { // add white space to buffer
    unassembled_buf += std::string( ( max_index - (cur_index + unassembled_buf.length())  ), ' ' );
    string_bmap += std::string( ( max_index - (cur_index + string_bmap.length())  ), '0' );
  }
  if (first_index < cur_index && (first_index + data.length() ) > cur_index ){   // 'scoot first index' to current index 
    data.erase(0,(cur_index - first_index)); 
    first_index = cur_index;
  }
  if (is_last_substring) {last_indx = first_index + data.length();} 

  if ( first_index == cur_index && first_index < max_index) { // add next bytes in the stream
    cur_index = first_index + min( data.length(), writer().available_capacity() );
    unassembled_buf.erase(0,  min( data.length(), writer().available_capacity() ) );
    string_bmap.erase(0,  min( data.length(), writer().available_capacity() ) );
  
    int count = 0;
    for ( char c : string_bmap ) { 
      if ( c == '1' ) {
        cur_index++;
        count++;
      } else {
        break;
      }
    }

    data += unassembled_buf.substr(0,count);
    unassembled_buf.erase(0,count);
    string_bmap.erase(0,count );
    output_.writer().push( data );

  } else if ( first_index > cur_index && first_index < ( cur_index + writer().available_capacity() ) ) { // add bytes to buffer
    long unsigned int start_inx = first_index - cur_index;
    if ( start_inx + data.length() > max_index ) { // if the string is too long clip the end
      data.erase((max_index - start_inx));
    }
    unassembled_buf = unassembled_buf.replace( start_inx, data.length(), data );
    string_bmap = string_bmap.replace( start_inx, data.length(), std::string( data.length(), '1' ) );
  }

  if ( (last_indx || is_last_substring) && last_indx == cur_index) { // close stream check
    output_.writer().close();
  }
}

uint64_t Reassembler::bytes_pending() const
{
  int count = 0;
  for ( char c : string_bmap ) {
    if ( c == '1' ) { count += 1;}
  }
  return count;
}
