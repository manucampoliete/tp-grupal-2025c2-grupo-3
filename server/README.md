
---

First commit:

Hi! I'm Manu and I'm in charge of protocol and server threads in this project. I'll start simple, and then I'll improve things as we need them to be improved.

As you can see I've implemented a super dummy protocol in common/protocol/dummy_protocol.h and I'll use it to test initial communication.

On the other hand, about the server threads, I'll start with the most simple thing: the server accepts only one client. So no more threads rather than the main thread are needed for now. The server accepts only one connection, talks with this client a little bit, and then shuts down.

In addition to the dummy protocol, I'll code a brief client that creates a Socket (common/socket/socket.h) and connects to the server (to test things without depending on my teammates work). This will be another binary separated from the server binary and I'll put it in client/ directory. Of course, that is temporary. It is just for testing purposes. In the future it'll be erased.

---

Second commit:

Let's spice things up. Let's accept multiple clients, BUT one at at time. In order to visualize better how the server handles one client, let's do something better than sending two messages, let's do an echo server that talks with a client until it closes the connection (entering 'exit' via stdin).

--- 

Third commit:

Next step is to accept multiple clients, but also to be able to communicate with them at the same time, not sequentially. The communication with each client will remain the same, synchronous. But now we will need more threads in the server. We will have n + 2 threads, being n the amount of clients connected at a given moment. We will have the main thread, the acceptor thread, and then one thread per client. The main thread will launch the acceptor thread and then will block waiting for a 'q' via stdin to shutdown the server. The acceptor thread will accept new connections and launch a new thread for each of them, that will be in charge of the communication with that client that has just arrived. And for now, the communications will be synchronous.

Note: an incremental id will be asigned to each new client, in order to distinguish them.

--- 

Fourth commit:

Now we need asynchronous communication, and I think we need a little bussiness logic to make make sense. Let's improve a little bit the protocol. In the final game we will have cars, and those cars will want to move in some of the 4 directions: up, down, left or right. Of course they can move diagonally, when they want to turn for example. But the keys are 4: W, S, A and D. Or the arrows. So we can have a very simple protocol in which we ask the server to move. I think of the message:

COD_MOVE \<directions-byte>

Where \<directions-byte> encodes which of W, S, A or D keys are pressed (or released) in its last 4 bits, in that order. Each time one of these keys is pressed or released, a request to the server is sent. For example, if I start pressing W, this request will be sent to the server:

COD_MOVE 0b00001000

Note that the fifth bit encodes W pressed (1) or W released (0). Same for S (sixth), A (seventh) and D (eighth).

Later on, If I start pressing D (while maintaining W pressed as well) this request will be sent to the server:

COD_MOVE 0b00001001

Finally, If I stop pressing W (because my objective was to turn right) this request will be sent to the server:

COD_MOVE 0b00000001

And so on...

So the bussiness logic will be to mantain client cars positions and to modify them through client requests.

Clients will send move requests, but the server? What can the server send to their clients? Well, a broadcast of all car positions. The message can be:

COD_POSITIONS \<n> \<id1> \<x1> \<y1> ... \<idn> \<xn> \<yn>

For now, I'm handling only one match! Then we will have to allow multiple matches.

--- 

