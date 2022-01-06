#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(const char *path, const char *name)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        exit(1);
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find cannot stat %s\n", path);
        close(fd);
        exit(1);
    }

    switch (st.type) {
    case T_FILE:
        fprintf(2, "useage: find <directory> <name>\n");
        break;
    
    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            fprintf(2, "find: path too long\n");
            break;
        }

        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
                continue;
            }

            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0) {
                fprintf(2, "find: cannot stat %s\n", buf);
                continue;
            }

            if (st.type == T_FILE) {
                if (strcmp(de.name, name) == 0) {
                    printf("%s\n", buf);
                }
            } else if (st.type == T_DIR) {
                find(buf, name);
            }

        }
    }
    close(fd);
}

int
main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("useage: find <directory> <name>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}