
#include <kernel.h>
WINDOW* train_wnd = NULL;
PORT train_port;

#define COMMAND_OUTPUT_MAX_SIZE 10
#define COMMAND_INPUT_MAX_SIZE 5

#define COMMAND_SLEEP 10
#define CONFIG_3_SLEEP 80
#define CONFIG_4_SLEEP 210
#define CONFIG_4Z_SLEEP 160
#define CHECK_ZAMBONI_SLEEP 20

BOOL zamboni_appear = FALSE;
BOOL zamboni_go_left = TRUE;
BOOL train_running = FALSE;

/****************Helper functions**************/

/*Send train command to com1 port*/
BOOL execute_train_command(const char* cmd) {
	
	int len = k_strlen(cmd);
	if(len <= 0 || len > COMMAND_OUTPUT_MAX_SIZE)
		return FALSE;

	//put the command into the output_buffer
	char output_buffer[COMMAND_OUTPUT_MAX_SIZE];
	k_memcpy(output_buffer, cmd, len);
	output_buffer[len] = '\015'; //add '\015' for each command
	output_buffer[len+1] = '\0';

	char input_buffer[COMMAND_INPUT_MAX_SIZE];

	COM_Message msg;
	msg.output_buffer = output_buffer;
	msg.input_buffer = input_buffer;
	if(cmd[0] == 'C')
		msg.len_input_buffer = 3;
	else
		msg.len_input_buffer = 0;

	sleep(COMMAND_SLEEP);
	send(com_port, &msg);

	//return value for 'C' command
	if (cmd[0] == 'C') {
		return msg.input_buffer[1] == '1';
	} else {
		return FALSE;
	}

}

void set_switch(char *switch_number, char *color) {
	char command[4];
	command[0] = 'M';
	command[1] = switch_number[0];
	command[2] = color[0];
	command[3] = '\0';
	execute_train_command(command);
}

BOOL probe(char *contact_number) { 
	int len = k_strlen(contact_number);

	char command[4];
	command[0] = 'C';
	if(len == 1) {
		command[1] = contact_number[0];
		command[2] = '\0';	
	}
	else if(len == 2){
		//check if the contact number is beyond the bound
		if(contact_number[0] != '1' || contact_number[1] > '6' 
			|| contact_number[1] < '0') {
			wprintf(train_wnd, "Invalid Contact ID.\n");
			return FALSE;
		}	

		command[1] = contact_number[0];
		command[2] = contact_number[1];
		command[3] = '\0';
	}
	else
		return FALSE;

	//clear buffer
	execute_train_command("R");
	return execute_train_command(command);
}

void keep_probing_if_not_on(char *contact_number) {
	while (42) {
		if(probe((char *)contact_number) == TRUE) return;
   	}
}

/****************End of helper functions**************/

/****************Configuration functions**************/
void zamboni_route_1() {
	wprintf(train_wnd, "Set route for Zamboni.\n");
	set_switch("1", "R");

	if(zamboni_go_left)
		keep_probing_if_not_on("13");
	else
		keep_probing_if_not_on("14");

	set_switch("9", "R");
	set_switch("1", "R");
	set_switch("8", "R");
}

void zamboni_route_2() {
	wprintf(train_wnd, "Set route for Zamboni.\n");
	set_switch("8", "G");
	set_switch("5", "G");
	set_switch("4", "G");
	set_switch("7", "R");
	set_switch("2", "R");
	set_switch("9", "R");
	set_switch("1", "G");
}

void config_1_or_2() {
	wprintf(train_wnd, "Running config either 1 or 2 without Zamboni\n");
	set_switch("6", "R");
	set_switch("5", "R");
	set_switch("4", "R");
	set_switch("3", "G");
	execute_train_command("L20S5");

	keep_probing_if_not_on("1");

	execute_train_command("L20S0");
	execute_train_command("L20D");
	execute_train_command("L20S5");

	keep_probing_if_not_on("8");
	execute_train_command("L20S0");
	wprintf(train_wnd, "Train brings wagon back to HOME in either config 1 or 2.\n");
}

void config_1Z() {
	wprintf(train_wnd, "Running config 1 with Zamboni\n");
	keep_probing_if_not_on("12");
	set_switch("6", "R");
	set_switch("5", "R");
	set_switch("4", "R");
	set_switch("3", "G");
	execute_train_command("L20S5");

	keep_probing_if_not_on("1");

	execute_train_command("L20S0");
	execute_train_command("L20D");
	execute_train_command("L20S5");

	keep_probing_if_not_on("8");
	execute_train_command("L20S0");
	wprintf(train_wnd, "Train brings wagon back to HOME in config 1 with Zamboni.\n");

}

void config_2Z() {
	wprintf(train_wnd, "Running config 2 with Zamboni\n");
	keep_probing_if_not_on("12");
	set_switch("6", "R");
	set_switch("5", "R");
	set_switch("4", "R");
	set_switch("3", "G");
	execute_train_command("L20S5");

	keep_probing_if_not_on("1");

	execute_train_command("L20S0");
	execute_train_command("L20D");
	execute_train_command("L20S5");

	keep_probing_if_not_on("8");
	execute_train_command("L20S0");
	wprintf(train_wnd, "Train brings wagon back to HOME in config 2 with Zamboni.\n");
}

void config_3() {
	wprintf(train_wnd, "Running config 3 without Zamboni\n");
	set_switch("5", "R");
	set_switch("6", "G");
	execute_train_command("L20S5");

	keep_probing_if_not_on("12");

	execute_train_command("L20S0");
	execute_train_command("L20D");
	execute_train_command("L20S5");

	keep_probing_if_not_on("13");
	
	execute_train_command("L20S0");
	execute_train_command("L20D");
	execute_train_command("L20S5");

	set_switch("5","G");
	set_switch("8","G");

	set_switch("4","R");
	set_switch("3","R");

	keep_probing_if_not_on("5");
	execute_train_command("L20S0");
	wprintf(train_wnd, "Train brings wagon back to HOME in config 3.\n");
}

void config_3Z() {
	wprintf(train_wnd, "Running config 3 with Zamboni\n");

	set_switch("3", "R");
	set_switch("4", "R");
	set_switch("5", "R");
	set_switch("6", "G");
	
	keep_probing_if_not_on("10");
	execute_train_command("L20S5");

	keep_probing_if_not_on("14");
	set_switch("1", "G");

	keep_probing_if_not_on("12");

	execute_train_command("L20S0");
	execute_train_command("L20D");

	set_switch("7", "R");
	set_switch("8", "R");
	execute_train_command("L20S5");

	keep_probing_if_not_on("13");
	execute_train_command("L20S0");
	execute_train_command("L20D");

	set_switch("8", "G");
	execute_train_command("L20S5");

	keep_probing_if_not_on("5");
	execute_train_command("L20S0");

	set_switch("4", "G");

	wprintf(train_wnd, "Train brings wagon back to HOME in config 3 with Zamboni.\n");
}

void config_4() {
	wprintf(train_wnd, "Running config 4 without Zamboni\n");
	set_switch("3","R");
	set_switch("4","R");
	set_switch("5","G");
	set_switch("8","G");
	set_switch("9","G");
	execute_train_command("L20S5");

	keep_probing_if_not_on("14");
	sleep(CONFIG_4_SLEEP);

	execute_train_command("L20S0");
	execute_train_command("L20D");
	execute_train_command("L20S5");

	keep_probing_if_not_on("5");
	execute_train_command("L20S0");
	wprintf(train_wnd, "Train brings wagon back to HOME in config 4.\n");

}

void config_4Z() {
	wprintf(train_wnd, "Running config 4 wit Zamboni\n");
	keep_probing_if_not_on("10");
	keep_probing_if_not_on("14");

	set_switch("9", "R");
	set_switch("1", "R");
	set_switch("8", "R");

	set_switch("3", "R");
	execute_train_command("L20S5");

	keep_probing_if_not_on("6");
	execute_train_command("L20S0");
	execute_train_command("L20D");
	execute_train_command("L20S5");

	set_switch("4", "G");
	set_switch("1", "G");
	set_switch("8", "G");

	keep_probing_if_not_on("10");
	set_switch("9", "G");
	sleep(30);
	execute_train_command("L20S0");
	execute_train_command("L20D");
	execute_train_command("L20S5");
	

	keep_probing_if_not_on("14");
	sleep(CONFIG_4Z_SLEEP);
	execute_train_command("L20S0");
	execute_train_command("L20D");
	execute_train_command("L20S5");

	keep_probing_if_not_on("14");
	set_switch("9", "R");

	keep_probing_if_not_on("4");

	set_switch("3", "R");
	set_switch("4", "R");

	keep_probing_if_not_on("5");
	execute_train_command("L20S0");
	set_switch("4", "G");

	wprintf(train_wnd, "Train brings wagon back to HOME in config 4 with Zamboni.\n");

}

//determine the configuration according to the current contacts staus
void run_according_to_config() {
	wprintf(train_wnd, "Checking the contact board and deciding Config...\n");

	zamboni_route_2();

	sleep(CHECK_ZAMBONI_SLEEP);
	//probe contact 3 and 6 to determain if zamboni appears
	//if it's on 3 ---> zamboni goes to right first
	//if it's on 6 ---> zamboni goes to left first  
	if(probe("3")) {
		zamboni_appear = TRUE;
		zamboni_go_left = FALSE;
	} 
	else {
		if(probe("6") || probe("7")) {
			zamboni_appear = TRUE;
			zamboni_go_left = TRUE;
		} else {
			zamboni_appear = FALSE;
		}
	}

	//check the start position of the train
	//if it's on 8 ---> configuration 1 or 2
	//if it's on 5 ---> configuration 3 or 4
	if(probe("8")) {
		if(!zamboni_appear) {
			config_1_or_2();
		}
		else if(zamboni_go_left) {
			zamboni_route_1();
			config_1Z();
		}
		else {
			zamboni_route_1();
			config_2Z();
		}
	}
	else if(probe("5")) {	
		//if wagon is on 11 ---> configuration 3
		if(probe("11")){
			if(!zamboni_appear) {
				config_3();
			}
			else if(zamboni_appear && zamboni_go_left){
				config_3Z();
			}
		}
		//if wagon is on 16 ---> configuration 4
		else if(probe("16")) {
			if(!zamboni_appear) {
				config_4();
			}
			else if(zamboni_appear && !zamboni_go_left){
				config_4Z();
			}
		}
	}


}


/**************End of configuration functions************/

//**************************
//run the train application
//**************************

void train_process(PROCESS self, PARAM param)
{
	train_running = TRUE;

	clear_window(train_wnd);	

	wprintf(train_wnd, "Train process initialized.\n");	

	run_according_to_config();

	train_running = FALSE;

	remove_ready_queue(active_proc);
    resign();
}


void init_train(WINDOW* wnd)
{
	train_wnd = wnd;	
	train_port = create_process(train_process, 3, 0, "Train Process");
	return;
}
