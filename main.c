#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>

#define HEIGHT 20
#define WIDTH 60
#define FIELDS 24
#define COLORS 2
#define ALLFIELDS FIELDS + 1
#define OPPOSITE turn ? 0 : 1
#define DIRECTION(x) turn ? x : -x
#define SMALLLETTER c >= 97 && c <= 122
#define ISINBAR (board[24][0] > 0 && !turn) || (board[24][1] > 0 && turn)
#define SAVELEN 165
#define BUFF ftell(history) / SAVELEN
#define WHITEPIECE "%c%c", 178, 178 //"WW"
#define BLACKPIECE "%c%c", 176, 176 //"BB"

typedef struct
{
    int a;
    int b;
} Tuple;

void drawBar(int board[][COLORS], int row)
{
    int white = board[24][0];
    int black = board[24][1];
    if (row < 12)
    {
        if (black < 10)
            printf("%d ", black);
        else
            printf("%d", black);
    }
    else
    {
        if (white < 10)
            printf("%d ", white);
        else
            printf("%d", white);
    }
}

int drawPieces(int board[][COLORS], int field, int row, int is_up)
{
    int cur_row = is_up ? row - 2 : 19 - row;
    int white = board[field][0];
    int black = board[field][1];
    if (cur_row == 7 && (white > 7 || black > 7))
        printf("+%d", white ? white - 7 : black - 7);
    else if (cur_row == 7)
        printf("  ");
    else if (white > cur_row)
        printf(WHITEPIECE);
    else if (black > cur_row)
        printf(BLACKPIECE);
    else
        return 0;
    return 1;
}

void drawBoard(int board[][COLORS])
{
    FILE *file = fopen("board.txt", "r");
    char c;
    int row = 0;
    while ((c = fgetc(file)) != EOF)
    {
        if (SMALLLETTER) // small letters mark columns
        {
            int field = c - 97;
            if (field < 12)
            {
                if (drawPieces(board, field, row, 0) == 0)
                {
                    if (field % 2)
                        printf("::");
                    else
                        printf("/%c", 92);
                }
            }
            else if (field == 24)
                drawBar(board, row);
            else
            {
                if (drawPieces(board, field, row, 1) == 0)
                {
                    if (field % 2)
                        printf("%c/", 92);
                    else
                        printf("::");
                }
            }
        }
        else
            printf("%c", c);
        if (c == 10)
            row += 1;
    }
}

void drawGame(int board[][COLORS], int your_moves[], int turn, int moves_left)
{
    drawBoard(board);
    printf("%s: %s   ", "tura", turn ? "czarnych" : "bialych");
    printf("pozostalo ruchow: %d\n", moves_left);
    for (int i = 0; i < 4; i++)
    {
        if (your_moves[i] != 0)
            printf("kosc nr.%d -> %d\n", i + 1, your_moves[i]);
    }
}

void drawMenu(char label[], int ai_on)
{
    FILE *menu = fopen(label, "r");
    char c;
    while ((c = fgetc(menu)) != EOF)
    {
        if (c == 42)
        {
            if (ai_on)
                printf("ON");
            else
                printf("OFF");
        }
        else
            printf("%c", c);
    }
    fclose(menu);
}

void rollDice(int *die_one, int *die_two)
{
    *die_one = rand() % 6 + 1;
    *die_two = rand() % 6 + 1;
}

Tuple readInput(int board[][COLORS], int turn)
{
    int column, number;
    Tuple move;
    if (ISINBAR)
    {
        scanf("%d", &number);
        move = (Tuple){25, number};
    }
    else
    {
        scanf("%d %d", &column, &number);
        move = (Tuple){column, number};
    }
    return move;
}

int farthestPiece(int board[][COLORS], int turn, int die)
{
    int start, finish, dir;
    if (turn)
    {
        start = 18 + (6 - die);
        finish = 23;
        dir = 1;
    }
    else
    {
        start = 5 - (6 - die);
        finish = 0;
        dir = -1;
    }
    for (; start * dir <= finish; start += dir)
    {
        if (board[start][turn] > 0)
            return start;
    }
    return -1;
}

int canGoHome(int board[][COLORS], int turn)
{
    int start, finish;
    if (turn)
    {
        start = 0;
        finish = 17;
    }
    else
    {
        start = 6;
        finish = 23;
    }
    for (; start <= finish; start++)
    {
        if (board[start][turn] > 0)
            return 0;
    }
    if (board[24][turn] > 0)
        return 0;
    return 1;
}

int check(int board[][COLORS], int die, int turn, int i, int captures_only)
{
    int dir = DIRECTION(die);
    if (ISINBAR) // only moves from bar are available when on bar
    {
        if (i != 24)
            return 0;
        if (turn) // black has to loop back to zero
        {
            if (captures_only)
            {
                if (board[-1 + dir][OPPOSITE] == 1)
                    return 1;
            }
            else if (board[-1 + dir][OPPOSITE] < 2)
                return 1;
        }
    }
    if (board[i][turn] > 0)
    {
        if (canGoHome(board, turn))
        {
            if ((i + dir >= FIELDS && turn) || (i + dir < 0 && !turn))
            {
                if (i == farthestPiece(board, turn, die))
                    return 1;
            }
            else if (farthestPiece(board, turn, die) > -1)
                return 0; // forced home
        }
        if (i + dir < FIELDS && i + dir >= 0)
        {
            if (captures_only)
            {
                if (board[i + dir][OPPOSITE] == 1)
                    return 1;
            }
            else if (board[i + dir][OPPOSITE] < 2)
                return 1;
        }
    }
    return 0;
}

int movesAvailable(int moves[])
{
    for (int i = 0; i < ALLFIELDS; i++)
    {
        if (moves[i] == 1)
            return 1;
    }
    return 0;
}

void getValidMoves(int board[][COLORS], int moves[], int die, int turn)
{
    for (int i = 0; i < ALLFIELDS; i++) // forced capture
    {
        moves[i] = check(board, die, turn, i, 1);
    }
    if (!(movesAvailable(moves)))
    {
        for (int j = 0; j < ALLFIELDS; j++)
        {
            moves[j] = check(board, die, turn, j, 0);
        }
    }
}

int canMove(int board[][COLORS], int your_moves[], int turn)
{
    int moves[ALLFIELDS];
    for (int i = 0; i < 4; i++)
    {
        if (your_moves[i] != 0)
        {
            getValidMoves(board, moves, your_moves[i], turn);
            if (movesAvailable(moves))
                return 1;
        }
    }
    return 0;
}

void updateBoard(int board[][COLORS], int die, int start, int turn)
{
    int finish = (board[24][1] > 0 && turn) ? -1 + die : start + die;
    board[start][turn] -= 1;
    if (finish < FIELDS && finish >= 0)
    {
        board[finish][turn] += 1;
        if (board[finish][OPPOSITE] == 1) // bicie
        {
            board[finish][OPPOSITE] -= 1;
            board[24][OPPOSITE] += 1;
        }
    }
}

int makeMove(int board[][COLORS], int your_moves[], int turn, Tuple move)
{
    int moves[ALLFIELDS]; // tells if you can move this piece
    for (int i = 0; i < 4; i++)
    {
        if (your_moves[i] == move.b && move.b != 0)
        {
            getValidMoves(board, moves, move.b, turn);
            if (moves[move.a - 1] == 1)
            {
                updateBoard(board, DIRECTION(move.b), move.a - 1, turn);
                your_moves[i] = 0;
                return 1;
            }
            break;
        }
    }
    return 0;
}

int getMoveType(int your_moves[], int die_one, int die_two)
{
    if (die_one == die_two)
    {
        for (int i = 0; i < 4; i++)
            your_moves[i] = die_one;
        return 4;
    }
    else
    {
        your_moves[0] = die_one;
        your_moves[1] = die_two;
        your_moves[2] = 0;
        your_moves[3] = 0;
        return 2;
    }
}

void startingPos()
{
    for (int i = 0; i < 2; i++)
    {
        FILE *rewrite = i ? fopen("save.txt", "w") : fopen("history.txt", "w");
        fprintf(rewrite, "02 00 00 00 00 00 00 00 00 00 00 05 00 00 00 03 00 00 00 00 00 00 05 00 00 05 00 00 00 00 00 00 03 00 00 00 05 00 00 00 00 00 00 00 00 00 00 02 00 00 0 0 0 0 0 0 0\n");
        fclose(rewrite);
    }
}

void saveMachine(int board[][COLORS], int your_moves[], int turn, int move_count, int repeat, FILE *save)
{
    for (int i = 0; i < ALLFIELDS; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (board[i][j] > 9)
                fprintf(save, "%d ", board[i][j]);
            else
                fprintf(save, "0%d ", board[i][j]);
        }
    }
    for (int i = 0; i < 4; i++)
    {
        fprintf(save, "%d ", your_moves[i]);
    }
    fprintf(save, "%d %d %d\n", turn, move_count, repeat);
}

void saveGame(int board[][COLORS], int your_moves[], int turn, int move_count, int repeat)
{
    FILE *save = fopen("save.txt", "w");
    saveMachine(board, your_moves, turn, move_count, repeat, save);
    fclose(save);
    FILE *history = fopen("history.txt", "a");
    saveMachine(board, your_moves, turn, move_count, repeat, history);
    fclose(history);
}

void loadGame(int board[][COLORS], int your_moves[], int *turn, int *move_count, int *repeat, FILE *load)
{
    int temp, side = 0;
    for (int i = 0; i < ALLFIELDS; i++)
    {
        for (int d = 0; d < 2; d++)
        {
            fscanf(load, "%d", &temp);
            board[i][side] = temp;
            side = side ? 0 : 1;
        }
    }
    for (int j = 0; j < 4; j++)
    {
        fscanf(load, "%d", &(your_moves[j]));
    }
    fscanf(load, "%d %d %d", turn, move_count, repeat);
}

int whoWon(int board[][COLORS])
{
    int white = 0, black = 0;
    for (int i = 0; i < ALLFIELDS; i++)
    {
        if (board[i][0] == 0)
            white++;
        if (board[i][1] == 0)
            black++;
    }
    if (white == 25)
        return 1;
    if (black == 25)
        return 2;
    return 0;
}

void endGame(int win)
{
    if (win == 1)
        drawMenu("white.txt", 0);
    else if (win == 2)
        drawMenu("black.txt", 0);
    printf("                                (B)ack to menu");
    char c;
    while (1)
    {
        c = getch();
        if (c == 'b')
            break;
    }
}

int AIMove(int board[][COLORS], int your_moves[], int turn)
{
    int moves[ALLFIELDS];
    Tuple move;
    for (int i = 0; i < 4; i++)
    {
        if (your_moves[i])
        {
            getValidMoves(board, moves, your_moves[i], turn);
            for (int j = ALLFIELDS - 1; j >= 0; j--)
            {
                if (moves[j] == 1)
                {
                    move.a = j + 1;
                    move.b = your_moves[i];
                    return makeMove(board, your_moves, turn, move);
                }
            }
        }
    }
}

void newGame(int board[][COLORS], int your_moves[], int die_one, int die_two, int turn, Tuple move, int repeat, int i, int new, int ai_on)
{
    int win;
    while (i < 5)
    {
        if (new)
        {
            rollDice(&die_one, &die_two);
            repeat = getMoveType(your_moves, die_one, die_two); // tells how many moves you can do
            saveGame(board, your_moves, turn, 0, repeat);
            i = 0;
        }
        while (i < repeat)
        {
            drawGame(board, your_moves, turn, repeat - i);
            if (canMove(board, your_moves, turn))
            {
                if (turn && ai_on)
                {
                    AIMove(board, your_moves, turn);
                    Sleep(1000);
                }
                else
                {
                    do
                    {
                        move = readInput(board, turn);
                    } while (makeMove(board, your_moves, turn, move) == 0); // repeat as long as move is unavailable
                }
            }
            i++;
            if (i < repeat)
                saveGame(board, your_moves, turn, i, repeat);
            system("cls");
            win = whoWon(board);
            if (win)
                i = 5; // stop loops
        }
        turn = OPPOSITE;
        new = 1;
    }
    endGame(win);
}

void historyOptions(int *row, int *end, FILE *history)
{
    char c;
    switch (c = getch())
    {
    case 'r':
        (*row)++;
        break;
    case 'l':
        if ((*row) > 1)
        {
            fseek(history, -SAVELEN * 2, SEEK_CUR);
            (*row)--;
        }
        else
        {
            fseek(history, 0, SEEK_SET);
            (*row) = 0;
        }
        break;
    case 's':
        fseek(history, 0, SEEK_SET);
        (*row) = 0;
        break;
    case 'f':
        fseek(history, -SAVELEN, SEEK_END);
        (*row) = BUFF;
        break;
    case 'e':
        (*end) = 0;
        break;
    }
}

void matchHistory(int board[][COLORS], int your_moves[], int turn, int i, int repeat)
{
    FILE *history = fopen("history.txt", "r");
    loadGame(board, your_moves, &turn, &i, &repeat, history);
    int row = 0, end = 1;
    while (end)
    {
        drawGame(board, your_moves, turn, repeat - i);
        printf("(S)tart    (L)eft    (R)ight    (F)inish    (E)xit\n");
        historyOptions(&row, &end, history);
        system("cls");
        loadGame(board, your_moves, &turn, &i, &repeat, history);
        if (feof(history))
            end = 0;
    }
    fclose(history);
}

void gameInit(int new, int old, int ai_on)
{
    system("cls");
    int board[ALLFIELDS][COLORS] = {0}; // 24 miejsca + 1 na BAR
    int your_moves[4];
    int die_one = 0, die_two = 0;
    int turn = 0; // białe - 0 ; czarne - 1
    Tuple move;
    int repeat;
    int i;
    if (old)
        matchHistory(board, your_moves, turn, i, repeat);
    else
    {
        if (new)
            startingPos();
        FILE *load = fopen("save.txt", "r");
        loadGame(board, your_moves, &turn, &i, &repeat, load);
        fclose(load);
        if (new)
            turn = rand() % 2;
        newGame(board, your_moves, die_one, die_two, turn, move, repeat, i, new, ai_on);
    }
}

int main()
{
    srand(time(NULL));
    int ai_on = 0;
    while (1)
    {
        drawMenu("menu.txt", ai_on);
        char option = getch();
        switch (option)
        {
        case 'n':
            gameInit(1, 0, ai_on);
            break;
        case 'l':
            gameInit(0, 0, ai_on);
            break;
        case 'h':
            gameInit(0, 1, 0);
            break;
        case 't':
            ai_on = ai_on ? 0 : 1;
            break;
        case 'e':
            return 0;
        }
        system("cls");
    }
    return 0;
}