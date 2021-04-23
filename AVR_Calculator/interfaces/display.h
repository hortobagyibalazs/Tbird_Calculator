// Enables display
void display_enable();

// Disables display
void display_disable();

// Returns 1 when display is turned on, 0 when turned off
int display_is_on();

// Makes cursor visible
void display_enable_cursor();

// Makes cursor invisible
void display_disable_cursor();

// Returns 1 when cursor is enabled, 0 when disabled
int display_is_cursor_visible();

// Enables blinking
void display_enable_blink();

// Disables blinking
void display_disable_blink();

// Returns 1 when blinking is enabled, 0 when disabled
int display_is_blink_enabled();

// Clears entire screen and resets cursor position
void display_clear();

// Writes text to the screen
void display_write(char *text);