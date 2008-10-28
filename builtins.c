/*
 * redshell - simple and light UNIX shell
 * builtins.c: builtin commands
 *
 * Copyright (C) 2008 Ricardo Martins <ricardo@scarybox.net>
 * See LICENSE or http://www.opensource.org/licenses/mit-license.php
 * for copyright and license details.
 */

#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>

#include "common.h"
#include "helper.h"

/* Print file properties */
static void print_file_info(char *name, char *path, struct stat s)
{
    lstat(path, &s);
    unsigned long size = s.st_size; /* Using long for supporting files >=4GiB */
    int inode = s.st_ino;
    int perm = s.st_mode & PERM_MASK;
    char type = S_ISREG(s.st_mode) ? 'F' : (S_ISDIR(s.st_mode) ? 'D' : 'U');
    char *owner = getusername(s.st_uid);
    char *group = getgroupname(s.st_gid);
    printf("%-30s %9lu bytes [%c %03o] [%s:%s] - %9d\n",
            name, size, type, perm, owner, group, inode );
}

/* String comparison for quicksort */
static int string_cmp(const void *a, const void *b)
{
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
}

/* List directory contents and/or file properties */
int cmd_listar(char **argv)
{
    /*
     * TODO:
     * - Converter '~' para getenv("HOME")
     */
    struct dirent *d;
    struct stat s;
    DIR *dir = NULL;
    char buffer[BUF_SIZE];
    char *path = *argv;

    /* Assume current working directory as path if none given */
    if (path == NULL)
        path = getcwd(buffer, BUF_SIZE);

    dir = opendir(path);

    if (dir != NULL) {
        int i = 0, j, dir_size = 0;

        /* Count used inodes, so we can create an array of the correct size */
        while ( (d = readdir(dir)) )
            if (d->d_ino != 0) dir_size++;

        closedir(dir);
        dir = opendir(path);

        char *list[dir_size];
        while ( (d = readdir(dir)) ) {
            if (d->d_ino != 0) /* XXX: I can't for the life of me remember why this is here */
                list[i++] = strdup(d->d_name);
        }
        list[i] = NULL;

        /* Sort entries */
        qsort(list, i, sizeof(char *), string_cmp);

        for (j = 0; j < i; j++) {
            char buf[BUF_SIZE];
            char *name = list[j];
            if (strncmp(name, ".", 1) != 0) { /* Don't list hidden files nor special dirs */
                snprintf(buf, BUF_SIZE, "%s/%s", path, name);
                print_file_info(name, buf, s);
            }
            free(list[j]);
        }

        closedir(dir);
    }
    else if (errno == ENOTDIR) {               /* Then it's probably a file */
        char *name = strrchr(path,  '/');      /* We only want the filename */
        name = (name == NULL) ? path : name+1; /* Get rid of the trailing slash */
        print_file_info(name, path, s);
    }
    else {
        perror(path);
        return -1;
    }

    return 0;
}

/* Display a line of text */
int cmd_eco(char **argv)
{
    while ( *argv != NULL )
        printf("%s\n", *(argv++));
    return 0;
}

/* Display the current directory */
int cmd_pwd(char **argv)
{
    char buffer[BUF_SIZE];
    char *dir = getcwd(buffer, BUF_SIZE);

    printf("%s\n", dir);

    return 0;
}

/* Change directory */
int cmd_cd(char **argv)
{
    const char *path = *argv;
    int status;

    /* Assume the user's home directory as path if none given */
    if (path == NULL)
        path = getenv("HOME");

    status = chdir(path);

    if ( status == -1 )
        perror(path);

    return 0;
}

/* Exit shell */
int cmd_exit(char **argv)
{
    printf("Bye!\n");
    exit(EXIT_SUCCESS);
}

// vim: et ts=4 sw=4 sts=4
