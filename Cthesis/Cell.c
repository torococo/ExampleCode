#include "structs.h"
#include "Ability.h"
#include "Camera.h"
#include "Body.h"
#include "Cell.h"
#include "LL.h"
#include "Brain.h" 
#include "math.h"
#include "Grid.h" 
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include <pthread.h>
#include "DataCollection.h"
#include <GL/glx.h>

//need to make abilities as modular as possible, somehow...


void calcBehindPoint(double* ret, double* cellPos){
  ret[0]=-cos(cellPos[2])*BEHIND_POINT_DIST+cellPos[0];
  ret[1]=-sin(cellPos[2])*BEHIND_POINT_DIST+cellPos[1];
}

void moveCell(Grid* W,Cell* moveMe, double* newLoc){/*Cells moved by summing move ability vectors and bumping into other cells (cellBump)*/
  if(insideWorld(newLoc,0)){
  Square* currSq=getSquare(W,moveMe->CellPos);
  Square* nextSq=getSquare(W,newLoc);
  if(!nextSq){
    fprintf(stderr,"cell detected it has moved off the world, this should never happen, the body should catch this first!");

    //fprintf(stderr,"cell trying to move outside of world");
  }
  //addColor(W,moveMe->color,0,.7,0,MOVEMENT);
  if(currSq!=nextSq){
      remLL(&currSq->cells,&moveMe->mySqElem);
      addLL(&nextSq->cells,&moveMe->mySqElem);
  }
  /* cells lose energy based on how far they have moved */
  double moveCost=pow(getDist(moveMe->CellPos,newLoc)/CELLRAD,MOVE_EXP)*MOVE_COST_MULT;
  EnTransfer(&W->SUN,&moveMe->myBody->energy,moveCost*((1-MOVE_COST_DROP_PROP)+(MOVE_COST_DROP_PROP*(1.0/moveMe->myBody->cellSize))));//getTerrain(W,moveMe->CellPos)->moveCost);///log(moveMe->myBody->cellSize+2));//,__func__,__LINE__);
  //EnTransfer(&W->SUN,&moveMe->myBody->energy,pow(getDist(moveMe->CellPos,newLoc)/CELLRAD,MOVE_EXP)*getTerrain(W,moveMe->CellPos)->moveCost/moveMe->myBody->cellSize);//,__func__,__LINE__);
  moveMe->CellPos[0]=newLoc[0];
  moveMe->CellPos[1]=newLoc[1];
  }
  moveMe->CellPos[2]=wrapAngle(newLoc[2]);
  calcBehindPoint(moveMe->behindPoint,moveMe->CellPos);
};

int checkBondedCells(Cell* c){
  if(lengthLL(c->BondedCells)!=c->numBonds){
    fprintf(stdout,"error, number of Bonded Cells does not equal numBonds");
    return 1;
  }
  return 0;
}

void initCell(Grid* W,Cell* ret,Brain* parent,Brain* childBrain,double* pos, Body* parentBody, int BodyCellI,int keepBonds,Body* myParent,Ecosystem* data,int* Is){/*if cell has a parent brain, then abilities are copied, otherwise brain is initialized randomly*/
  ret->alive=1;
  ret->myBrain=childBrain;
  ret->myBody=parentBody;
  ret->myParent=myParent;
  ret->BodyCellI=BodyCellI;
  ret->BondedCells=NULL;
  ret->numBonds=0;
  if(data){
    setupCellFromData(data,ret,Is,parentBody);//setup bonds, cellpos and brain from data
    setupBrainFromData(data,childBrain,ret,Is);}//init brain from data
  else{
  if(parent){
    initBrain(W,childBrain,parent,ret,NULL,NULL);
    Cell* parentCell=parent->myCell;
    if(keepBonds){
      for(LL* ParentBondedIs=parentCell->BondedCells;ParentBondedIs;ParentBondedIs=ParentBondedIs->next){
        LL* newBondILL=malloc(sizeof(LL));
        initLL(newBondILL,parentBody->cells[((Cell*)ParentBondedIs->val)->BodyCellI]);
        addLL(&ret->BondedCells,newBondILL);}
      ret->numBonds=parentCell->numBonds;}}
  else{initBrain(W,childBrain,NULL,ret,NULL,NULL);}
  ret->CellPos[0]=pos[0];
  ret->CellPos[1]=pos[1];
  ret->CellPos[2]=pos[2];
  }
  calcBehindPoint(ret->behindPoint,ret->CellPos);
  ret->rotAmount=0;
  ret->color[0]=0.2;
  ret->color[1]=0.2;
  ret->color[2]=0.2;
  ret->color[3]=1;
  ret->livingCost=calcLivingCost(childBrain);
  initLL(&ret->mySqElem,ret);
  Square* birthPlace=getSquare(W,ret->CellPos);
  if(!birthPlace){fprintf(stderr,"%lf, %lf attempting to place a Cell outside the world!",ret->CellPos[0],ret->CellPos[1]);}
  addLL(&birthPlace->cells,&ret->mySqElem);};

void remCell(Grid* W,Cell* killMe){/*dead cell is removed from Grid and added to list of dead cells*/
  killMe->alive=0;
  LL* myDeadLL=malloc(sizeof(LL));
  initLL(myDeadLL,killMe);
  remLL(&getSquare(W,killMe->CellPos)->cells,&killMe->mySqElem);
  addLL(&W->deadCells,myDeadLL);
  freeLL(killMe->BondedCells);
  killMe->BondedCells=NULL;};

double cellDist(Cell* Cs){
  double xComp=Cs[0].CellPos[0]-Cs[1].CellPos[0];
  double yComp=Cs[0].CellPos[1]-Cs[1].CellPos[1];
  return (xComp*xComp+yComp*yComp)-CELLRAD*2;
  //if ret==0, then cells are perfect distance
  //if ret<0 then cells are too close
  //if ret>0 then cells are too far
}

void SeparateCellsTime(Grid* W,Cell* Cs, Body* Bs){
  //only init push if an answer > 0 comes up!
  //push cells apart until they are fully separated by going back in time, unless your getting closer in which case leave it alone!
  //
  //get dist/time formula
  //take derivative and find 0 point(s)?!
  //simpler soln: simply move apart cells using linear interpolation between 
}

void CellBump(Grid* W,Cell* c){/* cells with overlapping radii get force vectors applied to them to bounce off each other*/
  double usageColor=c->myBody->usageB/(BINSIZE*NUMBINS*0.7);
  //usageColor=0.7<usageColor?0.7:usageColor;
  addColor(W,c->color,usageColor,usageColor,usageColor,EVOLUTIONVIEW);
  LL* touching=withinRad(W,c->CellPos,CELLRAD,c,0);
  if(touching){
    for(LL* curr=touching;curr!=NULL;curr=curr->next){
      //AddNeuron(&c->myBrain->inputs[1],0.5,__func__);
      Cell* touching=curr->val;
      double dist=getDist(c->CellPos,touching->CellPos);
      //double forceMag=(1-(dist/pow((2*CELLRAD),3)))*0.1;//function to bounce cells with differing severity
      double forceMag=(1-(pow(dist/(2.0*CELLRAD),BUMP_FUNCTION_EXPONENT)));
      /*BUMP_FUNCTION_EXPONENT combined with BUMP_FORCE_MULT dictate how strongly bumping cells repel each other*/
      double diff[]={touching->CellPos[0]-c->CellPos[0],touching->CellPos[1]-c->CellPos[1]};
      double touchLoc[]={c->CellPos[0]+diff[0]/2,c->CellPos[1]+diff[1]/2};
      double scale=(forceMag*BUMP_FORCE_MULT)/dist;
      double xyComp[]={-(touching->CellPos[0]-c->CellPos[0])*scale,-(touching->CellPos[1]-c->CellPos[1])*scale};
      /*Force is applied from the touching location to the center of the cell (for single cells exerts no torque)*/
      FORCE_FUN(W,c->myBody,touchLoc,xyComp,0);
      //here we force closest cells to separate instantly! no matter what, by rewinding time until they are fully apart
      //applyImpulse(W,c,touching);
      c->myBody->touching=1;

      //c->dxy[0]-=distx*(forceMag/dist);
      //c->dxy[1]-=disty*(forceMag/dist);
    }
    freeLL(touching);
  }
  else{
    //AddNeuron(&c->myBrain->inputs[1],-0.5,__func__);
  }
}

int hasAbility(Cell* checkMe,AbilityType findMe){
  Ability* abilitiesToCheck=checkMe->myBrain->abilities;
  for(int i=0;i<ABILITIES_PER_BRAIN;i++){
    if(abilitiesToCheck[i].typeHistory[0]==findMe){
      return 1;
    }
  }
  return 0;
}

void CellRandom(Grid* W, Cell* c,Ability* ab){
  double randNum=getRandDouble(0,1);
  writeNeuron(c,ab->IONeurons[0][0],randNum);
  //  addColor(W,c->color,randNum,randNum,randNum,RANDVIEW);
}

//Cell* getBondedCell(Cell* c,int BondIndex){
//  if(c->BondedCellBodyIs[BondIndex]==-1){
//    return NULL;
//  }
//  return c->myBody->cells[c->BondedCellBodyIs[BondIndex]];
//}

void readOutputs(Grid* w, Cell* c){/* each cell's abilities are cycled through, and those abilities that require outputs to function have their read functions called.*/
  for(int i=0;i<ABILITIES_PER_BRAIN;i++){
    Ability* currAb=&c->myBrain->abilities[i];
    if(currAb->OutputFunc!=NULL){
      currAb->OutputFunc(w,c,currAb);
    }
  }
}


void writeInputs(Grid* w, Cell* c){/* each cell's abilities are cycled through, and those abilities that require inputs to function have their write functions called*/
  for(int i=0;i<ABILITIES_PER_BRAIN;i++){
    Ability* currAb=&c->myBrain->abilities[i];
    if(currAb->InputFunc!=NULL){
      currAb->InputFunc(w,c,currAb);
    }
  }
}

void ActAbilities(Grid* w,Cell* c){/*each cell's abilities are cycled through, and those that change the state of the world have their act functions called during this phase.*/
  //addColor(w,c->color,(1-SeeBody->energy/(MAX_ENERGY*1.0)/SeeBody->cellSize)/5,SeeBody->energy/(MAX_ENERGY*1.0)/SeeBody->cellSize,0,ENERGYVIEW);
  for(int i=0;i<ABILITIES_PER_BRAIN;i++){
    Ability* currAb=&c->myBrain->abilities[i];
    if(currAb->ActFunc!=NULL){
      currAb->ActFunc(w,c,currAb);
    }
  }
}

int CellCheckBody(Cell* c,const char* file, const char* func,int line){
  Body* myBody=c->myBody;
  for(int i=0;i<myBody->cellSize;i++){
    if(myBody->cells[i]==c){
      return 0;
    }
  }
  fprintf(stdout,"cell thinks it's in a body that does not have it as a member! %s %s %i\n",file,func,line);
  return 1;
}
