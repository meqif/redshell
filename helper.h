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

#endif /* HELPER_H */
