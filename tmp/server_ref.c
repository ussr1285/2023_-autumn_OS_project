#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define FIFO_PATH "./myfifo"

int main()
{
    int fd;
    char request[256];

    // Create a FIFO (named pipe)
    mkfifo(FIFO_PATH, 0666);

    while (1)
    {
        // Open FIFO for reading
        fd = open(FIFO_PATH, O_RDONLY);

        // Read request from client
        read(fd, request, sizeof(request));

        // Close FIFO
        close(fd);

        // Process request in a child process
        if (fork() == 0)
        {
            char filename[256], access_type, response[256];
            int bytes;

            // Parse request
            sscanf(request, "%s %c %d", filename, &access_type, &bytes);

            // Perform file access based on request
            if (access_type == 'r')
            {
                // Read from file and prepare response
                // ...

                // Send response back to client
                int fd_response = open(FIFO_PATH, O_WRONLY);
                write(fd_response, response, strlen(response) + 1);
                close(fd_response);
            }
            else if (access_type == 'w')
            {
                // Write data to file and prepare response
                // ...

                // Send response back to client
                int fd_response = open(FIFO_PATH, O_WRONLY);
                write(fd_response, response, strlen(response) + 1);
                close(fd_response);
            }

            exit(0);
        }
    }

    return 0;
}
