/* Helper functions */

#ifndef HELPER_H
#define HELPER_H

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

int strstrcnt(char *str, char ch);
char *getusername(uid_t uid);
char *getgroupname(gid_t gid);
void tokenize(char **dst, char *buffer, const char *delimiters);
void expandGlob(command_t *command, char *cmd);
char *expandAlias(char *command);
void findRedirections(pipeline_t *pipeline, char **argv);
void perror_exit(char *msg);

#endif /* HELPER_H */
