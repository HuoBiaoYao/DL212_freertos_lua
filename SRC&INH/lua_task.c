/*#include "lprefix.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
*/
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "lua_task.h"
 
const char lua_test[] = {   
"print(\"Hello,I am lua!\\n--this is newline printf\")\n"
"function foo()\n"
"  local i = 0\n"
"  local sum = 1\n"
"  while i <= 10 do\n"
"    sum = sum * 2\n"
"    i = i + 1\n"
"  end\n"
"return sum\n"
"end\n"
"print(\"sum =\", foo())\n"
"print(\"and sum = 2^11 =\", 2 ^ 11)\n"
"print(\"exp(200) =\", math.exp(200))\n"
}; 

int lua_main(void) {
  int status,result;
  lua_State *L = luaL_newstate();//创建一个新的Lua状态机
  if (L == NULL){ 
    return 1;
  } 
  luaL_openlibs(L);//打开指定状态机中的所有Lua标准库
  luaopen_base(L);//打开基础库
  luaL_dostring(L, lua_test);//加载并运行指定的字符串
  lua_close(L);//销毁指定Lua状态机中的所有对象
	
  return 0;
}



void exit(int status){  
  
}


int system(const char *string){  
  return 0;  
} 

unsigned int time(unsigned int *time){  
  return 0;  
}