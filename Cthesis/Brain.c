//$ includes
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "structs.h"
#include "ViewWindow.h"
#include "Ability.h"
#include "CellAbilities.h"
#include "Grid.h"
#include "Camera.h"
#include "Brain.h"
#include <stdio.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include "MathFuncs.h"
//#include "/home/rafael/Dropbox/ClusterFMRI/regression/print_matrix.c"
#include <math.h>
#include <string.h>
//#include <execinfo.h>
//<

//$ mutual info
void ClearMutualInfoDatBins(mutualInfoDat* M){
  for(int iBin=0;iBin<NBINS*NBINS;iBin++){
    M->bins[iBin]=0;}}

void ResetMutualInfoDat(mutualInfoDat* initMe,Brain* myBrain,int* inputs,int* outputs){
  int iNeuron;int iBin;

  for(iNeuron=0;iNeuron<NUMNEURONS;iNeuron++){
    initMe->inputNeurons[iNeuron]=0;
    initMe->outputNeurons[iNeuron]=0;
    initMe->inputBinVals[iNeuron]=-1;
    if(inputs[myBrain->IOTypes[0][iNeuron]]){initMe->inputNeurons[iNeuron]=1;}
    if(outputs[myBrain->IOTypes[1][iNeuron]]){initMe->outputNeurons[iNeuron]=1;}}
  initMe->inputBinVals[NUMNEURONS]=-1;
  for(iBin=0;iBin<NBINS*NBINS;iBin++){
    initMe->bins[iBin]=0;}}


void SetupMutualInfoDat(Body* BO){
  for(int iCell=0;iCell<BO->cellSize;iCell++){
    Brain* B=BO->cells[iCell]->myBrain;
    int acceptedInputs[100]={0};
    int acceptedOutputs[100]={0};
    acceptedInputs[NINPUT]=1;
    acceptedOutputs[EAT01]=1;
    acceptedOutputs[MOVE02]=1;
    acceptedOutputs[SEE21]=1;
    ResetMutualInfoDat(B->CommInputInfo,B,acceptedInputs,acceptedOutputs);
    ClearIntArr(acceptedInputs,100,0);
    ClearIntArr(acceptedOutputs,100,0);
    acceptedInputs[EAT01]=1;
    acceptedInputs[MOVE02]=1;
    acceptedInputs[SEE21]=1;
    acceptedOutputs[NOUTPUT]=1;
    ResetMutualInfoDat(B->CommOutputInfo,B,acceptedInputs,acceptedOutputs);
  }
}

void RecInputs(mutualInfoDat* M,Brain* myBrain){
  double binStep=1.0/NBINS;int iNeuron;int iBinVal=0;Cell* C=myBrain->myCell;

  for(iNeuron=0;iNeuron<NUMNEURONS;iNeuron++){
    if(M->inputNeurons[iNeuron]){
      int binVal=readNeuron(C,iNeuron)/binStep;
      binVal=binVal>=NBINS?NBINS-1:binVal;binVal=binVal<0?0:binVal;
      M->inputBinVals[iBinVal]=binVal;
      iBinVal+=1;}}}

void OpenMutualInfoFile(ExptPlan* P){
  char fullPath[PATH_LEN];
  strcpy(fullPath,P->basePath);
  strcat(fullPath,P->dataFolder);
  strcat(fullPath,P->exptName);
  int error=mkdir(fullPath,0777);//create output directory with full permissions
  if(error){fprintf(stdout,"MutualInfoError: %s\n",strerror(errno));}
  strcat(fullPath,"/mutualInfo.txt");
  P->mutualInfoFile=fopen(fullPath,"w");}

void RecMutualInfo(long tick,double* avgInfoIO,FILE* infoFile){
  fprintf(infoFile,"\n[%li,%lf,%lf,%lf,%lf,%lf,%lf]\n",tick,avgInfoIO[0],avgInfoIO[1],avgInfoIO[2],avgInfoIO[3],avgInfoIO[4],avgInfoIO[5]);}

void RecOutputs(mutualInfoDat* M,Brain* myBrain){
  int iNeuron;int iBinVal;Cell* C=myBrain->myCell;double binStep=1.0/NBINS;

  for(iNeuron=0;iNeuron<NUMNEURONS;iNeuron++){
    if(M->outputNeurons[iNeuron]){
      for(iBinVal=0;M->inputBinVals[iBinVal]!=-1;iBinVal++){
        int outputBin=readNeuron(C,iNeuron)/binStep;
        outputBin=outputBin>=NBINS?NBINS-1:outputBin;outputBin=outputBin<0?0:outputBin;
        M->bins[M->inputBinVals[iBinVal]*NBINS+outputBin]+=1;}}}}

void GetMutualInfo(mutualInfoDat* M,double* infoWeight){
  int iBin;long binSum=0;long xBinSums[NBINS]={0};long yBinSums[NBINS]={0};double binSumD=0;infoWeight[0]=0;infoWeight[1]=0;infoWeight[2]=0;infoWeight[3]=0;int iBinSum;

  for(iBin=0;iBin<NBINS*NBINS;iBin++){
    int x=iBin%NBINS;
    int y=iBin/NBINS;
    long cBinVal=M->bins[iBin];
    xBinSums[x]+=cBinVal;
    yBinSums[y]+=cBinVal;
    binSum+=cBinVal;}
  if(binSum){
  binSumD=binSum*1.0;
  for(iBinSum=0;iBinSum<NBINS;iBinSum++){
    double pX=xBinSums[iBinSum]/binSumD;
    double pY=yBinSums[iBinSum]/binSumD;
    if(pX){infoWeight[2]+=-pX*(log(pX)/log(2));}
    if(pY){infoWeight[3]+=-pY*(log(pY)/log(2));}
    }
  for(iBin=0;iBin<NBINS*NBINS;iBin++){
    int x=iBin%NBINS;
    int y=iBin/NBINS;
    long cBinVal=M->bins[iBin];
    if(cBinVal){
      double cVal=(log((cBinVal/binSumD)/((xBinSums[x]/binSumD)*(yBinSums[y]/binSumD)))/log(2))*(cBinVal/binSumD);
      infoWeight[0]+=cVal;
    }}
  if(infoWeight[0]<-0.00001){fprintf(stdout,"\ninfoWeight value < 0 : %lf\n",infoWeight[0]);}
  infoWeight[1]=binSumD;}}

void AddMutualInfoList(Body* B,LL** infosI,LL** infosO){
  if(B->cellSize>1){
  for(int iCell=0;iCell<B->cellSize;iCell++){
    mutualInfoDat* cM;LL** cLL;
    for(int IO=0;IO<2;IO++){
      if(IO){
        cM=B->cells[iCell]->myBrain->CommInputInfo;
        cLL=infosI;}
      else{cM=B->cells[iCell]->myBrain->CommOutputInfo;
        cLL=infosO;}

      double* newInfo=malloc(4*sizeof(double));
      LL* newInfoLL=malloc(sizeof(LL));
      GetMutualInfo(cM,newInfo);
      initLL(newInfoLL,newInfo);
      addLL(cLL,newInfoLL);
      ClearMutualInfoDatBins(cM);}}}}

void AvgMutualInfo(Grid* W,double* retIOEs){
  double weightSums[2]={0};double cWeight;Body* cB;double* cInfoWeight;retIOEs[0]=0;retIOEs[1]=0;retIOEs[2]=0;retIOEs[3]=0;retIOEs[4]=0;retIOEs[5]=0;LL* cinfoLL;

  for(int iBody=0;iBody<NUMCELLS;iBody++){
    cB=&W->bodies[iBody];
    if(cB->alive){AddMutualInfoList(cB,&W->MutualInfosInputs,&W->MutualInfosOutputs);}}
    //sum weights by iterating over,
    //then iterate over again and divide by weightsum to get cWeight
    for(cinfoLL=W->MutualInfosInputs;cinfoLL!=NULL;cinfoLL=cinfoLL->next){weightSums[0]+=((double*)cinfoLL->val)[1];}
    for(cinfoLL=W->MutualInfosOutputs;cinfoLL!=NULL;cinfoLL=cinfoLL->next){weightSums[1]+=((double*)cinfoLL->val)[1];}
    for(cinfoLL=W->MutualInfosInputs;cinfoLL!=NULL;cinfoLL=cinfoLL->next){
      if(((double*)cinfoLL->val)[1]){
      retIOEs[0]+=((double*)cinfoLL->val)[0]*(((double*)cinfoLL->val)[1]/weightSums[0]);
      retIOEs[2]+=((double*)cinfoLL->val)[2]*(((double*)cinfoLL->val)[1]/weightSums[0]);
      retIOEs[3]+=((double*)cinfoLL->val)[3]*(((double*)cinfoLL->val)[1]/weightSums[0]);}}
    for(cinfoLL=W->MutualInfosOutputs;cinfoLL!=NULL;cinfoLL=cinfoLL->next){
      if(((double*)cinfoLL->val)[1]){
      retIOEs[1]+=((double*)cinfoLL->val)[0]*(((double*)cinfoLL->val)[1]/weightSums[1]);
      retIOEs[4]+=((double*)cinfoLL->val)[2]*(((double*)cinfoLL->val)[1]/weightSums[1]);
      retIOEs[5]+=((double*)cinfoLL->val)[3]*(((double*)cinfoLL->val)[1]/weightSums[1]);}}
    for(cinfoLL=W->MutualInfosInputs;cinfoLL!=NULL;cinfoLL=cinfoLL->next){free(cinfoLL->val);}
    for(cinfoLL=W->MutualInfosOutputs;cinfoLL!=NULL;cinfoLL=cinfoLL->next){free(cinfoLL->val);}
    freeLL(W->MutualInfosInputs);
    freeLL(W->MutualInfosOutputs);
    W->MutualInfosInputs=NULL;
    W->MutualInfosOutputs=NULL;
    for(int iCell=0;iCell<NUMCELLS;iCell++){
      ClearMutualInfoDatBins(W->cells[iCell].myBrain->CommInputInfo);
      ClearMutualInfoDatBins(W->cells[iCell].myBrain->CommOutputInfo);}
}
//<

//$ everything else
double* getNeuron(Cell* c, int index){
  Brain* theBrain=c->myBrain;
  if(theBrain->n12Swap){
    return &theBrain->n2[index];
  }
  return &theBrain->n1[index];
}


void initBrain(Grid* w,Brain* ret,Brain* model, Cell* myCell,Ecosystem* data,int* Is){/* brain is initialized with random values. if a parent brain is given, the returned brain will be a mutated copy of the original*/
  double WEIGHT_RANGE=w->mutationConfigs[myCell->myBody->ImutationConfig].WEIGHT_RANGE;
  ret->myCell=myCell;
  ret->n12Swap=0;
  if(!model){
    ret->IOUsed[0]=0;
    ret->IOUsed[1]=0;
    for(int i=0;i<NUMNEURONS*NUMNEURONS;i++){
      ret->weights[i]=getRandDouble(-WEIGHT_RANGE,WEIGHT_RANGE);//for debugging://ret->weights[i]=0;
      //ret->weights[i]=0;//for debugging://ret->weights[i]=0;
      //weights are forced from -1 to 1
      for(int i=0;i<ABILITIES_PER_BRAIN;i++){
        for(int j=0;j<NEURONS_PER_ABILITY;j++){
          ret->abilities[i].IONeurons[0][j]=-1;
          ret->abilities[i].IONeurons[1][j]=-1;
        }
      }
      for(int i=0;i<NUMNEURONS;i++){
        ret->IOTypes[0][i]=NONE;
        ret->IOTypes[1][i]=NONE;
      }
    }
    for(int i=0;i<ABILITIES_PER_BRAIN;i++){
      initAbility(&ret->abilities[i],NULL,ret);
      ret->usageA[i]=0;
      if(i<RESERVED_FOR_COMM){
        changeAbility(&ret->abilities[i],ret,NONE,0,i);
      }
    }
      if(INIT3ABS0==1){//will have to move stuff around if MYmutationConfig ends up being important
        changeAbility(&ret->abilities[RESERVED_FOR_COMM+2],ret,MOVE02,0,RESERVED_FOR_COMM+2);
        changeAbility(&ret->abilities[RESERVED_FOR_COMM+1],ret,EAT01,0,RESERVED_FOR_COMM+1);
        changeAbility(&ret->abilities[RESERVED_FOR_COMM],ret,SEE21,0,RESERVED_FOR_COMM);
      }
  }
  else{
    MutationConfig* MYmutationConfig=&w->mutationConfigs[model->myCell->myBody->ImutationConfig];
    long parentAge=model->myCell->myBody->age;
    ret->IOUsed[0]=model->IOUsed[0];
    ret->IOUsed[1]=model->IOUsed[1];
    for(int i=0;i<NUMNEURONS;i++){
      ret->IOTypes[0][i]=model->IOTypes[0][i];
      ret->IOTypes[1][i]=model->IOTypes[1][i];
    }
    for(int i=0;i<NUMNEURONS*NUMNEURONS;i++){
      ret->weights[i]=model->weights[i];
//      ret->usageN[i]=model->usageN[i]+parentAge;
    }
    if(w->evolving){
      //for(int i=0;i<MYmutationConfig->numMutations;i++){
      for(int i=0;i<NUMNEURONS*NUMNEURONS;i+=2){
        //fprintf(stdout,"index: %i\n",index);
        double alteration[2];//=(-MYmutationConfig->mutationStrength,MYmutationConfig->mutationStrength);
        RandNormalDist(alteration,0,exp(MYmutationConfig->mutationStrength));
        //fprintf(stdout,"alteration: %lf\n",alteration);
        ret->weights[i]+=alteration[0];
        ret->weights[i+1]+=alteration[1];
        boundValue(&ret->weights[i],-WEIGHT_RANGE,WEIGHT_RANGE);
        boundValue(&ret->weights[i+1],-WEIGHT_RANGE,WEIGHT_RANGE);}}
//      ret->usageN[index]=0;
    for(int i=0;i<ABILITIES_PER_BRAIN;i++){
      initAbility(&ret->abilities[i],&model->abilities[i],ret);
      ret->usageA[i]=model->usageA[i]+parentAge;
    }
  }
  for(int i=0;i<NUMNEURONS;i++){
    *getNeuron(myCell,i)=0.5;
  }
  ret->n12Swap=1;
  for(int i=0;i<NUMNEURONS;i++){
    *getNeuron(myCell,i)=0.5;
  }
  ret->n12Swap=0;
//  myCell->numStraws=0;
//  for(int i=RESERVED_FOR_COMM;i<ABILITIES_PER_BRAIN;i++){
//    if(ret->abilities[i].typeHistory[0]==EAT01){
//      myCell->numStraws+=1;
//    }
//  }
}

double readNeuron(Cell* c, int index){
  //  if(c->index==10){ fprintf(stdout,"Neuron Read: %i\n",index); }
  if(index<0||index>NUMNEURONS){
    fprintf(stderr,"ERROR, attempting to read uninitialized neuron index INDEX:%i",index);
  }
  return *getNeuron(c,index);
};

void writeNeuron(Cell* c, int index, double val){
  if(index<0||index>NUMNEURONS){
    fprintf(stderr,"ERROR, attempting to write uninitialized neuron index INDEX:%i",index);
  }
  if(val<-0.001||val>1.001){
    fprintf(stderr,"ERROR, neuron write value too high or low %f\n",val);
  }
  double currVal=readNeuron(c,index);
  if(NEURON_INERTIA>0){ *getNeuron(c,index)=(currVal*NEURON_INERTIA)+(val*(1-NEURON_INERTIA)); }
  else{ *getNeuron(c,index)=val; }
}

void AddNeuron(Cell* c, int index, double val, const char* func){
  double* neuron=getNeuron(c,index);
  //  if(val>1){val=1;}
  //  if(val<-1){val=-1;}
  if(val>1||val<-1){
    fprintf(stderr,"Neuron input value is too high or low: %lf, %s\n",val,func);
  }
  //  if(c->index==10){ fprintf(stdout,"Neuron Write: %i\n",index); }
  //*neuron+=val*NEURON_SENSITIVITY;
  *neuron=val>0?*neuron+val*(1-*neuron):*neuron-val*(*neuron);
}

void think(Grid* w,Cell* c,Brain* theBrain){/* the brain runs it's think step by multiplying the neuron vector with the weight matrix. the result is the neuron vector for the next time step*/
  //  int brainSize=sizeof(double)*NUMNEURONS;
  gsl_vector_view input=gsl_vector_view_array(getNeuron(c,0),NUMNEURONS);
  int NEURON_MODE=w->mutationConfigs[theBrain->myCell->myBody->ImutationConfig].NEURON_MODE;
  theBrain->n12Swap=theBrain->n12Swap?0:1;
  //theBrain->n12Swap=0;
  gsl_matrix_view weights=gsl_matrix_view_array(theBrain->weights,NUMNEURONS,NUMNEURONS);
  gsl_vector_view output=gsl_vector_view_array(getNeuron(c,0),NUMNEURONS);
  //print_vector(stdout,&input.vector);
  //print_matrix(stdout,&weights.matrix);
  gsl_blas_dgemv(CblasNoTrans,1.0,&weights.matrix,&input.vector,0.0,&output.vector);
  for(int i=0;i<NUMNEURONS;i++){
    if(NEURON_MODE==1){writeNeuron(c,i,sigmoidify(readNeuron(c,i),NEURON_SENSITIVITY,0.5,0,1));}
    else{
      double modded=fmod(fabs(readNeuron(c,i)),2);
      modded=modded<1?modded:2-modded;
      if(modded<0||modded>1){fprintf(stderr,"wtf?!\n");}
      writeNeuron(c,i,modded);}
    
    if(NEURON_INERTIA>0){
    //AT THIS POINT WE ADD THE NEURON_INERTIA
    gsl_blas_dscal(1-NEURON_INERTIA,&output.vector);
    //scale output vec by 1-NEURON_INERTIA
    gsl_blas_daxpy(NEURON_INERTIA,&input.vector,&output.vector);
    //scale input vec by NEURON_INERTIA and add to output
    }



    //*getNeuron(c,i)=1/(1+exp2(-*getNeuron(c,i)));
  }
  //print_vector(stdout,&output.vector);
}

void initBrainMonitor(ViewElem* BrainDisp, Cell* drawMe){
  Brain* drawBrain=drawMe->myBrain;char text[1000];double black[3]={0,0,0};double white[3]={1,1,1};double bltr[4];
  int chartW=8+NUMNEURONS;//6 for ability spaces, 2 for neuron vals, NUMNEURONS for weights
  int chartH=NUMNEURONS;
  setColor(BrainDisp,white);

  for(int i=0;i<NUMNEURONS;i++){//draw input and output abilities
    getBoxCorners(chartW,chartH,0,chartH-(1+i),3,1,bltr);
    ViewElem* currElem=initElem(bltr,BrainDisp,NULL);
    getAbilityString(drawBrain->IOTypes[0][i],text);
    setText(currElem,black,text);}
  for(int i=0;i<NUMNEURONS;i++){
    getBoxCorners(chartW,chartH,4,chartH-(1+i),3,1,bltr);
    ViewElem* currElem=initElem(bltr,BrainDisp,NULL);
    getAbilityString(drawBrain->IOTypes[1][i],text);
    setText(currElem,black,text);}

  for(int y=0;y<NUMNEURONS;y++){//set weight matrix colors
    for(int x=0;x<NUMNEURONS;x++){
      getBoxCorners(chartW,chartH,x+8,chartH-(1+y),1,1,bltr);
      ViewElem* currElem=initElem(bltr,BrainDisp,NULL);
      double color[3]={0,0,0};
      getHotColdColor(color,drawBrain->weights[y*NUMNEURONS+x],-1,1);
      setColor(currElem,color);}}

  for(int i=0;i<NUMNEURONS;i++){//setup elems for neurons
    getBoxCorners(chartW,chartH,7,chartH-(1+i),1,1,bltr);
    initElem(bltr,BrainDisp,NULL);}
  for(int i=0;i<NUMNEURONS;i++){
    getBoxCorners(chartW,chartH,3,chartH-(1+i),1,1,bltr);
    initElem(bltr,BrainDisp,NULL);}}

void updateBrainMonitor(ViewElem* BrainDisp, Cell* drawMe){
  Brain* drawBrain=drawMe->myBrain;
  double* NtoRead=drawBrain->n12Swap?drawBrain->n1:drawBrain->n2;
  double color[3];
  LL* currLL=BrainDisp->children;
  for(int i=NUMNEURONS-1;i>=0;i--){
    getHotColdColor(color,NtoRead[i],0,1);
    setColor(currLL->val,color);
    currLL=currLL->next;}
  NtoRead=drawBrain->n12Swap?drawBrain->n2:drawBrain->n1;
  for(int i=NUMNEURONS-1;i>=0;i--){
    getHotColdColor(color,NtoRead[i],0,1);
    setColor(currLL->val,color);
    currLL=currLL->next;}}

//int main(int argc, char** argv){
//  Brain* testBrain=initBrain(NULL);
//  //Brain* testBrain2=initBrain(testBrain);
//  for(int i=0;i<10;i++){
//  think(testBrain);
//  }
//  return 0;
//};
//<
