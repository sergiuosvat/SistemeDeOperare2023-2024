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
    char fullPath[512] = {};
    struct stat statbuf;
    if (path == NULL)
    {
        puts("ERORR");
        puts("invalid directory path");
        return -1;
    }
    dir = opendir(path);
    if (dir == NULL)
    {
        puts("ERORR");
        puts("Could not open file");
        return -1;
    }
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
            if(lstat(fullPath, &statbuf) == 0)
            {
                if(statbuf.st_uid == getuid() && (statbuf.st_mode & S_IWUSR) != 0)
                {
                    printf("%s\n", fullPath);
                }
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
            if(lstat(fullPath, &statbuf) == 0)
            {
                if(statbuf.st_uid == getuid() && (statbuf.st_mode & S_IWUSR) != 0)
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
        memset(fullPath, 0, sizeof(fullPath));
    }
    closedir(dir);
}

int parse(const char* path)
{
    FILE* fp;
    fp = fopen(path,"r");
    char magic[3];
    int hs;
    if(fp == NULL)
    {
        puts("ERORR");
        puts("Could not open file");
        return -1;
    }
    fseek(fp,-1,SEEK_END);
    magic[0] = fgetc(fp);
    fseek(fp,-2,SEEK_END);
    magic[1] = fgetc(fp);
    if(magic[0] != 'I' || magic[1] != 'Z')
    {
        puts("ERROR");
        puts("wrong magic");
    }
    fseek(fp,-3,SEEK_END);
    fscanf(fp,"%d",&hs);
    
    fclose(fp);
    return 0;
}

int main(int argc, char **argv)
{
    char *recursive = NULL;
    char *filter = (char *)calloc(512, sizeof(char));
    char *path = (char *)calloc(512, sizeof(char));
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
        if(strcmp(argv[1], "parse") == 0)
        {
            strcpy(path, argv[2]+5);
            parse(path);
        }
        free(path);
        free(filter);
    }

    return 0;
}
