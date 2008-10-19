/* Helper functions */

#ifndef HELPER_H
#define HELPER_H

void fatal(char *msg);
int strstrcnt(char *str, char ch);
char *getusername(uid_t uid);
char *getgroupname(gid_t gid);
void tokenize(char **dst, char *buffer, const char *delimiters);

#endif /* HELPER_H */
