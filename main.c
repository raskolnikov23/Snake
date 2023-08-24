#include "main.h"

int main() 
{
    Initialization(); 
    DrawMap(); 

    while (1) 
    {
        InputLogic(); 
        SnakeLogic();
        AppleLogic();
        RenderUI();
        Debugger();

	    fflush(stdout);
        usleep(3 * 1000000 / 60); // framerate limiter
    }
}

void AppleLogic()
{
    // if no apple
    if (apple_x == -1)
    {
        apple_x = rand() % COLS;
        apple_y = rand() % ROWS;

        // check if apple isn't on tail
        for (int i = tail; i != head; i = (i + 1) % map_size)
            if (x[i] == apple_x && y[i] == apple_y)
                apple_x = -1;

        // if apple valid, draw 
        if (apple_x >= 0) 
        {
            printf("\e[%iB\e[%iC❤", apple_y + 1, apple_x + 1); 
            printf("\e[%iF", apple_y + 1);                   
        }
    }
}

void SnakeLogic()
{
    if (ai_mode == 1) AI_Mode();
    fflush(stdout);

    printf("\e[%iB\e[%iC ", y[tail] + 1, x[tail] + 1);      // clear last tail part from screen
    printf("\e[H");                                         // send cursor back to top left

    // if apple eaten
    if (x[head] == apple_x && y[head] == apple_y) 
    {
        apple_x = -1;
        score += 1;
        printf("\a"); // beep (mac)
    }
    else tail = (tail + 1) % map_size;

    // recalculate head
    int newhead = (head + 1) % map_size;
    x[newhead] = (x[head] + dir_x + COLS) % COLS;
    y[newhead] = (y[head] + dir_y + ROWS) % ROWS;
    head = newhead;

    // if snake collides with itself
    for (int i = tail; i != head; i = (i + 1) % map_size)
        if (x[i] == x[head] && y[i] == y[head])
            GameOver();

    // draw head
    printf("\e[%iB\e[%iC▓", y[head] + 1, x[head] + 1);
    printf("\e[H");  
}

void InputLogic()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);

    if (FD_ISSET(STDIN_FILENO, &fds)) 
    {
        int ch = getchar();

        if (ch == 'q') ExitGame();
        else if (ch == 'z') ai_mode *= -1;
        else if (ch == 'x') debug *= -1;

        // controls
        else if (ch == 'a' && dir_x != 1)
        {
            dir_x = -1;
            dir_y = 0;
        }
        else if (ch == 'd' && dir_x != -1)
        {
            dir_x = 1;
            dir_y = 0;
        }
        else if (ch == 's' && dir_y != -1)
        {
            dir_x = 0;
            dir_y = 1;
        }
        else if (ch == 'w' && dir_y != 1)
        {
            dir_x = 0;
            dir_y = -1;
        }
    }
}

void DrawMap()
{
    // Top
    printf("╔");

    for (int i = 0; i < COLS; i++) 
    {
        usleep(2 * 1000000 / 60);
        printf("═");
        fflush(stdout);
    }

    printf("╗\n");

    // Middle
    for (int j = 0; j < ROWS; j++) 
    {
        usleep(1 * 1000000 / 60);
        printf("║");

        for (int i = 0; i < COLS; i++) 
            printf(" ");
        
        usleep(1 * 1000000 / 60);
        printf("║\n");
    }

    // Bottom
    printf("╚");

    for (int i = 0; i < COLS; i++) 
    {
        usleep(2 * 1000000 / 60);
        printf("═");
        fflush(stdout);
    }

    printf("╝\n");
    printf("\e[H"); 
}

void RenderUI()
{
    WriteLine("\e[1mSCORE: ", score, 0);
    WriteLine("\e[mControls: \e[1mWASD\e[m", -2, 2);
    WriteLine("Debug: \e[1mX\e[m", -2, 3);
}

int ExitGame()
{
    printf("\e[?25h");  // show cursor
    system("reset");    // reset terminal
    exit(0);
}

void GameOver()
{
    // game over screen
    printf("\e[%iB\e[%iC GAME OVER!", ROWS / 2 - 1, COLS / 2 - 5);
    printf("\e[10B\e[15DR to retry, Q to quit");
    printf("\e[H"); 

    int pressed_key;

    do {
        pressed_key = getchar();

        if (pressed_key == 'r') main();
        else if (pressed_key == 'q') ExitGame();

    } while (pressed_key != 'r' || 'q');
}

void Initialization()
{
    printf("\e[?25l");      // hide cursor
    system("clear");        // clear terminal

    // setup raw mode  
    tcgetattr(STDIN_FILENO, &orig_termios);
    new_termios = orig_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

    // init values 
    head = 0, tail = 0;
    dir_x = 0, dir_y = 1; 
    apple_x = -1;
    score = 0;
    debug = -1;
    debug_cleared = 1;
    ai_mode = 1;
    x_diff = 0;
    y_diff = 0;

    // start pos   
    x[head] = COLS / 2; 
    y[head] = ROWS / 2; 
}

void Debugger()
{
    if (debug == 1) 
    {
        printf("\e[?25h");  // show cursor
        
        WriteLine("=================", -2, 5);

        WriteLine("head = ", head, 7);
        WriteLine("tail = ", tail, 8);

        WriteLine("x[head] = ", x[head], 9);
        WriteLine("y[head] = ", y[head], 10);

        WriteLine("x[tail] = ", x[tail], 11);
        WriteLine("y[tail] = ", y[tail], 12);

        WriteLine("x diff: ", x_diff, 14);
        WriteLine("y diff: ", y_diff, 15);

        WriteLine("x dir: ", dir_x, 17);
        WriteLine("y dir: ", dir_y, 18);

        fflush(stdout);
        debug_cleared = 0;
    }

    else 
    {
        if (debug_cleared == 0) 
        {
            // clear 'em
            WriteLine("", -2, 5);

            WriteLine("", -2, 7);
            WriteLine("", -2, 8);

            WriteLine("", -2, 9);
            WriteLine("", -2, 10);

            WriteLine("", -2, 11);
            WriteLine("", -2, 12);

            WriteLine("", -2, 14);
            WriteLine("", -2, 15);

            WriteLine("", -2, 17);
            WriteLine("", -2, 18);

            printf("\e[?25l");  // hide cursor
            debug_cleared = 1;
        }
    }
}

void WriteLine(const char* line, int value, int line_offset)
{
    char* line2 = malloc(200); // to array?
    strcpy(line2, line);

    printf("\e[%iC\e[%iB\e[K", UI_OFFSET_X, UI_OFFSET_Y + line_offset); // erase line
    printf("\e[H"); 

    if (value == -2) printf("\e[%iC\e[%iB%s", UI_OFFSET_X, UI_OFFSET_Y + line_offset, line2);
    else printf("\e[%iC\e[%iB%s%i", UI_OFFSET_X, UI_OFFSET_Y + line_offset, line2, value);

    printf("\e[H"); 
    free(line2);
}

void AI_Mode()
{
    // determine relative apple pos
    if (x[head] > apple_x) x_diff = -1;         // apple left of head
    else if (x[head] == apple_x) x_diff = 0;    // on same horizontal level
    else x_diff = 1;                            // apple right of head


    if (y[head] > apple_y) y_diff = -1;         // apple above head
    else if (y[head] == apple_y) y_diff = 0;    // on same vertical level
    else y_diff = 1;                            // apple below head
    

    // determine if approaching tail  
    for (int i = tail; i != head; i = (i + 1) % map_size)
    {
        // if next step is tail  
        if (x[head] + dir_x == x[i] % COLS && y[head] + dir_y == y[i] % ROWS)
        {
            WriteLine("x", -2, 19);

            // if moving vertically 
            if (dir_x == 0)     
            {
                dir_y = 0;

                for (int j = tail; j != head; j = (j + 1) % map_size)
                {
                    // if to the left from head isnt tail
                    if (x[head] - 1 != x[j])
                    {
                        dir_x = -1;
                        return;
                    }
                    else 
                    {
                        dir_x = 1;
                        return;
                    }
                }
            }
            
            // if moving horizontally  
            else if (dir_y == 0)    
            {
                dir_x = 0;

                for (int j = tail; j != head; j = (j + 1) % map_size)
                {
                    // check above for tail
                    if (y[head] - 1 != y[j])
                    {
                        dir_y = -1;
                        return;
                    }
                    else 
                    {
                        dir_y = 1;
                        return;                        
                    } 
                }  
            }
        }
    }

    // if approaching apple return
    if (x[head] == apple_x && dir_x == 0 || y[head] == apple_y && dir_y == 0) return;

    if (dir_x == 0)
    {
        if (y_diff != 0) return;

        if (y_diff == 0)
        {
            dir_y = 0;
            dir_x = x_diff;
        }
    }
    else if (dir_y == 0)
    {
        if (x_diff != 0) return;
            
        if (x_diff == 0)
        {
            dir_x = 0;
            dir_y = y_diff;
        }
    }
}