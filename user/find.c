#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void find(char *path, char *file)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0)
    {
        fprintf(2, "find: cannot find path %s\n", path);
        exit(1);
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        exit(1);
    }

    if (st.type != T_DIR)
    {
        fprintf(2, "find: %s is not a directory\n", path);
        close(fd);
        exit(1);
    }


    // st.type == T_DIR
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
        fprintf(2, "find: path too long\n");
        close(fd);
        exit(1);
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/'; // append a slash to the end of the path
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, "..")) // not recurse into . and ..
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0) 
        {
            fprintf(1, "find: cannot stat %s\n", buf);
            continue;
        }
        if (st.type == T_FILE && !strcmp(de.name, file)) // it is a file and its name is the same as the file we are looking for
        {
            fprintf(1, "%s\n", buf);
        }
        else if (st.type == T_DIR)  // keep finding it recursively
        {
            find(buf, file);
        }
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "Usage: find <path> <file_name>\n");
        exit(1);
    }
    else if (argc == 2) // provide only the path
    {
        find(".", argv[1]);
    }
    else // provide the path and the file name
    {
        for (int i = 2; i <= argc; i++)
            find(argv[1], argv[i]);
    }

    exit(0);
}