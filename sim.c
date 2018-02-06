#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define DEST 1 //DO NOT CHANGE
#define K 4
int destNumber = 3; //# of middlebox
int globalStartNumber = 1; //# of connections
//Starting location test
int currentLayer; //which layer, PM = 1, Edge = 2, Agg = 3, Core = 4
int start; //node # in layer
//---------------------
int Core[((K*K)/4)+1];
int Aggregation[(K*(K/2))+1];
int Edge[(K*(K/2))+1];
int PM[((K*K*K)/4)+1];

int DestAggFlag = 0;
int DestEdgeFlag = 0;
int DestCoreFlag = 0;
int DestReached = 0;
int FirstHalf = 0;
int DestPod = 0;
int hopCounter = 0;
int totalHops = 0;
int finalHops = 0;


//set arrays to 0
void populate_arrays(){
	int i, x, y, z;
	for(i = 0; i < ((K*K)/4)+1; i++){
		Core[i] = 0;
	}
	for(x = 0; x < (K*(K/2))+1; x++){
		Aggregation[x] = 0;
	}
	for(y = 0; y < (K*(K/2))+1; y++){
		Edge[y] = 0;
	}
	for(z = 0; z < ((K*K*K)/4)+1; z++){
		PM[z] = 0;
	}
}

/*
takes index of PM currently at (int)
return index of edge switch above current PM (int)  */
int atPM(int i){
	if(PM[i] == DEST){
		currentLayer = 1;
		return i;
	}
	int edgeIndex = ((i + ((K/2)-1))/(K/2));
	currentLayer = 2;
	printf("e%d ", edgeIndex);
	hopCounter++;
	totalHops++;
	return atEdge(edgeIndex);
}

/*
takes index of Edge currently at (int)
return index of Agg switch or PM along path to dest (int)  */
int atEdge(int i){

	if(Edge[i] == DEST){
		currentLayer = 2;
		return i;
	}

	int pod = ((i + ((K/2)-1))/(K/2));
	int podMax = pod*(K/2);
	int podMin = podMax - (K/2) + 1;
	int z;
	 //check dest in PM below edge
	for(z = (i*(K/2))-(K/2)+1; z < (i*(K/2))+1; z++){
	 	if(PM[z] == DEST){
	 		currentLayer = 1;
	 		printf("p%d ", z);
	 		hopCounter++;
			totalHops++;
	 		return atPM(z);
	 	}
	 }

	 //check agg in pod if agg true
	if(DestAggFlag == 1){
	 	//checks agg switchs in same pod
	 	for(z = podMin; z < podMax + 1; z++){
	 		if(Aggregation[z] == DEST){
	 			currentLayer = 3;
	 			printf("a%d ", z);
	 			hopCounter++;
				totalHops++;
	 			return atAgg(z);
	 		}
	 	}

	} 

	if(FirstHalf == 1){
			currentLayer = 3;
			printf("a%d ", podMin);
			hopCounter++;
			totalHops++;
			return atAgg(podMin);
	} else {
			currentLayer = 3;
			printf("a%d ", podMax);
			hopCounter++;
			totalHops++;
			return atAgg(podMax);
	}
}

/*
takes index of Agg switch currently at (int)
return index of core or edge switch along path to dest (int)  */
int atAgg(int i){
	if(Aggregation[i] == DEST){
		currentLayer = 3;
		return i;
	}

	int pod = ((i + ((K/2)-1))/(K/2));
	int podMax = pod*(K/2);
	int podMin = (podMax - (K/2)) + 1;
	double podMid = (double)(podMax+podMin)/2; 
	int iFirstHalf;
	if(i < podMid){
		iFirstHalf = 1;
	} else {
		iFirstHalf = 0;
	}
	//check flags
	if(DestAggFlag == 1){
	//check if half flags match
		if(iFirstHalf == FirstHalf){
			if(FirstHalf == 1){
				currentLayer = 4;
				printf("c%d ", 1);
				hopCounter++;
				totalHops++;
				return atCore(1); //first core switch
			} else {
				currentLayer = 4;
				printf("c%d ", (K*K)/4);
				hopCounter++;
				totalHops++;
				return atCore((K*K)/4); //last core switch
			}
		} else { //if half flags not equal, go down to approp edge 
			currentLayer = 2;
			printf("e%d ", i);
			hopCounter++;
			totalHops++;
			return atEdge(i); 
		}
	}
	int x, z;
	//check core flag
	if(DestCoreFlag == 1){
	//check half flags match
		if(iFirstHalf == FirstHalf){
			if(FirstHalf == 1){
				for(x = 1; x < (((K*K)/4)/2)+1; x++){
					if(Core[x] == DEST){
						currentLayer = 4;
						printf("c%d ", x);
						hopCounter++;
						totalHops++;
						return atCore(x);
					}
				}
			} else {
				for(x = (((K*K)/4)/2)+1; x < ((K*K)/4)+1; x++){
					if(Core[x] == DEST){
						currentLayer = 4;
						printf("c%d ", x);
						hopCounter++;
						totalHops++;
						return atCore(x);
					}
				}
			}
		} else { //core and agg not in same half
			currentLayer = 2;
			printf("e%d ", i);
			hopCounter++;
			totalHops++;
			return atEdge(i); 
		}
	}
	//check edges and pms in the same pod
	for(x = podMin; x < podMax + 1; x++){
		if(Edge[x] == DEST){
			currentLayer = 2;
			printf("e%d ", x);
			hopCounter++;
			totalHops++;
			return atEdge(x);
		} else {
	 		//check dest in PM below edge 
			for(z = (x*(K/2))-(K/2)+1; z < (x*(K/2))+1; z++){
	 			if(PM[z] == DEST){
	 				currentLayer = 2;
	 				printf("e%d ", ((z + ((K/2)-1))/(K/2)));
	 				hopCounter++;
					totalHops++;
	 				return atEdge((z + ((K/2)-1))/(K/2));;
	 			}
	 		}
		}
	}
	//not in pod, move to core
	if(iFirstHalf == 1){
		currentLayer = 4;
		printf("c%d ", 1);
		hopCounter++;
		totalHops++;
		return atCore(1); //first core switch
	} else {
		currentLayer = 4;
		printf("c%d ", (K*K)/4);
		hopCounter++;
		totalHops++;
		return atCore((K*K)/4); //last core switch
	}

}

/*
takes index of core switch currently at (int)
return index of agg switch along path to dest (int)  */
int atCore(int i){
	if(Core[i] == DEST){
		currentLayer = 4;
		return i;
	}

	//check which half of core switch currently at
	int iFirstHalf;
	double coreHalf = (double)(((K*K)/4)+1)/2;
	if(i < coreHalf){
		iFirstHalf = 1;
	} else {
		iFirstHalf = 0;
	}

	//check core flag
	if(DestCoreFlag == 1){
		if(iFirstHalf == 1){
			//count here somehow
			currentLayer = 3;
			printf("a%d ", 1);
			hopCounter++;
			totalHops++;
			return atAgg(1); //go to first agg and run atAGG
		} else {
			//count here somehow
			currentLayer = 3;
			printf("a%d ", (K/2)*K);
			hopCounter++;
			totalHops++;
			return atAgg((K/2)*K); //go to last agg and run atAGG
		}
	}

	int y;
	int podMin = (K/2)*DestPod - (K/2) +1;
	int podMax = (K/2)*DestPod+1; //max of pod +1
	double podMid = (double)(podMin+(podMax-1))/2; //mid of pod +1
	//check agg flag 
	if(DestAggFlag == 1){
		if(FirstHalf == iFirstHalf){
			if(FirstHalf == 1){
				for(y = podMin; y < podMid; y++){
					if(Aggregation[y] == DEST){
						currentLayer = 3;
						printf("a%d ", y);
						hopCounter++;
						totalHops++;
						return atAgg(y);
					}
				}
			} else {
				for(y = podMax; y > podMid; y--){
					if(Aggregation[y] == DEST){
						currentLayer = 3;
						printf("a%d ", y);
						hopCounter++;
						totalHops++;
						return atAgg(y);
					}
				}
			}
		} else { //core and agg not on same half
			if(iFirstHalf == 1){
				currentLayer = 3;
				printf("a%d ", podMin);
				hopCounter++;
				totalHops++;
				return atAgg(podMin);
			} else {
				currentLayer = 3;
				printf("a%d ", podMax - 1);
				hopCounter++;
				totalHops++;
				return atAgg(podMax - 1);
			}
		}
	}
	//checking edge and PM no longer neccessary
	//just run atAGG
	if(iFirstHalf == 1){
		currentLayer = 3;
		printf("a%d ", podMin);
		hopCounter++;
		totalHops++;
		return atAgg(podMin); 
	} else {
		currentLayer = 3;
		printf("a%d ", podMax - 1); 
		hopCounter++;
		totalHops++;
		return atAgg(podMax - 1); 
	}
}

/*
Sets flags for destination. Flags set:
DestAggFlag - set to 1 if destination is in Agg switch
DestCoreFlag - set to 1 if destination is in Core switch
DestEdgeFlag - set to 1 if destination is in Edge switch
DestPod - if destination is not in core, set to pod #
FirstHalf - which half of pod destination resides (which half of tree for cores)*/
void pathFlags(){
	int z;
	 //check all agg first then set flags
	 for(z  = 1; z < (K*(K/2))+1; z++){
	 	if(Aggregation[z] == DEST){
	 		DestAggFlag = 1;
	 		int pod = ((z + ((K/2)-1))/(K/2));
	 		DestPod = pod;
	 		int podMax = pod*(K/2);
	 		int podMin = podMax - (K/2) + 1;
	 		double podMid = (double)(podMax+podMin)/2;
	 		if(z < podMid){
	 			FirstHalf = 1;
	 		} else {
	 			FirstHalf = 0;
	 		}
			z = (K*(K/2))+1;
	 	} else {
	 		DestAggFlag = 0;
	 	}
	 }

	 //core switch check if agg is not dest
	 if(DestAggFlag == 0){
	 	int j;
	 	int coreMax = (K*K)/4;
		double coreMid = (double)(coreMax+1)/2;
	 	//check all cores then set flags
	 	for(j = 1; j < ((K*K)/4)+1; j++){
	 		if(Core[j] == DEST){
	 			DestCoreFlag = 1;
	 			if(j < coreMid){
	 				FirstHalf = 1;
	 			} else {
	 				FirstHalf = 0;
	 			}
	 			j = ((K*K)/4)+1;
	 		} else {
	 			DestCoreFlag = 0;
	 		}
	 	}
	}

	//find edge pod
	if(DestAggFlag == 0 && DestCoreFlag == 0){
		for(z  = 1; z < (K*(K/2))+1; z++){
		 	if(Edge[z] == DEST){
		 		DestEdgeFlag = 1;
		 		int pod = ((z + ((K/2)-1))/(K/2));
		 		DestPod = pod;
		 		FirstHalf = 1;
		 		z = (K*(K/2))+1;
		 	} else {
		 		DestEdgeFlag = 0;
		 	}
	 	}
	}
	//find PM pod
	if(DestAggFlag == 0 && DestCoreFlag == 0 && DestEdgeFlag == 0){
		for(z = 1; z < ((K*K*K)/4)+1; z++){
			if(PM[z] == DEST){
				int pod = ((z) + ((K/2)*(K/2)) -1)/((K/2)*(K/2));
				DestPod = pod;
				FirstHalf = 1;
				z = ((K*K*K)/4)+1;
			}
		}
	}

	//set destination reached flag to 0
	DestReached = 0;
}

void Runner(){

	pathFlags();
	//driver for functions/ checks if at dest
	while(DestReached == 0){
		switch(currentLayer){
			case 1: //pms
				start = atPM(start);
				if(PM[start] == DEST){
					DestReached = 1;
				}
			break;
			case 2: //edge
				start = atEdge(start);
				if(start<=(K*(K/2))+1){
					if(Edge[start] == DEST){
						DestReached = 1;
					}
				}
			break;
			case 3: //agg
				start = atAgg(start);
				if(start<=(K*(K/2))+1){
					if(Aggregation[start] == DEST){
						DestReached = 1;
					}
				}
			break;
			case 4: //core
				start = atCore(start);
				if(start<=((K*K)/4)+1){
					if(Core[start] == DEST){
						DestReached = 1;
					}
				}
			break;
		}
	}
	//reset flags when destination is reached
	if(DestReached == 1){ 
		DestAggFlag = 0;
		DestEdgeFlag = 0;
		DestCoreFlag = 0;
		DestReached = 0;
		FirstHalf = 0;
		DestPod = 0;
	}
}

//returns a single random node
int randomNode(){
	
	int index = 0;
	index = rand() % ((((K * K * K)/4) + 1) - 1) + 1; // physical
	return index;
}

//random middlebox placement
void randomLayer(int numberOfMiddleBoxes, int destArray[][((K*K*K)/4)+1]){
	int middleBox = numberOfMiddleBoxes;
	srand(time(NULL));
	int layer = 0;
	int index = 0;
	int tempCounter = 1;
	do{
		layer = rand() % ((4+1) - 2) + 2;
		switch(layer)
		{
			case 2:
			{
				index = rand() % ((((K * K)/2) + 1) - 1) + 1; // edge
				break;
			}
			
			case 3:
			{
				index = rand() % ((((K * K)/2) + 1) - 1) + 1;
				break;
			}
			
			case 4:
			{
				index = rand() % ((((K * K)/4) + 1) - 1) + 1;
				break;
			}
		}
		//printf("middlebox: l:%d, n: %d order: %d \n", layer, index, tempCounter);
		if(destArray[layer][index] == 0){
			destArray[layer][index] = tempCounter;
			tempCounter++;
			middleBox--;
		} 

	}while(middleBox != 0);

}

void destPathFinder(int FDlayer, int FDnode, int destArray[][((K*K*K)/4)+1]){

	//print out starting location
	switch(currentLayer){
		case 1: //pms
			printf("Starting at p%d -> ", start);
		break;
		case 2: //edge
			printf("Starting at e%d -> ", start);
		break;
		case 3: //agg
			printf("Starting at a%d -> ", start);
		break;
		case 4: //core
			printf("Starting at c%d -> ", start);
		break;
	}

	int layerIndex = 4; // 3 layers +1 (not starting at 0)
	int nodeIndex = ((K*K*K)/4)+1; // max possible nodes in a layer +1
	int t, u;
	int destCounter = 1; 

	while(destCounter <= destNumber){	
		for(t = 1; t < layerIndex+1; t++){
			for(u = 1; u < nodeIndex; u++){
				if(destArray[t][u] == destCounter){
					switch(t){
						case 1: //pms
							populate_arrays();
							PM[u] = DEST;
							Runner();
						break;
						case 2: //edge
							populate_arrays();
							Edge[u] = DEST;
							Runner();
						break;
						case 3: //agg
							populate_arrays();
							Aggregation[u] = DEST;
							Runner();
						break;
						case 4: //core
							populate_arrays();
							Core[u] = DEST;
							Runner();
						break;
					}
				}
			}
		}
		destCounter++;
	}

	switch(FDlayer){
		case 1: //pms
			populate_arrays();
			PM[FDnode] = DEST;
			Runner();
		break;
		case 2: //edge
			populate_arrays();
			Edge[FDnode] = DEST;
			Runner();
		break;
		case 3: //agg
			populate_arrays();
			Aggregation[FDnode] = DEST;
			Runner();
		break;
		case 4: //core
			populate_arrays();
			Core[FDnode] = DEST;
			Runner();
		break;
	}
}

// void greedyAlg(int startGArray[startNumber*2], int endGArray[startNumber*2], int midboxArray[][((K*K*K)/4)+1]){

// 	int to, tp, coreTp, u, midt, midu, minCounter = 0;
// 	int layerIndex = 4; 
// 	int nodeIndex = (K*(K/2));
// 	int minLayer = 0;
// 	int minNode = 0;
// 	int startCounter = 0; 
// 	int endCounter = 0;
// 	int destChecker = 1;

// 	//set temp to 0
// 	int tempMidboxArray[layerIndex+1][((K*K*K)/4)+1];
// 	int tempmidlayer, tempmidnode;
// 	for(tempmidlayer = 1; tempmidlayer < layerIndex+1; tempmidlayer++){
// 		for(tempmidnode = 1; tempmidnode < nodeIndex; tempmidnode++){
// 			tempMidboxArray[tempmidlayer][tempmidnode] = 0;
// 		}
// 	}

// 	while(destChecker < destNumber+1){
// 		printf("startNumber: %d\n", startNumber);
// 		for(to = 2; to < layerIndex; to++){
// 			for(tp = 1; tp < nodeIndex+1; tp++){
// 				if(midboxArray[to][tp] == 0){
// 					midboxArray[to][tp] = destChecker;
// 					printf("midbox: %d %d \n", to, tp);
// 					for(u = 0; u < startNumber*2; u++){
// 						currentLayer = startGArray[u];
// 						u++;
// 						start = startGArray[u];
// 						destPathFinder(endGArray[endCounter], endGArray[endCounter+1], midboxArray);
// 						endCounter = endCounter+2;
// 						printf("--> hops: %d\n", hopCounter);
// 						hopCounter = 0;	
// 					}
// 					printf("totalHops: %d\n", totalHops);
// 					if((totalHops < minCounter || minCounter == 0)){
// 						minCounter = totalHops;
// 						minLayer = to;
// 						minNode = tp;
// 					}
// 					printf("minCounter: %d\n", minCounter);
// 					totalHops = 0;
// 					endCounter = 0;
// 					midboxArray[to][tp] = 0;
// 				}
// 			}
// 		}

// 		for(coreTp = 1; coreTp < ((K*K)/4)+1; coreTp++){
// 			if(midboxArray[4][coreTp] == 0){
// 				midboxArray[4][coreTp] = destChecker;
// 				printf("midbox: %d %d \n", 4, coreTp);
// 				for(u = 0; u < startNumber*2; u++){
// 					currentLayer = startGArray[u];
// 					u++;
// 					start = startGArray[u];
// 					destPathFinder(endGArray[endCounter], endGArray[endCounter+1], midboxArray);
// 					endCounter = endCounter+2;
// 					printf("--> hops: %d\n", hopCounter);
// 					hopCounter = 0;	
// 				}
// 				printf("totalHops: %d\n", totalHops);
// 				if((totalHops < minCounter || minCounter == 0)){
// 					minCounter = totalHops;
// 					minLayer = 4;
// 					minNode = coreTp;
// 				}
// 				printf("minCounter: %d\n", minCounter);
// 				totalHops = 0;
// 				endCounter = 0;
// 				midboxArray[4][coreTp] = 0;
// 			}
// 		}

// 		tempMidboxArray[minLayer][minNode] = destChecker;
// 		midboxArray[minLayer][minNode] = destChecker;
// 		printf("------------------midbox set: %d %d\n", minLayer, minNode);
// 		printf("------------------hop: %d\n", minCounter);
// 		destChecker++;
// 		minCounter = 0;
// 		minLayer = 0;
// 		minNode = 0;
// 	}

// 	for(tempmidlayer = 1; tempmidlayer < layerIndex+1; tempmidlayer++){
// 		for(tempmidnode = 1; tempmidnode < nodeIndex; tempmidnode++){
// 			if(tempMidboxArray[tempmidlayer][tempmidnode] != 0){
// 				midboxArray[tempmidlayer][tempmidnode] = tempMidboxArray[tempmidlayer][tempmidnode];
// 			}
// 		}
// 	}
// 	printf("number of mid boxs = %d\n", destChecker -1);
// }

void simCreate(int startNode, int endNode, int destinationArray[5][((K*K*K)/4)+1]){
	srand(time(NULL));
	int layerIndex = 4; 
	int nodeIndex = ((K*K*K)/4)+1; // max nodes in a layer +1
	int startNumber = 1; //number of start points
	int randomFlag = 0; // set to 1 if want start and end points random
	//int destinationArray[layerIndex+1][nodeIndex]; //single path to be used
	// int to, tp;
	// for(to = 1; to < layerIndex+1; to++){
	// 	for(tp = 1; tp < nodeIndex; tp++){
	// 		destinationArray[to][tp] = 0;
	// 	}
	// }
	
	int startArray[startNumber*2], startFill;
	for(startFill = 0; startFill < startNumber*2; startFill++){
		startArray[startFill] = -1;
	}

	//startArray[0] = layer number
	//startArray[1] = node number
	//startArray[2] = layer number for next start point .... ect .
//-----------------------------START POINTS-----------------------
	int startP, randomStartTemp;
	int notRandomStart = 0;
	if(randomFlag == 1){
		for(startP = 0; startP < startNumber*2; startP++){
			if(startP % 2){
				randomStartTemp = randomNode();
				startArray[startP] = randomStartTemp;
			} else {
				startArray[startP] = 1;
			}
		}
	} else{
		for(startP = 0; startP < startNumber*2; startP++){
			if(startP % 2){
				notRandomStart++;
				if(notRandomStart > ((K*K*K)/4)){
					notRandomStart = 1;
				}
				startArray[startP] = notRandomStart;
			} else {
				startArray[startP] = 1;
			}
		}
	}

	startArray[0] = 1;
	startArray[1] = startNode;
	// startArray[2] = 1;
	// startArray[3] = 6;
	// startArray[4] = 1;
	// startArray[5] = 7;
	// startArray[6] = 1;
	// startArray[7] = 2;
	// startArray[8] = 1;
	// startArray[9] = 14;
//----------------------------------------------------------------
	int endArray[startNumber*2], fill;
	for(fill = 0; fill < startNumber*2; fill++){
		endArray[fill] = -1;
	}

	//endArray[0] = layer number
	//endArray[1] = node number
	//endArray[2] = layer number for next final dest .... ect ....
//-----------------------------END POINTS-------------------------

	int endP, randomEndTemp;
	int notRandomEnd = ((K*K*K)/4)+1;
	if(randomFlag == 1){
		for(endP = 0; endP < startNumber*2; endP++){
			if(endP % 2){
				randomEndTemp = randomNode();
				endArray[endP] = randomEndTemp;
			} else {
				endArray[endP] = 1;
			}
		}
	} else {
		for(endP = 0; endP < startNumber*2; endP++){
			if(endP % 2){
				notRandomEnd--;
				if(notRandomEnd < 1){
					notRandomEnd = ((K*K*K)/4);
				}
				endArray[endP] = notRandomEnd;
			} else {
				endArray[endP] = 1;
			}
		}
	}

	endArray[0] = 1;
	endArray[1] = endNode;
	// endArray[2] = 1;
	// endArray[3] = 14;
	// endArray[4] = 1;
	// endArray[5] = 4;
	// endArray[6] = 1;
	// endArray[7] = 13;
	// endArray[8] = 1;
	// endArray[9] = 16;
//----------------------------------------------------------------	

	int t, u;
	int startCounter = 0; 
	int endCounter = 0;

//--------reset flags and run random alg-------------------------

	printf("************MAIN RUNNING******************************************\n");
	for(u = 0; u < startNumber*2; u++){
		currentLayer = startArray[u];
		u++;
		start = startArray[u];
		destPathFinder(endArray[endCounter], endArray[endCounter+1], destinationArray);
		endCounter = endCounter+2;
		printf("--> hops: %d\n", hopCounter);
		hopCounter = 0;	
	}

	//printing out the middle boxes
	int too, tpp, destPrintOutCounter = 1;
	while(destPrintOutCounter <= destNumber){
		for(too = 1; too < layerIndex+1; too++){
			for(tpp = 1; tpp < nodeIndex; tpp++){
				if(destinationArray[too][tpp] == destPrintOutCounter){
					switch(too){
						case 2: //edge
							printf("middlebox #%d: e%d \n", destPrintOutCounter, tpp);
							break;
						case 3: //agg
							printf("middlebox #%d: a%d \n", destPrintOutCounter, tpp);
							break;
						case 4: //core
							printf("middlebox #%d: c%d \n", destPrintOutCounter, tpp);
							break;
					}
					destPrintOutCounter++;
				}
			}
		}
	}
	printf("For K = %d\n", K);
	printf("Number of middle boxs: %d\n", destNumber);
	printf("Number of connections: %d\n", startNumber);
	printf("Number of hops (RANDOM): %d\n", totalHops);
	int totalHopsRandom = totalHops;
	finalHops += totalHops;
	DestAggFlag = 0;
	DestEdgeFlag = 0;
	DestCoreFlag = 0;
	DestReached = 0;
	FirstHalf = 0;
	DestPod = 0;
	hopCounter = 0;
	totalHops = 0;
	// for(to = 1; to < layerIndex+1; to++){
	// 	for(tp = 1; tp < nodeIndex; tp++){
	// 		destinationArray[to][tp] = 0;
	// 	}
	// }
	startCounter = 0; 
	endCounter = 0;
	//-------------------------------GREEDY------------------------------
	// greedyAlg(startArray, endArray, destinationArray);
	
	// printf("************MAIN RUNNING (GREEDY)******************************************\n");
	// for(u = 0; u < startNumber*2; u++){
	// 	currentLayer = startArray[u];
	// 	u++;
	// 	start = startArray[u];
	// 	destPathFinder(endArray[endCounter], endArray[endCounter+1], destinationArray);
	// 	endCounter = endCounter+2;
	// 	printf("--> hops: %d\n", hopCounter);
	// 	hopCounter = 0;	
	// }

	// printf("******************Complete**********************\n");
	//printing out the middle boxes
	// destPrintOutCounter = 1;
	// while(destPrintOutCounter <= destNumber){
	// 	for(too = 1; too < layerIndex+1; too++){
	// 		for(tpp = 1; tpp < nodeIndex; tpp++){
	// 			if(destinationArray[too][tpp] == destPrintOutCounter){
	// 				switch(too){
	// 					case 2: //edge
	// 						printf("middlebox test #%d: e%d \n", destinationArray[too][tpp], tpp);
	// 						break;
	// 					case 3: //agg
	// 						printf("middlebox test #%d: a%d \n", destinationArray[too][tpp], tpp);
	// 						break;
	// 					case 4: //core
	// 						printf("middlebox test #%d: c%d \n", destinationArray[too][tpp], tpp);
	// 						break;
	// 				}
	// 				destPrintOutCounter++;
	// 			}
	// 		}
	// 	}
	// }
	
	// printf("For K = %d\n", K);
	// printf("Number of middle boxs: %d\n", destNumber);
	// printf("Number of connection pairs: %d\n", startNumber);
	// printf("Number of hops (GREEDY): %d\n", totalHops);
	// printf("Number of hops(RANDOM): %d\n", totalHopsRandom);
}


//start of new code
/**
 * Find the policy chain from the given group of mbs
 * and feed the resulting 'destArray' into main
 * loop untill all vms have been served
 **/

int main(){
/**
 *TO DO LIST
 1. modify simCreate to accept 1 com pair
 2. modify simCreate to accept 1 policy chain (destArray)
 3. create communication pair array in main
 4. create mid box placement array in main
 5. create mid box capacity array in main
 6. feed created arrays from main to simCreate 1 at a time (loop through)
 7. create algorithms 
 **/
	int layerIndex = 4; 
	int nodeIndex = ((K*K*K)/4)+1; // max nodes in a layer +1
	int destinationArrayMain[layerIndex+1][nodeIndex]; //single path to be used
	int to, tp;
	for(to = 1; to < layerIndex+1; to++){
		for(tp = 1; tp < nodeIndex; tp++){
			destinationArrayMain[to][tp] = 0;
		}
	}
	randomLayer(destNumber, destinationArrayMain); //random mid box
	int startNode[globalStartNumber], endNode[globalStartNumber], i;
	for(i = 0; i < globalStartNumber; i++){
		srand(time(NULL)+i);
		startNode[i] = randomNode();
		printf("startNode: %d \n", startNode[i]);
		endNode[i] = randomNode();
		printf("endNode: %d \n", endNode[i]);

		simCreate(startNode[i], endNode[i], destinationArrayMain); //running simCreate
	}
	printf("total hops: %d \n", finalHops);
	return 0;
}
