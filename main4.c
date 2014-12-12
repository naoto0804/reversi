//240175I
//井上直人
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "move1.h"

//以下二つは、強制的にpassの状況を作り出す為のもの。
//確かめのときにこの二つと置き換えた

void init_board(int (*board)[8])//boardの初期化
{
  int x, y;
  for (y = 0; y < 8; y++){ 
    for (x = 0; x < 8; x++){
      board[x][y] = 0;
    }
  }
  board[4][3] = 1;
  board[3][4] = 1;  // black disks
  board[3][3] = -1;
  board[4][4] = -1;  // white disks
}

void print_board(int board[8][8])//boardのプリント
{
  int x, y;
  puts("\n  a b c d e f g h");
  for (y = 0; y < 8; y++) {
    printf("%d ", y + 1);
    for (x = 0; x < 8; x++) {
      const int d = board[x][y];
      char c = '-';
      if (d ==  1) c = 'B';
      if (d == -1) c = 'W';
      printf("%c ", c);
    }
    putchar('\n');
  }
  putchar('\n');
}

void judge(int human_side,int board[8][8]){//課題１用
  int black=0;
  int white=0;
  int i,j;
  for(i=0;i<8;i++){
    for(j=0;j<8;j++){
      if(board[i][j]==1){
	black++;
      }else if(board[i][j]==-1){
	white++;
      }
    }
  }
  printf("black:%d white:%d ",black,white);
  int n;//黒の勝敗に応じて数をつける。
  if(black>white){
    n=1;
  }else if(black<white){
    n=-1;
  }else{
    n=0;
  }
  if(human_side==0){//com同士の対戦の時を先に処理
    if(n==1){
      printf("black win!\n");
    }else if(n==-1){
      printf("white win!\n");
    }else{
      printf("draw\n");
    }
  }else if(n*human_side==1){
    printf("player win!\n");
  }else if(n*human_side==-1){
    printf("player lose!\n");
  }else{
    printf("draw\n");
  }
}

int main(int argc, char **argv)
{
  int board[8][8];//グローバル変数ではなく、いちいち渡すことに
  const int human_side = (argc >= 2) ? atoi(argv[1]) : 0;//MANの先攻後攻の決定
  init_board(board);

  int turn;
  int pass=0;
  for (turn = 1;; turn *= -1) {//先手の時はturn=1,後手の時はturn=-1
    print_board(board);
    Pair legal_moves[60];//合法手を全て収納するための配列
    const int nmoves = generate_all_legal_moves(turn, legal_moves,board);//空きマス0能時にnamoves=-1を返す ←一回目のここで既に何かミスってる
    if (nmoves == -1) break;     // no empty square→ゲーム終了
    if (nmoves ==  0) {
      pass++;
      printf("turn = %d, move = Pass\n", turn);//パスの処理
      if(pass>=2){
	break;
      }
      continue;  
    }else{//passの初期化
      pass=0;
    }

    Pair move;
    if (turn == human_side) {//人間側の操作のとき。com同士ならこれはおこらない
      char buf[1000];
      while (1) {
	printf("Where? ");//どこにうつのか？
	scanf("%s", buf);
	move.x = buf[0] - 'a';
	move.y = buf[1] - '1';
	if (is_legal_move(turn, move,board)) break;//legalかどうかの判定メソッド
	printf("%s is not a legal move!\n\n", buf);//legalじゃなければ再び入力させる
      } 
    } else {//COM側の操作
      move = goodmove(turn,board);//turnは少なくとも必要
      assert(is_legal_move(turn,move,board));//念のため。最適解がちゃんとlegalmoveか確認
      printf("turn = %d, move = %c%c\n", turn, 'a' + move.x, '1' + move.y);
    }
    place_disk(turn, move,board);//moveをそもそも受け取れていなかった
  }//breakするまでfor文は繰り返す
  judge(human_side,board);//先攻か後攻かを返す
  return 0;
}
