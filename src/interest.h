#include <ncurses.h>
#include <form.h>
#include <stdlib.h>
#include <string.h>

// Special key compinations
#define MAC_BACKSPACE 127   // Backspace on mac
#define ENTER 10            // Enter key

// Input field parameters
#define INPUT_WIDTH 15

// Form window parameters
#define INPUT_WIN_WIDTH 40

// Output window parameters
#define OUTPUT_WIN_WIDTH 40

// Function prototypes
void create_output(     // Create formatted output in window w
        WINDOW *w,      // Output window
        double total,   // Total base cost
        double bottom,  // Bottom loan
        double top,      // Top loan,
        double bot_int // Interest on bottom loan
        );
double get_field_as_double(FIELD *f, int i); // Get field value as double. I have no idea what the second argument does, just set it to 0
void print_in_middle_of_win( // Print string c in the middle of window w
        WINDOW *w,
        int starty,
        char * c
        );

// Calculations
double calc_bottom(double total); // Calculate bottom loan part
