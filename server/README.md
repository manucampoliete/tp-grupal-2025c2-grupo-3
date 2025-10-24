
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

