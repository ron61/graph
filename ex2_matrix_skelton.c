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

// 初回の深さ優先探索。flag は既に頂点に訪れたか否かのフラグ。頂点 v から探索を再帰的に進める
// stack は探索が終了した頂点を入れるスタック。sp はスタックポインタ。
void _dfs1(Graph *g, int *flag, int v, int *stack, int *sp)
{
  int j;
  flag[v] = 1;
  // 全ての頂点 j にたいして、 v から j の辺があり且つ j に訪れていないなら再帰呼び出し。
// ADD SOME CODE HERE
  stack[(*sp)++] = v;  // 探索終了を記録
}

// 深さ優先探索。flag は既に頂点に訪れたか否かのフラグ。頂点 v から探索を再帰的に進める
// component は各頂点の属する強連結成分の番号を格納。c は現在探索中の強連結成分の番号
void _dfs2(Graph *g, int *flag, int v, int *component, int c)
{
  int j;
  flag[v] = 1;                 // この頂点に来たことを記録
  component[v] = c;            // この頂点の強連結成分の番号は c
  // 全ての頂点 j にたいして、 v から j の辺があり且つ j に訪れていないなら再帰呼び出し。
  // ただし、エッジの向きは逆向きであることに注意
// ADD SOME CODE HERE
}

// 強連結成分分解。component に各頂点の属する強連結成分の番号が入る
int scc(Graph *g, int *component)
{
  int i;
  int *flag = malloc(sizeof(int)*g->n);  // 探索済みかどうかのフラグ
  int *stack = malloc(sizeof(int)*g->n); // 初回深さ優先探索の終了順に頂点を格納する場所
  int sp = 0, c = 0;

  for(i = 0; i < g->n; i++) flag[i] = 0; // フラグクリア
  for(i = 0; i < g->n; i++) {            // 一般には、複数の頂点から深さ優先探索をする
    if(flag[i] == 0) _dfs1(g, flag, i, stack, &sp); // 初回深さ優先探索
  }
  for(i = 0; i < g->n; i++) flag[i] = 0; // フラグクリア
  for(sp = g->n-1; sp >=0; sp--) {       // 初回探索の終了の遅い順に
    if(flag[stack[sp]] == 0) {           // まだ訪れていない頂点から逆向きの深さ優先探索
      _dfs2(g, flag, stack[sp], component, c);
      c++;                               // 探索が終了したら強連結成分がひとつ見つかる
    }
  }
  free(stack);
  free(flag);
  return c;                              // 強連結成分の個数
}

// PageRank
void pagerank(Graph *g, double *rank)
{
  int * ls = malloc(sizeof(int) * g->n);  // 出ていくリンク数
  double * new_rs = malloc(sizeof(double) * g->n); // 新しいランク用の配列
  double d = 0.85;                   // damping factor
  int i, v;
  double r, err, er;

  // リンク数のカウント
  for(v = 0; v < g->n; v++) {
    ls[v] = 0;
    for(i = 0; i < g->n; i++) {  
      if(g->matrix[v][i] > 0) ls[v]++;  // v から i への辺があれば加算
    }
  }
  for(i = 0; i < g->n; i++) {  // 初期 rank
    rank[i] = 1.0/g->n;
  }
  
  for(;;) {
    for(v = 0; v < g->n; v++) { // 全頂点に対して
      r = 0; // 新しいランクを計算する一時変数
      for(i = 0; i < g->n; i++) { // すべての頂点に対して 
        // i から v に入ってくる辺があるなら
        if(g->matrix[i][v] > 0) {
          // ランク加算
// ADD SOME CODE HERE
        }
      }
      new_rs[v] = (1-d)/g->n + d * r; // 新しいランク値
    }
    err = 0;            // ランクの更新量
    for(v = 0; v < g->n; v++) {
      er = rank[v] - new_rs[v];
      err += er*er;
      rank[v] = new_rs[v];
    }
    if(err < 1e-6) break;      // 不動点で繰り返し終了
  }
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

  // 強連結成分分解
  {
    int *component = malloc(sizeof(int) * g->n);
    int j;
    scc(g, component);
    
    printf("digraph{\n");
    for(i = 0; i < g->n; i++) {
      printf("%d [label=\"%d\"]\n", i, component[i]);
    }
    for(i = 0; i < g->n; i++) {
      for(j = 0; j < g->n; j++) {
        if(g->matrix[i][j]) {
          printf("%d -> %d\n", i, j);
        }
      }
    }
    printf("}\n");
  }
  // PageRank
  {
    double *rank = malloc(sizeof(double) * g->n);
    pagerank(g, rank);
    
    for(i = 0; i < g->n; i++) {
      printf("// rank of %d is %g\n", i, rank[i]);
    }
  }

  
  return 0;
}