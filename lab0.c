#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define BUFSIZE  512

void sig_handle(int signum){
  fprintf(stderr, "Caught segfault. Signal handler number: %d\n", signum);
  exit(4);
}

void cause_segfault(){
  char* s = 0;
  *s = 's';
}

void freemem(char* input, char* output, char* buf){
  if (input != 0) { free(input);}
  if (output != 0) { free(output);}
  if (buf != 0) { free(buf);}
}

int main(int argc, char * argv[]){

  //int opt;
  //gint arg_array_counter = 0;
  int input_opt = 0;
  int output_opt = 0;
  int segfault_opt = 0;
  int catch_opt = 0;
  char* input_filename = 0;
  char* output_filename = 0;
  char* buf = 0; //(char *)malloc(BUFSIZE*sizeof(char));
  int filename_len;
  int buf_cap = BUFSIZE;
  /*int terminal_is_stdin = 0;
  char* terminal_input = 0;
  int terminal_input_cap = BUFSIZE;
  int terminal_input_len = 0;
  */
  
  while (1){
    //int flag_endofopt = 0; 
    int co = 0;
    static struct option long_options[] = {
      {"input",    required_argument, 0, 'i'},
      {"output",   required_argument, 0, 'o'},
      {"segfault", no_argument,       0, 's'},
      {"catch",    no_argument,       0, 'c'},
      {0,          0,                  0, 0}
    };

    
    co = getopt_long(argc, argv, "", long_options, &optind);

    if (co == -1){
      //flag_endofopt = 1;
      break;
    }

    switch (co){
    case 'i':
      //printf("settin input_opt");
      input_opt = 1;
      filename_len = strlen(optarg);
      input_filename = (char *) malloc(filename_len*sizeof(char));
      strcpy(input_filename, optarg);
      break;
    case 'o':
      output_opt = 1;
      filename_len = strlen(optarg);
      output_filename = (char *) malloc(filename_len*sizeof(char));
      strcpy(output_filename, optarg);
      break;
    case 's':
      segfault_opt = 1;
      break;
    case 'c':
      catch_opt = 1;
      break;
    case '?':
      freemem(input_filename,output_filename,buf);
      exit(1);
	};
    
  }

  /*
  //get the stuff for stdin
  if (optind < argc){
    terminal_is_stdin = 1;
    while (optind < argc){
      str
    }
  }
  */
  
  if (input_opt){
    //fprintf(stderr, "input_opt set1\n");
    //printf("redirecting input\n");
    int ifd = open(input_filename, O_RDONLY);
    //fprintf(stderr, "ifd = %d\n", ifd);
    if (ifd < 0){
      fprintf(stderr, "Error with '--input'. Cannot open '%s': %s\n", input_filename, strerror(errno));
      //fprintf(stderr, "input_opt set2\n");
      freemem(input_filename,output_filename,buf);
      exit(2);
    }
    if (ifd >=0){
      int check_close = close(0);
      if (check_close < 0) {
	fprintf(stderr, "Unable to close stdin: %s\n", strerror(errno));
	freemem(input_filename,output_filename,buf);
	exit(2);
      }
      int check_dup = dup(ifd);
      if (check_dup < 0) {
	fprintf(stderr, "Error with '--input'. Unable to duplacte the file descriptor for '%s': %s\n", input_filename, strerror(errno));
	freemem(input_filename,output_filename,buf);
	exit(2);
      }
      
	check_close = close(ifd);
      if (check_close < 0) {
	fprintf(stderr, "Error with '--input'. Unable to close '%s': %s\n", input_filename, strerror(errno));
	freemem(input_filename,output_filename,buf);
	exit(2);
      
      }
    }
  }

  if (output_opt){
    int ofd = creat(output_filename, 0666);
    if (ofd < 0){
      fprintf(stderr, "Error with '--output'. Unable to open or create '%s': %s\n", output_filename, strerror(errno));
      freemem(input_filename,output_filename,buf);
      exit(3);
    }
    //else { fprintf(stderr, "Success with '%s'", output_filename);}
    int check_close = close(1);
    if (check_close < 0){
      fprintf(stderr, "Unable to close stdout: %s\n", strerror(errno));
      freemem(input_filename,output_filename,buf);
      exit(3);
    }
    int check_dup = dup(ofd);
    if (check_dup < 0){
      fprintf(stderr, "Unable to dupliacte file descriptor for '%s': %s\n", output_filename, strerror(errno));
      freemem(input_filename,output_filename,buf);
      exit(3);
    }
    check_close = close(ofd);
    if (check_close < 0){
      fprintf(stderr, "Error with '--output'. Unable to close '%s': %s\n", output_filename, strerror(errno));
      freemem(input_filename,output_filename,buf);
      exit(3);
    }
  }

  if (catch_opt){
    if (signal(SIGSEGV, sig_handle) == SIG_ERR){
      fprintf(stderr, "Error with '--catch': %s\n", strerror(errno));
      freemem(input_filename,output_filename,buf);
      exit(4);
    }
  }

  if (segfault_opt){
    cause_segfault();
  }
  
  buf = (char *)malloc(BUFSIZE*sizeof(char));
  int len = 0;
  int c;
  while(1){
    //fprintf(stderr, "now reading\n");
    if (len == buf_cap){
      //fprintf(stderr, "realloc\n");
      char* temp = (char *)realloc(buf, len + BUFSIZE);
      buf_cap = len+BUFSIZE;
      buf = temp;
      if (buf == 0){
	fprintf(stderr, "Error with '--input'. Unable to reallocate memory for '%s'\n", input_filename);
	freemem(input_filename,output_filename,buf);
	exit(2);
      }
      //fprintf(stderr, "realloc success\n");
    }
    c = read(0, buf+len, 1);
    if (c < 0){
      //fprintf(stderr, "passed input_opt check\n");
      if (input_opt){
	fprintf(stderr, "Error with '--input'. Unable to read '%s': %s\n", input_filename, strerror(errno));
      }
      else{
	fprintf(stderr, "Error reading from stdin: %s\n", strerror(errno));
      }
      freemem(input_filename,output_filename,buf);
      exit(3);
    }
    if (c == 0)
      break;
    len++;
  }
  //fprintf(stderr, "%d\n", len);
  int check_write = write(1, buf, len);
  if (check_write < 0){
    if (output_opt){
      fprintf(stderr, "Error with '--output'. Unable to write to '%s': %s\n", output_filename, strerror(errno));
    }
    else{
    fprintf(stderr, "Error writing to stdout: %s\n", strerror(errno));
    }
    freemem(input_filename,output_filename,buf);
    exit(3);  }
  exit(0);
}
