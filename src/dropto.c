#include <errno.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char *account;
struct passwd *pw;

int main(int argc,const char *const *argv,const char *const *envp)
{
  account = *++argv;
  if (!account || !*++argv) {
    fprintf(stderr, "dropto: usage: dropto account child [childargs]\n");
    exit(100);
  }

  pw = getpwnam(account);
  if (!pw) {
    fprintf(stderr, "dropto: unknown account %s\n", account);
    exit(111);
  }

  if (setgid(pw->pw_gid) == -1) {
     fprintf(stderr, "dropto: unable to setgid: %s\n", strerror(errno));
     exit(111);
  }

  if (pw->pw_name != NULL) {
    if (initgroups(pw->pw_name, pw->pw_gid) == -1) {
      fprintf(stderr, "dropto: unable to initgroups: %s\n", strerror(errno));
      exit(111);
    }
  }
  else {
    fprintf(stderr, "dropto: incomplete struct passwd (pw_name == NULL)");
    exit(111);
  }

  if (setuid(pw->pw_uid) == -1) {
    fprintf(stderr, "dropto: unable to setuid: %s\n", strerror(errno));
    exit(111);
  }

  execvp(*argv,argv);

  fprintf(stderr, "dropto: unable to run %s: %s\n", *argv, strerror(errno));
  exit(111);
}

