#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>


int my_strlen(char *s) {
    int i = 0;
    while (s[i] != 0)
    {
        i++;
    }
    return i;
}

void error_msg(char *msg) {
    int len = my_strlen(msg); 
    write(STDERR_FILENO, msg, len);
    return;
}

void my_cp(char *rd_file, char *wr_file){
    int rd_fd = open(rd_file, O_RDONLY);
    if (rd_fd < 0)
    {
        error_msg("Error open readable file!\n");
        _exit(1);
    }
    
    int wr_fd = open(wr_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (wr_fd < 0)
    {
        error_msg("Error open writable file!\n");
        close(rd_fd);
        _exit(1);
    }
    void *buf = mmap(
        NULL,
        512,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE,
        -1,
        0
    );
    if (buf == MAP_FAILED)
    {
        error_msg("Mapping error!\n");
        close(rd_fd);
        close(wr_fd);
        _exit(1);
    }
    size_t read_bytes;
    while ((read_bytes = read(rd_fd, buf, 16)) > 0)
    {
        ssize_t written = 0;
        while (written < read_bytes) 
        {
            ssize_t result = write(wr_fd, buf + written, read_bytes - written);
            if (result < 0)
            {
                error_msg("Write error!\n");
                close(rd_fd);
                close(wr_fd);
                munmap(buf, 512);
                _exit(1);
            }
            written+=result;
        }
    }
    if (read_bytes < 0)
    {
        error_msg("Read error!\n");
        close(rd_fd);
        close(wr_fd);
        munmap(buf, 512);
        _exit(1);
    }
    close(rd_fd);
    close(wr_fd);
    munmap(buf, 512);
    error_msg("File copied!\n");
    return;
}

int main(int argc, char *argv[]) {
    my_cp(argv[1], argv[2]);
    return 0;
}