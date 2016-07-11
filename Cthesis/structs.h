//implement learning soon!
//LEARN ABOUT HOPFIELDS, NEURAL NETWORK MEMORY
#ifndef STRUCT_H//$__INCLUDES and DEFINES__
#define STRUCT_H

#include <pthread.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif
//<
//TOP LIST
#define NUMCELLS 5000
#define WORLDSIZE 90
#define PATH_LEN 10000
#define EVOLUTION_METHOD 0//0 is consistent rates, 1 is changing rates
#define D_MUTATION_RATE 0.1//how fast the mutation rate logs change
#define MUTATION_RATE_MIN -10.0//min value mutation rate can take

//MUTUAL INFORMATION
#define NBINS 100
//sparse ratio: 4 cells per square
//<
//
//WISH LIST:
//  SET NEW ABILITY WITH 1 LINE!
//  MAKE EACH MULTICELL BONUS ON A SLIDER OR SOMETHING

//$__GRIDWORLD CONSTANTS__
#define TERRAINSIZE 70
#define SHAPEX WORLDSIZE
#define SHAPEY WORLDSIZE
#define TERRAINX TERRAINSIZE
#define TERRAINY TERRAINSIZE
#define APOCALYPSETIME 0
//<

//$__CELL CONSTANTS__
//#define CELLRAD 0.085
#define CELLRAD 0.045
#define MAX_BODY_SIZE 50
#define DEATHAGE 0
#define MAX_BONDS 6
//<

//$__ENERGY CONSTANTS__
#define SPEEDUP 8
#define SUN_START_ENERGY 0
#define CELL_START_ENERGY 1000000

#define MAX_ENERGY 4000000
#define LIVING_COST 20*SPEEDUP
#define UNIQUE_AB_COST 50*SPEEDUP

#define EDGE_HIT_COST 0

//LARGE BODY BONUSES

#define EAT_PAYOFF_DROP_PROP 0.45
#define MOVE_COST_DROP_PROP 0.0

//ABILITY COST
//#define ABILITY_COST_PER_TURN 10
//#define LIVING_COST_MULT 1
//#define UNIQUE_COUNT_MULT 8
//<

//$__ABILITY ENERGY_
#define CELL_EAT_PAYOFF 40000*SPEEDUP
#define CELL_EAT_COST 100*SPEEDUP
//prev 40

#define CELL_SEE_COST 15*SPEEDUP
//prev 40

#define MOVE_EXP 2 
#define MOVE_COST_MULT 100
#define CELL_MOVE_COST 5*SPEEDUP
//prev 500
//<

//$__BRAIN SIZE__
//
//turn up health
//make evolution buttons
#define NUMNEURONS 10
#define NEURON_SENSITIVITY 0.30
#define ABILITIES_PER_BRAIN 4
#define RESERVED_FOR_COMM 1
// used for computer resource management vv
//#define NUMABILITIES 4
#define NEURONS_PER_ABILITY 3
#define DOUBLES_PER_ABILITY 2
#define INTS_PER_ABILITY 2
//<

//$__FRESH FEATURES_
#define CELL_EAT_DISTANCE CELLRAD*3
#define NEURON_INERTIA 0
#define EAT_CHILDREN_PENALTY 0//not needed with global
#define CHILD_POSITION 1 //1:global 2:local
#define SEE_MAX_RAD_BASE 7//eye size
#define SEE_MAX_DIST_BASE 13//eye dist
#define ECO_HISTORY_LEN 5//max ecosystem history name entries
//<

//$__ABILITY CONSTANTS__
//EAT
#define STRAW_SPEED 2
//Default, CELLRAD*2

//SEE
//#define SEE_MAX_RAD_BASE 10
#define SEE_MAX_RAD SEE_MAX_RAD_BASE*CELLRAD
//#define SEE_MAX_DIST_BASE 25
#define SEE_MAX_DIST SEE_MAX_DIST_BASE*CELLRAD
//#define CELL_LOOK_COST -20
//#define SEE_MAX_RAD 5*CELLRAD
//#define SEE_MAX_DIST 25*CELLRAD
//<

//$__CAMERA CONSTANTS__
#define BINSIZE 1000
#define NUMBINS 1000
//#define MAX_CAM_MODE 6
#define CELL_DEFAULT_COLOR 0.2
#define BODY_FORCE_DISPLAY_MULT 3 
//<

//$__PHYSICS__
//PHYSICS
#define CELL_MASS 1
#define FORCES_TO_ACC 0.5
#define FRICTION 0.5
#define BUMP_FUNCTION_EXPONENT 2
#define BUMP_FORCE_MULT 1.5
#define CELL_THRUST_FORCE_MULT 0.5
#define BEHIND_POINT_DIST CELLRAD*0.25
#define INTERNAL_TORQUE_MULT 0.1//0.005 
#define EXTERNAL_TORQUE_MULT 0.1//0.01
//<

//$__SIGMOIDS__
//SIGMOIDS
#define NUMSEEN_SCALE 3
#define VELOCITY_SCALE 0.4
#define ENERGY_SCALE CELL_START_ENERGY
#define DELTA_HEALTH_SCALE 100
#define AGE_SCALE 100
//<

//$__UNUSED__
//#define CELL_EAT_MULT 50
//#define CELL_EAT_YEILD 400
//ENERGY
#define IMPULSE_STRENGTH 1
#define IMPULSE_MAX 100
#define CELL_PHOTO_MULT 200
#define CELL_PHOTO_YEILD 1.3
//SEE Grid.c for other energy related constants
//#define SUN_START_ENERGY 90000*NUMCELLS
//#define MOVE_COST_MULT 5000
//TOUCH
#define SENSE_TOUCH_COST -50
#define TOUCH_NEURON_CUTOFF 0.49
#define TOUCH_NEURON_SIGNAL 0.5

//SIZES
//usual is 0.045

//BONDING
#define MAX_BOND_DISTANCE 1.10*CELLRAD

//EVOLUTION
//#define RANDOMWEIGHTPOWER 10.0


//CELLTHRUST
//#define CELL_THRUST_COST_MULT 1000

//CELLROT
//#define CELL_ROT_COST_MULT 10
#define CELL_ROT_SPEED_MULT 2
#define CELL_MOMENT_OF_INERTIA 1

//FIND FOOD
#define FIND_FOOD_MAX_DIST 3

//CELLRELATION
#define WEIGHTS_COMPARED 10

#define CELL_EAT_RAD 0

//ROTATION
#define ROT_COST 50
//#define CONFIGS_MUTATED_PER_REP 3
//#define CONFIG_MUTATION_STRENGTH 0.1
//#define SEPARATE_CELLS_ENERGY_CUTOFF 0.1666

//MISC ABILITIES

//INIT VALUES
//for 10000 130
//for 20000 184
//for 30000 225
//for 40000 260
//#define KILL_POINTS 100000

//CELL MAX VALUES

//BRAINS
//lower value = more sensitive


//NOTE: some constants are stored inside grid squares so that they can be altered depending on location.
//<

//$__MODES__
#define FORCE_FUN applyForce //1)forceMode 2)impulseMode
//#define FORCE_FUN applyImpulse

//define printf(fmt,...) (0)//removes prints
//define fprintf(fmt,...) (0)//removes fprints
//<

//$__ECOSYSTEM CONSTANTS__
#define PATH_MAX_LEN 1000
#define ECONAME_MAX_LEN 1000
#define MAX_ECO_NUMBER 11
//<

//CONSTANTS ^^^

//STRUCTURES vvv

//$__ENUMS__
typedef enum {
  NONEVIEW=0,
  DATAVIEW,//shows data being collected
  EATVIEW,//shows eat straws
  SEEVIEW,//shows see circles
  BODYVIEW,//shows movement
  CONNVIEW,//shows message changes
  ABVIEW,//shows which abilities each cell has
  NUM_CAM_MODES,//sets which CamModes are active
  EVOLUTIONVIEW,//shows persistent bodyconfigs
  RELATIONVIEW,
  FOODFINDVIEW,
  HEALTHVIEW,
  GRASSVIEW,
  REPRVIEW,
  //GPSVIEW=9,
  BONDVIEW,
  ENERGYVIEW,
  AGEVIEW,
  HEADINGVIEW,
//  RANDVIEW=14
} CamMode;

typedef enum {
  EAT01=0,
  MOVE02,
  SEE21,
  NUMABILITIES,//sets which abilities are active
  NONE,
  NOUTPUT,
  ENERGY,
  ROTATE,
  BONDSEEK,
  PHOTO,
  HEALTH,
  NINPUT,
//  GPS=13,
  FINDFOOD,
  SEERELATION,
  REPRODUCE,
  BUD,
  REPANDCHANGE,
  ACCELEROMETER,
  AGE,
  SEESIZE,
  EAT,
  THRUST
//    RAND=14
} AbilityType;

typedef enum {
  INJECT_CHILDREN=0,
  BLOOD=1,
}EnergyType;
#define ENERGY_CHOICE 0

typedef enum {INT=0,FLOAT,LONG,DOUBLE} NumType;


typedef struct Ability Ability;
typedef struct Cell Cell;
typedef struct Camera Camera;
typedef struct LL LL;
typedef struct Creature Creature;
typedef struct Body Body;
typedef struct Brain Brain;
typedef struct Square Square;
typedef struct Terrain Terrain;
typedef struct Grid Grid;
typedef struct ViewElem ViewElem;
typedef struct MutationConfig MutationConfig;

typedef struct Ecosystem Ecosystem;//stores all Body Data
typedef struct EcoInfo EcoInfo;//stores multiple ecosystem information
typedef struct TickData TickData;//stores data from 1 tick
typedef struct ExptPlan ExptPlan;//stores experiment plans and settings given as args
typedef struct mutualInfoDat mutualInfoDat;

//<

//LEVEL 1
//-7.03
//LEVEL 2
//-6.27 -4.81
//LEVEL 3
//-5.87 -3.57 -1.61
//LEVEL 4
//-5.31 -4.16 -1.33 -4.97
//__NEW MUTATION CONSTANTS__
#define NUM_MUTATION_CONFIGS 1
#define HISTORY_ENTRIES 3

#define INIT3ABS0 0
//LEVEL 1 ON
#define MUTATIONSTRENGTH0 -5.31
#define CHANGE_ABILITY_PROB0 -4.16
#define PROB_NO_CHANGE0 0
#define PROB_HISTORY0 0
#define PROB_RANDOM0 1
#define PROB_NONE0 0
#define CHANGE_CELLSIZE_PROB0 -1.33
//SET TO 0 to switch to LEVEL 1/2
#define COMCHANGE_PROB0 -4.97//-8.0
#define ALIEN_CELL_PROB0 0
#define NEURON_MODE0 1
#define WEIGHT_RANGE0 1

#define INIT3ABS1 0
#define MUTATIONSTRENGTH1 0.05
#define CHANGE_ABILITY_PROB1 0.1
#define PROB_NO_CHANGE1 0
#define PROB_HISTORY1 0
#define PROB_RANDOM1 1
#define PROB_NONE1 0
#define CHANGE_CELLSIZE_PROB1 0.2
#define COMCHANGE_PROB1 0.1
#define ALIEN_CELL_PROB1 0
#define NEURON_MODE1 1
#define WEIGHT_RANGE1 1

//<

//$__LESS IMPORTANT STRUCTS__
struct mutualInfoDat{
  int inputNeurons[NUMNEURONS]; 
  int outputNeurons[NUMNEURONS];
  int inputBinVals[NUMNEURONS+1];
  long bins[NBINS*NBINS];
};

struct MutationConfig{//values should be e^x from -10 to 0
  int numMutations;
  double mutationStrength;
  double changeAbilityProb;
  double PROBnoChange;
  double PROBhistory;
  double PROBrandom;
  double PROBnone;
  double changeCellsizeProb;
  double comChangeProb;
  int alienCellProb;
  int NEURON_MODE;
  double WEIGHT_RANGE;
  int INIT3ABS;
};


#define ECOSYSTEM_LEN NUMCELLS
typedef enum{
  BODIES=0,
  BODY_SIZES,
  RADII,
  WEIGHTS,
  TYPE_HISTORIES,
  AB_INFO,
  AB_INFO_SIZES,
  BONDS,
  NUM_BONDS,
  REL_POS,
  ECOIS,
  ECOSYSTEM_ORIGINS,
  NUM_ECO_TYPES
} BodyInfoLen;

struct Ecosystem{
  int bodies[ECOSYSTEM_LEN][NUM_ECO_TYPES];//per body entries
  float bodySizes[ECOSYSTEM_LEN];
  float radii[ECOSYSTEM_LEN];
  float ecoIs[ECOSYSTEM_LEN];
  
  float weights[ECOSYSTEM_LEN*NUMNEURONS*NUMNEURONS];//per cell entries
  float typeHistories[ECOSYSTEM_LEN*ABILITIES_PER_BRAIN*HISTORY_ENTRIES];
  float abInfo[ECOSYSTEM_LEN*ABILITIES_PER_BRAIN*NEURONS_PER_ABILITY*2];//stores neuron bindings, as well as Noutput info
  float abInfoSize[ECOSYSTEM_LEN];//len AbilityInfo for each cell
  float bonds[ECOSYSTEM_LEN*MAX_BONDS];//need to fix bond information!
  float numBonds[ECOSYSTEM_LEN];
  float relPos[ECOSYSTEM_LEN*3];
  char ecosystemOrigins[MAX_ECO_NUMBER][ECONAME_MAX_LEN];//stores ecosystem of origin
  int  len[NUM_ECO_TYPES];
  char name[ECONAME_MAX_LEN];
};


struct LL{
  void* val;
  LL* next;
  LL* prev;
};
struct Square{
  LL* cells;
//  double maxVel;
//  int beforeEating;
//  long energy;
//  int PhotoerEn;
//  double eatAmount;
//  double CELL_EAT_YEILD;
//  double CELL_PHOTO_YEILD;
//  double MOVE_COST_MULT;
//  double LIVING_COST;
//  int rock;
};

struct ExptPlan{
  int keepGoing;
  char basePath[PATH_LEN];
  char ecoFolder[PATH_LEN];
  char dataFolder[PATH_LEN];
  char exptName[ECONAME_MAX_LEN];
  Ecosystem* ecos[10];
  int lEcos;
  int recordTicks[1000];
  int lRecordTicks;
  int loopTicks[1000];
  int lLoopTicks;
  int killTick;
  int mutualInfoTick;
  FILE* mutualInfoFile;
};

struct Terrain{
  int beforeEating;
  long energy;
  int moveCost;
  int PhotoerEn;
  double eatAmount;
//  double CELL_EAT_YEILD;
//  double CELL_PHOTO_YEILD;
//  double MOVE_COST_MULT;
//  double LIVING_COST;
};

struct ViewElem{
  double bltr[4];//relative to screen
//  double bltrRel[4];//relative to parent
  int dims[2];//0,0 = bottom left!
  ViewElem** childrenArr;//allows positional access of kids
  LL myLL;
  LL* children;
  ViewElem* parent;
  double color[3];
  double textColor[3];
  char text[PATH_LEN];
  int active;
  int textActive;
  int colorActive;
};

struct Camera{
  int bltr[4];
  int zoomPanXY[3];
  CamMode mode;
};

#define TICKS_PER_WRITE 100

struct EcoInfo{
  char ecoNames[MAX_ECO_NUMBER][ECONAME_MAX_LEN];
  int EcoCounts[TICKS_PER_WRITE*MAX_ECO_NUMBER];
  int lEcoNames;
  int lData;
};

struct TickData{
  long EcoCounts[MAX_ECO_NUMBER];
  int tick;
};

//<

//$__MORE IMPORTANT STRUCTS__
struct Ability{
  void (*InputFunc)(Grid*,Cell*,Ability*);
  void (*OutputFunc)(Grid*,Cell*,Ability*);
  void (*ActFunc)(Grid*,Cell*,Ability*);
  int IONeurons[2][NEURONS_PER_ABILITY];//0 is inputs, 1 is outputs
  //  int OutputNeurons[NEURONS_PER_ABILITY];
  double AbilityDoubs[DOUBLES_PER_ABILITY];
  AbilityType typeHistory[HISTORY_ENTRIES];
  //  int BrainIndex;
  int AbilityInts[INTS_PER_ABILITY];
  Cell* involvedCell;
};


struct Brain{
  double n1[NUMNEURONS];
  double n2[NUMNEURONS];
  int n12Swap;
  double weights[NUMNEURONS*NUMNEURONS];
//  long usageN[NUMNEURONS*NUMNEURONS];
  Cell* myCell;
  Ability abilities[ABILITIES_PER_BRAIN];
  long usageA[ABILITIES_PER_BRAIN];
  AbilityType IOTypes[2][NUMNEURONS];
  mutualInfoDat* CommInputInfo;
  mutualInfoDat* CommOutputInfo;
  //  AbilityType OutputTypes[NUMNEURONS];
  int IOUsed[2];
  //  int OutputsUsed;
};

struct Body{
  //  int index;
  int alive;
  long energy;
  long beforeEating;
  long lastBeforeEating;
  long EaterEn;
  Cell* cells[MAX_BODY_SIZE];
  long usageB;
  double radius;
  long cellSize;
  double moveAcc[3];
  double bumpAcc[3];
  double mass[2];//1:mass 2:moment of inertia
  double BodyPos[3];//1:X 2:Y 3:rotation
  double vel[3];//1:vX 2:vY 3:dRotation
//  double impulse[2];
//  double acc[3];//1:
//  Body* bondBuddyBody;
  int touching;
  int consumedEn;
  int killCt;
  int NumStraws;
//  int repPoints;
//  int reproducing;
//  double repEnTalley;
//  double repEn;
  long age;
  char ecosystemOrigin[ECONAME_MAX_LEN];
  int iEco;
  int ImutationConfig;
  MutationConfig* myConfig;
};


struct Cell{
  int alive;
  LL mySqElem;
//  LL myWorldElem;
  Body* myBody;
  Brain* myBrain;
  Body* myParent;
  double CellPos[3];
  double color[4];
  double rotAmount;
  LL* BondedCells;
  int numBonds;
//  int numStraws;
  int BodyCellI;//Body's cellIndices index of this cell
//  int reproducing;
//  Cell* BondBuddy;
  int livingCost;
  double behindPoint[2];
};

typedef enum {EVOL_CELLSIZE,EVOL_ABS,EVOL_CCC,EVOL_WEIGHTS,EVOL_ENERGY,L_EVOL_STATES}EVOL_CONTROL;
#define DELTA_DELTA_EVOL 1.1

struct Grid{
//  int shape[2];
  Square squares[SHAPEX*SHAPEY];
  Terrain terrains[TERRAINX*TERRAINY];
  Cell cells[NUMCELLS];
  Body bodies[NUMCELLS];
  Brain brains[NUMCELLS];
  long AbCounts[NUMABILITIES];
  long CellSizes[MAX_BODY_SIZE];
  //Cell* repMe[NUMCELLS*ABILITIES_PER_BRAIN];
  double repTypeOrProb[NUMCELLS*ABILITIES_PER_BRAIN];
  int numRepRequests;
  int timeRecords[2];
  LL* deadCells;
  long SUN;
  long SUNTURN;
  Camera cam;
  EcoInfo tickRecord;
  LL* Bonders;
  LL* Bondees;
  LL* deadBodies;
  int numDeadCells;
  int numDeadBodies;
  ViewElem* baseElem;
  ExptPlan myPlans;
  Cell* seeMyBrain;
  long ticks;
  int evolving;
  MutationConfig mutationConfigs[NUM_MUTATION_CONFIGS];
  EVOL_CONTROL controlState;
  double deltaStart[L_EVOL_STATES];
  double evolDelta;
  LL* MutualInfosInputs;
  LL* MutualInfosOutputs;
};


#endif//<

//PLANS
