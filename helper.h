/* Helper functions */

#ifndef HELPER_H
#define HELPER_H

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

int strstrcnt(char *str, char ch);
char *getusername(uid_t uid);
char *getgroupname(gid_t gid);
void tokenize(char **dst, char *buffer, const char *delimiters);
int add_pid(pid_t new_pid);
void expandGlob(char **dest, char *cmd);
void perror_exit(char *msg);
command_t *commandNew();
void commandFree(command_t *command);
pipeline_t *pipelineNew();
void pipelineFree(pipeline_t *pipeline);

#endif /* HELPER_H */
