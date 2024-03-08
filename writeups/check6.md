Checkpoint 6 Writeup
====================

My name: [vladimir gonzalez migal]

My SUNet ID: [vladgm24]

I collaborated with: [list sunetids here]

I would like to thank/reward these classmates for their help: [moserj]

This checkpoint took me about [4] hours to do. I [did not] attend the lab session.

Program Structure and Design of the Router [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]: [My add route function simply added all the parameters into a custom route struct that was composed of all of add's route parameters. For my route function I linearly iterated through each interface's queue of datagrams received. Then for each datagram in the queue I linearly iterated through my vector of routes and applied the longest-prefix match routing algorithm as described in the assignment handout. This vector was simply ordered by the order add route was called.  Another alternative I considered was storing the routes in a list sorted by the route prefix. A benefit of this alternative would have been improved speed when trying to find the matching route. However, a tradeoff would be the increased complexity involved in maintaining the sorted order of this vector.   ]

Implementation Challenges:
[na]

Remaining Bugs:
[na]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
