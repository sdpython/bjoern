#include <stdlib.h>

#include "portable_sendfile.h"

#define SENDFILE_CHUNK_SIZE 16*1024

#if defined __APPLE__

  /* OS X */

  #include <sys/socket.h>
  #include <sys/types.h>

  Py_ssize_t portable_sendfile(int out_fd, int in_fd, off_t offset) {
    off_t len = SENDFILE_CHUNK_SIZE;
    if(sendfile(in_fd, out_fd, offset, &len, NULL, 0) == -1) {
      if((errno == EAGAIN || errno == EINTR) && len > 0) {
        return len;
      }
      return -1;
    }
    return len;
  }

#elif defined(__FreeBSD__) || defined(__DragonFly__)

  #include <sys/socket.h>
  #include <sys/types.h>

  Py_ssize_t portable_sendfile(int out_fd, int in_fd, off_t offset) {
    off_t len;
    if(sendfile(in_fd, out_fd, offset, SENDFILE_CHUNK_SIZE, NULL, &len, 0) == -1) {
      if((errno == EAGAIN || errno == EINTR) && len > 0) {
        return len;
      }
      return -1;
    }
    return len;
  }
  
#elif defined(_MSC_VER)

    #include <winsock.h>
    #include <stdio.h>
    #define BUF_SIZE 1024

    ssize_t
    sendfile(int out_fd, int in_fd, off_t *offset, size_t count)
    {
        off_t orig;
        char buf[BUF_SIZE];
        size_t toRead, numRead, numSent, totSent;

        if (offset != NULL) {

            /* Save current file offset and set offset to value in '*offset' */

            orig = _lseek(in_fd, 0, SEEK_CUR);
            if (orig == -1)
                return -1;
            if (_lseek(in_fd, *offset, SEEK_SET) == -1)
                return -1;
        }

        totSent = 0;

        while (count > 0) {
            toRead = min(BUF_SIZE, count);

            numRead = _read(in_fd, buf, toRead);
            if (numRead == -1)
                return -1;
            if (numRead == 0)
                break;                      /* EOF */

            numSent = _write(out_fd, buf, numRead);
            if (numSent == -1)
                return -1;
            if (numSent == 0)               /* Should never happen */
                fatal("sendfile: write() transferred 0 bytes");

            count -= numSent;
            totSent += numSent;
        }

        if (offset != NULL) {

            /* Return updated file offset in '*offset', and reset the file offset
               to the value it had when we were called. */

            *offset = _lseek(in_fd, 0, SEEK_CUR);
            if (*offset == -1)
                return -1;
            if (_lseek(in_fd, orig, SEEK_SET) == -1)
                return -1;
        }

        return totSent;
    }

#else

  /* Linux */

  #include <sys/sendfile.h>

  Py_ssize_t portable_sendfile(int out_fd, int in_fd, off_t offset) {
    return sendfile(out_fd, in_fd, &offset, SENDFILE_CHUNK_SIZE);
  }

#endif
