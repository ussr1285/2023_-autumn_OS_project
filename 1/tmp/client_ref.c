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
    char request[256], response[256];

    // Get user input
    char filename[256], access_type;
    int bytes;

    printf("Enter filename, access type (r/w), and bytes (for read): ");
    scanf("%s %c %d", filename, &access_type, &bytes);

    // Create request string
    sprintf(request, "%s %c %d", filename, access_type, bytes);

    // Open FIFO for writing
    fd = open(FIFO_PATH, O_WRONLY);

    // Send request to server
    write(fd, request, strlen(request) + 1);

    // Close FIFO
    close(fd);

    // Open FIFO for reading response
    fd = open(FIFO_PATH, O_RDONLY);

    // Read response from server
    read(fd, response, sizeof(response));

    // Close FIFO
    close(fd);

    // Process response
    printf("Server response: %s\n", response);

    return 0;
}
