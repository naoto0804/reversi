//240175I　井上直人
//
#include <assert.h>
#include <stdio.h>
#include "move1.h"
#define DEPTH 5//再帰関数で何手先まで読むか

static int eval[8][8]={//重み付け用のグローバル関数
    {7,-3,2,2,2,2,-3,7},
    {-3,-3,0,0,0,0,-3,-3},
    {2,0,1,1,1,1,0,2},
    {2,0,1,0,0,1,0,2},
    {2,0,1,0,0,1,0,2},
    {2,0,1,1,1,1,0,2},
    {-3,-3,0,0,0,0,-3,-3},
    {7,-3,2,2,2,2,-3,7}};
//assertを随所に挟むことで、バグの検出が楽になっている


void copy(int a1[][8],int a2[][8]){//8*8配列の保存用
  int i,j;//ループ用
  for(i=0;i<8;i++){
    for(j=0;j<8;j++){
      a1[i][j]=a2[i][j];//代入
    }
  }
}
int calc(int a[8][8]){//評価関数を使った計算用。評価値を返す。
  int sum=0;
  int i,j;
  for(i=0;i<8;i++){
    for(j=0;j<8;j++){
      sum+=(a[i][j])*(eval[i][j]);
    }
  }
  return sum;
}
int search(int depth, int turn,int now[][8],Pair nextmove,int origin){//originは大元の探索が、どちら側のものであるか(１なら先攻、２なら後攻)
  int i,j;
  int buf[8][8];//配列を一度受け取る..　いじりたいから
  copy(buf,now);//保存。中身の変更が利くように。
  place_disk(turn,nextmove,buf);//一手を実行.

  //ここまでで、一手の処理が完了
  if(depth>=DEPTH){//深さが最下層のとき
    return origin*(calc(buf));//先攻後攻での違いを後に響かせない為。黒/白どちらの探索でも、自分側ならmax探索、相手側ならmin探索をさせるようにするので、値をひっくり返す。
  }else{//まだ潜るとき
    Pair bufxy[60];//legalmove生成用
    turn*=-1;//一応逆側へ変更
    int nmoves=generate_all_legal_moves(turn,bufxy,buf);//bufのturn側のlegalmoveを全て受け取り、bufxyへ
    if(nmoves==-1){//空きがないので、この時点で評価関数を返し終了
      return origin*(calc(buf));
    }
    if(nmoves==0){
      turn*=-1;//turnを交代して探索続行
      nmoves=generate_all_legal_moves(turn,bufxy,buf);
      if(nmoves==-1){
	return origin*(calc(buf));//同様に、これ以上潜れないので終了
      }
      if(nmoves==0){//ダブルパスによる探索打ち切り。この場面の石を計算し、石の数的に勝利であれば、大きな値を返し、逆ならとても小さな値を返す
	int black=0;
	int white=0;
	for(i=0;i<8;i++){
	  for(j=0;j<8;j++){
	    if(buf[i][j]==1){
	      black++;
	    }else if(buf[i][j]==-1){
	      white++;
	    }
	  }
	}
	if(black>white){//大元が黒番なら黒がかつよう、白なら白がかつよう
	  return 500;
	}else{
	  return -500;
	}
      }
    }
    int max=-999;
    int min=999;
    int temp;//値の借り受け用
    if(turn==origin){//goodmove考える側。max探索
    for(i=0;i<nmoves;i++){
      temp=search(depth+1,turn,buf,bufxy[i],origin);
      if(temp>max){
	max=temp;
      }
    }
    return max;
    }else{//goodomoveに対するがわ。min探索
      for(i=0;i<nmoves;i++){
	temp=search(depth+1,turn,buf,bufxy[i],origin);
	if(temp<min){
	  min=temp;
	}
      }
      return min;
    }
  }
}

Pair goodmove(int turn,int board[][8]){//main関数へ次の一手を返す(Pair型)
  int i;
  int buf[8][8];//配列を一度受け取る..　いじりたいから
  copy(buf,board);//保存。中身の変更が利くように。
  Pair bufxy[60];//legalmove生成用
  int nmoves=generate_all_legal_moves(turn,bufxy,buf);//bufのturn側のlegalmoveを全て受け取り、bufxyへ。nmoves==-1は上の階層で既に処理されている。
  if(nmoves==0){
    turn*=-1;
    nmoves=generate_all_legal_moves(turn,bufxy,buf);
    assert(nmoves!=0);//ダブルパスは、上の階層で既に処理されているはずなので、ここでは無視します。ここのassertが出ることはないはず。
  }
  int max=-999;
  int temp;//値の借り受け用
  Pair goodxy;
  for(i=0;i<nmoves;i++){
    temp=search(1,turn,buf,bufxy[i],turn);
    if(temp>max){
      max=temp;
      goodxy=bufxy[i];
    }
  }
  return goodxy;  
}

const static Pair directions[8] = {//８方向を全部見る（頻出）なので、あらかじめ用意
  { -1, -1 }, {  0, -1 }, { 1, -1 }, 
  { -1,  0 },             { 1,  0 }, 
  { -1,  1 }, {  0,  1 }, { 1,  1 } 
};


int can_flip(const int side, Pair square, const Pair direction,int board[][8])//引っくり返せるかのチェック。directionは調べる方向。squareは調べる座標。sideはどっちか。
{
  assert(board[square.x][square.y] == 0);//念のため。空いているかチェック
  int n = 0;
  do {
    square.x += direction.x; 
    square.y += direction.y; 
    n++;
    if (square.x < 0 || square.x >= 8 || square.y < 0 || square.y >= 8) 
      return 0;
  } while (board[square.x][square.y] == -side);//相手のディスクがおいてある限り探索が続く。-sideとすることで相手の値になる
  if (board[square.x][square.y] == 0) return 0;//何もなければ終了
  if (n <= 1) return 0;//間に敵を挟む前に探索終了したら、何もおこらない
  return 1;
}

int is_legal_move(const int side, const Pair square,int board[8][8])
{
  if (square.x < 0 || square.x >= 8) return 0;//場所がおかしいときに暴走しないように
  if (square.y < 0 || square.y >= 8) return 0;//

  int i;
  if (board[square.x][square.y] != 0) return 0;//おなじく。既にあるときに暴走しないように
  for (i = 0; i < 8; i++) {
    if (can_flip(side, square, directions[i],board)) return 1;//direction[i]の方向にひっくり返せるかチェック。返せるなら1(これは少なくとも一方向に、といういみなので一回出た時点でリターンでよい)
  }
  return 0;
}

int place_disk(const int side, const Pair square,int board[][8])//ひっくり返す為の関数。squareはおく場所の座標を表す
{
  assert(is_legal_move(side, square,board));
  int n = 0, i;
  for (i = 0; i < 8; i++) {//八つの方向全てについて、ひっくり返せるか試行
    const Pair dir = directions[i];
    if (!can_flip(side, square, dir,board)) continue;//ひっくり返せるかチェック
//引っくり返せるときにこの先に進む
    int x = square.x + dir.x;
    int y = square.y + dir.y;
    while (board[x][y] == -side) {//次の自分の駒にぶちあたるまで
      board[x][y] = side;//ひっくり返す
      n++;
      x += dir.x;
      y += dir.y;
    }
  }
  board[square.x][square.y] = side;
  assert(n > 0);
  return n;//引っくり返した枚数の合計が返される
}

int generate_all_legal_moves(const int side, Pair legal_moves[],int board[8][8])//合法手を全て生成
{
  int x, y, nmoves = 0, nempty = 0;
//全マスチェックしていく。
  for (x = 0; x < 8; x++) {
    for (y = 0; y < 8; y++) {
      if (board[x][y] != 0) continue;//boardにもうおかれているときスキップ
      nempty++;
      Pair square;
      square.x = x; square.y = y;
      if (!is_legal_move(side, square,board)) continue;//どっちの番か、及び座標を渡して、ひっくり返せるか判定。できないときスキップ
      assert(nmoves < 60);//nmoveが60こえたら意味が分からないから
      legal_moves[nmoves++] = square;//Pair型の構造体にその手を記憶
    }
  }
  if (nempty == 0) return -1;//合法的にうてる空いたところがないとき。いわゆるpass処理が必要
  return nmoves;//legal_moves[]とあわせることで、戻し先で全ての打ち方にアクセス可能
}

