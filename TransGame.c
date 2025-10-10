// Java Fhourstones 3.1 Transposition table logic
//
// implementation of the well-known game
// usually played on a vertical board of 7 columns by 6 rows,
// where 2 players take turns in dropping counters in a column.
// the first player to get four of his counters
// in a horizontal, vertical or diagonal row, wins the game.
// if neither player has won after 42 moves, then the game is drawn.
//
// This software is copyright (c) 1996-2008 by
//      John Tromp
//      600 Route 25A
//      East Setauket
//      NY 11733
// E-mail: john.tromp@gmail.com
//
// This notice must not be removed.
// This software must not be sold for profit.
// You may redistribute if your distributees have the
// same rights and restrictions.

#include "Game.c"

#define LOCKSIZE 26
#define TRANSIZE 8306069
// should be a prime no less than about 2^{SIZE1-LOCKSIZE}, e.g.
// 4194301,8306069,8388593,15999961,33554393,67108859,134217689,268435399

#define SYMMREC 10 // symmetry normalize first SYMMREC moves
#define UNKNOWN 0
#define LOSS 1
#define DRAWLOSS 2
#define DRAW 3
#define DRAWWIN 4
#define WIN 5
#define LOSSWIN 6

char *score_to_string(int score)
{
  static char *score_strings[] = {"UNKNOWN", "LOSS", "DRAWLOSS", "DRAW", "DRAWWIN", "WIN", "LOSSWIN"};
  return score_strings[score];
}

typedef struct {
#if (LOCKSIZE<=32)
  unsigned biglock:LOCKSIZE;
  unsigned bigwork:6;
  unsigned newlock:LOCKSIZE;
#else
  uint64 biglock:LOCKSIZE;
  unsigned bigwork:6;
  uint64 newlock:LOCKSIZE;
#endif
  unsigned newscore:3;
  unsigned bigscore:3;
} hashentry;

// Transposition table state
typedef struct {
  unsigned int htindex, lock;
  hashentry *ht;
  
  uint64 posed; // counts transtore calls
} TransState;
  
void trans_init(TransState *tstate)
{
  tstate->ht = (hashentry *)calloc(TRANSIZE, sizeof(hashentry));
  if (!tstate->ht) {
    printf("Failed to allocate %lu bytes\n", TRANSIZE*sizeof(hashentry));
    exit(0);
  }
  // TODO(kaushik): not sure if necessary
  tstate->htindex = 0;
  tstate->lock = 0;
  tstate->posed = 0;
}
  
void emptyTT(TransState *tstate)
{
  int i;

  for (i=0; i<TRANSIZE; i++) {
#if (LOCKSIZE<=32)
    tstate->ht[i].biglock = 0;
    tstate->ht[i].bigwork = 0;
    tstate->ht[i].newlock = 0;
#else
    tstate->ht[i].biglock = 0;
    tstate->ht[i].bigwork = 0;
    tstate->ht[i].newlock = 0;
#endif
    tstate->ht[i].newscore = 0;
    tstate->ht[i].bigscore = 0;
  }
  tstate->posed = 0;
}
  
void hash(GameState *state, TransState *tstate)
{
  bitboard htmp, htemp = positioncode(state);
  if (state->nplies < SYMMREC) { // try symmetry recognition by reversing columns
    bitboard htemp2 = 0;
    for (htmp=htemp; htmp!=0; htmp>>=H1)
      htemp2 = htemp2<<H1 | (htmp & COL1);
    if (htemp2 < htemp)
      htemp = htemp2;
  }
  tstate->lock = (unsigned int)(SIZE1>LOCKSIZE ? htemp >> (SIZE1-LOCKSIZE) : htemp);
  tstate->htindex = (unsigned int)(htemp % TRANSIZE);
}
  
// Compute hash and prefetch - call this early to hide memory latency
void hash_and_prefetch(GameState *state, TransState *tstate)
{
  hash(state, tstate);
  // Prefetch now, while caller does other work before calling transpose()
  __builtin_prefetch(&tstate->ht[tstate->htindex], 0, 3);
}

int transpose(TransState *tstate)
{
  hashentry he;

  // NOTE(kaushik): hash() already called by hash_and_prefetch() earlier
  he = tstate->ht[tstate->htindex];

  // NOTE(kaushik): using branchless comparisons
  int bigmatch = (he.biglock == tstate->lock);
  int newmatch = (he.newlock == tstate->lock);
  int result = UNKNOWN;
  result = newmatch ? he.newscore : result;
  result = bigmatch ? he.bigscore : result;
  return result;
}
  
void transtore(TransState *tstate, int x, unsigned int lock, int score, int work)
{
  hashentry he;

  tstate->posed++;
  he = tstate->ht[x];
  if (he.biglock == lock || work >= he.bigwork) {
    he.biglock = lock;
    he.bigscore = score;
    he.bigwork = work;
  } else {
    he.newlock = lock;
    he.newscore = score;
  }
  tstate->ht[x] = he;
}

void htstat(TransState *tstate)      /* some statistics on hash table performance */
{
  int total, i;
  int typecnt[8];                /* bound type stats */
  hashentry he;
 
  for (i=0; i<8; i++)
    typecnt[i] = 0;
  for (i=0; i<TRANSIZE; i++) {
    he = tstate->ht[i];
    if (he.biglock != 0)
      typecnt[he.bigscore]++;
    if (he.newlock != 0)
      typecnt[he.newscore]++;
  }
  for (total=0,i=LOSS; i<=WIN; i++)
    total += typecnt[i];
  if (total > 0) {
    printf("- %5.3f  < %5.3f  = %5.3f  > %5.3f  + %5.3f\n",
      typecnt[LOSS]/(double)total, typecnt[DRAWLOSS]/(double)total,
      typecnt[DRAW]/(double)total, typecnt[DRAWWIN]/(double)total,
      typecnt[WIN]/(double)total);
  }
}
