#ifndef __LUA_TASK_H
#define __LUA_TASK_H

int lua_main(void);

void exit(int status);
int system(const char *string);
unsigned int time(unsigned int *time);
#endif
