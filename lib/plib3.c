#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plib.h"

/* TODO
 * add type based conversion using universal var.
 * add functions and toggle pointers.
 * add error vars for better readability
 * ontop of that add a enum to stringifyer
 * so that i can print the error on exit aswell
 * as having the actual code from the enum
 * add verbose/debug mode
 * add quiet mode
 * write to plib.h 
 * remove deps (string.h)
 * add description or other STRING info to argument 
 * ^ swap over to struct input for arguments.
 *  ^ maybe add callback
 *  ^ maybe add toggle 
 *  rhose whrere the most important old values 
 *
 * rework help function
 */

char *strdup(const char *s); // Fuck you ALE
char *strsep(char **stringp, const char *delim) {
  if (*stringp == NULL) {
    return NULL;
  }
  char *token_start = *stringp;
  *stringp = strpbrk(token_start, delim);
  if (*stringp) {
    **stringp = '\0';
    (*stringp)++;
  }
  return token_start;
}

// argument globals
typedef struct argument {
  const char *name;
  char *value;
  int takes_value;
  int triggered;
} argument;

int argument_list_index = 0;
int argument_list_capacity = 0;
argument *argument_list;

// Basic checker function to ensure that
// 1. the argument_list is initialized
// and 2. that the argument list has
// enough space to hold another item.
int validate_argument_list() {
  // init argument list
  if (argument_list_capacity == 0) {
    argument_list_capacity = 2;
    argument_list = malloc(argument_list_capacity * sizeof(argument));
    if (!argument_list) {
      printf("Error: unable to initialize argument_list (malloc)\n");
      return 1;
    }
  }

  // re-allocate argument_list
  if (argument_list_capacity == argument_list_index) {
    argument_list_capacity *= 2;
    argument *temp = realloc(argument_list, argument_list_capacity * sizeof(argument));
    if (!temp) {
      printf("Error: unable to re-allocate argument_list (malloc)\n");
      free(argument_list);
      return 1;
    } else
      argument_list = temp;
  }

	if(argument_list == NULL) return 1;


  return 0;
}

// set an argument ( used for later in the
// process_arguments function )
int set_argument(argument **pointer_return,const char *name, const int takes_value) {
  if (validate_argument_list() != 0) return 1;
  argument *local_argument = &argument_list[argument_list_index];

	// zero it out 


  // set values
  local_argument->name = name;
  local_argument->takes_value = takes_value;
	local_argument->triggered = 0;
  local_argument->value = NULL;
	argument_list_index++;
	*pointer_return = local_argument;
  return 0;
}

// Print all of the arguments that have
// been set so far, this will be called
// if an argument is non-existant or if
// and invalid argument is ran.
void help() {
	if(argument_list == NULL) return;
  printf("Options:\n");
  for (int i = 0; i < argument_list_index; i++) {
    printf("%s\n", argument_list[i].name);
  }
}

// Deallocate the memory from argument_list
// this will be useful later when we have
// embedded pointers and functions that take
// up ram on the argument.
void dinit_argument_list() {
  if (validate_argument_list() == 0){
		for(int i=0;i<argument_list_index;i++){
			if(argument_list[i].value){	
				printf("INFO: free'd %lu bytes\n",strlen(argument_list[i].value));
				free(argument_list[i].value);
			}
		}
		printf("INFO: free'd argument_list\n");
    free(argument_list);
	}
}

int argument_exists(const char *name) {
  for (int i = 0; i < argument_list_index; i++)
    if (strcmp(argument_list[i].name, name) == 0)
      return i; // argument name found in argument_list

  return -1; // argument was not found in argument_list
}

// go through each argument and parse them
// by checking the record with argument_list
int parse_arguments(const int argc, const char *argv[]) {
  if (argc == 1) {
    printf("Error: No arguments given\n");
    return 1;
  }
  for (int i = 1; i < argc; i++) {
    if (!argv[i]) {
      printf("Error: NULL argument at index %d\n", i);
      continue;
    }

    char *str, *to_free, *token;
    const char *arg = argv[i];
    char *key = NULL, *value = NULL;
    int token_count = 0;
    int return_code = 0;

    to_free = str = strdup(arg);
    if (!str) {
      printf("Error: Memory allocation failed for argument '%s'\n", arg);
      return 1;
    }

    while ((token = strsep(&str, "="))) {
      if (strlen(token) == 0)
        continue;

      if (token_count == 0)
        key = token;
      else if (token_count == 1)
        value = token;
      else {
        return_code = 1;
        break;
      }
      token_count++;
    }

    // handle the arguments
    if (return_code == 1)
      printf("Error: too many '=' in argument: '%s'\n", arg);
    else if (!key || strlen(key) == 0) {
      return_code = 1;
      printf("Error: invalid or empty key in '%s'\n", arg);
    } else {
      const int argument_index = argument_exists(key);
      if (argument_index != -1) {
        argument *local_argument = &argument_list[argument_index];
        if (!value || strlen(value) == 0) {
          /* a key has been provided but it has
           * no value, this is good for if you have
           * void flags like --help. */
          if (local_argument->takes_value != 0) {
            printf("Error: argument '%s' takes a value but none was provided\n",
                   key);
            return_code = 1;
          }

          local_argument->triggered = 1;
        } else {
          /* a key has been provided and a value
           * has also been provided. */
          if (local_argument->takes_value != 1) {
            printf("Error: argument '%s' does not take a value but '%s' was "
                   "provided\n",
                   key, value);
            return_code = 1;
          }

          // set the value if needed
          local_argument->value = malloc(strlen(value));
					if(local_argument->value){
						strncpy(local_argument->value,value,strlen(value));
          	local_argument->triggered = 1;
					} else {
						printf("Error: couldent allocate memory for argument '%s' value\n",key);
					}
        }
      } else
        printf("Error: argument '%s' not found\n", key);
    }

    free(to_free);
    if (return_code != 0)
      return return_code;
  }

  return 0;
}

// get bool if argument has been run or not,
// useful for void argument flags like --help
int argument_run(const argument *local) {
  if (validate_argument_list() != 0)
    return -1;
	if(!local) return 1;

  if (local->triggered)
    return 0;
  return 1;
}

char *argument_value(const argument *local) {
  if (validate_argument_list() != 0){
		printf("Error: argument_list validation failed\n");
    return "";
	}

  if (local->triggered) {
   
    return local->value;
  } else {
		printf("Error: argument '%s' does not have a value\n",local->name);
    return "";
  }
  printf("Error: argument '%s' has not been run\nError: ^--> use '*argument_run(argument *)'\n",local->name);
}

typedef enum {
	NO_INPUT,
	TAKE_INPUT,
} set_argument_enum_values;


int main(const int argc, const char *argv[]) {
  
	// define arguments 
	argument *help_arg = NULL;
	argument *test_arg = NULL;
	set_argument(&help_arg, "--help", NO_INPUT);
  set_argument(&test_arg,"--test", TAKE_INPUT);

	// process arguments 
  if (parse_arguments(argc, argv) != 0){
		help();
		return 1;
	}

	// test values 
  if (argument_run(help_arg) == 0) {
    printf("help command run!!\n");
  }

	if (argument_run(test_arg) == 0) {
    char *value =  argument_value(test_arg);
  	printf("--test run with '%s' value\n", value);
	}

  dinit_argument_list();
  return 0;
}
