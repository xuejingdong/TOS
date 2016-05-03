#include <kernel.h>

static WINDOW shell_window_def = {0, 9, 61, 16, 0, 0, 0xDC};
WINDOW* shell_window = &shell_window_def;

static WINDOW pacman_window_def = {61, 9, 61, 16, 0, 0, 0xDC};
WINDOW* pacman_window = &pacman_window_def;

/* Command buffer */
#define MAX_COMMAND 100
char curr_command[MAX_COMMAND];
//variable to track the size of current command buffer
int command_size = 0;

/***************shell functions******************/
void func_help() {
	wprintf(shell_window, "TOS shell commands: \n");
	wprintf(shell_window, "help   : show help of commands\n");
	wprintf(shell_window, "clear  : clear shell window\n");
	wprintf(shell_window, "ps     : print all processes status\n");
	wprintf(shell_window, "about  : show about\n");
	wprintf(shell_window, "pacman : start pacman game\n");
	wprintf(shell_window, "train  : start train application\n");
	wprintf(shell_window, "go     : start train\n");
	wprintf(shell_window, "stop   : stop train\n");
	wprintf(shell_window, "switch : switch the direction of the train\n");
}

/* function that clears the shell window */
void func_clear() {
	clear_window(shell_window);
}

/* function that shows all the processes in the shell window */
void func_ps() {
	print_all_processes(shell_window);
}

/* function that shows some info about the writer the shell window*/
void func_about() {
	wprintf(shell_window, "TOS shell from Xuejing Dong, SFSU\n");
}

/* function that starts pacman in the pacman window*/
void func_pacman() {
	init_pacman(pacman_window, 2);
}

/* function that starts train application*/
void func_train() {
	wprintf(shell_window, "train command which starts the train application will be supported soon!\n");
}

/* function that starts the train*/
void func_go() {
	wprintf(shell_window, "go command which triggers the train to run will be supported soon!\n");
}

/* function that stops the train*/
void func_stop() {
	wprintf(shell_window, "stop command which stops the running train will be supported soon!\n");
}

/* function that reverse the train direction*/
void func_switch() {
	wprintf(shell_window, "switch command which changes the heading the running train will be supported soon!\n");
}

/* function that tells the user the invalid command*/
void func_invalid_command() {
	wprintf(shell_window, "Invalid command!\n");
}
 
/***************shell functions end******************/

/*Get rid of spaces in a string*/
char * remove_space(char *input)
{
  	char *output = input, *str = input;
  	for(; *input != '\0'; ++input)
  	{
    	if(*input != 32) //Check if the current char is a space
      		*str++ = *input;
  	}
  	*str = '\0';

  	return output;
}


/*Compare two command string*/
int command_compare(char* input1, char* input2) {
	char* input = remove_space(input1);

	while (*input == *input2 && *input2 != '\0') {
		input++;
		input2++;
	}

	return *input2 == '\0';
}

/*Execute the command user entered
**First find the matching command
**Call the corresponding shell fucntion to do the work
*/
void execute_command() {
	wprintf(shell_window, "\n");
	if(command_compare(curr_command, "help")) {
		func_help();
		return;
	}

	else if(command_compare(curr_command, "clear")) {
		func_clear();
		return;
	}

	else if(command_compare(curr_command, "ps")) {
		func_ps();
		return;
	}

	else if(command_compare(curr_command, "about")) {
		func_about();
		return;
	}

	else if(command_compare(curr_command, "pacman")) {
		func_pacman();
		return;
	}

	else if(command_compare(curr_command, "train")) {
		func_train();
		return;
	}

	else if(command_compare(curr_command, "go")) {
		func_go();
		return;
	}

	else if(command_compare(curr_command, "stop")) {
		func_stop();
		return;
	}

	else if(command_compare(curr_command, "switch")) {
		func_switch();
		return;
	}

	else {
		func_invalid_command();
		return;
	}
 
}

//shell process
void shell_process(PROCESS self, PARAM param) {
    wprintf(shell_window, "$ ");
    Keyb_Message message;
    char cur_char;
    
	while (42) {
		message.key_buffer = &cur_char; 
		send(keyb_port, &message); 
		/*if the input is a backspace
		**decrease buffer size, update window
		*/
		if (cur_char == 8) { 
			if(command_size > 0) { 
				command_size--;
				wprintf(shell_window, "%c", cur_char);
			}
		}
		/*if the input is a ENTER
		**first check if buffer size greater than one
		**if it's greater than one, then execument the command
		*/ 
		else if(cur_char == 13) {
			//execute the command only when there is command entered
			if(command_size > 0) {			
				execute_command();
				command_size = 0;	
			}
			else 
				wprintf(shell_window, "\n");

			wprintf(shell_window, "$ ");
		} 

		/*if the input is a normal char
		**put the char into the buffer, increase size
		**update the window
		*/ 
		else if(command_size < MAX_COMMAND) {
			curr_command[command_size++] = cur_char;
			curr_command[command_size] = '\0';
			wprintf(shell_window, "%c", cur_char);
		}
		
	}
}

void init_shell()
{
	clear_window(kernel_window);
	create_process(shell_process, 5, 0, "Shell Process");
    resign();
}
