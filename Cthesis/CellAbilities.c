//$INCLUDES
#include "structs.h"
#include "math.h"
#include "Brain.h"
#include "Body.h"
#include "stdlib.h"
#include "MathFuncs.h"
#include "Cell.h"
#include "Ability.h"
#include "CellAbilities.h"
#include <string.h>
#include <math.h>
//<

//$BINDINGS
int neededInputs(AbilityType type){/* indicates for each ability how many input neurons are required*/
  if(type==SEESIZE){return 4;}//BiggerSmaller, energy, distance, relatedness
  if(type==SEE21){return 3;}//distance, energy
  else if(type==EAT){return 1;}//whether eating anything!
  else if(type==EAT01){return 1;}//minimal version!
  else if(type==MOVE02){return 1;}//dot product!
  else if(type==THRUST){return 1;}//dot product!
  else if(type==ROTATE){return 0;}//
  else if(type==BONDSEEK){return 0;}//
  else if(type==PHOTO){return 0;}//
  else if(type==HEALTH){return 1;}//pain
  else if(type==AGE){return 1;}// current age
  else if(type==ENERGY){return 1;}// current Energy 
  //  else if(type==GPS){return 2;}// only location 
  else if(type==ACCELEROMETER){return 1;}// velocity
  else if(type==REPRODUCE){return 0;}//
  else if(type==NONE){return 0;}//
  else if(type==NOUTPUT){return 0;}// only need output
  else if(type==NINPUT){return 1;}// take output from other cell
  else if(type==FINDFOOD){return 1;}// FIND FOOD!
  else if(type==SEERELATION){return 1;}//relatedness
  else if(type==BUD){return 0;}//
  //  else if(type==RAND){return 1;}
  else{
    fprintf(stdout,"error! searching for num neurons with ability that does not exist");
    return -1;}
}
int neededOutputs(AbilityType type){/* indicates for each ability how many output neurons are required*/
  if(type==SEE21){return 2;}//version 2: only control angle
  else if(type==SEESIZE){return 3;}//angle, size, direction
  else if(type==EAT01){return 1;}//angle, magnitude
  else if(type==EAT){return 2;}//angle, magnitude
  else if(type==MOVE02){return 2;}// magnitude, angle
  else if(type==THRUST){return 2;}// magnitude, angle
  else if(type==ROTATE){return 1;}// magnitude
  else if(type==BONDSEEK){return 1;}// want to bond or not? (and how related?)
  else if(type==PHOTO){return 1;}// energy investment/payoff
  else if(type==HEALTH){return 0;}
  else if(type==AGE){return 0;}
  else if(type==ENERGY){return 0;}
  //  else if(type==GPS){return 0;}
  else if(type==ACCELEROMETER){return 0;}
  else if(type==REPRODUCE){return 1;}
  else if(type==NONE){return 0;}
  else if(type==NOUTPUT){return 1;}// input to other cell
  else if(type==NINPUT){return 0;}
  else if(type==FINDFOOD){return 1;}//angle
  else if(type==SEERELATION){return 1;}//angle
  else if(type==BUD){return 1;}//
  //  else if(type==RAND){return 0;}
  else{
    fprintf(stdout,"error! searching for num neurons with ability that does not exist");
    return -1;}
}

//int neededConfig(AbilityType type){/*indicates for each ability how many evolveable doubles are requred*/
//  if(type==SEESIZE){return 0;}//size angle dist
//  else if(type==EAT){return 0;}//angle
//  else if(type==THRUST){return 0;}//angle
//  else if(type==ROTATE){return 0;}
//  else if(type==BONDSEEK){return 0;}
//  else if(type==PHOTO){return 0;}
//  else if(type==HEALTH){return 0;}
//  else if(type==AGE){return 0;}
//  else if(type==ENERGY){return 0;}
//  //  else if(type==GPS){return 0;}
//  else if(type==ACCELEROMETER){return 0;}
//  else if(type==REPRODUCE){return 0;}
//  else if(type==NONE){return 0;}
//  else if(type==NOUTPUT){return 0;}
//  else if(type==NINPUT){return 0;}
//  else if(type==FINDFOOD){return 1;}//dist
//  else if(type==SEERELATION){return 3;}//size angle dist 
//  else if(type==BUD){return 0;}//
//  //  else if(type==RAND){return 0;}
//  else{
//    fprintf(stdout,"searching for num configs with ability that does not exist!, %i",(int)type);return -1;
//  }
//}

int abilityCost(AbilityType type){
  if(type==SEESIZE){return 1;}//size angle dist
  else if(type==SEE21){return 1;}//version 2: only control angle
  else if(type==EAT01){return 1;}//version 2: only control angle
  else if(type==MOVE02){return 1;}//version 2: only control angle
  else if(type==EAT){return 1;}//angle
  else if(type==THRUST){return 1;}//angle
  else if(type==ROTATE){return 1;}
  else if(type==PHOTO){return 1;}
  else if(type==HEALTH){return 1;}// purely observational
  else if(type==AGE){return 1;}// used to store age val!
  else if(type==ENERGY){return 1;}// purely observational
  //  else if(type==GPS){return 1;}// only location 
  else if(type==ACCELEROMETER){return 1;}// only relative heading 
  else if(type==REPRODUCE){return 1;}//only need prob
  else if(type==SEERELATION){return 1;}//size angle dist 
  else if(type==FINDFOOD){return 1;}// no need
  else if(type==BONDSEEK){return 0;}
  else if(type==NONE){return 0;}//obvious
  else if(type==NOUTPUT){return 0;}// no need
  else if(type==NINPUT){return 0;}// no need
  else if(type==BUD){return 1;}//
  //  else if(type==RAND){return 0;}
  else{fprintf(stdout,"trying to write abilityCost for ability that does not exist, %i",(int)type);return -1;
  }
}

void getAbilityString(AbilityType type, char* writeHere){
  if(type==SEESIZE){strcpy(writeHere,"SeeSize");}//size angle dist
  else if(type==SEE21){strcpy(writeHere,"SEE21");}//version 2: only control angle
  else if(type==EAT01){strcpy(writeHere,"EAT01");}//version 2: only control angle
  else if(type==MOVE02){strcpy(writeHere,"MOVE02");}//version 2: only control angle
  else if(type==EAT){strcpy(writeHere,"Eat");}//angle
  else if(type==THRUST){strcpy(writeHere,"Thrust");}//angle
  else if(type==ROTATE){strcpy(writeHere,"Rotate");}
  else if(type==HEALTH){strcpy(writeHere,"Health");}// purely observational
  else if(type==AGE){strcpy(writeHere,"Age");}// used to store age val!
  else if(type==ENERGY){strcpy(writeHere,"Energy");}// purely observational
  //  else if(type==GPS){return 1;}// only location 
  else if(type==ACCELEROMETER){strcpy(writeHere,"Accelerometer");}// only relative heading 
  else if(type==REPRODUCE){strcpy(writeHere,"Reproduce");}//only need prob
  else if(type==SEERELATION){strcpy(writeHere,"SeeRelation");}//size angle dist 
  else if(type==FINDFOOD){strcpy(writeHere,"FindFood");}// no need
  else if(type==BONDSEEK){strcpy(writeHere,"BondSeek");}
  else if(type==NONE){strcpy(writeHere,"None");}//obvious
  else if(type==NOUTPUT){strcpy(writeHere,"NOutPut");}// no need
  else if(type==NINPUT){strcpy(writeHere,"NInput");}// no need
  else if(type==PHOTO){strcpy(writeHere,"Photo");}// no need
  else if(type==BUD){strcpy(writeHere,"Bud");}//
  else{fprintf(stdout,"trying to get ability string that does not exist..., %i",(int)type);}
}

void (*getInputFunc(AbilityType type))(Grid*,Cell*,Ability*){
  if(type==SEESIZE){
    return *WriteCellSeeSize;
  }
  if(type==SEE21){
    return *WriteSEE21;
  }
  if(type==MOVE02){
    return *WriteMOVE02;}
  else if(type==SEERELATION){
    return *WriteCellSeeRelation;
  }
  else if(type==HEALTH){
    return *detectPain;
  }
  else if(type==AGE){
    return *detectAge;
  }
  else if(type==THRUST){
    return *thrustHeading;
  }
  else if(type==ENERGY){
    return *detectCellEn;
  }
  else if(type==ACCELEROMETER){
    return *detectHeading;
  }
  else if(type==NOUTPUT){
    return *sendMessage;
  }
  else if(type==FINDFOOD){
    return *findFood;
  }
  else{
    return NULL;
  }
}
//<

//$WRITING TO CELLS
void WriteMOVE02(Grid* W,Cell* c,Ability* ab){ double bodyVelMag=mag(c->myBody->vel); double dotProd=0;double angleColor[3];

  if(bodyVelMag!=0){
    double abPushAng=((ab->IONeurons[1][1])-0.5)*M_PI*2+c->CellPos[2];
    double velNormVec[2]={c->myBody->vel[0]/bodyVelMag,c->myBody->vel[1]/bodyVelMag};
    double abNormVec[2]={cos(abPushAng),sin(abPushAng)};
    dotProd=DotProduct(abNormVec,velNormVec);
//    printArr(velNormVec,DOUBLE,2,"velNorm");
//    printArr(abNormVec,DOUBLE,2,"abNorm");
//    if(ab->IONeurons[1][0]<0){abNormVec[0]=-abNormVec[0];abNormVec[1]=-abNormVec[1];}
//    fprintf(stdout,"WHAT THE FLYING FUCK IS GOING ON\n");
//    fprintf(stdout,"v1:[%lf,%lf], v2:[%lf,%lf], v1[0]*v2[0]:%lf, v1[1]*v2[1]:%lf, dotProd%lf\n",abNormVec[0],abNormVec[1],velNormVec[0],velNormVec[1],abNormVec[0]*velNormVec[0],abNormVec[1]*velNormVec[1],abNormVec[0]*velNormVec[0]+abNormVec[1]*velNormVec[1]);
//    fprintf(stdout,"%lf\n",abNormVec[0]*velNormVec[0]+abNormVec[1]*velNormVec[1]);
//    fprintf(stdout,"dotProd %lf\n",dotProd);
    dotProd=(DotProduct(abNormVec,velNormVec)+1)*0.5;///bodyVelMag+1)*0.5;
//    fprintf(stdout,"dotProdMod %lf\n",dotProd);
  getHotColdColor(angleColor,dotProd,0,1);
  addColor(W,c->color,angleColor[0],angleColor[1],angleColor[2],BODYVIEW);}
  writeNeuron(c,ab->IONeurons[0][0],dotProd);}

void WriteCellSeeSize(Grid* W, Cell* c, Ability* ab){/* using the input from PlanCellSee to construct the distance and size of the view area, the cell recieves information about how many cells it sees, and its relatedness to the closest individual*/
  double relation=0;
  double seePt[2];
  if(ENERGY_CHOICE==BLOOD){
    Terrain* terrVal=getTerrain(W,seePt);
    if(terrVal)
      if(terrVal->energy>0){
        relation=sigmoidify(terrVal->energy,MAX_ENERGY,0.5,0,1);
      }
      else{
        relation=0.5;
      }
    else{
      relation=0;
    }
  }
  /* to get see area, a point is drawn neuronOutput1*SEE_MAX_DIST away from the cell, and a circle of size neuronOutput2*SEE_MAX_RAD is drawn around that point.*/
  /* the direction in which the point is cast is determined by a double stored in the ability and evolved over time the double is between -0.5 and 0.5 and is multiplied by 2*PI, the direction is relative to the Cell's current heading*/
  if(ab->AbilityDoubs[0]>=0){
    double seeDist=(ab->AbilityDoubs[0])*SEE_MAX_DIST;
    double seeRad=SEE_MAX_RAD*ab->AbilityDoubs[2];
    getCellPoint(c,seePt,seeDist,(ab->AbilityDoubs[1]-0.5)*M_PI*2);
    LL* nearby=withinRad(W,seePt,seeRad,c,0);
    int numSeen=0;
    for(LL* currLL=nearby;currLL!=NULL;currLL=currLL->next){
      numSeen+=1;
    }
    Cell* getSize=getClosestCell(c->CellPos,nearby);
    freeLL(nearby);
    writeNeuron(c,ab->IONeurons[0][0],0);
    writeNeuron(c,ab->IONeurons[0][1],0);
    writeNeuron(c,ab->IONeurons[0][2],0);
    writeNeuron(c,ab->IONeurons[0][3],0);
    if(getSize){
      double sizeDiff=c->myBody->cellSize-getSize->myBody->cellSize;
      double seeVal=sigmoidify(sizeDiff,1,0,0,1);
      if(ENERGY_CHOICE!=BLOOD){
        relation=CellRelation(c,getSize);
      }
      //fprintf(stdout,"%f %li %li\n",seeVal,c->myBody->cellSize,getSize->myBody->cellSize);
      //addColor(W,c->color,1-seeVal,seeVal,0,SEEVIEW);
      //double seeValBig=(seeVal-0.5)+0.5;
      //double seeValSmall=(seeVal-0.5)+0.5;
      //   if(seeVal>=0.49){ writeNeuron(c,ab->IONeurons[0][0],seeValBig); 
      //   addColor(W,c->color,0,seeValBig,0,SEEVIEW);
      //   }
      //   if(seeVal<=0.51){ writeNeuron(c,ab->IONeurons[0][1],seeValSmall); 
      //   addColor(W,c->color,seeValSmall,0,0,SEEVIEW);
      //   }
      double seeTest=getDist(getSize->CellPos,c->CellPos)/(fabs(seeDist)+seeRad+CELLRAD);
      if(seeTest>1||seeTest<0){
        fprintf(stdout,"%f %f %f %f %f %f\n",seeTest,getDist(getSize->CellPos,c->CellPos),(seeDist+seeRad+CELLRAD),ab->AbilityDoubs[0],ab->AbilityDoubs[1],ab->AbilityDoubs[2]);
      }
      writeNeuron(c,ab->IONeurons[0][0],sigmoidify(numSeen,NUMSEEN_SCALE,0,-1,1));
      writeNeuron(c,ab->IONeurons[0][1],getDist(getSize->CellPos,c->CellPos)/(seeDist+seeRad+CELLRAD));
      writeNeuron(c,ab->IONeurons[0][2],sigmoidify(labs(getSize->myBody->energy),MAX_ENERGY,0,-1,1));
      writeNeuron(c,ab->IONeurons[0][3],relation);
      /* upper half of a sigmoid function, stretched by a factor of 3 in x, and a factor of 2 in y, is applied to the number of cells seen to keep the input value between 0 and 1*/
    }
    //writeNeuron(c,ab->IONeurons[0][0],0.5);
    //writeNeuron(W,c,ab->IONeurons[0][1],0);
    /* if no cells are seen, then both input neurons are written to zero */
    if(W->cam.mode==SEEVIEW&&insideCamera(W,c->CellPos)){
      double cellScreenPt[2];
      double seeScreenPt[2];
      double screenRad[2];
      toScreenCoords(W,c->CellPos,cellScreenPt);
      toScreenCoords(W,seePt,seeScreenPt);
      toScreenDist(W,seeRad,screenRad);
      double seeCircleColor[4]={0.2,0.2,0.2,1};
      drawCircle(seeScreenPt,screenRad,seeCircleColor);
      double seeLineColor[3]={0,1,0};
      drawLine(cellScreenPt,seeScreenPt,seeLineColor);
    }
  }
}

void WriteSEE21(Grid* W, Cell* c, Ability* ab){/* using the input from PlanCellSee to construct the distance and size of the view area, the cell recieves information about how many cells it sees, and its relatedness to the closest individual*/
  double seePt[2]; double enemySpines=0; double numSeenSig; double seeTest;int numSeen=0; double enemyHP=0;
  if(ab->AbilityDoubs[0]>=0){
    //double seeDist=SEE_MAX_DIST;
    //double seeRad=SEE_MAX_RAD;
    getCellPoint(c,seePt,SEE_MAX_DIST*ab->AbilityDoubs[1]+SEE_MAX_RAD,(ab->AbilityDoubs[0]-0.5)*M_PI*2);
    LL* nearby=withinRad(W,seePt,SEE_MAX_RAD,c,0);
    for(LL* currLL=nearby;currLL!=NULL;currLL=currLL->next){
      if(((Cell*)currLL->val)->myBody!=c->myBody){ numSeen+=1; }}
    Cell* cSeen=getClosestCell(c->CellPos,nearby);
    freeLL(nearby);
    writeNeuron(c,ab->IONeurons[0][0],0);
    writeNeuron(c,ab->IONeurons[0][1],0);
    writeNeuron(c,ab->IONeurons[0][2],0);
      //double healthDelta=sigmoidify(c->myBody->beforeEating-c->myBody->lastBeforeEating,CELL_EAT_PAYOFF,0,0,1);
    if(cSeen){
      Body* cSeenBody=cSeen->myBody;
      //relatedness=CellRelation(c,getSize);
      //double sizeDiff=c->myBody->cellSize-getSize->myBody->cellSize;
      //double seeVal=sigmoidify(sizeDiff,1,0,0,1);
      //fprintf(stdout,"%f %li %li\n",seeVal,c->myBody->cellSize,getSize->myBody->cellSize);
      //addColor(W,c->color,1-seeVal,seeVal,0,SEEVIEW);
      //double seeValBig=(seeVal-0.5)+0.5;
      //double seeValSmall=(seeVal-0.5)+0.5;
      //   if(seeVal>=0.49){ writeNeuron(c,ab->IONeurons[0][0],seeValBig); 
      //   addColor(W,c->color,0,seeValBig,0,SEEVIEW);
      //   }
      //   if(seeVal<=0.51){ writeNeuron(c,ab->IONeurons[0][1],seeValSmall); 
      //   addColor(W,c->color,seeValSmall,0,0,SEEVIEW);
      //   }
      double otherDist=getDist(cSeen->CellPos,c->CellPos);
      double MAXseeRange=SEE_MAX_DIST+SEE_MAX_RAD*2;
      seeTest=1-((otherDist-CELLRAD>0?otherDist-CELLRAD:0)/(MAXseeRange));//from 0 to 1 when something is seen!
      //if(seeTest>1||seeTest<0){
      //  fprintf(stdout,"%f %f %f %f %f %f\n",seeTest,getDist(getSize->CellPos,c->CellPos),(seeDist+seeRad+CELLRAD),ab->AbilityDoubs[0],ab->AbilityDoubs[1],ab->AbilityDoubs[2]);}
      //numSeenSig=sigmoidify(numSeen,NUMSEEN_SCALE,0,-1,1);
      writeNeuron(c,ab->IONeurons[0][2],seeTest);
      //writeNeuron(c,ab->IONeurons[0][0],1.0-getDist(getSize->CellPos,c->CellPos)/(seeDist+seeRad+CELLRAD));
      //writeNeuron(c,ab->IONeurons[0][1],sigmoidify(labs(getSize->myBody->energy),MAX_ENERGY,0,-1,1));
      
//      int spineSigmoidScale=ABILITIES_PER_BRAIN==1?4:ABILITIES_PER_BRAIN*2;
//      int* nStraws=&cSeenBody->NumStraws;
//      if(*nStraws==0){
        //count straws if not done yet!
//        CountStraws(cSeen->myBody);
//        if(*nStraws==0){*nStraws=-1;}}
//      if(*nStraws==-1){writeNeuron(c,ab->IONeurons[0][0],0);}
//      else{writeNeuron(c,ab->IONeurons[0][0],sigmoidify(*nStraws,spineSigmoidScale,0,-1,1));}
//      //tell cells numstraws of body
//      addColor(W,c->color,*nStraws,*nStraws,0,SEEVIEW);

      double normOtherDir[2]={(cSeen->CellPos[0]-c->CellPos[0])/otherDist,(cSeen->CellPos[1]-c->CellPos[1])/otherDist};
      double bodyVelMag=mag(c->myBody->vel);
      if(bodyVelMag>0.000001){
        double normBodyVelDir[2]={c->myBody->vel[0]/bodyVelMag,c->myBody->vel[1]/bodyVelMag};
        double dotProdDir=DotProduct(normOtherDir,normBodyVelDir);
        writeNeuron(c,ab->IONeurons[0][1],(dotProdDir+1)*0.5);
        double enemyHP=sigmoidify(cSeen->myBody->energy,MAX_ENERGY/2,0,-1,1);
        enemyHP=enemyHP>0?enemyHP:0;
        writeNeuron(c,ab->IONeurons[0][0],enemyHP);
        addColor(W,c->color,0,enemyHP,0,SEEVIEW);
        //addColor(W,c->color,(dotProdDir+1)*0.5,0,0,SEEVIEW);}
      //addColor(W,c->color,0,0,relatedness,SEEVIEW);//blue based on relation
      //writeNeuron(c,ab->IONeurons[0][2],enemyHP>0?enemyHP:0);//approx size and power combined with spines
    }
  }
    //writeNeuron(c,ab->IONeurons[0][2],healthDelta);
    if(W->cam.mode==SEEVIEW){
      //double drawHealth=healthDelta>0?healthDelta*2:-healthDelta*2;
    //addColor(W,c->color,0,drawHealth,0,SEEVIEW);//green based on healthDelta
//      addColor(W,c->color,0,seeTest,0,SEEVIEW);//green based on distance
//      addColor(W,c->color,0,0,enemyHP,SEEVIEW);//blue based on relatedness
      double cellScreenPt[2];
      double seeScreenPt[2];
      double screenRad[2];
      toScreenCoords(W,c->CellPos,cellScreenPt);
      toScreenCoords(W,seePt,seeScreenPt);
      toScreenDist(W,SEE_MAX_RAD,screenRad);
      double seeCircleColor[4]={0.2,0.2,0.2,0.5};
      drawCircle(seeScreenPt,screenRad,seeCircleColor);
      double seeLineColor[3]={0.1,0.1,0.1};
      drawLine(cellScreenPt,seeScreenPt,seeLineColor);}
    }
}

void thrustHeading(Grid* W,Cell* c,Ability* ab){
  double writeVal=sigmoidify(DotProduct(ab->AbilityDoubs,c->myBody->vel),0.1,0.5,0,1);
  if(writeVal==writeVal){
    writeNeuron(c,ab->IONeurons[0][0],writeVal);
  }
  //  if(ab->AbilityDoubs[0]&&ab->AbilityDoubs[1]&&c->myBody->vel[0]&&c->myBody->vel[1]){
  //  }
  else{
    writeNeuron(c,ab->IONeurons[0][0],0);
  }
}
void WriteCellSeeRelation(Grid* W, Cell* c, Ability* ab){
  double seePt[2];
  getCellPoint(c,seePt,ab->AbilityDoubs[0]*SEE_MAX_DIST,(ab->AbilityDoubs[1]-0.5)*M_PI*2);
  double seeRad=SEE_MAX_RAD*ab->AbilityDoubs[1];
  LL* nearby=withinRad(W,seePt,seeRad,c,0);
  Cell* getRelation=getClosestCell(c->CellPos,nearby);
  freeLL(nearby);
  if(getRelation){
    double relation=CellRelation(c,getRelation);
    double neuronVal=(1-relation)*0.7+0.3;
    writeNeuron(c,ab->IONeurons[0][0],neuronVal);
    addColor(W,c->color,neuronVal*0.7,0,neuronVal*0.7,RELATIONVIEW);
  }
  else{
    writeNeuron(c,ab->IONeurons[0][0],0);
  }
  if(W->cam.mode==RELATIONVIEW&&insideCamera(W,c->CellPos)){
    double cellScreenPt[2];
    double seeScreenPt[2];
    double screenRad[2];
    toScreenCoords(W,c->CellPos,cellScreenPt);
    toScreenCoords(W,seePt,seeScreenPt);
    toScreenDist(W,seeRad,screenRad);
    double relationCircileColor[4]={0.05,0.1,0.05,0.1};
    drawCircle(seeScreenPt,screenRad,relationCircileColor);
    double relationLineColor[3]={0.1,0.2,0.1};
    drawLine(cellScreenPt,seeScreenPt,relationLineColor);
  }
}
void detectPain(Grid* W,Cell* c,Ability* ab){/* the change in the last tick of the Cell's health (multiplied by a sigmoid function scaled by DELTA_HEALTH_SCALE) is written as NeuronInput1 during the writeOutputs phase */
  double painVal=sigmoidify(c->myBody->EaterEn,400,0,-1,1);
  //  if(painVal!=0){
  //    fprintf(stdout,"%i %f\n",c->EaterEn,painVal);
  //  }
  //  if(healthVal<0.5){
  //    addColor(W,c->color,(0.5-healthVal)*2,0,0,HEALTHVIEW);
  //  }
  //  else{
  addColor(W,c->color,painVal,0,painVal,HEALTHVIEW);
  //  }
  writeNeuron(c,ab->IONeurons[0][0],painVal);
}
void detectAge(Grid* W, Cell* c, Ability* ab){/* the age of the cell in ticks, (multiplied by the upper half a sigmoid function scaled by AGE_SCALE) is written to the cell during the writeOutputs phase.*/
  ab->AbilityInts[0]+=1;
  double age=sigmoidify(ab->AbilityInts[0],AGE_SCALE,0,-1,1);
  writeNeuron(c,ab->IONeurons[0][0],age);
  addColor(W,c->color,0,1-age,age,AGEVIEW);
}
void detectHeading(Grid* W,Cell* c,Ability* ab){/* the angle of the cell's current direction of movement, relative to the direction it is facing is written as neuronInput1. the upper half of a sigmoid function scaled by 0.4 in the x direction and 2 in the y direction applied to the magnitude of the cell's movement are written to the cell.*/
  //  double heading=wrapAngle(atan2(c->myBody->vel[1],c->myBody->vel[0])-c->CellPos[2])/(2*M_PI);
  double mag=sigmoidify(sqrt(c->myBody->vel[0]*c->myBody->vel[0]+c->myBody->vel[1]*c->myBody->vel[1])/CELLRAD,VELOCITY_SCALE,0,-1,1);
  //  writeNeuron(W,c,ab->IONeurons[0][0],heading);
  writeNeuron(c,ab->IONeurons[0][0],mag);
  addColor(W,c->color,0,0,mag,HEADINGVIEW);
}

void detectCellEn(Grid* W, Cell* c, Ability* ab){/* the cell's current energy with a sigmoid function applied to it (scaled by CELL_START_ENERGY in the x direction, and 2 in the y direction) are written as Neuroninput1*/
  //double enVal=sigmoidify(c->myBody->energy,ENERGY_SCALE,0,-1,1);
  if(c->myBody->energy>0){
    double enVal=c->myBody->energy*1.0/(MAX_ENERGY*c->myBody->cellSize);
    enVal=enVal<=1?enVal:1;
    writeNeuron(c,ab->IONeurons[0][0],enVal);
    addColor(W,c->color,0,enVal,enVal,ENERGYVIEW);
  }
}
void sendMessage(Grid* W,Cell* c,Ability* ab){/* called by NOUTPUT ability during writeOutputs. if cell has no recipients then skip, otherwise send recorded output value to recipient cell NINPUT ability */
  if(!ab->involvedCell||ab->AbilityInts[0]<0){
    fprintf(stderr,"Unassigned Output should not exist\n");
  }
  else{
    Cell* sendToMe=ab->involvedCell;
    if(c->myBody!=sendToMe->myBody){
      fprintf(stdout,"BondMessage Sent to Cells with different Body Indices\n");//: ToBody %i, fromBody %i, sentToMe %i, sentFromMe %i\n",sendToMe->BodyIndex,c->BodyIndex,sendToMe->index,c->index);
    }
    Ability* sendAb=&sendToMe->myBrain->abilities[ab->AbilityInts[0]];
    if(sendAb->typeHistory[0]!=NINPUT){
      fprintf(stdout,"recipient cell does not have the NINPUT!\n");
    }
    writeNeuron(sendToMe,sendAb->IONeurons[0][0],ab->AbilityDoubs[0]);
    //writeNeuron(sendToMe,sendAb->IONeurons[0][0],getRandDouble(0,1));
  }
}
void findFood(Grid* W, Cell* c, Ability* ab){/* using the input from PlanCellSee to construct the distance and size of the view area, the cell recieves information about how many cells it sees, and its relatedness to the closest individual*/
  double seePt[2];
  getCellPoint(c,seePt,ab->AbilityDoubs[0]*FIND_FOOD_MAX_DIST,(readNeuron(c,ab->IONeurons[1][0])-0.5)*M_PI*2);
  //  double theGrassIs=getSquare(W,c->CellPos)->beforeEating;
  Square* theOtherSide=getSquare(W,seePt);
  if(theOtherSide){
    double greenerOnTheOtherSide=getTerrain(W,seePt)->beforeEating;
    //  double neuronVal=sigmoidify(greenerOnTheOtherSide-theGrassIs,CELL_START_ENERGY,0,0,1);
    double neuronVal=sigmoidify(greenerOnTheOtherSide,CELL_START_ENERGY,0,0,1);
    if(W->cam.mode==FOODFINDVIEW){
      double cellScreenPt[2];
      double seeScreenPt[2];
      toScreenCoords(W,c->CellPos,cellScreenPt);
      toScreenCoords(W,seePt,seeScreenPt);
      double findFoodLineColor[3]={0.1,0.2,0.1};
      drawLine(cellScreenPt,seeScreenPt,findFoodLineColor);
    }
    writeNeuron(c,ab->IONeurons[0][0],neuronVal);
    if(neuronVal>0.5){
      addColor(W,c->color,0,(neuronVal-0.5)*2,0,FOODFINDVIEW);
    }
    else{
      addColor(W,c->color,0,0,neuronVal*2,FOODFINDVIEW);
    }
  }
  else{
    writeNeuron(c,ab->IONeurons[0][0],0);
    addColor(W,c->color,1,0,0,FOODFINDVIEW);
  }
}

//void checkUsedNeurons(Brain* myBrain,const char* function,int line){
//  int usedCount=0;
//  for(int i=0;i<NUMNEURONS;i++){
//    if(myBrain->neuronTypes[i]!=NONE){
//      usedCount++;
//    }
//  }
//  if(myBrain->neuronsUsed!=usedCount){
//    fprintf(stderr,"here's the problem func:%s line:%i",function,line);
//    exit(0);
//  }
//}
//<

//$GETTING CELL OUTPUT TO WORLD!
void (*getOutputFunc(AbilityType type))(Grid*,Cell*,Ability*){
  if(type==SEESIZE){
    return *PlanCellSee;
  }
  else if(type==SEE21){
    return *PlanSEE21;
  }
  else if(type==SEERELATION){
    return *PlanCellRel;
  }
//  else if(type==EAT){
//    return *PlanEat;
//  }
  else if(type==EAT01){
    return *PlanEAT01;
  }
  else if(type==THRUST){
    return *CellThrust;
  }
  else if(type==MOVE02){
    return *CellThrust;
  }
  else if(type==ROTATE){
    return *PlanCellRot;
  }
  else if(type==BONDSEEK){
    return *PlanCellBond;
  }
  else if(type==FINDFOOD){
    return *findFood;
  }
//  else if(type==REPRODUCE){
//    return *countRep;
//  }
//  else if(type==BUD){
//    return *countBud;
//  }
  else if(type==NOUTPUT){
    return *getMessage;
  }
  else if(type==PHOTO){
    return *PlanPhotosynth;
  }
  else{
    return NULL;
  }
}
void PlanSEE21(Grid* W,Cell* c,Ability* ab){
  EnTransfer(&W->SUN,&c->myBody->energy,CELL_SEE_COST);
  //store view angle
  ab->AbilityDoubs[0]=readNeuron(c,ab->IONeurons[1][0]);
  ab->AbilityDoubs[1]=readNeuron(c,ab->IONeurons[1][1]);
}
void PlanCellSee(Grid* W,Cell* c,Ability* ab){/* Cell see inputs are recorded during GridPlan, actual sight activity happens during the writeOutputs phase*/
  ab->AbilityDoubs[0]=readNeuron(c,ab->IONeurons[1][0]);
  ab->AbilityDoubs[1]=readNeuron(c,ab->IONeurons[1][1]);
}
void PlanCellRel(Grid* W,Cell* c,Ability* ab){/* Cell see inputs are recorded during GridPlan, actual sight activity happens during the writeOutputs phase*/
  ab->AbilityDoubs[1]=readNeuron(c,ab->IONeurons[1][0]);
  //  ab->AbilityDoubs[2]=readNeuron(W,c,ab->IONeurons[1][1]);
}

//void PlanEat(Grid* W,Cell* c, Ability* ab){/*Cell input*CELL_EAT_MULT energy is taken from the cell during the GridPlan phase. whether there is a cell to eat is also determined*/
//  double eatPower=readNeuron(c,ab->IONeurons[1][0]);
//  EnTransfer(&W->SUN,&c->myBody->energy,CELL_EAT_MULT*eatPower);
//  double eatPt[2];
//  getCellPoint(c,eatPt,CELL_EAT_DISTANCE/2,(readNeuron(c,ab->IONeurons[1][1])-0.5)*M_PI*2);
//  LL* edible=withinRad(W,eatPt,0,c,0);
//  Cell* eatMe=getClosestCell(c->CellPos,edible);
//  freeLL(edible);
//  if(!eatMe){
//    getCellPoint(c,eatPt,CELL_EAT_DISTANCE,(readNeuron(c,ab->IONeurons[1][1])-0.5)*M_PI*2);
//    edible=withinRad(W,eatPt,0,c,0);
//    eatMe=getClosestCell(c->CellPos,edible);
//    freeLL(edible);
//  }
//  double writeVal=0;
//
//  if(eatMe){
//    Body* eatBody=eatMe->myBody;
//    ab->involvedCell=eatMe;
//    double foodVal=eatBody->beforeEating>0?eatBody->beforeEating:0;
//    writeVal=0.3+sigmoidify(foodVal,MAX_ENERGY/8,0,-0.7,0.7);
//    int eatAmount=CELL_EAT_MULT*eatPower*CELL_EAT_YEILD;
//    /* the cell that is being eaten records the eatAmount, calculated by CELL_EAT_MULT*neuronOutput1*CELL_EAT_YEILD and adds it to any other eatAmounts calculated prior for use during ExecEat in the GridAct phase*/
//    eatBody->EaterEn+=eatAmount;
//    //    eatMe->numEaters+=1;
//    ab->AbilityInts[0]=eatAmount;
//  }
//
//  writeNeuron(c,ab->IONeurons[0][0],writeVal);
//  addColor(W,c->color,EATVIEW,0,0,writeVal);
//  if(W->cam.mode==EATVIEW&&insideCamera(W,c->CellPos)){
//    double cellScreenPt[2];
//    double seeScreenPt[2];
//    toScreenCoords(W,c->CellPos,cellScreenPt);
//    toScreenCoords(W,eatPt,seeScreenPt);
//    //toScreenDist(W,ab->config[1]*EAT_MAX_RAD,screenRad);
//    //glColor3f(1,0,0);
//    //drawCircle(W,seeScreenPt,screenRad);
//    double eatLineColor[3]={eatPower,0,0};
//    drawLine(cellScreenPt,seeScreenPt,eatLineColor);
//  } 
//}

void PlanEAT01(Grid* W,Cell* c,Ability* ab){
    writeNeuron(c,ab->IONeurons[0][0],0);
  double eatPt[2];
  getCellPoint(c,eatPt,CELL_EAT_DISTANCE/2,(readNeuron(c,ab->IONeurons[1][0])-0.5)*M_PI*2);
  EnTransfer(&W->SUN,&c->myBody->energy,CELL_EAT_COST);
  LL* edible=withinRad(W,eatPt,0,c,0);
  Cell* eatMe=getClosestCell(c->CellPos,edible);
  freeLL(edible);
  if(!eatMe){
    getCellPoint(c,eatPt,CELL_EAT_DISTANCE,(readNeuron(c,ab->IONeurons[1][0])-0.5)*M_PI*2);
    edible=withinRad(W,eatPt,0,c,0);
    eatMe=getClosestCell(c->CellPos,edible);
    freeLL(edible);
  }
  double writeVal=0;
  if(eatMe){
    Body* eatBody=eatMe->myBody;
    ab->involvedCell=eatMe;
    double foodVal=eatBody->beforeEating>0?eatBody->beforeEating:0;
    int eatAmount;
    if(EAT_PAYOFF_DROP_PROP==0||c->myBody->cellSize>=eatBody->cellSize){
      eatAmount=CELL_EAT_PAYOFF;
    }
    else{
      eatAmount=CELL_EAT_PAYOFF*((1.0-EAT_PAYOFF_DROP_PROP)+(EAT_PAYOFF_DROP_PROP*(c->myBody->cellSize/eatBody->cellSize)));
    }
    writeNeuron(c,ab->IONeurons[0][0],(eatAmount*1.0)/(CELL_EAT_PAYOFF*1.0));
    eatBody->EaterEn+=eatAmount;
    ab->AbilityInts[0]=eatAmount;
  }
  if(W->cam.mode==EATVIEW&&insideCamera(W,c->CellPos)){
    double cellScreenPt[2];
    double seeScreenPt[2];
    toScreenCoords(W,c->CellPos,cellScreenPt);
    toScreenCoords(W,eatPt,seeScreenPt);
    double eatLineColor[3]={1,0,0};
    drawLine(cellScreenPt,seeScreenPt,eatLineColor);
  }
}

void CellThrust(Grid* W,Cell* c,Ability* ab){/* called by THRUST ability, apply thrust value to cell using applyForce*/
  //  double magnitude=readNeuron(c,ab->IONeurons[1][0])-0.5;
  double dir=readNeuron(c,ab->IONeurons[1][1])-0.5;
  /* magnitude value is measured from neuron and ranges from -0.5 to 0.5 . magnitude value is multiplied by CELL_THRUST_FORCE_MULT to determine force vector*/
  double thrustMag=readNeuron(c,ab->IONeurons[1][0])-0.5;
  //  double thrustMag=magnitude;//>0?magnitude:magnitude/3;
  double xyComp[]={cos(c->CellPos[2]+((dir)*M_PI*2))*thrustMag*CELL_THRUST_FORCE_MULT,sin(c->CellPos[2]+((dir)*M_PI*2))*thrustMag*CELL_THRUST_FORCE_MULT};

  if(xyComp[0]!=xyComp[0]){
    fprintf(stdout,"%f,%f,%f\n",c->CellPos[2],ab->AbilityDoubs[0],thrustMag);
    fprintf(stdout,"this is it!?\n");
  }
  /* heading of thrust ability is stored as an ability config value and evolved over time*/
  Body* myBody=c->myBody;
  applyForce(W,myBody,c->behindPoint,xyComp,1);
  //EnTransfer(&W->SUN,&myBody->energy,pow(thrustMag,MOVE_EXP)*MOVE_COST_MULT*(1-myBody->cellSize/(MAX_BODY_SIZE+10)));//log(myBody->cellSize+2));//,__func__,__LINE__);
  EnTransfer(&W->SUN,&myBody->energy,CELL_MOVE_COST);
  normalize(xyComp,xyComp);
  ab->AbilityDoubs[0]=xyComp[0];
  ab->AbilityDoubs[1]=xyComp[1];}

void PlanCellRot(Grid* W, Cell* c,Ability* ab){/* neuron value between -0.5 and 0.5 is mulitplied by CELL_ROT_SPEED_MULT and will influence the cell's rotation during CellMove*/
  double magnitude=readNeuron(c,ab->IONeurons[1][0])-0.5;//,__func__,__LINE__)-0.5;
  c->rotAmount+=magnitude*CELL_ROT_SPEED_MULT;
  EnTransfer(&W->SUN,&c->myBody->energy,fabs(magnitude)*ROT_COST);
}

double CellRelation(Cell* c,Cell* other){/* measures relatedness of two cells by randomly choosing WEIGHTS_COMPARED weight values from matching locations in the weight matrix and summing the difference between them */
  Body* compareBody=other->myBody;
  if(c->BodyCellI<compareBody->cellSize){
    other=other->myBody->cells[c->BodyCellI];
  }
  double ret=0;
  for(int i=0;i<WEIGHTS_COMPARED;i++){
    int checkMe=getRandInt(0,NUMNEURONS*NUMNEURONS+1);
    double myVal=c->myBrain->weights[checkMe];
    double otherVal=other->myBrain->weights[checkMe];
    ret+=fabs(myVal-otherVal);
  }
  /* final return value is 1/(sum+1)*/
  return 1/(ret+1);
}
void PlanCellBond(Grid* W, Cell* c, Ability* ab){/*called by BONDSEEK during gridPlan phase. if neuron value > randomDouble, cell will attempt to bond with any surrounding cells close enough that are not already part of its body*/
  double Nval=readNeuron(c,ab->IONeurons[1][0]);
  addColor(W,c->color,Nval,Nval,0,BONDVIEW);
  if(c->numBonds<MAX_BONDS&&getRandDouble(0,1)>Nval){
    Body* bonderBody=c->myBody;
    /* if cell is closer than MAX_BOND_DISTANCE and its body is not overlapping any other cells, and is not already part of the same body, then it can be bonded to. If multiple cells fit this, then the closer one is chosen*/
    LL* potentials=withinRad(W,c->CellPos,MAX_BOND_DISTANCE,c,1);
    Cell* bondMe=NULL;
    double bondDist=CELLRAD*1000;
    for(LL* currLL=potentials;currLL;currLL=currLL->next){
      Cell* potential=potentials->val;
      Body* potentialBody=potential->myBody;
      double potentialDist=getDist(c->CellPos,potential->CellPos);
      if(potential->numBonds<MAX_BONDS&&potentialDist>CELLRAD&&potentialDist<bondDist&&c->myBody!=potential->myBody&&bonderBody->cellSize+potentialBody->cellSize<=MAX_BODY_SIZE){
        LL* interFereing=withinRad(W,potential->CellPos,CELLRAD,potential,1);
        double radSum1=bonderBody->radius+potentialBody->radius*2;
        double radSum2=bonderBody->radius*2+potentialBody->radius;
        if(!interFereing&&getWallDist(bonderBody->BodyPos)>radSum1&&getWallDist(potentialBody->BodyPos)>radSum2){
          bondMe=potential;
          bondDist=potentialDist;
        }
        freeLL(interFereing);
      }
    }
    freeLL(potentials);
    if(bondMe){
      combineBodies(W,c,bondMe);
      //        bonderBody->bondBuddyBody=bondMe->myBody;
      //        bondMe->myBody->bondBuddyBody=bonderBody;
      //      }
      //      c->BondBuddy=bondMe;
      //      bondMe->BondBuddy=c;
      //      LL* newBonder=malloc(sizeof(LL));
      //      LL* newBondee=malloc(sizeof(LL));
      //      initLL(newBonder,c);
      //      initLL(newBondee,bondMe);
      //      /* bonder and chosen bondee are added to bonder and bondee lists*/
      //      addLL(&W->Bonders,newBonder);
      //      addLL(&W->Bondees,newBondee);
      //      if(lengthLL(W->Bonders)!=lengthLL(W->Bondees)){
      //        fprintf(stdout,"first sign here!\n");
  }
  }
}
void planFindFood(Grid* W,Cell* c,Ability* ab){
  ab->AbilityDoubs[0]=readNeuron(c,ab->IONeurons[1][0]);
}
//void countRep(Grid* W,Cell* c,Ability* ab){/* if NeuronOutput1 is above 0.5, the cell is counted as intending to divide. the body will be added to the divide list when the number of division intentions is equal to the number of cells in the body*/
//  Body* repBody=c->myBody;
//  addColor(W,c->color,0.2,0,0,REPRVIEW);
//  if(!repBody->reproducing){
//    //  double repVal=readNeuron(c,ab->IONeurons[1][0]);
//    //  if(repVal>getRandDouble(0,1)){
//    //printf("here's another one!");
//    /* if NeuronOutput1 > 0.5, the energy proportion that the cell would like to commit to offspring is also recorded as (NeuronOutput1-0.5)*2. this value can be between 0 and 1. when reproduction happens, the average of these proportions is taken to determine how much energy the cell gives its offpsring.*/
//    //  c->reproducing=1;
//    repBody->repPoints+=1;
//    repBody->repEnTalley+=0.5;//repEnVal;
//    if(repBody->repPoints==repBody->cellSize){
//      repBody->reproducing=1;
//      repBody->repPoints=0;
//      repBody->repEn=repBody->repEnTalley/repBody->cellSize;
//      repBody->repEnTalley=0;
//      W->repMe[W->numRepRequests]=c;
//      W->repTypeOrProb[W->numRepRequests]=-1;
//      W->numRepRequests+=1;
//    }
//    //if(repBody->repPoints>repBody->cellSize){
//    //  fprintf(stdout,"this is a problem\n");
//  }
//  //}
//  //}
//}
//void countBud(Grid* W,Cell* c,Ability* ab){
//  W->repMe[W->numRepRequests]=c;
//  double attachProb=ab->IONeurons[1][0];
//  W->repTypeOrProb[W->numRepRequests]=attachProb;
//  W->numRepRequests+=1;
//  addColor(W,c->color,0,0,(1.0-attachProb)*0.2,REPRVIEW);
//  addColor(W,c->color,0,(attachProb)*0.2,0,REPRVIEW);
//
//}
//basic plan:
//change IO setup to make it simpler, more efficient, and easier to control
//no cell output half on bs. when cell divides, set all reserved for comm abilities that are none to noutput, and hopefully they can find a buddy. also, randomly shut off two noutput/ninput pairs with some small prob at every divide, so that things can shuffle!
//Ability* getInputPair(Ability* outputAb){
//  Cell* myCell=outputAb->involvedCell;
//  Body* myBody=myCell->myBody;
//  if(outputAb->AbilityInts[0]>=0){
//    return &outputAb->involvedCell->myBrain->abilities[outputAb->AbilityInts[0]];
//  }
//  else{
//    return NULL;
//  }
//}

void setupMessages(Body* b,int destroyNum){/* called once for every new Body, destroys connections and then facilitates the construction of new ones*/
  if(b->cellSize==1){
    return;
  }
  int cellIsDestroy[b->cellSize*RESERVED_FOR_COMM];
  int abIsDestroy[b->cellSize*RESERVED_FOR_COMM];
  int cellIsCreate[b->cellSize*RESERVED_FOR_COMM];
  int abIsCreate[b->cellSize*RESERVED_FOR_COMM];
  int IsDestroyLength=0;
  int IsCreateLength=0;
  for(int i=0;i<b->cellSize;i++){
    Brain* checkMyAbs=b->cells[i]->myBrain;
    for(int j=0;j<RESERVED_FOR_COMM;j++){
      if(checkMyAbs->abilities[j].typeHistory[0]==NONE){
        cellIsCreate[IsCreateLength]=i;
        abIsCreate[IsCreateLength]=j;
        IsCreateLength+=1;
      }
      else if(checkMyAbs->abilities[j].typeHistory[0]==NOUTPUT){
        cellIsDestroy[IsDestroyLength]=i;
        abIsDestroy[IsDestroyLength]=j;
        IsDestroyLength+=1;
      }
    }
  }
  for(int i=0;i<destroyNum&&i<IsDestroyLength;i++){
    int destroyI=getRandInt(i,IsDestroyLength);
    int destroyBrainI=getNextRandomizedInt(cellIsDestroy,i,destroyI);
    Brain* fromMyBrain=b->cells[destroyBrainI]->myBrain;
    int destroyAbI=getNextRandomizedInt(abIsDestroy,i,destroyI);
    Ability* destroyMe=&fromMyBrain->abilities[destroyAbI];
    Brain* destroyInputBrain=destroyMe->involvedCell->myBrain;
    int destroyInputAbI=destroyMe->AbilityInts[0];
    Ability* destroyMeInput=&destroyInputBrain->abilities[destroyInputAbI];
    changeAbility(destroyMe,fromMyBrain,NONE,0,destroyAbI);
    changeAbility(destroyMeInput,destroyInputBrain,NONE,0,destroyInputAbI);
    cellIsCreate[IsCreateLength]=destroyBrainI;
    abIsCreate[IsCreateLength]=destroyAbI;
    cellIsCreate[IsCreateLength+1]=destroyInputBrain->myCell->BodyCellI;
    abIsCreate[IsCreateLength+1]=destroyInputAbI;
    IsCreateLength+=2;
  }
  for(int j=0;j<IsCreateLength-1;j+=2){
    int outI=getRandInt(j,IsCreateLength);
    int inI=getRandInt(j+1,IsCreateLength);
    int outCellI=getNextRandomizedInt(cellIsCreate,j,outI);
    int outAbI=getNextRandomizedInt(abIsCreate,j,outI);
    int inCellI=getNextRandomizedInt(cellIsCreate,j+1,inI);
    int inAbI=getNextRandomizedInt(abIsCreate,j+1,inI);
    if(outCellI!=inCellI){
      Brain* outBrain=b->cells[outCellI]->myBrain;
      Ability* outAb=&outBrain->abilities[outAbI];
      Cell* inCell=b->cells[inCellI];
      Brain* inBrain=inCell->myBrain;
      Ability* inAb=&inBrain->abilities[inAbI];
      if(changeAbility(outAb,outBrain,NOUTPUT,0,outAbI)&&changeAbility(inAb,inBrain,NINPUT,0,inAbI)){
        outAb->AbilityInts[0]=inAbI;
        outAb->involvedCell=inCell;
      }
      else{
        changeAbility(outAb,outBrain,NONE,0,outAbI);
        changeAbility(inAb,inBrain,NONE,0,inAbI);
      }
      //      countIO(b);
    }
  }
}

void getMessage(Grid* W, Cell* c, Ability* ab){/*called by NOUTPUT ability during GridPlan. record neuron value for writing later*/
  double newMsg=readNeuron(c,ab->IONeurons[1][0]);
  if(W->cam.mode==CONNVIEW){
    double delta=newMsg-ab->AbilityDoubs[0];
    Cell* sendToMe=ab->involvedCell;
    if(delta>0){
      addColor(W,sendToMe->color,0,delta,0,CONNVIEW);
      addColor(W,c->color,0,0,delta,CONNVIEW);}
    else if(delta<0){
      addColor(W,sendToMe->color,-delta,0,0,CONNVIEW);
      addColor(W,c->color,0,0,-delta,CONNVIEW);}}
  ab->AbilityDoubs[0]=newMsg;
}
void PlanPhotosynth(Grid* W,Cell* c, Ability* ab){/* the cell's energy investment is given by NeuronOutput1*CELL_PHOTO_MULT during the GridPlan phase. this amount is taken from the cell immediately*/
  double photoPower=readNeuron(c,ab->IONeurons[1][0])*CELL_PHOTO_MULT;
  addColor(W,c->color,0,0,photoPower/CELL_PHOTO_MULT,GRASSVIEW);
  ab->AbilityInts[0]=photoPower*CELL_PHOTO_YEILD;
  //AddNeuron(W,c,9,1.0/(getSquare(W,c->CellPos)->energy/10000+10),__func__);
  getTerrain(W,c->CellPos)->PhotoerEn+=ab->AbilityInts[0];
  EnTransfer(&W->SUN,&c->myBody->energy,photoPower);
}


void (*getActFunc(AbilityType type))(Grid*,Cell*,Ability*){
  if(type==EAT){
    return *ExecEat;
  }
  if(type==EAT01){
    return *ExecEat;
  }
  else if(type==PHOTO){
    return *ActPhotosynth;
  }
  return NULL;
}

void ExecEat(Grid* W, Cell* c, Ability* ab){/*energy is taken from the cell being eaten and given to the eaters.*/ 
  double eatVal=ab->AbilityInts[0];
  if(ab->involvedCell!=NULL){
    Body* toEat=ab->involvedCell->myBody;
    if(toEat->beforeEating>0){
      //double eatVal=ab->AbilityInts[0];
      /*depending on whether the cell's beforeEating value is greater than the amount requested from it, the cell will either pay the full amount or will divide up whatever energy it has left amongst the cells eating it and will die*/
      int EatEnergy=0;
      if(toEat->beforeEating>toEat->EaterEn){
        EatEnergy=eatVal;
      }
      else{
        EatEnergy=eatVal*(toEat->beforeEating*1.0/toEat->EaterEn);
        EnTransfer(&W->SUN,&toEat->energy,10000000);
        c->myBody->killCt+=1;
        //kills cells that are being eaten up!
      }
      //      if(EatEnergy==eatVal){
      //        addColor(W,c->color,0,0,0.7,EATVIEW);
      //      }
      //      else{
      //        addColor(W,c->color,0.7,0,0,EATVIEW);
      //      }
      Body* eatenParent=ab->involvedCell->myParent;
      Body* eaterParent=c->myParent;
      //if(eaterParent==eatenParent||eaterParent==toEat||eatenParent==c->myBody){
      //EnTransfer(&W->SUN,&c->myBody->energy,EatEnergy);
      //EnTransfer(&W->SUN,&c->myBody->energy,EatEnergy);
      //}
      //else{
      if(EAT_CHILDREN_PENALTY){
      double relation=CellRelation(c,ab->involvedCell);
        EnTransfer(&c->myBody->energy,&toEat->energy,EatEnergy*(1-relation));
        EnTransfer(&W->SUN,&toEat->energy,EatEnergy*relation);}
        else{EnTransfer(&c->myBody->energy,&toEat->energy,EatEnergy);}
      c->myBody->consumedEn+=EatEnergy;
      //}
    }
    //    toEat->eatenCount+=1;
    //    if(toEat->eatenCount==toEat->numEaters){
    //      //addColor(W,toEat->color,0,0,0.7,EATVIEW);
    //      if(toEat->beforeEating<toEat->EaterEn){
    //        EnTransfer(&W->SUN,&toEat->myBody->energy,300000);
    //      }
    //      toEat->eatenCount=0;
    //      toEat->numEaters=0;
    //      //toEat->EaterEn=0;
    //    }
    if(ENERGY_CHOICE==BLOOD&&W->SUNTURN>0){
      Terrain*gutSpillLoc=getTerrain(W,ab->involvedCell->CellPos);
      EnTransfer(&gutSpillLoc->energy,&W->SUN,W->SUNTURN/10000);
    }
  }
  if(ENERGY_CHOICE==BLOOD){
    Terrain* mySquare=getTerrain(W,c->CellPos);
    if(mySquare->energy>0){
      EnTransfer(&c->myBody->energy,&mySquare->energy,eatVal);
    }
  }
  ab->AbilityInts[0]=0;
  ab->involvedCell=NULL;
}
void ActPhotosynth(Grid* W,Cell* c,Ability* ab){/* the amount of energy the cell can get back from it's investment during PlanPhotosynth is at maximum NeuronOutput1*CELL_PHOTO_MULT*CELL_PHOTO_YEILD*/
  int photoPower=ab->AbilityInts[0];
  Terrain* growFrom=getTerrain(W,c->CellPos);
  int EnergyYeild=0;
  if(growFrom->beforeEating>0){
    /* if the patch the cell is on does not have enough energy for all of the photosynthesis yeilds, the cells are given a proportion of the yeild requested that the patch is currently capable of*/
    EnergyYeild=growFrom->beforeEating>growFrom->PhotoerEn ? photoPower : photoPower*1.0*((growFrom->beforeEating*1.0)/(growFrom->PhotoerEn*1.0));
    EnTransfer(&c->myBody->energy,&growFrom->energy,EnergyYeild);
  }
  if(EnergyYeild<photoPower){
    addColor(W,c->color,1-EnergyYeild*1.0/photoPower,0,0,GRASSVIEW);
  }
}


//WRITING NEW FUNCTIONS HERE

