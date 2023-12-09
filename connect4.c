#include<stdio.h>
#include<stdint.h>
#include<assert.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>
#define board_rows (4)
#define board_columns (5)
#define MAX_ORD (28629151)
#define connect (4)
#define col_combinations (31)

typedef char player_t;
typedef char board_t[board_rows][board_columns];

uint16_t computed_moves[30*(31*31*31*31*31-1)+1];

enum {
    EMPTY = 0,
    RED,
    BLUE,
};

void init_board(board_t board){
    for (int row = 0; row < board_rows; row++)
    {
        for (int col = 0; col < board_columns; col++)
        {
            board[row][col] = '.';
        }
    }
}

void print_board(board_t board){
    for (int row = 0; row < board_rows; row++)
    {
        for (int col = 0; col < board_columns; col++)
        {
            printf("%3c ",board[row][col]);
        }
        printf("\n");
    }    
}

typedef struct {
    int col;
    int score;
} move_t;

uint16_t encode_move(move_t move){
    uint16_t b = 0;
    b |= move.col;
    
    switch (move.score)
    {
    case -1:
        b |= 1 << 3;
        break;
    case 0:
        b |= 1 << 4;
        break;
    case 1:
        b |= 1 << 5;
        break;
    default:
        break;
    }
    
    return b;
}

move_t decode_move(uint16_t b){
    move_t move;
    move.col = ((b) & 0x07);
    
    if (b & 0x8) move.score = -1;
    else if (b & 0x10) move.score = 0;
    else move.score = 1;
    
    return move;
}

int has_won(board_t board, player_t player){
    
    for (int row = 0; row < board_rows; row++)
    {   
        int temp = 0;
        for (int col = 0; col < board_columns; col++)
        {
                if (board[row][col] == player) {temp += 1;}
                else {temp = 0;}
                if (temp == connect) {return 1;}
        }
    }
    
    for (int col = 0; col < board_columns; col++)
    {   
        int temp = 0;
        for (int row = 0; row < board_rows; row++)
        {
            if (board[row][col] == player) {temp += 1;}
            else {temp = 0;}
            if (temp == connect) {return 1;}
        }   
    }
    
    if(board[0][0]==board[1][1] && board[0][0]==board[2][2] && board[0][0]==board[3][3] && board[0][0]!='.'){
        return 1;
    }
    if(board[0][1]==board[1][2] && board[0][1]==board[2][3] && board[0][1]==board[3][4] && board[0][1]!='.'){
        return 1;
    }
    if(board[0][3]==board[1][2] && board[0][3]==board[2][1] && board[0][3]==board[3][0] && board[0][3]!='.'){
        return 1;
    }
    if(board[0][4]==board[1][3] && board[0][4]==board[2][2] && board[0][4]==board[3][1] && board[0][4]!='.'){
        return 1;
    }
    return 0;
}

int is_full(board_t board){
    for (int row = 0; row < board_rows; row++)
    {
            for (int col = 0; col < board_columns; col++)
            {
                if(board[row][col]=='.') return 0;
            }

    }
    return 1;
}

player_t other_player(player_t player){
    switch (player)
    {
        case 'x':
            return 'o';
            break;
        case 'o':
            return 'x';
            break;
    }
}

int my_ord(board_t board)
{ 
    //returns the encoding for a board position
    char *position[] = { // 31 possibilities
        "0000",
        "0001","0002",
        "0011","0012","0021","0022",
        "0111","0112","0121","0122","0211","0212","0221","0222",
        "1111","1112","1121","1122","1211","1212","1221","1222","2111","2112","2121","2122","2211","2212","2221","2222",
    };
    int ans=0;
    char str[5]="aaaa"; // Initialize the string
    int mul = 1;
    int flag = 1;
    
    for(int i=0;i<5;i++)
    { // cols
        flag = 1;
        for(int j=0;j<4;j++){
            // printf("%c",board[i][j]);
            if(board[j][i]=='.') str[j] = '0';
            else if(board[j][i]=='x') str[j] = '1';
            else str[j] = '2';
        }
        
        for(int j=0;j<31;j++){
            if(strcmp(str, position[j])==0){
                ans = ans+j*mul;
                mul=mul*31;
                flag=0;
                break;
            }
        }
        assert(flag==0);
    }

    return ans;
}

move_t best_move(board_t board, player_t player)
{
    move_t response;
    move_t candidate;
    int no_candidate = 1;

    int track[5] = {0,0,0,0,0};
    for (int col = 0; col < board_columns; col++)
    {
        for (int row = 0; row < board_rows; row++)
        {
            if (board[row][col] != '.') track[col]++;
        }
    }

    int o = my_ord(board);
    if (computed_moves[o]){
        return decode_move(computed_moves[o]);
    }

    for (int col = 0; col < board_columns; col++)
    {   

        if (track[col] != board_rows)
        {
            board[board_rows-track[col]-1][col] = player;

            if (has_won(board,player))
            {
                board[board_rows-track[col]-1][col] = '.';
                candidate = (move_t) {
                        .col = col,
                        .score = 1
                        };
                computed_moves[o] = encode_move(candidate);
                return candidate;
            }
            board[board_rows-track[col]-1][col] = '.' ;
        }
    }
    for (int col = 0; col < board_columns; col++)
    {
        if (track[col] != board_rows)
        {
            board[board_rows-track[col]-1][col] = player;
            if (is_full(board))
            {
                board[board_rows-track[col]-1][col] = '.';
                candidate = (move_t) {
                        .col = col,
                        .score = 0
                        };
                computed_moves[o] = encode_move(candidate);
                return candidate;
            }
            response = best_move(board, other_player(player));
            // printf("nahi\n");
            board[board_rows-track[col]-1][col] = '.';
            if (response.score == -1) 
            {
                    computed_moves[o] = encode_move(candidate = (move_t) {
                        .col = col,
                        .score = 1
                        });
                    return candidate;
            } 
            else if (response.score == 0) 
            {
                    candidate = (move_t) {
                        .col = col,
                        .score = 0
                    };
                    no_candidate = 0;
            } 
            else 
            { 
                    if (no_candidate) {
                        candidate = (move_t) {
                            .col = col,
                            .score = -1
                        };
                        no_candidate = 0;
                    }
                }
        }

    }
    computed_moves[o] = encode_move(candidate);
    return candidate;
}

void print_key()
{
    int ii = 0;
    for (int col = 0; col < board_columns; ++col) 
    {    
            printf("%3d ", ii++);    
    }
    printf("\n");
}

int main(){

    for(int i=0;i<(30*(31*31*31*31*31-1)+1);i++){
        computed_moves[i] = 0;
    }

    int col;
    board_t board;
    move_t response;
    player_t player;

    printf("Welcome to the Connect 4 game!\n");
    printf("Player 'x' plays first!\n");
    printf("To choose your peg, enter 'x' or 'o':\n");
    char playerchar;
    scanf("%c",&playerchar);
    assert(playerchar=='x' || playerchar=='o');
    
    if(playerchar=='x')
    {
        player = 'x';
    }
    else{
        player = 'o';
    }

    player_t current = 'x';
    init_board(board);

    while(1) 
    {
        int track[5] = {0,0,0,0,0};
        for (int col = 0; col < board_columns; col++)
        {
            for (int row = 0; row < board_rows; row++)
            {
                if (board[row][col] != '.'){track[col]++;}
            }
        }

        print_board(board);
        if (current == player) 
        {
            print_key();
            printf("Enter your move: ");
            scanf("%d", &col);
            assert(0<=col & col<board_columns);
            board[board_rows-track[col]-1][col] = current;
        } else {
            response = best_move(board, current);
            printf("%d\n",response.score);
            printf("Computer's move: %d\n",response.col);
            board[board_rows-track[response.col]-1][response.col] = current;
        }
        if (has_won(board, current)) {
            print_board(board);
            printf("Player %c has won!\n", current);
            break;
        } else if (is_full(board)) {
            print_board(board);
            printf("Draw.\n");
            break;
        }
        current = other_player(current);
    }

    return 0;

}
