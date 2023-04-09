For this project I present to you a functional quantum circuit emulator 
library in C++. It has functionalities similar to its equivalent in 
python, qiskit:
- It allows for relatively easy creation of circuits
- It allows for the drawing of those circuits
- It allows for simulation of quantum system, using the 'circuit' class 
and the '.measure' method of the 'state' class

All the code is written by me, using only standard libraries. I have also
included two examples, one being quantum teleportation and the second 
being quantum error correction.

I also want to share some of my implementation decisions.
First of all, the most notable difference between qiskit and my 
implementation is the lack of a measuring gate in the circuit library.
I did this for mainly 2 reasons:
- The measurement cannot be represented via linear algebra. Thus, any 
attempt to include it would require a proper 'simulation' of sorts, 
in complete contrast with the rest of my implementation, that 
precalculates everything and simply multiplies a matrix with a vector. 
I had plans to implement a wrapper class called 'timeline' that would
combine both, but for both for the lack of time left and the following
reason that did not end up happening.
- I noticed that the separation of the measurement gate allows for much
more flexibility. I personally prefer working at a lower level when I'm
able to and the separate act of measurement after a pass through a circuit
allows for easy implementations of general algorithms. For example, you can
see in the second example that I'm able to easily simulate noise on the
channel with 3 checks of a rand() call. If this ends up being used by someone
else more knowledgeable than me, I'm excited to see what they come up with.

Perhaps other interesting implementation choices include the 'norm_factor'
that I implemented as a small optimization to avoid normalising all the time,
and another one might be the global phase shift I execute on RX, RY, RZ.
Technically the global phase is irrelevant when describing a state as a vector,
but I also use those gates as a basis for CRX, CRY, CRZ, and those gates
require 0 global phase or else problems arise because of the method I used to
make them.

Also, relating the controlled gates, their implementation is pretty much
an educated guess after I looked a bit over the internet, and they seemed to
work fine. If someone has either a proof or a better way to do a generalised
Toffoli gate please contact me (information below)!

Regarding any other question or discussion please open an issue or if
you have my mail contact me directly. Thanks for your interest and I'm
interested to see what you create with this tool!
