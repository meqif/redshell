/* Helper functions */

#ifndef HELPER_H
#define HELPER_H

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

int strstrcnt(char *str, char ch);
char *getusername(uid_t uid);
char *getgroupname(gid_t gid);
void tokenize(char **dst, char *buffer, const char *delimiters);
int add_pid(pid_t new_pid);
char *expand_tilde(char *dest, char *src);
int expand_env(char *dest, char *src);
void expandize(char **dest, char *cmd);
void perror_exit(char *msg);

#endif /* HELPER_H */
