#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

int list_dir(const char *path, const char *filter)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    struct stat statbuf = {};
    char fullPath[512];
    if (path == NULL)
    {
        perror("invalid directory path");
        return -1;
    }
    dir = opendir(path);
    if (dir == NULL)
    {
        perror("Could not open file");
        return -1;
    }
    printf("SUCCESS\n");
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        int is_dir = 0;
        snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
        if (lstat(fullPath, &statbuf) == 0)
        {
            is_dir = S_ISDIR(statbuf.st_mode);
        }
        if (filter == NULL)
        {
            printf("%s\n", fullPath);
                }
        else if (strcmp(filter, "has_perm_write") == 0)
        {
            if (access(fullPath, W_OK) == 0 && (S_ISREG(statbuf.st_mode) || is_dir))
            {
                printf("%s\n", fullPath);
            }
        }
        else if (strstr(entry->d_name, filter) != NULL)
        {
            printf("%s\n", fullPath);
        }

        memset(fullPath, 0, sizeof(fullPath));
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
        perror("Could not open directory");
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
            if (access(fullPath, W_OK) == 0)
            {
                printf("%s\n", fullPath);
            }
        }
        else if (strstr(entry->d_name, filter) != NULL)
        {
            printf("%s\n", fullPath);
        }
        if(lstat(fullPath, &statbuf) == 0)
        {
            if(S_ISDIR(statbuf.st_mode))
            {
                list_dir_rec(fullPath,filter);
            }
        }
        memset(fullPath, 0, sizeof(fullPath));
    }
    closedir(dir);
}

int main(int argc, char **argv)
{
    char *recursive = NULL;
    char *filter = NULL;
    char *path = NULL;
    path = malloc(512 * sizeof(char));
    filter = malloc(512 * sizeof(char));
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
                else if (strstr(argv[i], "="))
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
        free(path);
        free(filter);
    }

    return 0;
}
