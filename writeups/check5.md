Checkpoint 5 Writeup
====================

My name: [vladimir gonzalez migal]

My SUNet ID: [vladgm24]

I collaborated with: []

I would like to thank/reward these classmates for their help: [moserj, siwooahn]

This checkpoint took me about [6] hours to do. I [did] attend the lab session.

Program Structure and Design of the NetworkInterface : [To keep track of the required ip to ethernet address
mappings I used a map with the keys being the ip address. However, since we had to keep track of the entries expiring after 30 seconds I
created a custom struct for the map's value that kept track of the time the entry should expire as well as 
the ethernet address. I also used a map for the datagrams that were waiting for an arp reply, with the keys being an ip address and the value being a 
datagram with the time it was allowed to retransmit (again a custom struct). An alternative I considered instead of using a map to keep track of my ip address to ethernet mappings was using a vector with a structure that held information about the ip address as well as the ethernet address (plus the time it was going to expire). One benefit of this approach is that I could have ordered the items in the vector in terms of expiry time, which could have made it faster when needing to deleting expired map entries. However, a downside of this approach is looking up appropriate ip to ethernet mappings is not as quick as using a map. 
The logic of my send_datagram method involved checking whether we could find the next hop's ethernet address. If we couldn't then we generated an ARP request. The logic of my recv_frame method involved checking whether the frame's header was intended for our network interface or not and then checking to see whether the header was of type IPV4 or an ARP message. In the case of an ARP message I wanted to check whether the ARP message was looking for our ethernet address or if instead the ARP reply existed in our 'queue' of datagrams. ]

Implementation Challenges:
[I struggled with thinking of a data structure that 
could keep track of the expiry times. ]

Remaining Bugs:
[I do not have any remaining bugs left]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
