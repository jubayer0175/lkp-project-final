       #define _GNU_SOURCE
       #include <unistd.h>
       #include <sys/syscall.h>
       #include <stdio.h>
       #include <sys/mman.h>
	#include <unistd.h>
	#include <stdio.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <string.h>
	#include <arpa/inet.h>
	#include<pthread.h>
	#include <sys/types.h>
	#include <linux/userfaultfd.h>
	#include <errno.h>
	#include <stdlib.h>
	#include <fcntl.h>
	#include <signal.h>
	#include <poll.h>
	#include <string.h>
	#include <sys/mman.h>
	#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <stdio_ext.h>




       static inline void
       wrpkru(unsigned int pkru)
       {
           unsigned int eax = pkru;
           unsigned int ecx = 0;
           unsigned int edx = 0;

           asm volatile(".byte 0x0f,0x01,0xef\n\t"
                        : : "a" (eax), "c" (ecx), "d" (edx));
       }

       int
       pkey_set_(int pkey, unsigned long rights, unsigned long flags)
       {
           unsigned int pkru = (rights << (2 * pkey));
           wrpkru(pkru);
	   return 0; // this is just to by pass compilation error for now
       }

       int
       pkey_mprotect_(void *ptr, size_t size, unsigned long orig_prot,
                     unsigned long pkey)
       {
           return syscall(SYS_pkey_mprotect, ptr, size, orig_prot, pkey);
       }

       int
       pkey_alloc_(void)
       {
           return syscall(SYS_pkey_alloc, 0, 0);
       }

       int
       pkey_free_(unsigned long pkey)
       {
           return syscall(SYS_pkey_free, pkey);
       }
       #define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                                  } while (0)

       int
       main(void)
       {
           int status;
           int pkey;
           int *buffer;

           /*
            *Allocate one page of memory
            */
           buffer = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE,
                         MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
           if (buffer == MAP_FAILED)
               errExit("mmap");

           /*
            * Put some random data into the page (still OK to touch)
            */
           *buffer = __LINE__;
           printf("buffer contains: %d\n", *buffer);

           /*
            * Allocate a protection key:
            */
           pkey = pkey_alloc_();

           if (pkey == -1)
               errExit("pkey_alloc");
	   else printf("pkey:%d\n", pkey);
           /*
            * Disable access to any memory with "pkey" set,
            * even though there is none right now
            */
           status = pkey_set_(pkey, PKEY_DISABLE_WRITE, 0);
           if (status)
               errExit("pkey_set");

           /*
            * Set the protection key on "buffer".
            * Note that it is still read/write as far as mprotect() is
            * concerned and the previous pkey_set() overrides it.
            */
           status = pkey_mprotect_(buffer, getpagesize(),
                                  PROT_READ | PROT_WRITE, pkey);
           if (status == -1)
               errExit("pkey_mprotect");

           printf("about to read buffer again...\n");




           /*
            * This will crash, because we have disallowed access
            */
           printf("buffer contains: %d\n", *buffer);

	   // try writing it
	   *buffer = 20;




           status = pkey_free_(pkey);
           if (status == -1)
               errExit("pkey_free");

           exit(EXIT_SUCCESS);
       }
