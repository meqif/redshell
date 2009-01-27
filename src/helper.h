/* Helper functions */

#ifndef HELPER_H
#define HELPER_H

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

char *getusername(uid_t uid);
char *getgroupname(gid_t gid);
char *expand(char *input);
int expandGlob(command_t *command, char *cmd);
char *expandAlias(char *command);

#endif /* HELPER_H */
