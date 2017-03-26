#include <stdio.h>
#include <ncurses.h>
#include <form.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define THREADNUM 4

struct args {
	int arg1;
	FIELD **arg2;
	int arg3;
};

void *countdownThread(void *arg)
{
	struct args *args = arg;
	char buff[4];
	int i    = (int) args -> arg1;
	int curr = (int) args -> arg3;

	while (i-- != 0) {
		sleep(1);
		if (i % 60 == 0) {
			if ((i % 60) >= 0 && (i % 60) <= 9)
				sprintf(buff, "0%d", i/60);
			else
				sprintf(buff, "0%d", i/60);
			set_field_buffer(args -> arg2[curr], 0, buff);
		}
		refresh();
	}
	return (void *) 1;
}

int main(void)
{
	FIELD *field[5];
	FORM  *main_form;
	int ch, i, curr;
	pthread_t tid[THREADNUM];
	struct args args;

	/* Initialize ncurses */
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	/* Initialize Fields */
	/* legend:           height, width, y-coord, x-coord */
	field[0] = new_field(1,      2,     0,       18, 0, 0);
	field[1] = new_field(1,      2,     1,       18, 0, 0);
	field[2] = new_field(1,      2,     2,       18, 0, 0); 
	field[3] = new_field(1,      2,     3,       18, 0, 0);
	field[4] = NULL;

	/* Set field options */
	field_opts_off(field[0], O_AUTOSKIP);
	field_opts_off(field[1], O_AUTOSKIP);
	field_opts_off(field[2], O_AUTOSKIP);
	field_opts_off(field[3], O_AUTOSKIP);

	set_field_type(field[0], TYPE_INTEGER, 2, 100);
	set_field_type(field[1], TYPE_INTEGER, 2, 100);
	set_field_type(field[2], TYPE_INTEGER, 2, 100);
	set_field_type(field[3], TYPE_INTEGER, 2, 100);

	/* Create new form */
	main_form = new_form(field);
	post_form(main_form);

	mvprintw(0, 0, "Above the castle: ");
	mvprintw(1, 0, "Below the castle: ");
	mvprintw(2, 0, "Forest: ");
	mvprintw(3, 0, "Nui: ");
	mvprintw(4, 0, "Type timer into appropriate box and press q to begin "
	               "countdown.");
	move(0,18);

	refresh();
	curr = 0;
	while ((ch = getch()) != KEY_F(1)) {
		switch(ch) {
			case KEY_DOWN:
				/* 
				 * Track the current location. "Curr" is used instead of ncurses
				 * Built in method of getting the current field because it's
				 * easier.
				 */
				if (curr == 3)
					curr = 0;
				else
					curr++;
				/* Move to next field */
				form_driver(main_form, REQ_NEXT_FIELD);
				/* Last character of next buffer */
				form_driver(main_form, REQ_END_LINE);
				break;
			case KEY_UP:
				if (curr == 0)
					curr = 3;
				else
					curr--;
				form_driver(main_form, REQ_PREV_FIELD);
				form_driver(main_form, REQ_END_LINE);
				break;
			/* 
			 * Deletion only works with delete key right now,
			 * rather than delete & backspace.
			 *
			 * The character hovered over is deleted.
			 */
			case KEY_DC:
				form_driver(main_form, REQ_DEL_CHAR);
				break;
			case KEY_LEFT:
				form_driver(main_form, REQ_LEFT_CHAR);
				break;
			case KEY_RIGHT:
				form_driver(main_form, REQ_RIGHT_CHAR);
				break;
			case 'q':
				/* Get currently selected field's content */
				i  = atoi(field_buffer(field[curr], 0));
				/* Convert to seconds */
				i *= 60;
				args.arg1 = i;
				args.arg2 = field;
				args.arg3 = curr;
				/* Create new thread */
				pthread_create(&tid[curr], NULL, &countdownThread,
				              (void *) &args);
				break;
			default:
				if (isdigit(ch))
					form_driver(main_form, ch);
				break;
		}
	}
	/* Clean-up forms */
	unpost_form(main_form);
	free_form(main_form);
	free_field(field[0]);
	free_field(field[1]);

	/* End ncurses; close program */
	endwin();
	return 0;
}
