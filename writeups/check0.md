Checkpoint 0 Writeup
====================

My name: [vladimir gonzalez migal]

My SUNet ID: [vladgm24]

I collaborated with: []

I would like to credit/thank these classmates for their help: [siwooahn]

This lab took me about [7] hours to do. I [did] attend the lab session.

My secret code from section 2.1 was: [289645]

I was surprised by or edified to learn that: [the bytestream can be represented using a string]

Describe ByteStream implementation. [
I used a string to represent the bytestream buffer, and unsigned longs to represent the cumulative totals of bytes pushed and popped. Every new piece of data pushed got concatened to the end of the existing buffer and I popped off characters in a first in first out matter. I could have alternatively implemented the buffer as a queue of chars, which would have had the advantage of improved efficiency when appending since I would not need to copy the existing string like I need to do in concatenation. However, a benefit of my approach is the implementation time since the data that comes in is already a string. Another alternative design choice I could have made is a LIFO queue instead of my FIFO queue. However, in this scenario I think the advantage of a FIFO queue is that the reader will most likely care about the beginning characters when they are not able to read the entire buffer.  

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I contributed a new test case that catches a plausible bug
  not otherwise caught: [provide Pull Request URL]