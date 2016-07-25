UDP sockets with Select

1. server.c
implementation of a simple echo server using UDP and select,The server gets a message from the client, change it to upper case and returns it back to the specific client.

2. slist.c
implemintation of slist.h, reprisents a brief Implementation of a very simple single linked list.

3. slist.h
slist.c header file for a brith simple link list.

note: Valgrind checked, no memory leacs or errors what so ever.
compile CMD: gcc -o server server.c slist.c