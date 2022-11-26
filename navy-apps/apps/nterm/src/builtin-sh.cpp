#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

/*
static void sh_handle_cmd(const char *cmd) {
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
*/

static char* args[3];//args[0]->c_name, args[1]->c_arg, arg[2]->NULL
static char* c_args;
static char* const * envp = NULL;

static void sh_handle_cmd(const char *cmd) {
  char* cmd_p = (char*)cmd;
  char* c_name = strtok(cmd_p," \n");

  /* No Command */
  if(c_name == NULL) return;

  /* Echo Command */
  if(strcmp(c_name, "echo") == 0)
  {
    char* echo_str = strtok(NULL," ");
    sh_printf("%s", echo_str);
    return;
  }
  /* Exit Command */
  if(strcmp(c_name, "exit") == 0)
  {
    exit(0);
    return;
  }

  /* Execution */
  c_args = strtok(NULL," \n");
  args[0] = c_name;
  args[1] = c_args;
  args[2] = NULL;
  int ret = execvp(c_name, args);
  if(ret != -1) 
  {
    sh_printf("Execute App/File \"%s\" Successfully!\n", c_name);
    return;
  }
  else
  {
    if(c_name[0] == '/' && execve(c_name, args, envp) == -1) sh_printf("There is no \"%s\" file exists!!! \n", c_name);
    else sh_printf("The command \"%s\" is not supported or not implemented!!!\n", c_name);
  }     
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  setenv("PATH", "/bin:/usr/bin", 0);
  
  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}