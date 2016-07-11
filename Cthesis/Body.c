#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include "math.h"
#include "Grid.h"
#include "Ability.h"
#include "Cell.h"
#include "Camera.h"
#include "LL.h"
#include "MathFuncs.h"
#include "DataCollection.h"
#include <GL/glx.h>
#include "Body.h"
#include <string.h>

void EnTransfer(long* to, long* from, int amount){
  *from-=amount;
  *to+=amount;
}

void alphabetSoup(Grid* w,FILE* CellFile){
  //need to save timing and bodysize info to assess composition of alphabet componenets
  long res[NUMABILITIES*NUMCELLS];
  memset(res,0,sizeof(res));
  for (int i=0;i<NUMCELLS;i++){
    Cell currCell=w->cells[i];
    if(currCell.alive){
      Brain* currBrain=currCell.myBrain;
      for(int j=0;j<ABILITIES_PER_BRAIN;j++){
        int HistEntry=currBrain->abilities[j].typeHistory[0];
        if(HistEntry<NUMABILITIES){
        res[i*NUMABILITIES+HistEntry]+=1;
        }
      }
    }
  }
  fwrite(&res[0],sizeof(long),NUMABILITIES*NUMCELLS,CellFile);
}

//void saveBody(Body* saveMe, FILE* AnatomyFile){
//}

//void saveCell(Cell* saveMe, FILE* AnatomyFile){
//  //save cell position and orientation relative to body, weight matrix, abilities, and IO connections also need to save BodyCellI, numBonds, BondedCells
//  int retSize=NUMNEURONS*NUMNEURONS+ABILITIES_PER_BRAIN+ABILITIES_PER_BRAIN*NEURONS_PER_ABILITY*2+3+6+1+ABILITIES_PER_BRAIN*2;
//  float ret[retSize];
//  if(!saveMe->alive){
//    fprintf(stdout,"error, saving cell that is dead!");
//  }
//  int tot=0;
//  Brain* saveBrain=saveMe->myBrain;
//  for(int i=0;i<NUMNEURONS*NUMNEURONS;i++){
//    ret[tot+i]=saveBrain->weights[i];
//  }
//  tot+=NUMNEURONS*NUMNEURONS;
//  for(int i=0;i<ABILITIES_PER_BRAIN;i++){
//    ret[tot+i]=saveBrain->abilities->typeHistory[i];
//  }
//  tot+=ABILITIES_PER_BRAIN;
//  for(int i=0;i<ABILITIES_PER_BRAIN;i++){
//    for(int j=0;j<2;j++){
//      for(int k=0;k<NEURONS_PER_ABILITY;k++){
//        ret[i*ABILITIES_PER_BRAIN+j*NEURONS_PER_ABILITY+k]=saveBrain->IOTypes[i][j];
//      }
//    }
//  }
//  tot+=ABILITIES_PER_BRAIN*NEURONS_PER_ABILITY*2;
//  for(int i=0;i<3;i++){
//    ret[tot+i]=saveMe->CellPos[i];
//  }
//  tot+=3;
//  LL* currTouching=saveMe->BondedCells;
//  for(int i=0;i<6;i++){
//    if(!currTouching){
//      ret[tot+i]=((Cell*)currTouching->val)->BodyCellI;
//      currTouching=currTouching->next;
//    }
//    else{
//      ret[tot+i]=-1;
//    }
//  }
//  tot+=6;
//  ret[tot+1]=saveMe->BodyCellI;
//  tot+=1;
//  for(int i=0;i<ABILITIES_PER_BRAIN;i++){
//    if(saveBrain->abilities[i].typeHistory[0]==NOUTPUT){
//      ret[tot+2*i]=saveBrain->abilities[i].AbilityInts[0];
//      ret[tot+2*i+1]=saveBrain->abilities[i].AbilityInts[1];
//    }
//    else{
//      ret[tot+2*i]=0;
//      ret[tot+2*i+1]=0;
//    }
//  }
//  tot+=ABILITIES_PER_BRAIN*2;
//  if(tot!=retSize){
//  fprintf(stdout,"error: tot and retSize should be the same every time %i %i",tot,retSize);
//  }
//  fwrite(&ret[0],sizeof(float),retSize,AnatomyFile);
//}

//void recordUsages(Grid* w,FILE* UAbilityFile,FILE* UWeightFile,FILE* UBodyFile,FILE* RecordInfoFile){
//  long Info[3]={w->ticks,0,0};
//  long writeUsage;
//  for(int i=0;i<NUMCELLS;i++){
//    Cell* currCell=&w->cells[i];
//    if(currCell->alive){
//      long myAge=currCell->myBody->age;
//      Brain* fromMe=currCell->myBrain;
////      Info[1]+=NUMNEURONS*NUMNEURONS;
////      for(int j=0;j<NUMNEURONS*NUMNEURONS;j++){
////        writeUsage=fromMe->usageN[j]+myAge;
////        fwrite(&writeUsage,sizeof(long),1,UWeightFile);
////      }
//      for(int j=0;j<ABILITIES_PER_BRAIN;j++){
//        if(fromMe->abilities[j].typeHistory[0]!=NONE){
//          Info[1]+=1;
//          writeUsage=fromMe->usageA[j]+myAge;
//        fwrite(&writeUsage,sizeof(long),1,UAbilityFile);
//        }
//      }
//        Info[2]+=1;
//        writeUsage=currCell->myBody->usageB+myAge;
//        fprintf(stdout,"%li\n",currCell->myBody->usageB+myAge);
//        fwrite(&writeUsage,sizeof(long),1,UBodyFile);
//    }
//  }
//  fwrite(&Info[0],sizeof(long),3,RecordInfoFile);
//}
void recordUsages(Grid* w,FILE* UAbilityFile,FILE* UBodyFile,FILE* RecordInfoFile, FILE* AbFile, FILE* BodySizeFile){
  long AbCount[5];
  long AbBins[NUMBINS];
  long BodyBins[NUMBINS];
  long BodySizes[MAX_BODY_SIZE];
  for(int i=0;i<NUMBINS;i++){
    AbBins[i]=0;
    BodyBins[i]=0;
  }
  for(int i=0;i<5;i++){
    AbCount[i]=0;
  }
  for(int i=0;i<MAX_BODY_SIZE;i++){
    BodySizes[i]=0;
  }
  long Info[3]={w->ticks,0,0};
  for(int i=0;i<NUMCELLS;i++){
    Cell* currCell=&w->cells[i];
    if(currCell->myBody->alive){
      long myAge=currCell->myBody->age;
      Brain* fromMe=currCell->myBrain;
      //      Info[1]+=NUMNEURONS*NUMNEURONS;
      //      for(int j=0;j<NUMNEURONS*NUMNEURONS;j++){
      //        writeUsage=fromMe->usageN[j]+myAge;
      //        fwrite(&writeUsage,sizeof(long),1,UWeightFile);
      //      }
      for(int j=0;j<ABILITIES_PER_BRAIN;j++){
        if(fromMe->abilities[j].typeHistory[0]==EAT){ AbCount[1]+=1; }
        if(fromMe->abilities[j].typeHistory[0]==THRUST){ AbCount[2]+=1; }
        if(fromMe->abilities[j].typeHistory[0]==SEESIZE){ AbCount[0]+=1; }
        if(fromMe->abilities[j].typeHistory[0]==NONE){ AbCount[3]+=1; }
        if(fromMe->abilities[j].typeHistory[0]==NINPUT||fromMe->abilities[j].typeHistory[0]==NINPUT){ AbCount[4]+=1; }

        if(fromMe->abilities[j].typeHistory[0]!=NONE){
          Info[1]+=1;
          int incrIndex=(fromMe->usageA[j]+myAge)/BINSIZE;
          if(incrIndex>=BINSIZE){AbBins[BINSIZE-1]+=1;}
          else{AbBins[incrIndex]+=1;}
        }
      }
      Info[2]+=1;
      int incrIndex=(currCell->myBody->usageB+myAge)/BINSIZE;
      if(incrIndex>=BINSIZE){BodyBins[BINSIZE-1]+=1;}
      else{BodyBins[incrIndex]+=1;}
      BodySizes[currCell->myBody->cellSize-1]+=1;
    }
  }
  fwrite(&AbBins[0],sizeof(long),NUMBINS,UAbilityFile);
  fwrite(&BodyBins[0],sizeof(long),NUMBINS,UBodyFile);
  fwrite(&Info[0],sizeof(long),3,RecordInfoFile);
  fwrite(&AbCount[0],sizeof(long),5,AbFile);
  fwrite(&BodySizes[0],sizeof(long),MAX_BODY_SIZE,BodySizeFile);
}

void writeBodyInfo(Grid* w,Body* checkMe,FILE* InfoFile,FILE* WeightFile,FILE* AbFile,FILE* NAbFile,FILE* CommFile){
  long Info[3];
  int Abs[ABILITIES_PER_BRAIN];
  int commData[4];
  Info[0]=checkMe->age;
  Info[1]=checkMe->cellSize;
  Info[2]=w->ticks;
  fwrite(&Info[0],sizeof(long),3,InfoFile);
  for(int i=0;i<checkMe->cellSize;i++){
    fwrite(&checkMe->cells[i]->myBrain->weights[0],sizeof(double),NUMNEURONS*NUMNEURONS,WeightFile);
  }
  for(int i=0;i<checkMe->cellSize;i++){
    fwrite(&checkMe->cells[i]->myBrain->IOTypes[0][0],sizeof(int),NUMNEURONS,AbFile);
    fwrite(&checkMe->cells[i]->myBrain->IOTypes[1][0],sizeof(int),NUMNEURONS,AbFile);
  }
  for(int i=0;i<checkMe->cellSize;i++){
    Cell* currCell=checkMe->cells[i];
    Brain* myBrain=currCell->myBrain;
    for(int j=0;j<ABILITIES_PER_BRAIN;j++){
      Ability* ab=&myBrain->abilities[j];
      if(ab->typeHistory[0]==NOUTPUT){
        commData[0]=currCell->BodyCellI;
        commData[1]=ab->AbilityInts[0];
        commData[2]=ab->IONeurons[1][0];
        commData[3]=ab->AbilityInts[1];
        //need to change!
        fwrite(&commData[0],sizeof(int),4,CommFile);
      }
      Abs[j]=ab->typeHistory[0];
    }
  }
  fwrite(&Abs[0],sizeof(int),ABILITIES_PER_BRAIN,NAbFile);
}

void initBody(Grid* w,Body* newBody,double* massMoment, double* position,Cell* newCell,Brain* newBrain,Brain* prevBrain,Body* prevBody,int ImutationConfig){/* body is intialized either from scratch or copying previous single cell*/
  newBody->ImutationConfig=ImutationConfig;
  newBody->cellSize=1;
  initCell(w,newCell,prevBrain,newBrain,position,newBody,0,0,prevBody,NULL,NULL);
  newBody->energy=0;
  for(int i=0;i<MAX_BODY_SIZE;i++){newBody->cells[i]=NULL;}
  strcpy(newBody->ecosystemOrigin,"original");//bodies from scratch get orig as their original ecosystem
  char SImutationConfig[10];sprintf(SImutationConfig,"%d",ImutationConfig);
  strcat(newBody->ecosystemOrigin,SImutationConfig);
  newBody->iEco=getNameI(newBody,&w->tickRecord);
  newBody->cells[0]=newCell;
  newBody->alive=1;
  newBody->mass[0]=massMoment[0];
  newBody->mass[1]=massMoment[1];
  newBody->BodyPos[0]=position[0];
  newBody->BodyPos[1]=position[1];
  newBody->BodyPos[2]=position[2];
  newBody->vel[0]=0;
  newBody->vel[1]=0;
  newBody->vel[2]=0;
  newBody->moveAcc[0]=0;
  newBody->moveAcc[1]=0;
  newBody->moveAcc[2]=0;
  newBody->bumpAcc[0]=0;
  newBody->bumpAcc[1]=0;
  newBody->bumpAcc[2]=0;
  newBody->myConfig=NULL;
  //  newBody->impulse[0]=0;
  //  newBody->impulse[1]=0;
  newBody->radius=CELLRAD;
  newBody->touching=0;
//  newBody->repEnTalley=0;
//  newBody->repPoints=0;
  newBody->age=0;
  newBody->usageB=0;
//  newBody->reproducing=0;
  newBody->consumedEn=0;
  newBody->killCt=0;
  newBody->EaterEn=0;
};

void newBodyCalcVals(Body* newBody){/* newly formed body mass and moment of inertia are calculated, mass is simply the number of cells, inertia is sum of the distance of each (cell+CELLRAD)^2*CELL_MASS. called when bodies are combined*/
  newBody->BodyPos[0]=0;
  newBody->BodyPos[1]=0;
  for(int i=0;i<newBody->cellSize;i++){
    newBody->BodyPos[0]+=newBody->cells[i]->CellPos[0];
    newBody->BodyPos[1]+=newBody->cells[i]->CellPos[1];
  }
  newBody->BodyPos[0]=newBody->BodyPos[0]/newBody->cellSize;
  newBody->BodyPos[1]=newBody->BodyPos[1]/newBody->cellSize;
  newBody->radius=0;
  newBody->mass[0]=newBody->cellSize;
  for(int i=0;i<newBody->cellSize;i++){
    //w->cells[newBody->cellIndices[i]].BodyIndex=newBody->index;
    double rad=getDist(newBody->cells[i]->CellPos,newBody->BodyPos);
    if(getDist(newBody->cells[i]->CellPos,newBody->BodyPos)>CELLRAD*100){fprintf(stdout,"found it!\n");}//PROBABLY BUG!!!!!!!!!!!!
    newBody->mass[1]=0;
    //fprintf(stdout,"rad+CELLRAD: %lf\n",rad+CELLRAD);
    if(rad+CELLRAD>newBody->radius){newBody->radius=rad+CELLRAD;}//makes sense!
    newBody->mass[1]+=((rad+CELLRAD)/CELLRAD)*((rad+CELLRAD)/CELLRAD)*CELL_MASS;//MIGHT WANT TO ALTER IN THE FUTURE!!!
    //newBody->mass[1]+=1;
    //newBody->mass[1]+=rad*rad*CELL_MASS;
  }
  if(newBody->mass[1]==0){newBody->mass[1]=1;}
  newBody->vel[0]=0;
  newBody->vel[1]=0;
  newBody->vel[2]=0;
  newBody->usageB=0;}

//void resetCellProperties(Cell* c){
//  c->EaterEn=0;
//  c->numEaters=0;
//  c->eatenCount=0;
//  if(c->BodyCellI==0){
//    Body* resetBody=c->myBody;
//    resetBody->acc[0]=0;
//    resetBody->acc[1]=0;
//    resetBody->acc[2]=0;
//    resetBody->touching=0;
//  }
//}

//PUTS NEW BODY INTO PLACE
void getNewBodyLoc(double* parentLoc,double parentRad,double* childLoc,double childRad){
  double totRad=parentRad+childRad;
  //  getRandPtAroundCircle(parentLoc,totRad*5,childLoc);
  double angle=getRandDouble(0,M_PI*2);
  double xdisp=cos(angle)*totRad;
  double ydisp=sin(angle)*totRad;
  childLoc[0]=xdisp+parentLoc[0];
  childLoc[1]=ydisp+parentLoc[1];
  if(childLoc[0]<0+childRad||childLoc[0]>SHAPEX-childRad){
    childLoc[0]=parentLoc[0]-xdisp;}
  if(childLoc[1]<0+childRad||childLoc[1]>SHAPEY-childRad){
    childLoc[1]=parentLoc[1]-ydisp;}}

void combineBodies(Grid* w,Cell* c1,Cell* c2){
//  countIO(c1->myBody);
//  countIO(c2->myBody);
  Body* b1=c1->myBody;
  Body* b2=c2->myBody;
  if(b2->cellSize>b1->cellSize){
    Body* temp=b1;
    b1=b2;
    b2=temp;
  }
  for(int i=0;i<b2->cellSize;i++){
    b1->cells[b1->cellSize+i]=b2->cells[i];
    Cell* changeMyBody=b2->cells[i];
    changeMyBody->myBody=b1;
    changeMyBody->BodyCellI+=b1->cellSize;
    //    for(LL* CellIsLL=changeMyBody->BondedCellBodyIs;CellIsLL;CellIsLL=CellIsLL->next){
    //      double* CellI=CellIsLL->val;
    //      *CellI+=b1->cellSize;
    //    }
  }
  for(int c=0;c<b2->cellSize;c++){
    //changes abilityint0 of Noutput ability to still point ot correct cell, MAY NEED TO CHANGE IF TOO INEFFICIENT
    Brain* checkMe=b2->cells[c]->myBrain;
    for(int abI=0;abI<ABILITIES_PER_BRAIN;abI++){
      Ability* ab=&checkMe->abilities[abI];
      if(ab->typeHistory[0]==NOUTPUT&&ab->AbilityInts[0]>=0){
        ab->AbilityInts[0]+=b1->cellSize;
      }
    }
  }
    LL* c1NewBond=malloc(sizeof(LL));
    LL* c2NewBond=malloc(sizeof(LL));
    initLL(c1NewBond,c2);
    initLL(c2NewBond,c1);
    addLL(&c1->BondedCells,c1NewBond);
    addLL(&c2->BondedCells,c2NewBond);
    c1->numBonds+=1; 
    c2->numBonds+=1; 
//  countIO(c1->myBody);
//  countIO(c2->myBody);
    checkBondedCells(c1);
    checkBondedCells(c2);
//  countIO(c1->myBody);
//  countIO(c2->myBody);
  b1->cellSize+=b2->cellSize;
//  countIO(c1->myBody);
//  countIO(c2->myBody);
  EnTransfer(&b1->energy,&b2->energy,b2->energy);
  LL* leftBody=malloc(sizeof(LL));
  initLL(leftBody,b2);
  addLL(&w->deadBodies,leftBody);
//  countIO(c1->myBody);
//  countIO(c2->myBody);
  w->numDeadBodies+=1;
  b2->alive=0;
  newBodyCalcVals(b1);
  //  fprintf(stdout,"NEW BODY!!!!!!!!!!!\n");
  //  for(int i=0;i<b1->cellSize;i++){
  //    printCell(getCell(w,b1->cellIndices[i]));
  //  }
//  countIO(c1->myBody);
//  countIO(c2->myBody);
  for(int i=0;i<b1->cellSize;i++){
    CellCheckBody(b1->cells[i],__FILE__,__FUNCTION__,__LINE__);
  }
  for(int i=0;i<b2->cellSize;i++){
    CellCheckBody(b2->cells[i],__FILE__,__FUNCTION__,__LINE__);
  }
//  countIO(c1->myBody);
//  countIO(c2->myBody);
}

void transformCoords(double* oldBodyPos, double* newBodyPos, double* oldPos, double* retPos){
  double tempPos[3]={oldPos[0]-oldBodyPos[0],oldPos[1]-oldBodyPos[1],newBodyPos[2]-oldBodyPos[2]};
  rotateCoords(tempPos,retPos,tempPos[2]);
  retPos[0]=newBodyPos[0]+retPos[0];
  retPos[1]=newBodyPos[1]+retPos[1];
  retPos[2]=wrapAngle((newBodyPos[2]-oldBodyPos[2])+oldPos[2]);
}

void copyBody(Grid* w, Body* copyMe, Body* newBody, Cell** newCells,Brain** newBrains,double* newPos,Ecosystem* data,int i){/* copies a body, mutating each individual cell slightly along the way*/
    int cellIs[NUM_ECO_TYPES];
  if(copyMe&&!copyMe->alive){fprintf(stderr,"ERROR, copying body that is dead! bodyIndex");}
  if(newBody->alive){fprintf(stderr,"ERROR, copying over body that is not dead!");}
  if(data){
    setupBodyFromData(w,data,i,newBody,cellIs);}
  else{
    newBody->cellSize=copyMe->cellSize;
    newBody->ImutationConfig=copyMe->ImutationConfig;
    newBody->usageB=copyMe->usageB+copyMe->age;}
  newBody->alive=1;
  newBody->BodyPos[0]=newPos[0];
  newBody->BodyPos[1]=newPos[1];
  newBody->BodyPos[2]=newPos[2];
  newBody->energy=0;
  newBody->vel[0]=0; newBody->vel[1]=0; newBody->vel[2]=0;
  newBody->moveAcc[0]=0; newBody->moveAcc[1]=0; newBody->moveAcc[2]=0;
  newBody->bumpAcc[0]=0; newBody->bumpAcc[1]=0; newBody->bumpAcc[2]=0;
//  newBody->repPoints=0;
  newBody->age=0;
  newBody->killCt=0;
  newBody->consumedEn=0;
  for(int i=0;i<newBody->cellSize;i++){newBody->cells[i]=newCells[i];}//setting up cell pointers
  for(int i=0;i<newBody->cellSize;i++){
    if(!data){
      double newPos[3]={-1,-1,-1};
      transformCoords(copyMe->BodyPos,newBody->BodyPos,copyMe->cells[i]->CellPos,newPos);
      initCell(w,newCells[i],copyMe->cells[i]->myBrain,newBrains[i],newPos,newBody,i,1,copyMe,data,cellIs);}
    else{initCell(w,newCells[i],NULL,newBrains[i],NULL,newBody,i,1,copyMe,data,cellIs);}}
  if(w->evolving){
    MutationConfig* MYconfig=&w->mutationConfigs[copyMe->ImutationConfig];//will segfault if parent does not exist!
    for(int i=0;i<newBody->cellSize;i++){mutateAbilities(newBody->cells[i]->myBrain,MYconfig);}}
//  countIO(copyMe);
//  countIO(newBody);
};

void copyBodyIndividual(Grid* w, Cell* prevCell,Cell* newCell, Brain* newBrain, Body* newBody, double* newPos){/* copies the cells that make up a body, but makes a new body for each so each is a single cell*/
  Brain* copyMe=prevCell->myBrain;
  if(!prevCell->myBody->alive){
    fprintf(stderr,"ERROR, copying body that is dead! into individuals\n");
  }
  if(newBody->alive){
    fprintf(stderr,"ERROR, copying over body that is not dead!");// %i",newBodyIndex);
  }
  double defaultMassMoment[2]={CELL_MASS,CELL_MOMENT_OF_INERTIA};
  initBody(w,newBody,defaultMassMoment,newPos,newCell,newBrain,copyMe,prevCell->myBody,prevCell->myBody->ImutationConfig);
  //need to edit!
  newCell->BondedCells=NULL;
  newCell->numBonds=0;
  checkBondedCells(newCell);
  for(int i=0;i<ABILITIES_PER_BRAIN;i++){
    if(newBrain->abilities[i].typeHistory[0]==NOUTPUT){changeAbility(&newBrain->abilities[i],newBrain,NONE,0,i);}
    else if(newBrain->abilities[i].typeHistory[0]==NINPUT){changeAbility(&newBrain->abilities[i],newBrain,NONE,0,i);}}
  mutateAbilities(newCell->myBrain,&w->mutationConfigs[newCell->myBody->ImutationConfig]);
//  countIO(newBody);
};

void applyForce(Grid* W,Body* pushMe, double* position, double* xyComp,int internal){/* acceleration values for the cell are the sums of applied forces * XYCOMP_TO_ACC */
  //if(xyComp[0]!=xyComp[0]){
  //fprintf(stderr,"here is the problem!\n");
  //}
  //fprintf(stdout,"%lf,%lf,%lf,%lf\n",position[0],position[1],pushMe->BodyPos[0],pushMe->BodyPos[1]);
  double toContactPt[]={(position[0]-pushMe->BodyPos[0])/CELLRAD,(position[1]-pushMe->BodyPos[1])/CELLRAD};
  /*the cross product of the vector to the contact point and the force vector are taken to determine the torque on the body. this is multiplied by either INTERNAL_TORQUE_MULT or EXTERNAL_TORQUE_MULT depending on whether the force is from the cell or from bumping into something*/
  if(internal){
    pushMe->moveAcc[0]+=xyComp[0]*(FORCES_TO_ACC*CELLRAD);
    pushMe->moveAcc[1]+=xyComp[1]*(FORCES_TO_ACC*CELLRAD);
    pushMe->moveAcc[2]+=CrossProduct(toContactPt,xyComp)*(INTERNAL_TORQUE_MULT);}
  else{
    pushMe->bumpAcc[0]+=xyComp[0]*(FORCES_TO_ACC*CELLRAD);
    pushMe->bumpAcc[1]+=xyComp[1]*(FORCES_TO_ACC*CELLRAD);
    pushMe->bumpAcc[2]+=CrossProduct(toContactPt,xyComp)*(EXTERNAL_TORQUE_MULT);}
  if(W->cam.mode==BODYVIEW&&insideCamera(W,position)){
    double VecEnd[2];
    VecEnd[0]=position[0]+xyComp[0]*CELLRAD*16;VecEnd[1]=position[1]+xyComp[1]*CELLRAD*16;
    double VecStartScreen[2];
    double VecEndScreen[2];
    toScreenCoords(W,position,VecStartScreen);
    toScreenCoords(W,VecEnd,VecEndScreen);
    double lineColor[3]={0,0,1};
    drawLine(VecStartScreen,VecEndScreen,lineColor);}};

//void applyImpulse(Grid* W,Cell* c1,Cell* c2){
//  //need to replace force w impulse for collisions! more robust!
//  if(c1>c2){
//    Body* b1=c1->myBody;
//    Body* b2=c2->myBody;
//    double colNorm[2]={(c2->CellPos[0]-c1->CellPos[0]),(c2->CellPos[1]-c1->CellPos[1])};
//    normalize(colNorm,colNorm);
//    double vrel[2]={b2->vel[0]-b1->vel[0],b2->vel[1]-b1->vel[1]};
//    double impulse=((1+IMPULSE_STRENGTH)*DotProduct(vrel,colNorm))/(1/b1->mass[0]+1/b2->mass[0]);
//    impulse=impulse<IMPULSE_MAX?impulse:IMPULSE_MAX;
//    b1->acc[0]+=impulse*colNorm[0];
//    b1->acc[1]+=impulse*colNorm[1];
//    b2->acc[0]-=impulse*colNorm[0];
//    b2->acc[1]-=impulse*colNorm[1];}}


void moveBody(Grid* W, Body* myBody,int bResetMoveAcc){/* linear and angular Acceleration are applied to linear and angular velocity, velocity from the previous tick is divided by 2 to simulate friction before acc is added.*/
  //this ain't no RTS nigah!!
  double accVec[2];
  double oldBodyPos[3]={myBody->BodyPos[0],myBody->BodyPos[1],myBody->BodyPos[2]};
  myBody->vel[0]=myBody->vel[0]*FRICTION;
  myBody->vel[1]=myBody->vel[1]*FRICTION;
  myBody->vel[2]=myBody->vel[2]*FRICTION;
  myBody->vel[0]+=myBody->moveAcc[0];
  myBody->vel[1]+=myBody->moveAcc[1];
  myBody->vel[2]+=myBody->moveAcc[2];
  myBody->vel[0]+=myBody->bumpAcc[0];
  myBody->vel[1]+=myBody->bumpAcc[1];
  myBody->vel[2]+=myBody->bumpAcc[2];
  if(myBody->mass[0]==0){fprintf(stderr,"this is the problem!\n");}
  double newPt[2]={myBody->BodyPos[0]+myBody->vel[0]/myBody->mass[0],myBody->BodyPos[1]+myBody->vel[1]/myBody->mass[0]};
  double CheckPtTR[2]={newPt[0]+myBody->radius,newPt[1]+myBody->radius};
  double CheckPtBL[2]={newPt[0]-myBody->radius,newPt[1]-myBody->radius};
  myBody->BodyPos[2]=wrapAngle(myBody->BodyPos[2]+myBody->vel[2]/myBody->mass[1]);
  if(W->cam.mode==BODYVIEW){
    accVec[0]=myBody->moveAcc[0]*BODY_FORCE_DISPLAY_MULT;
    accVec[1]=myBody->moveAcc[1]*BODY_FORCE_DISPLAY_MULT;
    addToVecLength(accVec,CELLRAD,accVec);}
  myBody->bumpAcc[0]=0;
  myBody->bumpAcc[1]=0;
  myBody->bumpAcc[2]=0;
  if(bResetMoveAcc){
    myBody->moveAcc[0]=0;
    myBody->moveAcc[1]=0;
    myBody->moveAcc[2]=0;}
  if(!insideWorld(CheckPtBL,0)||!insideWorld(CheckPtTR,0)){
    /* if the cell is bumping a wall (the radius of the body would cross the wall) then the cell is not allowed to move)*/
    myBody->vel[0]=0;
    myBody->vel[1]=0;
    EnTransfer(&W->SUN,&myBody->energy,EDGE_HIT_COST);}
  else{
    myBody->BodyPos[0]=newPt[0];
    myBody->BodyPos[1]=newPt[1];}
  for(int i=0;i<myBody->cellSize;i++){
    Cell* moveMe=myBody->cells[i];
    double newLoc[3];
    transformCoords(oldBodyPos,myBody->BodyPos,moveMe->CellPos,newLoc);
    moveCell(W,moveMe,newLoc);}
  if(W->cam.mode==BODYVIEW){
    double bodyPos[2];
    double velVec[2]={myBody->vel[0]*BODY_FORCE_DISPLAY_MULT,myBody->vel[1]*BODY_FORCE_DISPLAY_MULT};
    addToVecLength(velVec,CELLRAD,velVec);
    accVec[0]+=myBody->BodyPos[0];
    accVec[1]+=myBody->BodyPos[1];
    velVec[0]+=myBody->BodyPos[0];
    velVec[1]+=myBody->BodyPos[1];
    toScreenCoords(W,myBody->BodyPos,bodyPos);
    toScreenCoords(W,accVec,accVec);
    toScreenCoords(W,velVec,velVec);
    glColor3f(0,1,0);
    double accVecColor[3]={0,1,0};
    drawLine(bodyPos,accVec,accVecColor);
    double velVecColor[3]={1,0,0};
    drawLine(bodyPos,velVec,velVecColor);}};

void checkBodyPointers(Body* b,const char* file, const char* func, int line){
  for(int i=0;i<b->cellSize;i++){
    if(b->cells[i]->myBody!=b){
      fprintf(stdout,"Body Pointer does not point to the right body! %s %s %i\n",file,func,line);}}} 

void CountStraws(Body* inspectMe){
  inspectMe->NumStraws=0;
  for(int i=0;i<inspectMe->cellSize;i++){
    for(int j=0;j<ABILITIES_PER_BRAIN;j++){
      if(inspectMe->cells[i]->myBrain->abilities[j].typeHistory[0]==EAT01){
        inspectMe->NumStraws+=1; } } } }

void countIO(Body* inspectMe){
  int AbITalley=0;
  int AbOTalley=0;
  int CommItalley=0;
  int CommOtalley=0;
  for(int i=0;i<inspectMe->cellSize;i++){
    Brain* checkMyBrain=inspectMe->cells[i]->myBrain;
    for(int j=0;j<ABILITIES_PER_BRAIN;j++){
      AbilityType checkType=checkMyBrain->abilities[j].typeHistory[0];
      AbITalley+=neededInputs(checkType);
      AbOTalley+=neededOutputs(checkType);
      if(checkType==NOUTPUT&&checkMyBrain->abilities[j].AbilityInts[0]<0){fprintf(stderr,"Body has output that does not go anywhere\n");}
      if(checkType==NINPUT){CommItalley+=1;}
      else if(checkType==NOUTPUT&&checkMyBrain->abilities[j].involvedCell){CommOtalley+=1;}}
    if(AbITalley!=checkMyBrain->IOUsed[0]){fprintf(stderr,"Body has wrong number of Inputs: needed: %i actual:%i\n",AbITalley,checkMyBrain->IOUsed[0]);}
    if(AbOTalley!=checkMyBrain->IOUsed[1]){fprintf(stderr,"Body has wrong number of Outputs: needed: %i actual:%i\n",AbOTalley,checkMyBrain->IOUsed[1]);}
    AbITalley=0;
    AbOTalley=0;}}

