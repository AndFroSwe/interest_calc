/* 
 * File: interest.c
 * Author: andfro
 * Description: Calculates monthly costs for a loan.
 * Todo-list:
 * TODO: Clear output before printing new
 */

#include "interest.h"

int main() 
{
    // Create fields for form
    // Enum, fields and string array below have to 
    // match so that enum can be used for indexing
    typedef enum input_fields {
        total_cost, // The total cost of the loan
        interest,   // Interest [%]
        NO_INPUTS   // Number of fields, should always be last in enum
    } Input_fields;
    FIELD *inputs[NO_INPUTS + 1];   // Create fields
    char *field_names[] = { // Names to be printed with fields
        "Huspris [MKr]",
        "Ränta [%]"
    };

    // Initialize curses
    initscr();  // Initialize curses
    cbreak();   // Read one character at a time
    noecho();   // Don't echo input
    keypad(stdscr, TRUE);   // Enable function keys

    // Initialize input fields
    inputs[total_cost] = new_field(
            1,      // Height
            INPUT_WIDTH,     // Width
            2,     // Starty
            2,     // Startx
            0,      // Offscreen rows
            0       // Additional buffers
            );
    inputs[interest] = new_field(1, INPUT_WIDTH, 4, 2, 0, 0);
    inputs[NO_INPUTS] = NULL;

    // Set field options
    // This sets all the input fields to the same options
    for (int i = 0; i < NO_INPUTS; ++i) {
        set_field_back(inputs[i], A_UNDERLINE);
        field_opts_off(inputs[i], O_AUTOSKIP);
    } // end for
    // Field validations
    // These have to be done manually since there are different
    // types of expected values in the fields.
    set_field_type( // Price is in MKr
            inputs[total_cost],     // Field
            TYPE_NUMERIC,           // Type = Decimal
            2,                      // Precision
            0.,                      // Vmin
            5.0                       // Vmax
            );
    set_field_type( // Interest is in percent
            inputs[interest],       // Field
            TYPE_NUMERIC,           // Type = Decimal
            2,                      // Precision
            0.0,                      // Vmin
            100.0                     // Vmax
            );

    // Create a window for the form
    WINDOW *input_win = NULL;
    input_win = newwin(     // Allocate input window
            NO_INPUTS + 7,   // Height
            //30,
            INPUT_WIN_WIDTH,// Width
            10,             // Starty
            10              // Startx
            );
    keypad(input_win, TRUE);    // Allow function keys
    box(input_win, 0, 0);       // Set a border around window

    // Create form and post it
    FORM *input_form = NULL;        // Initialize form
    input_form = new_form(inputs);  // Allocate form
    // Create window for form
    set_form_win(input_form, input_win);    // Set form to window
    set_form_sub(
            input_form, 
            derwin(             // Create local subwindow
                input_win,      // Parent
                NO_INPUTS + 4,  // Height
                17,             // Width
                2,              // Starty
                21              // Startx
                )
            );
    post_form(input_form);
    // Print items in input window
    mvwprintw(input_win, 1, 15, "Inputs"); // Title
    // Print field names
    for (int i = 0; i < NO_INPUTS; ++i) {
        int start_y = 4; // Y position at which fields start
        mvwprintw(
                input_win,      // Window
                start_y + i*2,  // Position increases with 2 each entry
                2,              // X position
                field_names[i]  // Text
                );
    } // end for

    // Create field names
    // Enum and array must match
    typedef enum output_fields {
        o_total,    // Total price
        o_bottom,   // Bottom loan
        o_top,      // Top loan
        o_bot_year, // Yearly interest in kr
        NO_OUTPUTS  // Used for indexing, should always be last
    } Output_fields;
    char *outputs[] = {
        "Pris:",
        "Bottenlån:",
        "Topplån:",
        "Arsränta, botten:"
    };
    // Create output window
    WINDOW *output_win = NULL;
    output_win = newwin(
            NO_OUTPUTS + 7,   // Height
            OUTPUT_WIN_WIDTH,// Width
            10,              // Starty
            60               // Startx
            );
    // Print names in output window
    for (int i = 0; i < NO_OUTPUTS; ++i) {
        mvwprintw(
                output_win, // Window
                3 + i*2,    // Starty
                2,          // Startx
                outputs[i]
                );
    } // end for
    mvwprintw( // Print title
            output_win,
            1,
            15,
            "Outputs"
            );
    box(output_win, 0, 0); // Box around output

    // Pimp and print the main window
    box(stdscr, 0, 0);
    print_in_middle_of_win(stdscr, 2, "Räntekalkylator");
    refresh();

    // Show input/output
    wrefresh(input_win);
    wrefresh(output_win);

    // Main loop
    int ch;
    while ((ch = getch()) != 'q')
    {
        switch (ch) 
        {
            // Navigation (Vim and arrow keys)
            case KEY_DOWN:
            case 'j':
                form_driver(input_form, REQ_NEXT_FIELD);
                form_driver(input_form, REQ_END_LINE);
                break;
            case KEY_UP:
            case 'k':
                form_driver(input_form, REQ_PREV_FIELD);
                form_driver(input_form, REQ_END_LINE);
                break;
            case KEY_RIGHT:
            case 'l':
                form_driver(input_form, REQ_NEXT_CHAR);
                break;
            case KEY_LEFT:
            case 'h':
                form_driver(input_form, REQ_PREV_CHAR);
                break;

            // Input characters
            case ENTER:
                form_driver(input_form, REQ_VALIDATION); // To force validation
                // Get data. I have just added a _t to these variables to
                // separate them from other vars.
                double total_t = get_field_as_double(inputs[total_cost], 0);
                double bot_int_t = get_field_as_double(inputs[interest], 0);
                double bottom_t = calc_bottom(total_t);
                // Calculate data
                double top_t = total_t - bottom_t;
                double bot_int_year_t = bottom_t*bot_int_t/100*1e6; // Calculate year 1 interest
                create_output(
                        output_win,
                        total_t,
                        bottom_t,
                        top_t,
                        bot_int_year_t
                        );
                break;
            case MAC_BACKSPACE:
                form_driver(input_form, REQ_DEL_PREV);
            default:
                form_driver(input_form, ch);
                mvprintw(30, 10, "Character was: %d", ch);
                break;
        } // end switch
        wrefresh(input_win); // Refresh after update
    } // end main loop

    // Free resources
    unpost_form(input_form);
    free_form(input_form);
    // Free all fields
    for (int i = 0; i < NO_INPUTS; ++i) {
        free_field(inputs[i]);
    } // end for
    // Delete windows
    delwin(input_win);

    endwin();
    return 0;
}



// Functions
void create_output(
        WINDOW *w,
        double total, 
        double bottom,
        double top,
        double bot_int
        )
{
    // Parameters
    char delim[] = "--------------------------------------"; // Delimiter between rows
    int x = 20; // x value to print from
    int y = 3; // First y value to print
    int i = 0; // Index variable
    // mvwprintw(window, starty, startx, string)
    mvwprintw(w, y + (i++)*2, x, "%0.2f miljoner kr", total);
    mvwprintw(w, y + (i++)*2, x, "%1.2f miljoner kr", bottom);
    mvwprintw(w, y + (i++)*2, x, "%1.2f miljoner kr", top);
    mvwprintw(w, y + (i)*2-1, 1, delim);
    mvwprintw(w, y + (i++)*2, x, "%1.2f kr", bot_int);


    mvprintw(30, 10, "Character was: %d", i);
    wrefresh(w); // Write to window
}

double get_field_as_double(FIELD *f, int i)
{
    char *c = NULL;
    c = field_buffer(f, i);

    return atof(c);
}

void print_in_middle_of_win(
        WINDOW *w,
        int starty,
        char * c
        )
{
    int y, x;   // Size variables for window
    size_t length; // Size of string

    // Get window size
    getyx(w, y, x);

    // Get string length
    length = strlen(c);
    length = (int) length;

    // Print
    mvwprintw(
            w,
            starty,
            (x - length)/2,
            c
            );
}
// Calculation functions
double calc_bottom(double total)
{
    int p = 85; // Bottom loan percentage

    return total*p/100;
}
