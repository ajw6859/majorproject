/*
what is this? (3-5)

why did you make it?

what was the hardest part?
what did you learn?

how it works? (usage and functionality, as if you were gonna run it)
*/

This program is a memory allocator in the lanuage C. I wrote my own versions of
malloc(), calloc(), realloc(), and free(). It uses pthread in the hopes of keeping the
program threadsafe. I made it because I really enjoy programming in C but I have not had
as much opportunity to do it in college. Also while I have programmed with C in the past,
I have not done anything to this degree and I was unfamiliar with a lot of these concepts.
Doing this project helped me learn more about how data is stored and how powerful it is to
be able to manage your memory. The most difficult aspect of this project was understanding
threads and understanding how to directly interact with the heap. The code is writen in C
and takes no parameters for the main function. The tests in the main are just there to make
sure each function works and if run with valgrind, you can see that there are no memory
leaks :) I keep track of all the allocated memory by storing it in a linked list, and marking
that block as free or not free. All the data is stored on the heap, and no block of memory
can be acquired unless the lock is obtained. Once the memory allocation is completed the block
is marked as not free and the thread is released. Overall it was a really fun and informative
project <3

