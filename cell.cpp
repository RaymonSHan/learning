
#include    <errno.h>
#include    <fcntl.h> 
#include    <sched.h>
#include    <signal.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <stdarg.h>
#include    <string.h>
#include    <time.h>
#include    <unistd.h>




void        ExpandRule(unsigned char rule3, unsigned char *rule8)
{
  unsigned char i, l7, l6, l5, l4, l3, l2, l1, l0;
  unsigned char ext_rule3[8];
  for (i=0; i<8; i++) {
    ext_rule3[i] = (rule3 >> i) & 1;
  }

  for (i=0; ; i++) {
    l7 = ((i>>6) & 3) + ((i<<2) & 4);
    l6 = (i>>5) & 7;
    l5 = (i>>4) & 7;
    l4 = (i>>3) & 7;
    l3 = (i>>2) & 7;
    l2 = (i>>1) & 7;
    l1 = i & 7;
    l0 = ((i>>7) & 1) + ((i<<1) & 6);

    (*rule8) = ext_rule3[l7] << 7;
    (*rule8) += ext_rule3[l6] << 6;
    (*rule8) += ext_rule3[l5] << 5;
    (*rule8) += ext_rule3[l4] << 4;
    (*rule8) += ext_rule3[l3] << 3;
    (*rule8) += ext_rule3[l2] << 2;
    (*rule8) += ext_rule3[l1] << 1;
    (*rule8) += ext_rule3[l0];

    rule8 ++;
     if (i==255) break;
  }


};

void        GetOneLoop(unsigned char *rule8)
{

  unsigned char haveDo[256];
  unsigned char loopNum[256];

  unsigned char loopSize[256];
  unsigned char minloop[256];
  unsigned char maxloop[256];
  unsigned char i = 0, val, loopnum = 0, count, lastloop;
  memset(haveDo, 0, 256);
  memset(loopNum, 0, 256);

  memset(loopSize, 0, 256);
  memset(minloop, 0, 256);
  memset(maxloop, 0, 256);

  for (i=0; ;i++) {
    count = 1;
    val = i;
    while (haveDo[val] == 0) {
      haveDo[val] = count++;
      loopNum[val] = loopnum;
      loopSize[loopnum] ++;

      val = rule8[val];

    }

    lastloop = loopNum[val];

    if (lastloop == loopnum) {

      minloop[loopnum] = count - haveDo[val];
      count --;
      maxloop[loopnum] = count;
      val = i;

      while (haveDo[val] != maxloop[loopnum]) {
	haveDo[val] = count;
	if (count > minloop[loopnum]) count--;
	val = rule8[val];
      }
      loopnum ++;
    } else {

      count += (haveDo[val] - 1);
      if (count > maxloop[lastloop]) {
	maxloop[lastloop] = count;
      }
      loopSize[lastloop] += loopSize[loopnum];
      loopSize[loopnum] = 0;

      val = i;
      while (loopNum[val] != lastloop) {
	haveDo[val] = count--;
	loopNum[val] = lastloop;
	val = rule8[val];
      }
    }

    if (i==255) break;
  }

  for (i=0; ;i++) {
    printf("%3d:%3d   ", haveDo[i], loopNum[i]);
    if (i % 8 == 7) printf("\n");
    if (i == 255) break;
  }
  //  printf("\e[0;31;40m");

  // for (i=0; ;i++) {
  //     printf("i:%d, size:%d, min:%d, max:%d\n", i, loopSize[i], minloop[i], maxloop[i]);
  //   if (i==loopnum) break;
  // }

}

int         main(int, char**)
{
  unsigned char rule[260];
  unsigned char i;
  for (i=0; ; i++) {

    printf("\n\nfor rule: %d\n", i);
    ExpandRule(i, rule);
  
    GetOneLoop(rule);
    if (i==255) break;
  }


  return 0;
}

