/*
 * Shell Redirection:
 *
 * ./tucat f1.txt f2.txt > combined.txt
 * The shell opens combined.txt for output and redirects file descriptor 1
 * (standard output) to that file before tucat begins executing. Therefore,
 * tucat continues writing to STDOUT_FILENO, but the bytes go to combined.txt
 * instead of the terminal.
 *
 * ./tucat f1.txt f2.txt >> appended.txt
 * The shell opens appended.txt in append mode and redirects file descriptor 1
 * to it before tucat starts. Writes to STDOUT_FILENO are therefore appended
 * to the file.
 *
 * ./tucat f1.txt 2> errors.txt
 * The shell redirects file descriptor 2 (standard error) to errors.txt before
 * tucat starts. Standard output remains unchanged, while error messages written
 * to standard error, including those from perror(), go to errors.txt.
 *
 * ./tucat f1.txt > /dev/null
 * The shell redirects file descriptor 1 to /dev/null before tucat starts.
 * tucat still writes normally to STDOUT_FILENO, but /dev/null discards the
 * data.
 *
 * The tucat program itself does not implement shell redirection. The shell
 * changes the relevant file descriptors before executing the program.
 */

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char buffer[4096];
    ssize_t bytes_read;

    /*
     * If there are no filename arguments, read from standard input.
     */
    if (argc == 1) {

        while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {

            ssize_t bytes_written =
                write(STDOUT_FILENO, buffer, bytes_read);

            if (bytes_written == -1 || bytes_written != bytes_read) {
                perror("tucat: write");
                return 1;
            }
        }

        if (bytes_read == -1) {
            perror("tucat: read");
            return 1;
        }
    }

    /*
     * Otherwise, process every filename supplied on the command line.
     */
    else {

        for (int i = 1; i < argc; i++) {

            int fd = open(argv[i], O_RDONLY);

            if (fd == -1) {
                perror("tucat: open");
                return 1;
            }

            while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {

                ssize_t bytes_written =
                    write(STDOUT_FILENO, buffer, bytes_read);

                if (bytes_written == -1 || bytes_written != bytes_read) {
                    perror("tucat: write");
                    close(fd);
                    return 1;
                }
            }

            if (bytes_read == -1) {
                perror("tucat: read");
                close(fd);
                return 1;
            }

            if (close(fd) == -1) {
                perror("tucat: close");
            }
        }
    }

    return 0;
}