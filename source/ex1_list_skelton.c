// Example Usage
//
// gcc ex1_list_skelton.c -O3
// ./a.out < result.txt > samplegraph.dot
// dot -Tpng < samplegraph.dot > samplegraph.png

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 辺リスト用の構造
struct Edge_t {
  int from;
  int to;
  struct Edge_t *next;
};

typedef struct Edge_t Edge;

// 隣接リスト表現のグラフ構造体
struct Graph_t {
  int n;
  int m;
  int _m;             // 内部使用：登録された辺の数
  Edge *_edges;       // 内部使用：全辺
  Edge **edges;       // 各頂点の最初の辺へのポインタ
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
  g->edges = malloc(sizeof(Edge*)*n);
  for(i = 0; i < n; i++) g->edges[i] = NULL;
  g->_edges = NULL;
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

// エッジの数をあらかじめ指定しておく（エッジ情報のための領域確保をここで行う）
void reserve_edges(Graph *g, int m)
{
  g->m = m;
  g->_m = 0;
  g->_edges = malloc(sizeof(Edge)*m);
}

// from_vid から to_vid への辺を追加する
void add_edge(Graph *g, int from_vid, int to_vid)
{
  Edge *e = &g->_edges[g->_m++];   // エッジバッファの空きを取ってくる
  e->to = to_vid;                  // エッジに情報を書き込む
  e->from = from_vid;
  e->next = g->edges[from_vid];    // 始点の辺リストの最初の辺の手前に新しい辺を入れる
  g->edges[from_vid] = e;
}

// 深さ優先探索。flag は既に頂点に訪れたか否かのフラグ。頂点 v から探索を再帰的に進める
void _dfs(Graph *g, int *flag, int v)
{
  int j;
  Edge *e;
  flag[v] = 1;              // この頂点に探索で訪れたことを記録
  e = g->edges[v];    // 頂点 v の辺リストの最初の辺を取得
  while(e!=NULL) {          // 辺リストの終了まで
    j = e->to;              // 辺の行き先の頂点

    // ここで、j への辺があり且つまだ j に訪れていないなら j へ再帰する。
    if (flag[j] != 1){
      _dfs(g, flag, j);
    }
    
    printf("%d->%d;",e->from,e->to);

    e = e->next;            // 辺リスト中の次の辺へ
  }
}

// 深さ優先探索。頂点 v から探索を始める。再帰の本体は上の _dfs 
void dfs(Graph *g, int v)
{
  int i;
  int *flag = malloc(sizeof(int)*g->n);    // 頂点に訪れたかどうかのフラグ
  for(i = 0; i < g->n; i++) flag[i] = 0;   // 全ての点はまだ訪れていない
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
  Edge *e;  
  int *tmpp;
  int *flag = malloc(sizeof(int)*g->n);
  int distance = 0;
  int add_que = 0;
  for(i = 0; i < g->n; i++) flag[i] = 0;  // キューに追加したかどうかのフラグ

  que[qp++] = v;   // 開始頂点をキューに入れて探索開始
  flag[v] = 1;
  while(qp > 0) {  // キューが空になるまで
    printf("{rank = same; ");
    for(i = 0; i < qp; i++) {  // 現在のキューの中にある頂点に対して
      //printf("// distance of %d is %d\n", que[i], distance);
      printf("%d; ", que[i]);
      e = g->edges[que[i]];
      while(e!=NULL) {         // 頂点からの辺全てに対して
        j = e->to;             // 辺の行き先の頂点 j について

        // j がまだキューに入ったことがないなら、 j をキューへ追加
        if (flag[j] != 1)
        {
          add_que = 1;
        } else {
          add_que = 0;
        }
        
        if(add_que) {
          nextque[nqp++] = j; // 次のキューへ追加
          flag[j] = 1;        //　キュー追加フラグを立てる
        }
        e = e->next;
      }
    }
    printf("}\n");
    // キュー配列のスワップ（nextque の中身を que に移す）
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