#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

typedef struct sec_header
{
    char name[15];
    int type;
    int offset;
    int size;
} sec_header;

int list_dir(const char *path, const char *filter)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512] = {};
    struct stat statbuf;
    if (path == NULL)
    {
        puts("ERORR");
        puts("invalid directory path");
        return -1;
    }
    dir = opendir(path);
    printf("SUCCESS\n");
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
        if (filter == NULL)
        {
            printf("%s\n", fullPath);
        }
        else if (strcmp(filter, "has_perm_write") == 0)
        {
            if (lstat(fullPath, &statbuf) == 0)
            {
                if (statbuf.st_uid == getuid() && (statbuf.st_mode & S_IWUSR) != 0)
                {
                    printf("%s\n", fullPath);
                }
            }
        }
        else if (strstr(entry->d_name, filter) != NULL)
        {
            printf("%s\n", fullPath);
        }
    }
    closedir(dir);
    return 0;
}

void list_dir_rec(const char *path, const char *filter)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf = {};

    dir = opendir(path);
    if (dir == NULL)
    {
        puts("ERORR");
        puts("Could not open directory");
        return;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
        if (strcmp(filter, "has_perm_write") == 0)
        {
            if (lstat(fullPath, &statbuf) == 0)
            {
                if (statbuf.st_uid == getuid() && (statbuf.st_mode & S_IWUSR) != 0)
                {
                    printf("%s\n", fullPath);
                }
            }
        }
        else if (strstr(entry->d_name, filter) != NULL)
        {
            printf("%s\n", fullPath);
        }
        if (lstat(fullPath, &statbuf) == 0)
        {
            if (S_ISDIR(statbuf.st_mode))
            {
                list_dir_rec(fullPath, filter);
            }
        }
    }
    closedir(dir);
}

int parse(const char *path)
{
    int fd, version, nr_sect = 0;
    short hs;
    fd = open(path, O_RDONLY);
    char magic[3];
    if (fd == -1)
    {
        puts("ERORR");
        puts("Could not open file");
        return -1;
    }
    lseek(fd, -1, SEEK_END);
    read(fd, &magic[0], 1);
    lseek(fd, -2, SEEK_END);
    read(fd, &magic[1], 1);
    if (magic[0] != 'I' || magic[1] != 'Z')
    {
        puts("ERROR");
        puts("wrong magic");
        return -1;
    }
    lseek(fd, -4, SEEK_END);
    read(fd, &hs, 2);
    lseek(fd, -hs, SEEK_END);
    read(fd, &version, 1);
    if (version < 23 || version > 65)
    {
        puts("ERROR");
        puts("wrong version");
        return -1;
    }
    read(fd, &nr_sect, 1);
    if (nr_sect < 6 || nr_sect > 14)
    {
        puts("ERROR");
        puts("wrong sect_nr");
        return -1;
    }
    sec_header *arr = calloc(nr_sect, sizeof(sec_header));
    for (int i = 0; i < nr_sect; i++)
    {
        read(fd, arr[i].name, 15);
        read(fd, &arr[i].type, 2);
        read(fd, &arr[i].offset, 4);
        read(fd, &arr[i].size, 4);
        if (arr[i].type != 51 && arr[i].type != 69 && arr[i].type != 63 && arr[i].type != 45 && arr[i].type != 53 && arr[i].type != 90 && arr[i].type != 40)
        {
            puts("ERROR");
            puts("wrong sect_types");
            free(arr);
            return -1;
        }
    }
    printf("SUCCESS\n");
    printf("version=%d\n", version);
    printf("nr_sections=%d\n", nr_sect);
    for (int i = 0; i < nr_sect; i++)
    {
        printf("section%d: %s %d %d\n", i + 1, arr[i].name, arr[i].type, arr[i].size);
    }
    free(arr);
    close(fd);
    return 0;
}

int extract(const char *path, int nr, int line)
{
    int fd1, version1 = 0, nr_sect1 = 0, hs1 = 0;
    int nr_lines = 1;
    off_t off;
    fd1 = open(path, O_RDONLY);
    if (fd1 == -1)
    {
        puts("ERROR");
        puts("invalid file");
    }
    lseek(fd1, -4, SEEK_END);
    read(fd1, &hs1, 2);
    lseek(fd1, -hs1, SEEK_END);
    read(fd1, &version1, 1);
    read(fd1, &nr_sect1, 1);
    if (nr > nr_sect1)
    {
        puts("ERROR");
        puts("invalid section");
    }
    sec_header *array = calloc(nr_sect1 + 1, sizeof(sec_header));
    for (int i = 0; i < nr_sect1; i++)
    {
        read(fd1, array[i + 1].name, 15);
        read(fd1, &array[i + 1].type, 2);
        read(fd1, &array[i + 1].offset, 4);
        read(fd1, &array[i + 1].size, 4);
    }
    off = lseek(fd1, array[nr].offset + array[nr].size, SEEK_SET);
    lseek(fd1, off, SEEK_SET);
    char *buffer = calloc(1, sizeof(char));
    while (line != nr_lines)
    {
        read(fd1, buffer, 1);
        lseek(fd1, -2, SEEK_CUR);
        if (*buffer == '\n')
        {
            nr_lines++;
        }
    }
    buffer[0] = 0;
    if (nr_lines != line)
    {
        puts("ERROR");
        puts("invalid line");
        return -1;
    }
    puts("SUCCESS");
    lseek(fd1, 1, SEEK_CUR);
    while (lseek(fd1, -2, SEEK_CUR) > (array[nr].offset - 1) && buffer[0] != '\n')
    {
        read(fd1, buffer, 1);
        printf("%c", buffer[0]);
    }
    free(array);
    free(buffer);
    close(fd1);
    return 0;
}

int findall(const char *path, int flag)
{
    if (path == NULL)
    {
        puts("ERROR");
        puts("Invalid directory path");
        return -1;
    }
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512] = {};
    dir = opendir(path);
    struct stat statbuf = {};
    int fd1, nr_sect1 = 0, version1 = 0, nr_lines = 1;
    short hs1;
    off_t off;
    if (flag)
    {
        puts("SUCCESS");
        flag = 0;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
        if (lstat(fullPath, &statbuf) == 0)
        {
            if (S_ISDIR(statbuf.st_mode))
            {
                findall(fullPath, flag);
            }
            else
            {
                fd1 = open(fullPath, O_RDONLY);
                char magic[3];
                lseek(fd1, -1, SEEK_END);
                read(fd1, &magic[0], 1);
                lseek(fd1, -2, SEEK_END);
                read(fd1, &magic[1], 1);
                if (magic[0] != 'I' || magic[1] != 'Z')
                {
                    continue;
                }
                lseek(fd1, -4, SEEK_END);
                read(fd1, &hs1, 2);
                lseek(fd1, -hs1, SEEK_END);
                read(fd1, &version1, 1);
                if (version1 < 23 || version1 > 65)
                {
                    continue;
                }
                read(fd1, &nr_sect1, 1);
                if (nr_sect1 < 6 || nr_sect1 > 14)
                {
                    continue;
                }
                sec_header *array = calloc(nr_sect1 + 1, sizeof(sec_header));
                for (int i = 0; i < nr_sect1; i++)
                {
                    read(fd1, array[i + 1].name, 15);
                    read(fd1, &array[i + 1].type, 2);
                    read(fd1, &array[i + 1].offset, 4);
                    read(fd1, &array[i + 1].size, 4);
                }
                for (int i = 0; i < nr_sect1; i++)
                {
                    off = lseek(fd1, array[i + 1].offset + array[i + 1].size, SEEK_SET);
                    lseek(fd1, off, SEEK_SET);
                    char *buffer = calloc(1, sizeof(char));
                    while (off >= array[i + 1].offset)
                    {
                        read(fd1, buffer, 1);
                        lseek(fd1, -2, SEEK_CUR);
                        if (*buffer == '\n')
                        {
                            nr_lines++;
                        }
                        off--;
                    }
                    free(buffer);
                    if (nr_lines > 13)
                    {
                        printf("%s\n", fullPath);
                        break;
                    }
                }
                free(array);
                close(fd1);
            }
        }
    }
    closedir(dir);
    return 0;
}

int main(int argc, char **argv)
{
    char *recursive = NULL;
    char *filter = (char *)calloc(512, sizeof(char));
    char *path = (char *)calloc(512, sizeof(char));
    char *sect = (char *)calloc(512, sizeof(char));
    char *line = (char *)calloc(512, sizeof(char));
    int flag = 1;
    if (argc >= 2)
    {
        if (strcmp(argv[1], "variant") == 0)
        {
            printf("78380\n");
        }
        if (strcmp(argv[1], "list") == 0)
        {
            for (int i = 2; i < argc; i++)
            {
                if (strcmp(argv[i], "recursive") == 0)
                {
                    recursive = argv[i];
                }
                else if (strncmp(argv[i], "path=", 5) == 0)
                {
                    strcpy(path, argv[i] + 5);
                }
                else if (strstr(argv[i], "name_ends_with="))
                {
                    strcpy(filter, argv[i] + 15);
                }
                else if (strstr(argv[i], "_"))
                {
                    strcpy(filter, argv[i]);
                }
            }
            if (recursive == NULL)
            {
                list_dir(path, filter);
            }
            else
            {
                printf("SUCCESS\n");
                list_dir_rec(path, filter);
            }
        }
        if (strcmp(argv[1], "parse") == 0)
        {
            strcpy(path, argv[2] + 5);
            parse(path);
        }
        if (strcmp(argv[1], "extract") == 0)
        {
            strcpy(path, argv[2] + 5);
            strcpy(sect, argv[3] + 8);
            strcpy(line, argv[4] + 5);
            extract(path, atoi(sect), atoi(line));
        }
        if (strcmp(argv[1], "findall") == 0)
        {
            strcpy(path, argv[2] + 5);
            findall(path, flag);
        }
        free(path);
        free(filter);
        free(sect);
        free(line);
    }

    return 0;
}
