/* Auxiliary functions */
char *getusername(uid_t uid);
char *getgroupname(gid_t gid);

/* Builtin commands */
void print_file_info(char *name, char *path, struct stat s);
int listar(char *path);
int eco(char **myArgv);
int pwd();
int cd(char *path);
