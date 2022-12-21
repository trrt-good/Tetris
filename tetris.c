#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <conio.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

#define ANSI_COLOR_GRAY     "\x1b[30m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_LIGHT_PURPLE    "\x1b[36;1m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36;1m"
#define ANSI_COLOR_WHITE   "\x1b[37;1m"

typedef enum
{
    LEFT,
    RIGHT,
    ROTATE,
    DOWN,
    DROP,
} Input;

// Enum for the different types of Tetris pieces
typedef enum
{
    I,
    O,
    T,
    S,
    Z,
    J,
    L
} PieceType;

// Struct for a single Tetris piece
typedef struct
{
    PieceType type;
    int x;
    int y;
    int rotation;
} Piece;

// Struct for the Tetris game state
typedef struct
{
    int board[BOARD_WIDTH][BOARD_HEIGHT];
    Piece current_piece;
    int score;
    int over;
} TetrisGame;

void tetris_init(TetrisGame *game);
Piece tetris_generate_piece();
int tetris_can_place_piece(TetrisGame *game, Piece piece, int x, int y);
int tetris_can_move_piece(TetrisGame *game, Piece piece, int dx, int dy);
void tetris_remove_completed_lines(TetrisGame *game);
int tetris_get_piece_square(Piece piece, int i, int j);
int tetris_can_rotate_piece(TetrisGame *game, Piece piece);
void tetris_add_piece_to_board(TetrisGame *game, Piece piece);
void tetris_update(TetrisGame *game, Input input);
void tetris_print(TetrisGame *game);
void gameOver(TetrisGame *game);
Input processInput(char c);
char getInput();

int main()
{
    srand(time(NULL));
    TetrisGame *mainGame = malloc(sizeof(TetrisGame));
    tetris_init(mainGame);
    tetris_print(mainGame);
    
    while (1)
    {
        Input input = processInput(getInput());
        tetris_update(mainGame, DOWN);
        gameOver(mainGame);
        tetris_update(mainGame, input);
        gameOver(mainGame);
        tetris_print(mainGame);
        printf("\ncurrent score: %d\n", mainGame->score);
    }

    tetris_print(mainGame);
    return 1;
}

Input processInput(char c)
{
    switch (c)
    {
        case 'w':
            return ROTATE;
            break;
        case 'a':
            return LEFT;
            break;
        case 'd':
            return RIGHT;
            break;
        case ' ':
            return DROP;
            break;
        case 's':
            return DOWN;
            break;

        case 'x':
            exit(1);
            break;
    }
}

char getInput()
{
    printf("enter your move (WASD, space to drop, x to exit): ");
    return getch();
}

void gameOver(TetrisGame *game)
{
    if (game->over)
    {
        printf("game over! \nyour score was %d", game->score);
        exit(1);
    }
}

// Initializes a new Tetris game
void tetris_init(TetrisGame *game)
{
    memset(game->board, 0, sizeof(game->board));
    game->current_piece = tetris_generate_piece();
    game->score = 0;
    game->over = 0;
}

// Generates a new Tetris piece
Piece tetris_generate_piece()
{
    Piece piece;
    piece.type = (PieceType)(rand() % 7);
    piece.x = BOARD_WIDTH / 2;
    piece.y = 0;
    piece.rotation = 0;
    return piece;
}

void tetris_print(TetrisGame *game)
{
    int i, j;
    printf(ANSI_COLOR_WHITE "\n\n+-");
    for (j = 0; j < BOARD_WIDTH; j++)
    {
        printf("--");
    }
    printf("+\n");
    for (i = 0; i < BOARD_HEIGHT; i++)
    {
        printf(ANSI_COLOR_WHITE  "| ");
        for (j = 0; j < BOARD_WIDTH; j++)
        {
            if (tetris_get_piece_square(game->current_piece, j-game->current_piece.x, i-game->current_piece.y) == 1)
                printf(ANSI_COLOR_GRAY "$ ");
            else
            {
                switch (game->board[j][i])
                {
                    case 0:
                        printf("  "); 
                        break;
                    case 1:
                        printf(ANSI_COLOR_RED "1 "); 
                        break;
                    case 2:
                        printf(ANSI_COLOR_BLUE "2 "); 
                        break;
                    case 3:
                        printf(ANSI_COLOR_YELLOW "3 "); 
                        break;
                    case 4:
                        printf(ANSI_COLOR_GREEN "4 "); 
                        break;
                    case 5:
                        printf(ANSI_COLOR_MAGENTA "5 "); 
                        break;
                    case 6:
                        printf(ANSI_COLOR_CYAN "6 "); 
                        break;
                    case 7:
                        printf(ANSI_COLOR_LIGHT_PURPLE "7 "); 
                        break;
                }
                //printf(ANSI_COLOR_RED "%c ", (game->board[j][i] == 0)? ' ' : game->board[j][i]+48);
            }
                
        }
        printf(ANSI_COLOR_WHITE "|\n");
    }
    printf("+-");
    for (j = 0; j < BOARD_WIDTH; j++)
    {
        printf("--");
    }
    printf("+\n");
}

// Returns 1 if the given piece can be placed at the specified position, 0 otherwise
int tetris_can_place_piece(TetrisGame *game, Piece piece, int x, int y)
{
    // Check each square in the piece
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // Skip empty squares
            if (tetris_get_piece_square(piece, i, j) == 0)
            {
                continue;
            }

            // Check if the square is out of bounds
            if (x + i < 0 || x + i >= BOARD_WIDTH || y + j < 0 || y + j >= BOARD_HEIGHT)
            {
                return 0;
            }

            // Check if the square is occupied on the board
            if (game->board[x + i][y + j] != 0)
            {
                return 0;
            }
        }
    }
    return 1;
}

// Returns 1 if the given piece can be moved by the specified delta, 0 otherwise
int tetris_can_move_piece(TetrisGame *game, Piece piece, int dx, int dy)
{
    return tetris_can_place_piece(game, piece, piece.x + dx, piece.y + dy);
}

// Returns 1 if the given piece can be rotated, 0 otherwise
int tetris_can_rotate_piece(TetrisGame *game, Piece piece)
{
    Piece rotated = piece;
    rotated.rotation = (rotated.rotation + 1) % 4;
    return tetris_can_place_piece(game, rotated, rotated.x, rotated.y);
}

int tetris_get_piece_square(Piece piece, int i, int j)
{
    // Get the piece's type
    int type = piece.type;

    // Check each piece type to get the corresponding rotation
    switch (type)
    {
    // Piece I
    case I:
        switch (piece.rotation)
        {
        case 0:
            if (i == 1 && j == 0) return 1;
            if (i == 1 && j == 1) return 1;
            if (i == 1 && j == 2) return 1;
            if (i == 1 && j == 3) return 1;
            break;
        case 1:
            if (i == 0 && j == 2) return 1;
            if (i == 1 && j == 2) return 1;
            if (i == 2 && j == 2) return 1;
            if (i == 3 && j == 2) return 1;
            break;
        case 2:
            if (i == 2 && j == 0) return 1;
            if (i == 2 && j == 1) return 1;
            if (i == 2 && j == 2) return 1;
            if (i == 2 && j == 3) return 1;
            break;
        case 3:
            if (i == 0 && j == 1) return 1;
            if (i == 1 && j == 1) return 1;
            if (i == 2 && j == 1) return 1;
            if (i == 3 && j == 1) return 1;
            break;
        }
        break;

    // Piece O
    case O:
        if (i == 0 && j == 0) return 1;
        if (i == 0 && j == 1) return 1;
        if (i == 1 && j == 0) return 1;
        if (i == 1 && j == 1) return 1;
        break;

    // Piece T
    case T:
        switch (piece.rotation)
        {
        case 0:
            if (i == 0 && j == 1) return 1;
            if (i == 1 && j == 0) return 1;
            if (i == 1 && j == 1) return 1;
            if (i == 1 && j == 2) return 1;
            break;
        case 1:
            if (i == 1 && j == 0) return 1;
            if (i == 0 && j == 1) return 1;
            if (i == 1 && j == 1) return 1;
            if (i == 2 && j == 1) return 1;
            break;
        case 2:
            if (i == 2 && j == 1) return 1;
            if (i == 1 && j == 0) return 1;
            if (i == 1 && j == 1) return 1;
            if (i == 1 && j == 2) return 1;
            break;
        case 3:
            if (i == 0 && j == 1) return 1;
            if (i == 1 && j == 2) return 1;
            if (i == 1 && j == 1) return 1;
            if (i == 2 && j == 1) return 1;
            break;
        }
        break;

    // Piece S
    case S:
        switch (piece.rotation)
        {
        case 0:
            if (i == 0 && j == 0) return 1;
            if (i == 0 && j == 1) return 1;
            if (i == 1 && j == 1) return 1;
            if (i == 1 && j == 2) return 1;
            break;
        case 1:
            if (i == 0 && j == 2) return 1;
            if (i == 1 && j == 1) return 1;
            if (i == 1 && j == 2) return 1;
            if (i == 2 && j == 1) return 1;
            break;
        case 2:
            if (i == 1 && j == 1) return 1;
            if (i == 1 && j == 2) return 1;
            if (i == 0 && j == 0) return 1;
            if (i == 0 && j == 1) return 1;
            break;
        case 3:
            if (i == 1 && j == 2) return 1;
            if (i == 0 && j == 2) return 1;
            if (i == 1 && j == 1) return 1;
            if (i == 2 && j == 1) return 1;
            break;
        }
        break;
        
    case Z:
        switch (piece.rotation)
        {
        case 0:
            if (i == 1 && j == 0) return 1;
            if (i == 1 && j == 1) return 1;
            if (i == 0 && j == 1) return 1;
            if (i == 0 && j == 2) return 1;
            break;
        case 1:
            if (i == 0 && j == 1) return 1;
            if (i == 1 && j == 1) return 1;
            if (i == 1 && j == 2) return 1;
            if (i == 2 && j == 2) return 1;
            break;
        case 2:
            if (i == 0 && j == 1) return 1;
            if (i == 0 && j == 2) return 1;
            if (i == 1 && j == 0) return 1;
            if (i == 1 && j == 1) return 1;
            break;
        case 3:
            if (i == 0 && j == 0) return 1;
            if (i == 1 && j == 0) return 1;
            if (i == 1 && j == 1) return 1;
            if (i == 2 && j == 1) return 1;
            break;
        }
        break;

    case J:
        switch (piece.rotation)
        {
        case 0:
            if (i == 0 && j == 0) return 1;
            if (i == 0 && j == 1) return 1;
            if (i == 0 && j == 2) return 1;
            if (i == 1 && j == 2) return 1;
            break;
        case 1:
            if (i == 0 && j == 0) return 1;
            if (i == 1 && j == 0) return 1;
            if (i == 2 && j == 0) return 1;
            if (i == 0 && j == 1) return 1;
            break;
        case 2:
            if (i == 0 && j == 0) return 1;
            if (i == 1 && j == 0) return 1;
            if (i == 1 && j == 1) return 1;
            if (i == 1 && j == 2) return 1;
            break;
        case 3:
            if (i == 2 && j == 0) return 1;
            if (i == 0 && j == 1) return 1;
            if (i == 1 && j == 1) return 1;
            if (i == 2 && j == 1) return 1;
            break;
        }
        break;

    // Piece L
    case L:
        switch (piece.rotation)
        {
        case 0:
            if (i == 1 && j == 0) return 1;
            if (i == 1 && j == 1) return 1;
            if (i == 1 && j == 2) return 1;
            if (i == 0 && j == 2) return 1;
            break;
        case 1:
            if (i == 0 && j == 1) return 1;
            if (i == 1 && j == 1) return 1;
            if (i == 2 && j == 1) return 1;
            if (i == 0 && j == 0) return 1;
            break;
        case 2:
            if (i == 0 && j == 0) return 1;
            if (i == 0 && j == 1) return 1;
            if (i == 0 && j == 2) return 1;
            if (i == 1 && j == 0) return 1;
            break;
        case 3:
            if (i == 0 && j == 0) return 1;
            if (i == 1 && j == 0) return 1;
            if (i == 2 && j == 0) return 1;
            if (i == 2 && j == 1) return 1;
            break;
        }
        break;
    }

    // Return 0 if no squares are filled
    return 0;
}

// Updates the Tetris game state based on the player's input
void tetris_update(TetrisGame *game, Input input)
{
    switch (input)
    {
    case LEFT:
        // Move the piece to the left if possible
        if (tetris_can_move_piece(game, game->current_piece, -1, 0))
        {
            game->current_piece.x--;
        }
        break;
    case RIGHT:
        // Move the piece to the right if possible
        if (tetris_can_move_piece(game, game->current_piece, 1, 0))
        {
            game->current_piece.x++;
        }
        break;
    case DOWN:
        // Move the piece to the right if possible
        if (tetris_can_move_piece(game, game->current_piece, 0, 1))
        {
            game->current_piece.y++;
        }
        break;
    case ROTATE:
        // Rotate the piece if possible
        if (tetris_can_rotate_piece(game, game->current_piece))
        {
            game->current_piece.rotation = (game->current_piece.rotation + 1) % 4;
        }
        break;
    case DROP:
        // Drop the piece to the bottom of the board
        while (tetris_can_move_piece(game, game->current_piece, 0, 1))
        {
            game->current_piece.y++;
        }
        break;
    }

    //printf("%d, %d\n", game->current_piece.x, game->current_piece.y);
    // If the piece can no longer move down, add it to the board and generate a new piece
    if (!tetris_can_move_piece(game, game->current_piece, 0, 1))
    {
        tetris_add_piece_to_board(game, game->current_piece);
        tetris_remove_completed_lines(game);
        game->current_piece = tetris_generate_piece();

        // If the new piece cannot be placed on the board, the game is over
        for (int i = 0; i < BOARD_WIDTH; i ++)
        {
            if (game->board[i][0] == 1)
            {
                game->over = 1;
            }
        }
    }
}

void tetris_add_piece_to_board(TetrisGame *game, Piece piece)
{
    // Check each square in the piece
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // Check if i and j are in the piece
            if (tetris_get_piece_square(piece, i, j) == 1)
            {
                // Set the board square to the piece's type
                game->board[piece.x + i][piece.y + j] = piece.type+1;
            }
        }
    }
}

void tetris_remove_completed_lines(TetrisGame *game)
{
    // Keep track of the number of lines removed
    int lines_removed = 0;

    // Check each row of the board
    for (int y = 0; y < BOARD_HEIGHT; y++)
    {
        // Check if the row is complete (i.e. every square is filled)
        int row_complete = 1;
        for (int x = 0; x < BOARD_WIDTH; x++)
        {
            if (game->board[x][y] == 0)
            {
                row_complete = 0;
                break;
            }
        }

        // If the row is complete, remove it and shift the rows above it down
        if (row_complete)
        {
            // Increment the number of lines removed
            lines_removed++;

            // Shift the rows above the completed row down
            for (int y2 = y; y2 > 0; y2--)
            {
                for (int x = 0; x < BOARD_WIDTH; x++)
                {
                    game->board[x][y2] = game->board[x][y2 - 1];
                }
            }

            // Clear the top row
            for (int x = 0; x < BOARD_WIDTH; x++)
            {
                game->board[x][0] = 0;
            }

            // Check the same row again (since it has now been shifted down)
            y--;
        }
    }

    // Update the score based on the number of lines removed
    if (lines_removed == 1)
    {
        game->score += 40;
    }
    else if (lines_removed == 2)
    {
        game->score += 100;
    }
    else if (lines_removed == 3)
    {
        game->score += 300;
    }
    else if (lines_removed == 4)
    {
        game->score += 1200;
    }

}