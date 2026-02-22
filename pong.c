#include "pong.h"
#include "intdefs.h"
#include "graphics.h"
#include "io.h"

#define LINE_LENGTH 15
#define LINE_WIDTH 2
#define PADDLE_HEIGHT 20
#define PADDLE_WIDTH 5

//=====================
//      Types
//=====================

typedef struct Point {
    uint16_t x;
    uint16_t y;
} Point;

typedef struct Vector {
    int16_t x;
    int16_t y;
} Vector;

//=====================
//     Variables
//=====================

// writescore uses this for writing the numbers
// writescore draws numbers by drawing the sides of a digit like you would see in a digital clock
// 1 means draw the side 0 means dont
// the order is: top, top left, top right, middle, bottom left, bottom right, bottom
const uint8_t digit_sides[10][7] = {
    { // Zero
        1,1,1,0,1,1,1
    },
    { // One
        0,1,0,0,1,0,0
    },
    { // Two
        1,0,1,1,1,0,1
    },
    { // Three
        1,0,1,1,0,1,1
    },
    { // Four
        0,1,1,1,0,1,0
    },
    { // Five
        1,1,0,1,0,1,1
    },
    { // Six
        1,1,0,1,1,1,1
    },
    { // Seven
        1,0,1,0,0,1,0
    },
    { // Eight
        1,1,1,1,1,1,1
    },
    { // Nine
        1,1,1,1,0,1,1
    }
};

// Player score
// Never goes above 9
uint8_t left_score = 0;
uint8_t right_score = 0;

// Position of the paddles
Point paddle_left = {10, SCRN_HEIGHT / 2};
Point paddle_right = {310, SCRN_HEIGHT / 2};

// Velocity of the ball
Vector ball_velocity = {-2, 0};

// Position of the ball
// Im not using floats so ball position is scaled by 4
// To get the real position of the ball divide by 4
Vector ball_position = {(SCRN_WIDTH / 2) * 4, (SCRN_HEIGHT / 2) * 4};

// First hit is set when the ball is first hit by a paddle
// When a round first starts the velocity of the ball is only 2
// After the first hit, this variable is set to true and velocity is multiplied by 4
// This variable is set to false after a round is won
bool first_hit = false;


//=====================
//     Functions
//=====================


// Writes the score to the top of the screen
void writescore() {
    left_score %= 10;
    right_score %= 10;
    const uint8_t* number_data = digit_sides[left_score];

    if (*number_data) {
        drawrect(120, 10, 120 + LINE_LENGTH, 10 + LINE_WIDTH, 0xF);
    }
    number_data++;
    if (*number_data) {
        drawrect(120, 10, 120 + LINE_WIDTH, 10 + LINE_LENGTH, 0xF);
    }
    number_data++;
    if (*number_data) {
        drawrect(120 + LINE_LENGTH, 10, 120 + LINE_LENGTH + LINE_WIDTH, 10 + LINE_LENGTH, 0xF);
    }
    number_data++;
    if (*number_data) {
        drawrect(120, 10 + LINE_LENGTH, 120 + LINE_LENGTH + LINE_WIDTH, 10 + LINE_LENGTH + LINE_WIDTH, 0xF);
    }
    number_data++;
    if (*number_data) {
        drawrect(120, 10 + LINE_LENGTH, 120 + LINE_WIDTH, 10 + LINE_LENGTH + LINE_LENGTH + LINE_WIDTH, 0xF);
    }
    number_data++;
    if (*number_data) {
        drawrect(120 + LINE_LENGTH, 10 + LINE_LENGTH, 120 + LINE_WIDTH + LINE_LENGTH, 10 + LINE_LENGTH + LINE_LENGTH + LINE_WIDTH, 0xF);
    }
    number_data++;
    if(*number_data) {
        drawrect(120, 10 + LINE_LENGTH * 2, 120 + LINE_LENGTH, 10 + LINE_LENGTH * 2 + LINE_WIDTH, 0xF);
    }

    number_data = digit_sides[right_score];
    if (*number_data) {
        drawrect(200, 10, 200 + LINE_LENGTH, 10 + LINE_WIDTH, 0xF);
    }
    number_data++;
    if (*number_data) {
        drawrect(200, 10, 200 + LINE_WIDTH, 10 + LINE_LENGTH, 0xF);
    }
    number_data++;
    if (*number_data) {
        drawrect(200 + LINE_LENGTH, 10, 200 + LINE_LENGTH + LINE_WIDTH, 10 + LINE_LENGTH, 0xF);
    }
    number_data++;
    if (*number_data) {
        drawrect(200, 10 + LINE_LENGTH, 200 + LINE_LENGTH + LINE_WIDTH, 10 + LINE_LENGTH + LINE_WIDTH, 0xF);
    }
    number_data++;
    if (*number_data) {
        drawrect(200, 10 + LINE_LENGTH, 200 + LINE_WIDTH, 10 + LINE_LENGTH + LINE_LENGTH + LINE_WIDTH, 0xF);
    }
    number_data++;
    if (*number_data) {
        drawrect(200 + LINE_LENGTH, 10 + LINE_LENGTH, 200 + LINE_WIDTH + LINE_LENGTH, 10 + LINE_LENGTH + LINE_LENGTH + LINE_WIDTH, 0xF);
    }
    number_data++;
    if(*number_data) {
        drawrect(200, 10 + LINE_LENGTH * 2, 200 + LINE_LENGTH, 10 + LINE_LENGTH * 2 + LINE_WIDTH, 0xF);
    }
}


// Draws the center line
void drawcenter(void) {
    for (int i = 0; i < SCRN_HEIGHT; i++) {
        uint16_t middle = SCRN_WIDTH/2;

        for (int i = 0; i < SCRN_HEIGHT; i++) {
            uint8_t color;
            if (i % 10 < 7) {
                color = 0xF;
            }
            else {
                color = 0;
            }
            putpixel(middle, i, color);
            putpixel(middle+1, i, color);
        }
    }
}

// Draws the paddles
void drawpaddles(void) {
    drawrect(paddle_left.x, paddle_left.y, paddle_left.x + PADDLE_WIDTH, paddle_left.y + PADDLE_HEIGHT, 0xF);
    drawrect(paddle_right.x, paddle_right.y, paddle_right.x + PADDLE_WIDTH, paddle_right.y + PADDLE_HEIGHT, 0xF);
}

// Reads the input keys and adjusts the paddles position accordingly
void read_input(void) {
    if (key_w == KEY_DOWN) {
        if (paddle_left.y - 1 >= 0) {
            paddle_left.y -= 1;
        }
    }
    if (key_s == KEY_DOWN) {
        if (paddle_left.y + 1 + PADDLE_HEIGHT <= SCRN_HEIGHT) {
            paddle_left.y += 1;
        }
    }
    if (key_up_arrow == KEY_DOWN) {
        if (paddle_right.y - 1 >= 0) {
            paddle_right.y -= 1;
        }
    }
    if (key_down_arrow == KEY_DOWN) {
        if (paddle_right.y + 1 + PADDLE_HEIGHT <= SCRN_HEIGHT) {
            paddle_right.y += 1;
        }
    }
}

// Clears the screen
void clear_screen(void) {
    clearbuf();
}

// Checks if the ball is hitting one of the paddles
// Returns:
// 0 = not hitting
// 1 = hitting left
// 2 = hitting right
uint8_t is_ball_hitting_paddle(void) {
    uint16_t real_x = ball_position.x / 4;
    uint16_t real_y = ball_position.y / 4;
    if (real_x <= paddle_left.x + PADDLE_WIDTH && real_y >= paddle_left.y && real_y < paddle_left.y + PADDLE_HEIGHT) {
        return 1;
    }
    if (real_x >= paddle_right.x && real_y >= paddle_right.y && real_y < paddle_right.y + PADDLE_HEIGHT) {
        return 2;
    }
    return 0;
}


// Calculates the position of the ball
void calculate_ball(void) {
    uint8_t hit = is_ball_hitting_paddle();
    if (hit) {
        // If the ball is hit for the first time, multiply the ball velocity by 4
        if (!first_hit) {
            ball_velocity.x *= 4;
            first_hit = true;
        }

        // Reflect the ball by reverseing the x velocity
        ball_velocity.x = -ball_velocity.x;

        // Get the position on the paddle that the ball was hit
        uint16_t hit_y = hit == 1 ? ball_position.y/4 - paddle_left.y : ball_position.y/4 - paddle_right.y;

        // If the ball was hit on the edge of the paddle, set the y velocity to 3
        // If it was hit in the middle, set it to 1
        // This could be done by calculating the angle of the ball based on how close to the edge of the paddle the ball made contact
        // But im lazy i guess, this works good enough
        uint16_t quarter = PADDLE_HEIGHT / 4;
        if (hit_y < quarter) {
            ball_velocity.y = -3;
        }
        else if (hit_y < quarter * 2) {
            ball_velocity.y = -1;
        }
        else if (hit_y < quarter * 3) {
            ball_velocity.y = 1;
        }
        else {
            ball_velocity.y = 3;
        }
    }

    // if the ball hit the edge of the screen, reflect the y velocity
    if (ball_position.y/4 == 0 || ball_position.y/4 == SCRN_HEIGHT) {
        ball_velocity.y = -ball_velocity.y;
    }

    // Finally set the ball position
    ball_position.x += ball_velocity.x;
    ball_position.y += ball_velocity.y;
}

// Draws the ball to the screen
void drawball() {
    uint16_t x = ball_position.x / 4;
    uint16_t y = ball_position.y / 4;
    putpixel(x, y, 0xF);
    putpixel(x + 1, y, 0xF);
    putpixel(x, y + 1, 0xF);
    putpixel(x + 1, y + 1, 0xF);
}

// Checks if the round is won
void check_win(void) {
    if (ball_position.x <= 0) {
        right_score++;
        ball_position.x = (SCRN_WIDTH / 2 ) * 4;
        ball_position.y = (SCRN_HEIGHT / 2 ) * 4;
        ball_velocity.x = -2;
        ball_velocity.y = 0;
        first_hit = false;
    }
    if (ball_position.x/4 >= SCRN_WIDTH) {
        left_score++;
        ball_position.x = (SCRN_WIDTH / 2 ) * 4;
        ball_position.y = (SCRN_HEIGHT / 2 ) * 4;
        ball_velocity.x = 2;
        ball_velocity.y = 0;
        first_hit = false;
    }
}

// Main game loop
void game(void) {
    while (true) {
        clear_screen();
        read_input();
        drawcenter();
        calculate_ball();
        check_win();
        writescore();
        drawball();
        drawpaddles();
        wait_vblank();
        copybuf();
    }

}