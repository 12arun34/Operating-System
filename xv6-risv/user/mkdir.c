#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])                       // argc = size of argv -1;
{
 // int i;

  if(argc < 2){
    fprintf(2, "Usage: mkdir files...\n");
    exit(1);
  }
 // if(argc==2)
  //{
  //  fprintf(2, "No Argument given \n");
 // }
  if(argc>2)
  {
    fprintf(2, "Argument error\n");
  }

  //for(i = 1; i < argc; i++){
    if(mkdir(argv[1]) < 0){
      fprintf(2, "mkdir: %s failed to create\n", argv[1]);
    //  break;
    }
 // }

  exit(0);
}
