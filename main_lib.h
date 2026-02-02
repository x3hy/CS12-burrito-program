#ifndef MAIN_LIB_H        // guard so that main_lib cant be used twice
#define MAIN_LIB_H 

// == Global Variables == 
extern int verbose;
extern int EXIT_CODE; // i know i know this isent a constant but its important, trust
extern int help_toggle;
extern int exit_verbose;
extern int orders_capacity;
extern int order_amount_global;
extern float order_price_global;
extern const char *BURRITO_TYPE_LIST[];

// == Data Types == 
typedef enum {
    PICKUP,
    DELIVERY,
    DINEIN
} OrderMode;

typedef enum {
	TRUE,
	FALSE,
} Bool;

typedef enum {
	NOTCANCELED,
	CANCELED,
} Flag;

typedef struct Value {
	int array_length;
}Value;

typedef struct Burrito_type{
	char *type;
	int amount;
	float price;
}Burrito_type;

typedef struct Burrito {
	OrderMode mode;
	Flag flag;
	char*name;
	char*number;
	char*address;
	float price;
	int amount;
	Burrito_type *type;
}Burrito;

// plib argument structure
struct plib_argument {
	char *arg;
	char *desc;
	char *type;
	char *value;
	void (*call)(const char *value);
	void (*func)(void);
	char (*str_call)(const char *value);
	int  (*int_call)(const char *value);
	int scall;
};

// == Functions ==
// void:
void quit(int a);
void wait(void);
void draw_header(void); 
void handle_quit(void);
void help_callback(void);
void draw_header_sep(void);
void verbose_callback(void);
void exit_verbose_callback(void);
void draw_mangement(Burrito *order_list, int order_index);
void *load_from_file(const char *filename, int *size); 
int save_to_file(void *data, int size, const char *filename);
void free_orders(Burrito *orders, int  count);
int achar(void);
int draw_kitchen_screen(Burrito *order_list, int order_index);
int proccess_arguments(int argc, char*argv[],struct plib_argument *local);
int set_argument(char* argument, char* description, char* type,void (*callback)(const char *value), void (*function)(void),int self_callback,struct plib_argument *local, int  max_length);

// misc:
Burrito_type *display_burrito_menu(void);

// == Definitions ==
// macros: 
#define clear_a() printf("\033[0;0H")

// colors:
#define BOLD_ANSI "\033[1m"
#define RESET_ANSI "\033[0m"
#define SEL_ANSI  "\033[0;38;2;32;160;153;49m"
#define DIS_ANSI  "\033[0;38;2;68;66;122;49m"

// prices:
#define BURRITO_CHEAP_PRICE      8.5
#define BURRITO_EXPENSIVE_PRICE  13.5
#define DELIVERY_CHARGE          2.5
#define BURRITO_PRICE_CHANGE_NUM 2


// misc:
#define BURRITO_LIMIT       900000
#define USER_SLEEP_DELAY    1
#define BURRITO_TYPE_AMOUNT 6
#define FILENAME "data.bin" // NOTE: UN USED / DEPRICATED

// bounds :3c
#define INPUT_MIN_NAME    2 
#define INPUT_MAX_NAME    32
#define INPUT_MIN_NUMBER  8 
#define INPUT_MAX_NUMBER  16
#define INPUT_MIN_ADDRESS 8
#define INPUT_MAX_ADDRESS 128

// == Compat Data == 
#ifdef _WIN32 // Windows
	#define clear()  system("cls")
	int achar_WIN32(void);
	#define achar() achar_WIN32()

#elif __unix__ // Linux / MacOS
	#define clear()  system("clear")
	int achar_UNIX(void);
	#define achar() achar_UNIX()
#endif

#endif // MAIN_LIB_H
