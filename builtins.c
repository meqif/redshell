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
#include "jobs.h"

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
        int i = 0, dir_size = 0;

        /* Count used inodes, so we can create an array of the correct size */
        while ( (d = readdir(dir)) )
            if (d->d_ino != 0) dir_size++;

        closedir(dir);
        dir = opendir(path);

        char *list[dir_size];
        while ( (d = readdir(dir)) ) {
            if (d->d_ino != 0)                 /* Ignore invalid inodes */
                list[i++] = strdup(d->d_name);
        }
        list[i] = NULL;

        /* Sort entries */
        qsort(list, dir_size, sizeof(char *), string_cmp);

        for (i = 0; i < dir_size; i++) {
            char buf[BUF_SIZE];
            char *name = list[i];
            if (strncmp(name, ".", 1) != 0) {  /* Don't list hidden files and directories */
                snprintf(buf, BUF_SIZE, "%s/%s", path, name);
                print_file_info(name, buf, s);
            }
            free(list[i]);
        }

        closedir(dir);
    }
    else if (errno == ENOTDIR) {               /* Then it's probably a file */
        char *name = strrchr(path, '/');       /* We only want the filename */
        name = (name == NULL) ? path : name+1; /* Remove the trailing slash */
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

/* Export/set environment variables */
int cmd_export(char **argv)
{
    char *key = *argv;
    char *value = strstr(*argv, "=");
    *value = '\0';
    value++;

    return setenv(key, value, 1);
}

/* Display the current directory */
int cmd_pwd(char **argv)
{
    (void) argv; /* avoid warning about unused parameter */
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

    return status;
}

/* Exit shell */
int cmd_exit(char **argv)
{
    (void) argv; /* avoid warning about unused parameter */
    printf("Bye!\n");
    exit(EXIT_SUCCESS);
}

/* Executes a command and lets it run for at most the seconds given in the
 * first position of the argument */
int cmd_timeout(char **argv)
{
    extern int fg_pid;
    int seconds = atoi(*argv);

    if (seconds <= 0) {
        fprintf(stderr, "Invalid waiting time\n");
        return -1;
    }

    argv++; /* Ignore timeout parameter */

    pid_t pid = fork();
    switch(pid) {
        case -1:
            perror("fork failed");
            return -1;
            break;                            /* Execution never arrives here */
        case 0:
            setpgrp(); /* Set process group so this process and its children can be killed */
            executioner(argv); /* TODO: Allow execution of more than one command */
            exit(0);
            break;                            /* Execution never arrives here */
        default:
            fg_pid = pid;
            alarm(seconds);                   /* Set alarm */
            while (waitpid(pid, NULL, 0) == -1 && errno == EINTR); /* Wait! */
            alarm(0);                         /* Unset alarm */
            break;
    }

    return 0;
}

// vim: et ts=4 sw=4 sts=4
