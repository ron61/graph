#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 隣接行列表現のグラフ構造体
struct Graph_t {
  int n;
  int m;
  int **matrix;
  char **info;
};

typedef struct Graph_t Graph;

// 頂点数 n のグラフ（辺なし）を作成
Graph *create_empty_graph(int n)
{
  Graph *g;
  int i, j;
  g = malloc(sizeof(Graph));
  g->n = n;
  g->m = 0;
  g->matrix = malloc(sizeof(int*)*n);
  for(i = 0; i < n; i++) {
    g->matrix[i] = malloc(sizeof(int)*n);
    for(j = 0; j < n; j++) {
      g->matrix[i][j] = 0;
    }
  }
  g->info = malloc(sizeof(char*)*n);
  for(i = 0; i < n; i++) g->info[i] = NULL;
  return g;
}
// 頂点 vid に情報を付加（タイトルのみ保持）
void add_vertex_info(Graph *g, int vid, char *url, char *title)
{
  int l = strlen(title);
  if(g->info[vid] != NULL) free(g->info[vid]);
  g->info[vid] = malloc(l+1);
  strcpy(g->info[vid], title);
}

// エッジの数をあらかじめ指定しておく（隣接行列表現ではほぼ無意味）
void reserve_edges(Graph *g, int m)
{
  g->m = m;
}

// from_vid から to_vid への辺を追加する
void add_edge(Graph *g, int from_vid, int to_vid)
{
  g->matrix[from_vid][to_vid] = 1;
}

// 深さ優先探索。flag は既に頂点に訪れたか否かのフラグ。頂点 v から探索を再帰的に進める
void _dfs(Graph *g, int *flag, int v)
{
  int j;
  flag[v] = 1;                 // この頂点に探索で訪れたことを記録
  for(j = 0; j < g->n; j++) {  // 全ての頂点 j に対し
    // j への辺があり且つまだ j に訪れていないなら j へ再帰する。
// ADD SOME CODE HERE
  }
}

// 深さ優先探索。頂点 v から探索を始める。再帰の本体は上の _dfs 
void dfs(Graph *g, int v)
{
  int i;
  int *flag = malloc(sizeof(int)*g->n);  // 頂点に訪れたかどうかのフラグ
  for(i = 0; i < g->n; i++) flag[i] = 0; // 全ての点はまだ訪れていない
  _dfs(g, flag, v);
  free(flag);
}

// 幅優先探索。頂点 v から探索を始める。
// キューを明示的につくる代わりに、配列2本を交互に使う（配列内の頂点は全て同じ距離の頂点）
void bfs(Graph *g, int v)
{
  int i, j;
  int *que = malloc(sizeof(int)*g->n);    // 前ステップでキューに追加された頂点の配列
  int qp = 0;
  int *nextque = malloc(sizeof(int)*g->n);// 現ステップでキューに追加される頂点の配列
  int nqp = 0;
  int *tmpp;
  int *flag = malloc(sizeof(int)*g->n);
  int distance = 0;
  int add_que;
  for(i = 0; i < g->n; i++) flag[i] = 0; // キューに追加したかどうかのフラグ


  que[qp++] = v; // 開始頂点をキューに入れて探索開始
  flag[v] = 1;
  while(qp > 0) {                  // キューが空になるまで
    printf("{rank = same; ");
    for(i = 0; i < qp; i++) {      // 現在のキューの中にある頂点に対して
      //printf("// distance of %d is %d\n", que[i], distance);
      printf("%d; ", que[i]);
      for(j = 0; j < g->n; j++) {  // 全ての頂点 j に対して
        // que[i] から j への辺があり且つまだ j はキューに入ったことがないなら、
        // j をキューへ追加
        add_que = 0; // CHANGE HERE
        if(add_que) {
          nextque[nqp++] = j; // 次のキューへ追加
          flag[j] = 1;        //　キュー追加フラグを立てる
        }
      }
    }
    printf("}\n");
    // キュー配列のスワップ（nextque の中身を que に移す）a
    qp = nqp;
    nqp = 0;
    tmpp = que;
    que = nextque;
    nextque = tmpp;
    // 次の距離は +1 
    distance++;
  }
  free(flag);
  free(nextque);
  free(que);
}

int main(int argc, char *argv[])
{
  int n, m, i, vid, from_vid, to_vid;
  char url[1024];
  char title[1024];
  Graph *g;
  gets(url);
  scanf("%d", &n);

  g = create_empty_graph(n);

  for(i = 0; i < n; i++) {
    scanf("%d", &vid);
    scanf("%s", url);
    gets(title);
    add_vertex_info(g, vid, url, title);
  }
  scanf("%d", &m);
  reserve_edges(g, m);
  for(i = 0; i < m; i++) {
    scanf("%d", &from_vid);
    scanf("%d", &to_vid);
    add_edge(g, from_vid, to_vid);
  }

  // ここまでで Graph の情報の読み込みが完了

  // dot ファイルを dfs で生成する例(dfsの中で辺の情報を出力するべし)：
  printf("digraph { \n");
  for(i = 0; i < n; i++) {
    printf("%d [label=%s]\n", i, g->info[i]);
  }
  dfs(g, 0);

  // 最短路長を bfs で計算し、それに基づいて頂点を並べる
  bfs(g, 0);

  printf("} \n");

  return 0;
}