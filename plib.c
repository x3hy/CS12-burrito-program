#include <stdio.h>
#include "main_lib.h"

#define PLIB_ARG_HELP_TITLE_FORMAT "[OPTIONS]"
#define color(hex1,hex2,out) plib_color_implicit(hex1,hex2,out,sizeof(out))
#define plib_set_arg(a,b,c,d,e,f,g) plib_set_arg_implicit(a,b,c,d,e,f,g,sizeof(g)/sizeof(g[0]))
#define plib_argument_help(a) plib_argument_help_table_implicit(a,__FILE__)

// take input from user.
char *lowwer(char *a){
	for (int i = 0; a[i]; i++) {
		if(a[i] >= 'A' && a[i] <= 'Z'){
			a[i] = a[i] + ('a'- 'A'); // or a[i]+32
		}
	}
 return a;
}
char *upper(char *a){
	for (int i = 0; a[i]; i++) {
		if(a[i] >=  'a' && a[i] <= 'z'){
			a[i] = a[i] - ('a' - 'A'); // 32
		}
  }
 return a;
}

int plib_length(char *a){
	int out=0;
	for (int i=0; a[i];i++){
		out++;
	}
	return out;
}




int hex_to_int(char c) {
	if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return 0; // fallback value :3
}

int plib_color_implicit(char* hex1,char *hex2, char*out,size_t out_size){
	char *out1 = hex1+1;
	char *out2 = hex2+1;
	int r1,b1,g1,r2,b2,g2;
  r1 = (hex_to_int(out1[0]) << 4) | hex_to_int(out1[1]);
  g1 = (hex_to_int(out1[2]) << 4) | hex_to_int(out1[3]);
  b1 = (hex_to_int(out1[4]) << 4) | hex_to_int(out1[5]);
	r2 = (hex_to_int(out2[0]) << 4) | hex_to_int(out2[1]);
  g2 = (hex_to_int(out2[2]) << 4) | hex_to_int(out2[3]);
  b2 = (hex_to_int(out2[4]) << 4) | hex_to_int(out2[5]);
	int written = snprintf(out,out_size,"\033[38;2;%d;%d;%dm\033[48;2;%d;%d;%dm",r1,g1,b1,r2,g2,b2);
	if(written < 0 || (size_t)written >= sizeof(out)) return 1;
	return 0;
}

char * plib_strcpy(char * dest, char const * src){
	if (dest == NULL || src == NULL) return NULL;
	char *ptr = dest;
  while ((*dest++ = *src++));
  return ptr;
}

int plib_strcmp(const char *s1, const char *s2) {
    // Check if both strings are not NULL
    if (s1 == NULL && s2 == NULL) {
        return 0; // Both are NULL, considered equal
    }
    if (s1 == NULL) {
        return -1; // NULL is considered smaller than any non-NULL string
    }
    if (s2 == NULL) {
        return 1; // NULL is considered smaller than any non-NULL string
    }

    // Compare the strings
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }

    // Return the difference between the first non-matching characters
    return (unsigned char)*s1 - (unsigned char)*s2;
}



int plib_list_contains(struct plib_argument *array, size_t array_size, char*value){
	for(int i=0;i<array_size;i++){
		if(plib_strcmp(array[i].arg,value)==0) return 0;
	}
	return 1;
}


int plib_list_index(struct plib_argument *array, size_t array_size, char*value){
	for(int i=0;i<array_size;i++){
		if(plib_strcmp(array[i].arg,value)==0) return i;
	}
	return -1;
}


int plib_list_length(struct plib_argument *local){
	size_t local_length = 0;
  while (local[local_length].arg != NULL) {
  	local_length++;
  }
	return local_length;
}

char *plib_get_arg_value(char *arg_name,struct plib_argument *array){
	for(int i=0;i<plib_list_length(array);i++){
		if(plib_strcmp(array[i].arg,arg_name)==0){
			return array[i].value;
		}
	}
	return NULL;
}



int set_argument(char* argument, char* description, char* type,void (*callback)(const char *value), void (*function)(void),int self_callback,struct plib_argument *local, int max_length) {
	size_t i;
  for (i = 0; i < max_length; i++) {
  	if (local[i].arg == NULL) break;
  	if (plib_strcmp(local[i].arg, argument) == 0) return 2;
  }
  if (i >= max_length) return 1;
	local[i].arg = argument;
  local[i].desc = description;
  local[i].type = type;
  local[i].call = callback;
  local[i].func = function;
	local[i].scall = self_callback;
  return 0;
}


int plib_argument_help_table_implicit(struct plib_argument *local, char *name){
  size_t local_length = plib_list_length(local);
	if(local_length>0){
		int name_len=7;
		int desc_len=7;
		int type_len=7;
		// first counter;
		for(int i=0;i<local_length;i++){
			if(name_len<plib_length(local[i].arg)) name_len=plib_length(local[i].arg);
			if(desc_len<plib_length(local[i].desc)) desc_len=plib_length(local[i].desc);
			if(type_len<plib_length(local[i].type)) type_len=plib_length(local[i].type);
		}

		printf("\n%s %s ->\n",name,PLIB_ARG_HELP_TITLE_FORMAT);
			
		printf("+-");
		for(int ii=0;ii<name_len+1;ii++) printf("-");
		printf("+");
		for(int ii=0;ii<type_len+2;ii++) printf("-");
		printf("+");
		for(int ii=0;ii<desc_len+2;ii++) printf("-");
		printf("+\n");
	
		printf("| ");
		printf("arg:");
		for(int ii=0;ii<name_len-plib_length("arg:");ii++) printf(" ");
		printf(" | type:");
		for(int ii=0;ii<type_len-plib_length("type:");ii++) printf(" ");
		printf(" | desc:");
		for(int ii=0;ii<desc_len-plib_length("desc:");ii++) printf(" ");
		printf(" |\n");	
	
		printf("+-");
		for(int ii=0;ii<name_len+1;ii++) printf("-");
		printf("+");
		for(int ii=0;ii<type_len+2;ii++) printf("-");
		printf("+");
		for(int ii=0;ii<desc_len+2;ii++) printf("-");
		printf("+\n");

		for(int i=0;i<local_length;i++){
			printf("| %s",local[i].arg); // argument
			for(int ii=0;ii<name_len-plib_length(local[i].arg);ii++) printf(" "); // space
			printf(" | %s",local[i].type);
			for(int ii=0;ii<type_len-plib_length(local[i].type);ii++) printf(" "); // space
			printf(" | %s",local[i].desc);
			for(int ii=0;ii<desc_len-plib_length(local[i].desc);ii++) printf(" "); 
			printf(" |\n");
		}

		printf("+-");
		for(int ii=0;ii<name_len+1;ii++) printf("-");
		printf("+");
		for(int ii=0;ii<type_len+2;ii++) printf("-");
		printf("+");
		for(int ii=0;ii<desc_len+2;ii++) printf("-");
		printf("+\n");

		printf("\n");

	} else return 1; // no arguments set
	return 0; // executed correctly
}


int proccess_arguments(int argc, char *argv[],struct plib_argument *local){
	size_t local_length=0;
	while(local[local_length].arg != NULL) local_length++;
	if(local_length>0){
		for(int i=1;i<argc;i++){
			if(plib_list_contains(local,local_length,argv[i])==0){

				// argument is defined 
				int argument_index = plib_list_index(local,local_length,argv[i]);	
				if(plib_strcmp(local[argument_index].type,"void")!=0 && plib_list_contains(local,local_length,argv[i+1])!=0 && argv[i+1]!=NULL){

					// argument is valid so far
					local[argument_index].value = argv[i+1];
				} else if(plib_strcmp(local[argument_index].type,"void")!=0) {
					// argument is valid but does not contain a value
					printf("Error: argument %s requires a value.\n",argv[i]);
				}
				if(local[argument_index].call){
					local[argument_index].call(local[argument_index].value);
				}
				if(local[argument_index].func){
					local[argument_index].func();
				}
				if(local[argument_index].scall){
					plib_argument_help(local);
				}
			} else if(plib_list_contains(local,local_length,argv[i-1])==0){
				// argument is value
			} else {
				printf("Error: argument %s is not a valid argument.\n",argv[i]);
				return 1;
			}
		}
	} else {
		printf("Error: no arguments provided.\n");
	}
	return 0;
}
