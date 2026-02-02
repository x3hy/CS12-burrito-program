#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "plib.c"
#include "main_lib.h"

const char *BURRITO_TYPE_LIST[]={"Cheese","Plain","Spicy","Deluxe","Large","Gourmet"};
int orders_capacity=1;
int order_amount_global =0;
float order_price_global = 0;
int EXIT_CODE = 0;

#ifdef _WIN32
	#include <windows.h>
	int achar_WIN32() {
		HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
		DWORD mode, bytesRead;
		INPUT_RECORD ir;
  	KEY_EVENT_RECORD ker;
  	GetConsoleMode(hStdin, &mode);
  	SetConsoleMode(hStdin, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));
  	ReadConsoleInput(hStdin, &ir, 1, &bytesRead);
		if (ir.EventType == KEY_EVENT && ir.Event.KeyEvent.bKeyDown) {
  		ker = ir.Event.KeyEvent;
  		char ch = ker.uChar.AsciiChar;
    	if (ch == '\r') { 
      	ch = '\n';
    	} else if (ker.wVirtualKeyCode >= VK_UP && ker.wVirtualKeyCode <= VK_DOWN) {
    		switch (ker.wVirtualKeyCode) {
      		case VK_UP:    ch = 'A'; break;
        	case VK_DOWN:  ch = 'B'; break;
        	case VK_RIGHT: ch = 'C'; break;
        	case VK_LEFT:  ch = 'D'; break;
      	}
    	}
    	SetConsoleMode(hStdin, mode);
    	return (unsigned char)ch;
  	}
  	SetConsoleMode(hStdin, mode);
  	return -1;
	}
#elif __unix__
	#include <termios.h>
	int achar_UNIX(){
		static struct termios oldt, newt;
		tcgetattr(STDIN_FILENO,&oldt);
		newt=oldt;
		newt.c_lflag &= ~(ICANON | ECHO );
		tcsetattr(STDIN_FILENO,TCSANOW,&newt);
		int ch=getchar();
		tcsetattr(STDIN_FILENO,TCSANOW,&oldt);
		return ch;
	}
#endif



// define callback toggles
int verbose = 0;

// define callback functions
void verbose_callback(){verbose = 1;}


void draw_header_sep(){
	printf("%s----------------------------\033[0m\n",DIS_ANSI);
}
void draw_header(){
printf("Welcome To %sBanjo's Burritos\033[0m!%s Order #%d ($%0.2f)\033[0m\n",BOLD_ANSI,DIS_ANSI,order_amount_global+1,order_price_global);
draw_header_sep();
}

// all this does is run when the user presses control+c
void quit(int a){
	printf("\n\033[0mCTRL+C pressed have fun with your leaked memory, Goodbye\n");
	exit(0);
}

void handle_quit(){
	signal(SIGINT,quit);
}

Burrito_type *display_burrito_menu(){
	int burrito_list_malloc = 0;
	/*for(int i=0;i<BURRITO_TYPE_AMOUNT;i++){
		burrito_list_malloc+=sizeof(BURRITO_TYPE_LIST[i]);
	}*/ 
	burrito_list_malloc = sizeof(Burrito)*BURRITO_TYPE_AMOUNT;
	printf("%d\n",burrito_list_malloc);


	Burrito_type *burrito_list = malloc(burrito_list_malloc);
	for(int i=0;i<BURRITO_TYPE_AMOUNT;i++){
		burrito_list[i].type = (char *)BURRITO_TYPE_LIST[i];
	}
	for(int i=0;i<BURRITO_TYPE_AMOUNT;i++){
		burrito_list[i].price = (i<3) ? BURRITO_CHEAP_PRICE : BURRITO_EXPENSIVE_PRICE;
		burrito_list[i].amount = 0;
	}
	
	// draw ui 
	int largest_name = 0;
	for(int i=0;i<BURRITO_TYPE_AMOUNT;i++){
		char*  cur_name  = burrito_list[i].type;
		if(largest_name<strlen(cur_name)) largest_name = strlen(cur_name);
	}

	// draw frame
	int selected_line_index=0;
	clear();
	while(1){
		int largest_amount=0;
		float price_sum=0;
		int  amount_sum=0;

		const char *error_message = "Invalid order! Please enter at least 1 burrito to continue\n";
		clear_a();
		draw_header();
		printf("Use the up and down arrows to navigate through items,\nuse left and right to increase and decrease amount,\npress enter on any item to submit order info\n");
		draw_header_sep();
		printf("Keybinds: [r]eset, [c]ancel order\n");
		draw_header_sep();
		for(int i=0;i<BURRITO_TYPE_AMOUNT;i++){
			char largest_char[16];
			snprintf(largest_char,sizeof(largest_char),"%d",burrito_list[i].amount);
			if(largest_amount<strlen(largest_char)) largest_amount = strlen(largest_char);
			price_sum+=burrito_list[i].amount*burrito_list[i].price;
			amount_sum+=burrito_list[i].amount;
		}

		for(int i=0;i<(BURRITO_TYPE_AMOUNT);i++){
			if(selected_line_index==i) printf("%s%s> ",BOLD_ANSI,SEL_ANSI);
			else printf("  ");

			Burrito_type local = burrito_list[i];
		
			char cur_amount[16]="";
			char cur_price[16]="";
			sprintf(cur_amount,"%d",local.amount);
			sprintf(cur_price,"%1.2f",local.price);
			printf("%s",cur_amount);
			if(largest_amount>=1) for(int j=0;j<largest_amount-strlen(cur_amount);j++) printf(" ");
			printf(" | %s",local.type);
			if(largest_name>=1) for(int j=0;j<largest_name-strlen(local.type);j++) printf(" ");

			printf(" |%s\033[0m",cur_price);
			if(strlen(cur_price)>=1) for(int j=0;j<strlen(cur_price);j++) printf(" ");
			printf("\n");
		}
		draw_header_sep();
		printf("price: %s$%0.2f\033[0m %s(%d burritos)\033[0m",BOLD_ANSI,price_sum,DIS_ANSI,amount_sum);
		char price_sum_char[64];
		sprintf(price_sum_char,"%0.2f",BURRITO_TYPE_AMOUNT*(BURRITO_LIMIT*BURRITO_EXPENSIVE_PRICE));
		
		char amount_sum_char[64];
		sprintf(price_sum_char,"%d",BURRITO_TYPE_AMOUNT*BURRITO_LIMIT);
		for(int i=0;i<strlen(amount_sum_char)+strlen(price_sum_char);i++) printf(" ");
		printf("\n");
		int ch=achar(); 
		switch(ch){
			case 'B':
				// down
				if(selected_line_index==BURRITO_TYPE_AMOUNT-1) selected_line_index = 0;
				else selected_line_index++;
				break;
			case 'A':
				if(selected_line_index==0) selected_line_index=BURRITO_TYPE_AMOUNT-1;
				else selected_line_index--;
				// up
				break;
			case 'C':
				// right
				if(burrito_list[selected_line_index].amount < BURRITO_LIMIT) burrito_list[selected_line_index].amount++;
				break;
			case 'D':
				// left
				if(burrito_list[selected_line_index].amount > 0) burrito_list[selected_line_index].amount--;
				break;
			case 'r':
				for(int i=0;i<BURRITO_TYPE_AMOUNT;i++){
					burrito_list[i].amount = 0;
				}
				break;
			case 'c':
				return NULL;
			case 10:
				// enter
				if(amount_sum == 0){
					draw_header_sep();
					printf("%s%s\033[0m",DIS_ANSI,error_message);
					sleep(USER_SLEEP_DELAY);
					clear();
					break;
				}
				order_price_global+=price_sum;
				return burrito_list;
				break;
		}
	}
}


/* order_num | name | price
 *
 *
 */

int draw_kitchen_screen(Burrito *order_list, int order_index){
	clear();
	int selected_line_index = 0;

	while (1){
		int show_recept = 0;
		int only_show_one_recept = 0;
		int recept_index_to_show = 0;
		clear_a();
		draw_header();
		printf("Keybinds [b]ack,  [c]ancel (order), [v]eiw, view [a]ll, [u]ndo all\n");
		draw_header_sep();
		for(int i=0;i<order_index;i++){
			if(i == selected_line_index) printf("> ");
			else printf("  ");
			
			Burrito local = order_list[i];
			if((Flag)local.flag == (Flag)NOTCANCELED){
				printf("#%d: Name: %s - Price: $%0.2f",i+1,local.name,local.price);
				printf("\n");
			}
		}

		int ch=achar(); 
		switch(ch){
			case 'B':
				if(selected_line_index==order_index-1) selected_line_index = 0;
				else selected_line_index++;
				break;
			case 'A':
				if(selected_line_index==0) selected_line_index=order_index-1;
				else selected_line_index--;
				break;
			case 'b':
				return 0;
			case 'a':
				show_recept = 1;
				break;
			case 'c':
				order_list[selected_line_index].flag = (Flag)CANCELED;
			case 'u':
				for(int i=0;i<order_index;i++){
					order_list[i].flag = (Flag)NOTCANCELED;
				}
			case 'v':
				show_recept = 1;
				only_show_one_recept=1;
				recept_index_to_show = selected_line_index;
				break;
		}


		if(show_recept){
			clear();
			for(;recept_index_to_show<order_index;recept_index_to_show++){
				Burrito local = order_list[recept_index_to_show];
				if((Flag)local.flag == (Flag)CANCELED) continue;
				printf("Order: %d\n",recept_index_to_show+1);
				draw_header_sep();
				printf("Name: %s\n",local.name);
				printf("Number: %s\n",local.number);
				printf("Location: ");

				if (local.mode == DELIVERY) printf("Delivery\n");
				else if (local.mode == PICKUP) printf("Pickup\n");
				else if (local.mode == DINEIN) printf("Dine In\n");

				if(local.mode == DELIVERY)
			  	printf("Address: %s\n",local.address);
				
				printf("Price: $%0.2f\n",local.price);
				printf("Burritos ordered: %d\n",local.amount);
				draw_header_sep();
				printf("Burritos ordered:\n");
				draw_header_sep();
				for(int i=0;i<BURRITO_TYPE_AMOUNT;i++){
					if((local.type[i].amount || 0) >= 1){
						printf("%d - %s ($%0.2f each, $%0.2f total)\n",
							local.type[i].amount || 0,
							local.type[i].type,
							local.type[i].price,
							(local.type[i].amount || 0) * local.type[i].price
						);
					}
				}
				printf("\n");
				if (only_show_one_recept) break;
			}
			wait();
			clear();
		}
	}
}

/* draw_management_screen:
 * ARGS:
 * 	(Burrito*)order_list: a list of Burrito orders. 
 * 	(int)order_index: the amount of items in order_list
 * 
 * RETURNS:
 * 	nothing.
 *
 * DESCRIPTION:
 *	this function uses the burrito list to generate and display 
 *	different data. some of the data used is the sum price and 
 *	amount of orders made. */
void draw_mangement(Burrito *order_list, int order_index){
	float total_sales=0;
	int pickup=0, 
			delivery=0, 
			dine_in=0,
			total_burritos=0;
	char pickup_suffix[1]="",
			 delivery_suffix[1]="",
			 dine_in_suffix[1]="",
			 total_suffix[1]="";

	// Loop through and calculate sums
	for(int i=0;i<order_index;i++){
		if(order_list[i].flag == CANCELED) continue;
		if(order_list[i].mode == PICKUP) pickup++;
		else if (order_list[i].mode == DELIVERY) delivery++;
		else if (order_list[i].mode == DINEIN) dine_in++;				
		total_sales+=order_list[i].price;
		total_burritos+=order_list[i].amount;
	}
	
	// Add suffixes
	if(pickup > 1) pickup_suffix[0] = 's';
	if(delivery > 1) delivery_suffix[0] = 's';
	if(dine_in > 1) dine_in_suffix[0] = 's';
	if(dine_in>1 || pickup>1 || delivery>1) total_suffix[0] = 's';

	// draw the ui
	draw_header_sep();
	printf("there have been a total of %s%d\033[0m customer%s with a order sum price of %s$%0.2f\033[0m (%s%d Burritos)\n",BOLD_ANSI,pickup+delivery+dine_in,total_suffix,BOLD_ANSI,total_sales,BOLD_ANSI,total_burritos);
	if(pickup > 0){
		printf("%s%d\033[0m customer%s picked up their order",BOLD_ANSI,pickup,pickup_suffix);
	}
	
	if(delivery > 0){
		printf(", %s%d\033[0m customer%s had their order delivered.",BOLD_ANSI,delivery,delivery_suffix);
	}

	if(dine_in){
		printf(", %s%d\033[0m customer%s ate their order in store",BOLD_ANSI,dine_in,dine_in_suffix);
	}
	printf(".\n");

	draw_header_sep();
	wait();
}

int save_to_file(void *data, int size, const char *filename) {
    Burrito *orders = (Burrito *)data;
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file for writing");
        return 0;
    }

    // Write the number of orders
    int order_count = size;
    if (fwrite(&order_count, sizeof(int), 1, file) != 1) {
        perror("Failed to write order count");
        fclose(file);
        return 0;
    }

    for (int i = 0; i < order_count; i++) {
        // Write mode, price, amount
        if (fwrite(&orders[i].mode, sizeof(OrderMode), 1, file) != 1 ||
            fwrite(&orders[i].price, sizeof(float), 1, file) != 1 ||
            fwrite(&orders[i].amount, sizeof(int), 1, file) != 1) {
            perror("Failed to write order fields");
            fclose(file);
            return 0;
        }

        // Write name
        size_t name_len = orders[i].name ? strlen(orders[i].name) + 1 : 0;
        if (fwrite(&name_len, sizeof(size_t), 1, file) != 1 ||
            (name_len && fwrite(orders[i].name, 1, name_len, file) != name_len)) {
            perror("Failed to write name");
            fclose(file);
            return 0;
        }

        // Write number
        size_t number_len = orders[i].number ? strlen(orders[i].number) + 1 : 0;
        if (fwrite(&number_len, sizeof(size_t), 1, file) != 1 ||
            (number_len && fwrite(orders[i].number, 1, number_len, file) != number_len)) {
            perror("Failed to write number");
            fclose(file);
            return 0;
        }

        // Write address if delivery
        size_t address_len = (orders[i].mode == DELIVERY && orders[i].address) ? strlen(orders[i].address) + 1 : 0;
        if (fwrite(&address_len, sizeof(size_t), 1, file) != 1 ||
            (address_len && fwrite(orders[i].address, 1, address_len, file) != address_len)) {
            perror("Failed to write address");
            fclose(file);
            return 0;
        }

        // Write burrito types
        if (!orders[i].type) {
            fprintf(stderr, "Error: Burrito type array is NULL.\n");
            fclose(file);
            return 0;
        }

        for (int j = 0; j < BURRITO_TYPE_AMOUNT; j++) {
            if (fwrite(&orders[i].type[j].amount, sizeof(int), 1, file) != 1 ||
                fwrite(&orders[i].type[j].price, sizeof(float), 1, file) != 1) {
                perror("Failed to write type");
                fclose(file);
                return 0;
            }
        }
    }

    fclose(file);
    return 1;
}



void *load_from_file(const char *filename, int *size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file for reading");
        return NULL;
    }

    // Read the number of orders
    size_t order_count;
    if (fread(&order_count, sizeof(size_t), 1, file) != 1) {
        perror("Failed to read order count");
        fclose(file);
        return NULL;
    }

    // Allocate memory for orders
    Burrito *orders = calloc(order_count, sizeof(Burrito));
    if (!orders) {
        perror("Failed to allocate memory for orders");
        fclose(file);
        return NULL;
    }

    // Read each order
    for (size_t i = 0; i < order_count; i++) {
        // Read mode
        if (fread(&orders[i].mode, sizeof(OrderMode), 1, file) != 1) {
            perror("Failed to read mode");
            free_orders(orders, i);
            fclose(file);
            return NULL;
        }
        // Read price
        if (fread(&orders[i].price, sizeof(float), 1, file) != 1) {
            perror("Failed to read price");
            free_orders(orders, i);
            fclose(file);
            return NULL;
        }
        // Read amount
        if (fread(&orders[i].amount, sizeof(int), 1, file) != 1) {
            perror("Failed to read amount");
            free_orders(orders, i);
            fclose(file);
            return NULL;
        }
        // Read name
        size_t name_len;
        if (fread(&name_len, sizeof(size_t), 1, file) != 1) {
            perror("Failed to read name length");
            free_orders(orders, i);
            fclose(file);
            return NULL;
        }
        if (name_len) {
            orders[i].name = malloc(name_len);
            if (!orders[i].name || fread(orders[i].name, 1, name_len, file) != name_len) {
                perror("Failed to read name");
                free_orders(orders, i);
                fclose(file);
                return NULL;
            }
        }
        // Read number
        size_t number_len;
        if (fread(&number_len, sizeof(size_t), 1, file) != 1) {
            perror("Failed to read number length");
            free_orders(orders, i);
            fclose(file);
            return NULL;
        }
        if (number_len) {
            orders[i].number = malloc(number_len);
            if (!orders[i].number || fread(orders[i].number, 1, number_len, file) != number_len) {
                perror("Failed to read number");
                free_orders(orders, i);
                fclose(file);
                return NULL;
            }
        }
        // Read address
        size_t address_len;
        if (fread(&address_len, sizeof(size_t), 1, file) != 1) {
            perror("Failed to read address length");
            free_orders(orders, i);
            fclose(file);
            return NULL;
        }
        if (address_len) {
            orders[i].address = malloc(address_len);
            if (!orders[i].address || fread(orders[i].address, 1, address_len, file) != address_len) {
                perror("Failed to read address");
                free_orders(orders, i);
                fclose(file);
                return NULL;
            }
        }
        // Read type array
        orders[i].type = malloc(BURRITO_TYPE_AMOUNT * sizeof(Burrito_type));
        if (!orders[i].type) {
            perror("Failed to allocate type array");
            free_orders(orders, i);
            fclose(file);
            return NULL;
        }
        for (int j = 0; j < BURRITO_TYPE_AMOUNT; j++) {
            if (fread(&orders[i].type[j].amount, sizeof(int), 1, file) != 1 ||
                fread(&orders[i].type[j].price, sizeof(float), 1, file) != 1) {
                perror("Failed to read type");
                free_orders(orders, i);
                fclose(file);
                return NULL;
            }
        }
    }

    fclose(file);
    *size = order_count;
    return orders;
}

// Helper function to free partially loaded orders
void free_orders_OLD(Burrito *orders, int  count) {
		int memory_sum = 0,
				memory     = 0;

		/* Through out the program orders and inputs have been taking up allocated memory 
		 * what this block does is it goes through every order and frees any items that 
		 * take up memory, the input function for example returns malloced data and if we 
		 * dident free it that bit of data will be trapped in your ram until you reboot which 
		 * is not only stupid it is dangourous. */
		for(int i=0;i<count;i++){
			if(verbose){
				memory = ((orders_capacity*sizeof(Burrito_type))/count);
				memory_sum += memory; 
				printf("verbose -> %d: Free'd %d (tot %d) bytes from order[%d].type\n",__LINE__,memory,memory_sum,i);

				// Free the order name
				memory = INPUT_MAX_NAME;
				memory_sum += memory;
				if(orders[i].name)
					printf("verbose -> %d: free'd %lu (pot %d, tot %d) bytes from order[%d].name\n",__LINE__,strlen(orders[i].name),memory,memory_sum,i);	
							
				// Free the order number 
				memory = INPUT_MAX_NUMBER;
				memory_sum += memory;
				if(orders[i].number)
					printf("verbose -> %d: Free'd %lu (pot %d, tot %d) bytes from order[%d].number\n",__LINE__,strlen(orders[i].number),memory,memory_sum,i);
			}

			// ehm actually free the memory
			free(orders[i].type);
			free(orders[i].name);
			free(orders[i].number);
						
			// Free the order address
			if(orders[i].mode == DELIVERY){
				memory = INPUT_MAX_ADDRESS;
				memory_sum += memory;
				if (verbose ) printf("verbose -> %d: Free'd %lu (pot %d,tot %d) bytes from order[%d].address\n",__LINE__,strlen(orders[i].address),memory,memory_sum,i);
				free(orders[i].address);
			}
		}

		// Free the list and exit the program :D
		if(verbose){
			memory = orders_capacity*sizeof(Burrito);
			memory_sum += memory;
			printf("verbose -> %d: Free'd %d (tot %d) bytes of memory from order_list\n",__LINE__,memory, memory_sum);
			printf("verbose -> %d: Free'd %d total bytes of memory successfully!\n",__LINE__,memory_sum);
		}
		free(orders);
}

void free_orders(Burrito *orders, int order_count) {
    if (!orders || order_count <= 0) return;

    for (int i = 0; i < order_count; i++) {
        Burrito *order = &orders[i];

        // Free name
        if (order->name) {
            free(order->name);
            order->name = NULL;
        }

        // Free phone number
        if (order->number) {
            free(order->number);
            order->number = NULL;
        }

        // Free address (only if it was delivery)
        if (order->mode == DELIVERY && order->address) {
            free(order->address);
            order->address = NULL;
        }

        // Free burrito types if they were dynamically allocated
        if (order->type) {
            free(order->type);
            order->type = NULL;
        }
    }

    // Free the full order list
    free(orders);
}

void wait(){
	printf("%sPress %sEnter\033[0m%s to continue\033[0m\n",DIS_ANSI,BOLD_ANSI,DIS_ANSI);
	int c;
  while ((c = getchar()) != '\n' && c != EOF);
}
