#ifndef FUNCTIONS_SIM_INCLUDED
#define FUNCTIONS_SIM_INCLUDED
// include guards

/* Prototypes for the functions */
void populate_arrays();
int atPM(int i);
int atEdge(int i);
int atAgg(int i);
int atCore(int i);
void pathFlags();
void Runner();
int randomNode();
void randomLayer(int numberOfMiddleBoxes, int destArray[][((K*K*K)/4)+1]);
void destPathFinder(int FDlayer, int FDnode, int destArray[][((K*K*K)/4)+1]);
void greedyAlg(int startGArray[startNumber*2], int endGArray[startNumber*2], int midboxArray[][((K*K*K)/4)+1]);
void simCreate();

#endif