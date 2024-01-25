Checkpoint 1 Writeup
====================

My name: [Vladimir Gonzalez Migal]

My SUNet ID: [vladgm24]

I collaborated with: []

I would like to thank/reward these classmates for their help: [siwooahn, moserj]

This lab took me about [12] hours to do. I [did] attend the lab session.

I was surprised by or edified to learn that: [I could use a string for my buffer]

Describe Reassembler structure and design. I used as string for my buffer's data structure, keeping an additional binary string to represent which bytes in the original buffer had been written to. I also kept an integer to keep track of the first unassembled index as well as the last byte in the byte stream. At a high level my design keeps both the buffer string and its binary string at the available capacity while 
constantly updating the value of the current index as new strings come in. I also used the number of 1s (byte written to) in the binary string to figure out how many bytes were pending in the buffer. An alternative I considered was using a linked list to keep track of the strings in the buffer. One benefit of this approach would have been imporved efficiency when calculating the number of bytes in the buffer. However, one advantage in my approach is not having to keep track of which pointers correspond to which indices and importantly not having to free pointers. 

Implementation Challenges:
[]

Remaining Bugs:
[]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I'm not sure about: [describe]
