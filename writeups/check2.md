Checkpoint 2 Writeup
====================

My name: [Vladimir Gonzalez Migal]

My SUNet ID: [vladgm24]

I collaborated with: []

I would like to thank/reward these classmates for their help: [jshen7, siwoohn]

This lab took me about [8] hours to do. I [did] attend the lab session.

Describe Wrap32 and TCPReceiver structure and design. [ For wrap32 in my 
unwrap function my approach was to find the two possible 'candidate' positions 
for the raw value and add or subtract the distance for whichever candidate
was smaller. However, in some cases it was only possible to add because subtracting
would imply going below the zero point. For my tcp receiver I used a std::optional wrap32 to keep track of the ISN. Once the ISN was set then I called insert with the payload data, using the number of bytes pushed to the bystream as my checkpoint (the first unassembled index in my reassebmler). I made sure to convert the sequence number in my insert call to a byte stream index, keeping in consideration whether or not 
the SYN flag was set. An alternative approach could have been using a wrap32 and a boolean
to check if the ISN had been set or not. By using an optional wrap32 I was able to save the 
need for an instance variable, but on the flip side I added a little bit of increased complexity.]

Implementation Challenges:
[unwrap function was tricky]

Remaining Bugs:
[none]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I made an extra test I think will be helpful in catching bugs: [describe where to find]
