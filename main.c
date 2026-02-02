/*
 * Lukan Snopovs (c) 2025 
 * --------------------------
 * view LICENCE for more details */

// Minimal libs
#include <stdio.h> // printf 
#include <string.h> // strlen strcmp
#include <stdlib.h> // malloc realloc 
#include <unistd.h> // exit 
#include <regex.h> // introduced for phone regex
#include "main_lib.h" // everything else

/* input: 
 * ARGS:
 * 	(char*)prompt: printed at the top of the selector menu 
 * 	(char*)keyword: keyword of the prompt eg USERNAME or PASSWORD 
 * 	(int)min: minimum string size 
 * 	(int)max: maximum string size
 * RETURNS:
 * 	malloc'd char of undefined size, program returns the string of the 
 * 	input that the user puts. the malloc'd string needs to be free'd after 
 * 	use.
 * DESCRIPTION:
 * 	this function very simply asks the user to enter input,
 * 	this input function has built in themeing and blank input 
 * 	checking */
char* input(char *prompt, char *keyword, const int MIN, const int MAX) {
    // Check for valid MIN and MAX
    if (MIN > MAX || MIN < 0 || MAX <= 0) {
        printf("Error: Invalid MIN (%d) or MAX (%d) values.\n", MIN, MAX);
        return NULL;
    }

    while (1) {
        clear();
        draw_header();
        int input_malloc_size = MAX + 2;
        char *input = malloc(input_malloc_size);

        if (verbose) {
            printf("verbose -> %d: allocated %d mem for an input func call\n", __LINE__, input_malloc_size);
            fflush(stdout);
        }

        if (!input) {
            printf("Error: a memory allocation error occurred in input function\n");
            return NULL;
        }

        // Print the prompt
        printf("%sNote: input must have a string length between %s%d-%d\033[0m%s.\n", DIS_ANSI, BOLD_ANSI, MIN, MAX, DIS_ANSI);
        if (strcmp(keyword, "number") == 0) {
            printf("%s %s%s (e.g., (123) 456-7890 or 123-456-7890)\033[0m%s%s: ", prompt, keyword,DIS_ANSI, BOLD_ANSI, SEL_ANSI);
        } else {
            printf("%s %s: %s%s", prompt, keyword, BOLD_ANSI, SEL_ANSI);
        }

        // Take the input
        fgets(input, input_malloc_size, stdin);
        printf("\033[0m");
        input[strcspn(input, "\n")] = 0;

        // Error check and return
        int input_length = strlen(input);
        if (verbose) {
            printf("verbose -> %d: raw input='%s'\n", __LINE__, input);
            printf("verbose -> %d: has %d strlen\n", __LINE__, input_length);
            wait();
        }

        if (input_length > 0) {
            if (input_length >= MIN && input_length <= MAX) {
                // Validate phone number if keyword is "number"
                if (strcmp(keyword, "number") == 0) {
                    const char *pattern = "^\\(?([0-9]{3})\\)?[-. ]?([0-9]{3})[-. ]?([0-9]{4})$";
                    regex_t regex;
                    int ret = regcomp(&regex, pattern, REG_EXTENDED);
                    if (ret) {
                        printf("Error: Could not compile regex\n");
                        free(input);
                        continue;
                    }
                    ret = regexec(&regex, input, 0, NULL, 0);
                    regfree(&regex);
                    if (ret == REG_NOMATCH) {
                        printf("%sInvalid phone number format. Please use (XXX) XXX-XXXX, XXX-XXX-XXXX, or XXXXXXXXXX\033[0m\n", DIS_ANSI);
                        wait();
                        free(input);
                        continue;
                    } else if (ret) {
                        printf("Error: Regex match failed\n");
                        free(input);
                        continue;
                    }
                }
                return input;
            } else if (input_length < MIN) {
                printf("%sPlease Enter a value with a string length %slarger\033[0m%s than %s%d\033[0m\n", DIS_ANSI, BOLD_ANSI, DIS_ANSI, BOLD_ANSI, MIN);
            } else {
                if (strchr(input, '\n') == NULL) {
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF);
                }
                printf("%sPlease Enter a value with a string length %ssmaller\033[0m%s than %s%d\033[0m\n", DIS_ANSI, BOLD_ANSI, DIS_ANSI, BOLD_ANSI, MAX);
            }
        } else {
            printf("%sPlease Enter an %sACTUAL %s\033[0m%s. you left it blank.\033[0m\n", DIS_ANSI, BOLD_ANSI, keyword, DIS_ANSI);
        }
        wait();
        free(input);
    }
}



/* draw_screen:
 * ARGS:
 * 	(char **)array: a array of words used as items in the menu 
 * 	(int)array_length: the amount of items in array.
 * 	(char *)prompt: text printed at the top of the menu 
 * RETURNS:
 * 	int, program returns the index of the current selected 
 * 	item. 
 * DESCRIPTION:
 * 	this function takes an array of words and uses those words 
 * 	to generate an interactive menu that lets the user use the 
 * 	arrow keys to choose options from the array of words, when 
 * 	the user presses enter the function returns the index of 
 * 	the selected value. */
int draw_screen(const char *array[],Value array_length,char *prompt){
	int selected=0;
	clear();
	while (1){
		clear_a();
		draw_header();
		printf("%s\n",prompt);

		// enumerate through the words 
		for(int i=0;i<array_length.array_length;i++){
			if(selected == i){
				printf("%s%s>",SEL_ANSI,BOLD_ANSI);
			} else printf(" ");
			printf(" %s\033[0m\n",array[i]);
		}

		// handle the user input
		int ch = achar();
		if(ch == 10) break;
		
		// simple switch case
		switch(ch){
			case 'B':
				if(selected==array_length.array_length-1) selected=0;
				else selected++;
				break;
			case 'A':
				if(selected==0) selected=array_length.array_length-1;
				else selected--;
				break;
		}
	}
	draw_header_sep();
	printf("\nYou selected %s%s\033[0m\n",BOLD_ANSI,array[selected]);
	fflush(stdout);
	sleep(USER_SLEEP_DELAY);
	return selected;
}

// System_argument_count and system_argument_array are the system vars parsed into the function on run
int main(int system_argument_amount, char *system_argument_array[]){

	/* What these lines do is declare some command line flags/arguments that allows the user 
	 * to view extra information, running this program with --verbose flag will cause it to 
	 * show extra verbose information like allocation sizes and other bits.
	 * */ 
	struct plib_argument args[1] = {0};

	/* The set argument is taken from the plib.c  library and adds the actual flag as mentioned 
	 * above, each argument has a bunch of differenet prameters that make it useful in files. 
	 * the function and callback arguments can take a function name which is run when the program 
	 * is called with the flag specifyed in NAME, the differneces between function and callback is 
	 * that function is able to be run with the value given in the flag eg --myflag <value> would allow 
	 * the value to be parsed into whatever function is set but it would not be able to be called in 
	 * the callback, also view main_lib.h to see all of the prameters. */
	set_argument(
			"--verbose",              // NAME 
			"show extra information", // DESCRIPTION
			"void",                   // TYPE 
			NULL,                     // FUNCTION 
			verbose_callback,         // CALLBACK 
			0,                        // SELF_CALL BOOL 
			args,                     // BASE ARRAY 
			1                         // ARRAY SIZE
	);
	
	/* This small line basically just binds a killcode to a function, in this case it will 
	 * run the quit() function if the user presses Control+C while the program is running,
	 * this is useful as we can free memory and perform other clean up before we close the 
	 * script, and yes this method can be abused into making a non quitable file ( on windows 
	 * at least ) */ 
	handle_quit();

	/* The next line runs the implicit plib_process_arguments function, what this does is 
	 * it preloads and runs callbacks and other functions based off of the arguments run 
	 * with the program eg --help and --verbose. */
	if(proccess_arguments(system_argument_amount,system_argument_array,args) == 0){

		// Allocate memory and initialize all values ( calloc != malloc )
		Burrito *order_list = calloc(orders_capacity, sizeof(Burrito));
		int break_while_loop = FALSE; 

		/* Checking if order list is defined is important as if it is not it shows that a 
		 * potential memory leak has occured. this can occur when order_capacity or the 
		 * Burrito struct are incorrectly set */
		if(!order_list){
			printf("error -> %d: failed to allocate %lu bytes of memory\n",__LINE__,orders_capacity * sizeof(Burrito));
			return 1;
		}
		
		/* Unlike python C does not come with booleans unless you import another library,
		 * instead of true and false C uses 1 and 0 this is why you will see int value returns 
		 * and conditinals throughout the code. 
		 *
		 * UPDATE: This loop now uses a boolean!! which was created as a custom enum type so I
		 * dident need to import any extra functions. :3 */

		while(break_while_loop == FALSE){
			const char *screen_main[]={
				"Enter Order",
				"Management Summary",
				"Kitchen Screen",
				"Exit",
				"Credits & Help",
			};

			// Set the 5 to a 4 to disable the credits if your going to steal my code
			int screen_main_value = draw_screen(screen_main,(Value){.array_length=5},"-- Select An Option --");

			/* After viewing the documentation on draw_screen you can see it returns a int 
			 * value corrasponding to the selection menu index, i've decided to use a switch 
			 * case block as i can easily set different outputs based off of the menu item the 
			 * user selects. */
			switch (screen_main_value){
				case 0:
					// Make a new callback point (used later on if the user wants to restart order)
					while(1){
						if(order_amount_global == orders_capacity){

							/* This if block will dynamically allocate and re-allocate memory to fit more orders 
							 * if the current order was about to overflow in the order_list malloc this block simply 
							 * adds more space to order_list. */
							if(verbose){
								printf("verbose -> %d: orders struct has been reallocated from %d (%d bytes) -> %d (%d bytes).\n",__LINE__,orders_capacity,orders_capacity*(int)sizeof(Burrito),orders_capacity*2,(orders_capacity*2)*(int)sizeof(Burrito));
								sleep(USER_SLEEP_DELAY);
							}
						
							// Multiply the capacity by two so 1, 2, 4, 8, 16... 
							orders_capacity *= 2;
							Burrito *temp = realloc(order_list,orders_capacity * sizeof(Burrito));

							/* Again error checking to make sure that no issues occured when allocating memory 
							* a good example of why this is necercerry is if the system is out of memory and the program 
							* types to allocate memory that the system doesnt have, that could cause fatal errors and ==
							 * other undefined behaviour. */
							if(!temp){
								printf("error -> %d: failed to allocate %lu bytes of memory\n",__LINE__,orders_capacity*sizeof(Burrito));
								free(order_list);
								return 1;
							} else order_list = temp;
						}

						/* Create a new local order, this is nice and easy as instead of having to call values like 
						 * order_list[order_global_amount].<value> it can simply be called using order.<value>.
						 *
						* Also, because order is now a pointer and an undefined (*) pointer, it means that instead 
						 * of using the conventional fullstop to query a value we use the -> operator, so  for 
						 * future reference order->value is the same as order.value */
						Burrito *order = &order_list[order_amount_global];
						*order = (Burrito){0};

						/* now the reason we initialize the price pram with a 0 is because later on in the code we 
						 * use the += operator to add to the price, if the value was previously uninitialized would
						 * lead to errors of undefined behavior. the reason the rest of the prams is because later 
						 * on when we exit the program we free a bunch of information, there is no good way to check 
						 * if a value is initialized or uninitialized so instead we simply allocate the memory before
						 * hand that way regardless of what happens there will be no errors due to the program trying 
						 * to free uninitialized values.*/
						order->price = 0;
						const char *screen_location[]={"Pickup","Delivery","Dine In",};

						/* I decided to use something ive never used before for this particular data type, that being 
						 * an ENUM data type, similar to a struct you can set const values easily, instead of having to 
						 * allocate memory for three porentail different words or having a hard to read int value corralating 
						 * to string values i can instead define three constants that can be used like ints eg one of these
						 * three consts is DELIVERY and i can also call it using the int 1*/
						order->mode = (OrderMode)draw_screen(screen_location,(Value){.array_length=3},"Is your order for pickup or delivery?");

						// See? easy to read
						if(order->mode==DELIVERY){
							printf("%sA %s%0.2f\033[0m%s delivery charge has been added to your order.\033[0m\n",DIS_ANSI,BOLD_ANSI,DELIVERY_CHARGE,DIS_ANSI);

							// Add a charge
							order->price += DELIVERY_CHARGE;
							order_price_global += DELIVERY_CHARGE;
							sleep(USER_SLEEP_DELAY);	
						}

						// Get other basic input info 
						order->name = input("Please enter your","name",INPUT_MIN_NAME,INPUT_MAX_NAME);
						if(order->name == NULL){

							// exit the program
							printf("Error: input returned NULL\n");
							break_while_loop = TRUE;
							break;
						}

						order->number = input("Please enter your","number",INPUT_MIN_NUMBER,INPUT_MAX_NUMBER);
						if(order->number == NULL){
							
							// exit the program
							printf("Error: input returned NULL\n");
							break_while_loop = TRUE;
							break;
						}

						// Only ask for address if the location mode is delivery 
						if(order->mode == DELIVERY){
							order->address = input("Please enter your","address",INPUT_MIN_ADDRESS,INPUT_MAX_ADDRESS);
							if(order->address == NULL){
								
								// exit the program
								printf("Error: input returned NULL\n");
								break_while_loop = TRUE;
								break;
							}
						}

						/* Now this might look like a rather insignificant line but really this line calls the main 
						 * attraction, that being the burrio selector program, this function is absolutly disgusting 
						 * to look at even though it is as pretty as i could make it which is why it is not included 
						 * in this release of the code. */ 
						order->type = display_burrito_menu();
						int end_sequence = TRUE;
						if(order->type == NULL){
							const char*screen_complete_order[]={"Yes","Restart Order"};
							if(draw_screen(screen_complete_order,(Value){.array_length=2},"Cancel Order\n")==1){
								continue;
							} else end_sequence = FALSE;
							order_price_global=0;
						} 

						if(end_sequence == TRUE){
							/* The display_burrito_menu returns a submenu of allocated memory, this submenu contains a 
							 * list of every burrito type, price and amount the user ordered, this simple loop adds up 
							 * the sum of all these values this number is used in various display elements. */
							for(int i=0;i<BURRITO_TYPE_AMOUNT;i++){
								int amount = order->type[i].amount; 
								order->price += amount*order->type[i].price;
								order->amount += amount;
							}

							// Draw the final screen 
							const char *screen_complete_order[]={"Yes","No, Restart Order",};
							char price_header[256];
							int snprintf_return = snprintf(price_header,sizeof(price_header),"Final Price: %s$%0.2f\033[0m, Confirm Order?",BOLD_ANSI,order->price);
							if(snprintf_return < 0 || (size_t)snprintf_return >= sizeof(price_header)){
								
								// NON FATAL error occured in snprintf we do not need to exit the proram
								printf("Error: snprintf failed!\n");
								break_while_loop = TRUE;
								break;
							}
						
					
						/* This conditional looks complex but simply will run if the user DOES NOT enter a value of 1
						 * as one would evaluate to true which would pass the conditional.
						 *
						 * UPDATE: the array_length is now a enum to improve visibilty instead of having '1' as the argument 
						 * it is now '(Value){.array_length = 1}' which has drastically improved readability. */ 
						if(!draw_screen(screen_complete_order,(Value){.array_length=2},price_header)){
							// Clean up for next frame
							order_amount_global++;
							order_price_global = 0;
							break;
						} else {

								// Go back to the start to redefine the values
								printf("Restarting order.\n");
								sleep(USER_SLEEP_DELAY);
								continue;
							}
						}
						break;
					}
					break;
			
				case 1:
					if(order_amount_global>0) draw_mangement(order_list,order_amount_global);
					else {
						printf("No orders to view\n");
						sleep(USER_SLEEP_DELAY);
					}
					break;

				case 2:

					// Draw the kitchen screen
					draw_kitchen_screen(order_list,order_amount_global);
					break;

				case 3:

					// Exit the while loop and free memory before exiting
					break_while_loop = TRUE;
					break;
				case 4:

					/* I think this is all quite straight forward just a whole bunch of print functions 
					 * to show some credits nothing fancy */
					clear();
					printf("== Credits ==\n");
					printf("plib3 library -> created by x3hy on github\n");
					printf("all other files where written, compiled and debugged by\n");
					printf("Lukan (2025)\n");
					printf("-------------\n");
					printf("If in doubt about rights to this code please refer to the\n");
					printf("LICENCE file that should have come with this code. if not\n");
					printf("you can view the code source at https://github.com/9jh1/CS12\n");
					printf("== Help ==\n");
					printf("this project's foundations are based on the presumption that\n");
					printf("your terminal converts the arrow keys to unicode chars (ABCD)\n");
					printf("if the arrow keys on your terminal are not functioning as you\n");
					printf("would expect refer to this guide to navigate through the menus\n");
					printf("-------------\n");
					printf("up    = A\n");
					printf("down  = B\n");
					printf("left  = C\n");
					printf("right = D\n");
					printf("-------------\n");
					printf("\nThank you for using my program!\n");
					wait();
					break;
			}
		}
		int memory_sum = 0,
				memory     = 0;

		/* Through out the program orders and inputs have been taking up allocated memory 
		 * what this block does is it goes through every order and frees any items that 
		 * take up memory, the input function for example returns malloced data and if we 
		 * dident free it that bit of data will be trapped in your ram until you reboot which 
		 * is not only stupid it is dangourous. */
		for(int i=0;i<order_amount_global;i++){
			if(verbose){
				memory = ((orders_capacity*sizeof(Burrito_type))/order_amount_global);
				memory_sum += memory; 
				printf("verbose -> %d: Free'd %d (tot %d) bytes from order[%d].type\n",__LINE__,memory,memory_sum,i);

				// Free the order name
				memory = INPUT_MAX_NAME;
				memory_sum += memory;
				if(order_list[i].name)
					printf("verbose -> %d: free'd %lu (pot %d, tot %d) bytes from order[%d].name\n",__LINE__,strlen(order_list[i].name),memory,memory_sum,i);	
							
				// Free the order number 
				memory = INPUT_MAX_NUMBER;
				memory_sum += memory;
				if(order_list[i].number)
					printf("verbose -> %d: Free'd %lu (pot %d, tot %d) bytes from order[%d].number\n",__LINE__,strlen(order_list[i].number),memory,memory_sum,i);
			}

			// ehm actually free the memory
			free(order_list[i].type);
			free(order_list[i].name);
			free(order_list[i].number);
						
			// Free the order address
			if(order_list[i].mode == DELIVERY){
				memory = INPUT_MAX_ADDRESS;
				memory_sum += memory;
				if (verbose ) printf("verbose -> %d: Free'd %lu (pot %d,tot %d) bytes from order[%d].address\n",__LINE__,strlen(order_list[i].address),memory,memory_sum,i);
				free(order_list[i].address);
			}
		}

		// Free the list and exit the program :D
		if(verbose){
			memory = orders_capacity*sizeof(Burrito);
			memory_sum += memory;
			printf("verbose -> %d: Free'd %d (tot %d) bytes of memory from order_list\n",__LINE__,memory, memory_sum);
			printf("verbose -> %d: Free'd %d total bytes of memory successfully!\n",__LINE__,memory_sum);
		}
		free(order_list);
	}
	return 0;
}
