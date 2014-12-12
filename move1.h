#ifndef _MOVE_H_
#define _MOVE_H_

typedef struct pair//構造体の座標を表す。move.cで内容を拡充するはず。
{
  int x;
  int y;
} Pair;
//その他も同様に、どんどん書き足していく。
int is_legal_move(const int side, const Pair square,int board[8][8]);
int place_disk(const int side, const Pair square,int board[][8]);
int generate_all_legal_moves(const int side, Pair legal_moves[],int board[8][8]);
Pair goodmove(int turn,int board[][8]);
int can_flip(const int side, Pair square, const Pair direction,int board[][8]);

#endif
