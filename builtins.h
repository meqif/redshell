#ifndef BUILTINS_H
#define BUILTINS_H

/* Auxiliary functions */
char *getusername(uid_t uid);
char *getgroupname(gid_t gid);
void tokenize(char **dst, char *buffer, const char *delimiters);

/* Builtin commands */
void print_file_info(char *name, char *path, struct stat s);
int listar(char *path);
int eco(char **myArgv);
int pwd();
int cd(char *path);

#endif /* BUILTINS_H */
