/*global window: false */
"use strict";
//$ PLANS
//Innovation Game
//
//note: addcolor maxes=0.2
//
//sound for damage!!!
//
// CURRENT BUGS:
// need to punish for not publishing before game start time...
// need to make square connection check more strict (only connect squares whose four corners all connect!) more direct los calc!
// make color choices lower than critical limit!
// some problems with atomicity of rules, should fix eventually for fairness, not a big deal though
// unit chase attack is pretty fucked up... (eventual fix)
// make all messages compressed first! like map export as example!
// units sometimes get caugth on corners if the destination looks accessible from the middle of the move square
//
// ESSENTIALS:
// gui interface for designing and buying units
// game setup and win conditions
// default ralley point is towards middle of world!
//
//TODAY
//VISUAL:
//color squares under units to show team! when both on same square, combine color! simple! make terrain default grey color! trust me it will look cool as shit!
//
//units sometimes get caugth on corners if the destination looks accessible from the middle of the move square
// UNIT BEHAVIOR
// make units stop in line without double click!
// implement splash attack
// implement attack cooldown
//
//OPTIMIZATION:
//TOMORROW/TODAY!!!
//combine all world grid squares into single geometry!
//represent units as sprites, not 3d objects!
//have units update on timers to prevent overcrowding
//use clojure compiler to compress and scramble code so these bastards can't take your secrets if they idiotically choose not to hire you...
//see cyclces through friendlies and enemies now, friendlies should always be visible!
//
//small: make bump function composed of friendly and enemy lists
//
//DREAM FEATURES:
//make right click on a terrain get all properties in editor!
// have main menu!
// mini map
// unit pathing is still fucked up! (only a little, it's pretty great as is)
// some more cool abilities (hopefully easily usable by AI)
// instructions/tutorial!
// make menu and stuff airtight, permit editing and loading of maps!
// two entries per square, one for updown, one for leftright (ignoring for now!)
// put in unit separation to negate splash
// select units of type, deselect with shift, etc.
// make selection box oblong shaped
// when clicking on unwalkable terrain, make ground units move close to it
// switch a for attack to a for move
// if d is pressed while giving orders, give move then defend order
// camera scrolling with edge of screen
//
//REFACTORING:
//make sound object that is called for sound, and stores copies of sounds to abstract away that stuff
//general refactoring search warning signs:
// make units move to enemy if attacked
//  repeated code
//  long functions
//  data access, followed by functionality that could be separated
//  hackey, ugly stuff, like too many variables
//  anything that causes bugs!
//  modification to threejs on line 7419 to intersect objects! bad thing to do, will eventually need to refactor to make the change in house rather than in the threejs source
//  unit chase attack is pretty fucked up...
//
//
//  MAKE SERVER ROBUST!!!!!!!!!!!!! THERE MUST BE AN EASY WAY TO DO THIS, JUST GOTTA IMAGINE!!!

//#

//$ GLOBALS
var SQUARE_SIZE=20;
var TERR_DIMS=[60,60];
var WORLD_DIMS=[TERR_DIMS[0]/2,TERR_DIMS[1]/2];
var SELECTSQUARE_MARGIN=5;
var CAMERA_HEIGHT=600;
var CAMERA_DISTZ=-20;
var SCROLL_LOCK_MS=500;
var BUMP_FORCE=1.0;
var BUMP_FORCE_FIGHTING=2;
var ENEMY_BUMP_FORCE=5;
var NUM_TEAMS=2;
var HEAL_RATE=5;

//var MAX_TIME=1000
//var MIN_TIME=150

//UNIT MINS/MAXES
var MIN_COST=150;
var MAX_COST=2000;
var MIN_ATTACK=4;
var MAX_ATTACK=100;
var MIN_COOLDOWN=5;
var MAX_COOLDOWN=50;
var MIN_RANGE=1;
var MAX_RANGE=80;
var MIN_SPLASH=0;
var MAX_SPLASH=6;
var MIN_HEALTH=30;
var MAX_HEALTH=999;
var MIN_ARMOR=0;
var MAX_ARMOR=5;
var MIN_HEALING=0;
var MAX_HEALING=1.5;
var MIN_SPEED=0.2;
var MAX_SPEED=1.5;

//#
//$ UTILITY MATH FUNCTIONS

var mag=function(p){
  return Math.sqrt(p[0]*p[0]+p[1]*p[1]);};
var magSq=function(p){
  return p[0]*p[0]+p[1]*p[1];};

var norm=function(p){
  var magVal=mag(p);
  return [p[0]/magVal,p[1]/magVal];};

var rescale=function(p,newMag){
  var currMag=mag(p);
  return[(p[0]/currMag)*newMag,(p[1]/currMag)*newMag];};

var dotProd=function(p1,p2){return p1[0]*p2[0]+p1[1]*p2[1];}; 

var getDelta=function(p1,p2){return [p2[0]-p1[0],p2[1]-p1[1]];};

var dist=function(p1,p2){
  var delta=[p2[0]-p1[0],p2[1]-p1[1]];
  if(delta[0]||delta[1]){
    return Math.sqrt(delta[0]*delta[0]+delta[1]*delta[1]);}
  return 0;};

var vecAdd=function(p1,p2){return [p1[0]+p2[0],p1[1]+p2[1]];};

var distSq=function(p1,p2){
  var delta=[p2[0]-p1[0],p2[1]-p1[1]];
  return delta[0]*delta[0]+delta[1]*delta[1];};

//p1=from p2=to
var normDirScaled=function(p1,p2,scale){
  var delta=getDelta(p1,p2);
  var ret=norm(delta);
  if(scale){
    ret[0]=ret[0]*scale;
    ret[1]=ret[1]*scale;}
  return ret;};

var getBLTR=function(p1,p2){
  var bl=[Math.min(p1[0],p2[0]),Math.min(p1[1],p2[1])];
  var tr=[Math.max(p1[0],p2[0]),Math.max(p1[1],p2[1])];
  return {bl:bl,tr:tr};};

var getNormDir3d=function(p1,p2){
  if(p1.x!==p2.x||p1.y!==p2.y||p1.z!==p2.z){
    var delta={x:p2.x-p1.x,y:p2.y-p1.y,z:p2.z-p1.z};
    var mag=Math.sqrt(delta.x*delta.x+delta.y*delta.y+delta.z*delta.z);
    normDelta={x:delta.x/mag,y:delta.y/mag,z:delta.z/mag};
    return normDelta;}
  console.error("getNormDir3d taken with identical points...");
  return undefined;};

var arrShuffle=function(arr){
  //shuffles array elements in place, used to shuffle colors array
  var i=arr.length;
  while(i!==0){
    var randomI=Math.floor(Math.random()*i);
    i-=1;
    var temp=arr[i];
    arr[i]=arr[randomI];
    arr[randomI]=temp;}};

var armorDamageCalc=function(atkDmg,armor){
  var ret=0;
  if(atkDmg>armor){ret=atkDmg-(armor*armor)/(armor+1);}
  else{ret=atkDmg/(armor+1);}
  return ret;};

var costFun=function(acrahss){
  var arr=acrahss;
  return -581.32+25.96*arr[0]+7.81*arr[1]+5.717*arr[2]+4.095*arr[4]+0.455*arr[5]+94.159*arr[6];};

//#

//$ PLAYER
var Player=function(playerNum,startMoney,team,gridWorld,teamSeeArr,color,RTSGame){
  this.RTSGame=RTSGame;
  this.playerNum=playerNum;
  this.selected=[];
  this.leftColors=['Red','Orange','Yellow','Slime','Green','Cyan','Blue','Purple','Pink'];
  arrShuffle(this.leftColors);
  this.controlGroups=[];
  this.startMoney=startMoney;
  this.myUnits=[];
  this.myUnitNames=[];
  this.numUnits=0;
  if(!team){
    this.myTeam=0;
    this.otherTeam=1;}
  else{
    this.myTeam=1;
    this.otherTeam=0;}
  this.seeArr=teamSeeArr;
  this.AKey=false;
  this.activating=undefined;
  this.activationChosen=false;
  this.shiftKey=false;
  this.color=color;
  this.threeColor=new THREE.Color(this.color[0],this.color[1],this.color[2]);
  this.ralleyPath=undefined;};
//#

//$ LINE OF SIGHT FUNCTIONS 
var getSeeArrI=function(loc,gridWorld){
  if(gridWorld.insideWorldInt(loc)){
    return loc[0]*gridWorld.terrDims[0]+loc[1];}
  else{return -1;}};
var spawnSeer=function(myLoc,gridWorld,seeArr,seeEn,seeDecs,flying){
  //seeDecs array organized in E,W,N,S,NE,NW,SE,SW
  var myLocI=getSeeArrI(myLoc,gridWorld);
  //if(myLocI>-1&&seeEn>seeArr[myLocI]&&(!gridWorld.Terrains[myLocI].blockVis||flying)){
    if(myLocI>-1&&seeEn>seeArr[myLocI]){
      seeArr[myLocI]=seeEn;
      if(!gridWorld.Terrains[myLocI].blockVis){
        if(seeDecs[0]){ spawnSeer([myLoc[0]+1,myLoc[1]],gridWorld,seeArr,seeEn-seeDecs[0],seeDecs);}
        else if(seeDecs[1]){ spawnSeer([myLoc[0]-1,myLoc[1]],gridWorld,seeArr,seeEn-seeDecs[1],seeDecs);}
        else if(seeDecs[2]){ spawnSeer([myLoc[0],myLoc[1]+1],gridWorld,seeArr,seeEn-seeDecs[2],seeDecs);}
        else if(seeDecs[3]){ spawnSeer([myLoc[0],myLoc[1]-1],gridWorld,seeArr,seeEn-seeDecs[3],seeDecs);}
        if(seeDecs[4]){ spawnSeer([myLoc[0]+1,myLoc[1]+1],gridWorld,seeArr,seeEn-seeDecs[4],seeDecs);}
        else if(seeDecs[5]){ spawnSeer([myLoc[0]-1,myLoc[1]+1],gridWorld,seeArr,seeEn-seeDecs[5],seeDecs);}
        else if(seeDecs[6]){ spawnSeer([myLoc[0]+1,myLoc[1]-1],gridWorld,seeArr,seeEn-seeDecs[6],seeDecs);}
        else if(seeDecs[7]){ spawnSeer([myLoc[0]-1,myLoc[1]-1],gridWorld,seeArr,seeEn-seeDecs[7],seeDecs);}}}};

var calcVisibleTeam=function(gridWorld,team,seeArr){
  //recursively create seers, that spread diagonally, along a line, or both, if they encounter a square with a bigger see value than theirs, they stop. if they encounter a wall, they stop.
  var horizDist=gridWorld.terrSize[0];
  var vertDist=gridWorld.terrSize[1];
  var diagDist=Math.sqrt(horizDist*horizDist+vertDist*vertDist);
  //console.log(seeArr);
  for(var i=0;i<gridWorld.terrDims[0]*gridWorld.terrDims[1];i++){
    seeArr[i]=0;}
  var unitList=gridWorld.things[team];
  if(unitList){unitList.apply(calcVisibleUnit,[gridWorld,seeArr,horizDist,vertDist,diagDist]);}};

var startSeer=function(gridWorld,seeArr,currUnit,unitCoord,dispX,dispY,seeDecs){
  var flying=false;
  //EFFICIENCY: note the many square roots per unit with dist calc...
  var currCoord=[unitCoord[0]+dispX,unitCoord[1]+dispY];
  var currTerr=gridWorld.getTerrInt(currCoord);
  if(currTerr){spawnSeer(currCoord,gridWorld,seeArr,currUnit.seeRange-dist(currUnit.loc,currTerr.center),seeDecs,flying);}};
var calcVisibleUnit=function(gridWorld,seeArr,horizDist,vertDist,diagDist,flying){
  var currUnit=this;
  var unitTerr=gridWorld.getTerr(currUnit.loc);
  var unitCoord=[unitTerr.x,unitTerr.y];
  if(seeArr[getSeeArrI(unitCoord,gridWorld)]<currUnit.seeRange){
    seeArr[getSeeArrI(unitCoord,gridWorld)]=currUnit.seeRange;
    startSeer(gridWorld,seeArr,currUnit,unitCoord,1,0,[horizDist,0,0,0,0,0,0,0],flying);
    startSeer(gridWorld,seeArr,currUnit,unitCoord,-1,0,[0,horizDist,0,0,0,0,0,0],flying);
    startSeer(gridWorld,seeArr,currUnit,unitCoord,0,1,[0,0,vertDist,0,0,0,0,0],flying);
    startSeer(gridWorld,seeArr,currUnit,unitCoord,0,-1,[0,0,0,vertDist,0,0,0,0],flying);
    startSeer(gridWorld,seeArr,currUnit,unitCoord,1,1,[0,0,0,0,diagDist,0,0,0],flying);
    startSeer(gridWorld,seeArr,currUnit,unitCoord,-1,1,[0,0,0,0,0,diagDist,0,0],flying);
    startSeer(gridWorld,seeArr,currUnit,unitCoord,1,-1,[0,0,0,0,0,0,diagDist,0],flying);
    startSeer(gridWorld,seeArr,currUnit,unitCoord,-1,-1,[0,0,0,0,0,0,0,diagDist],flying);
    var currTerr1=gridWorld.getTerrInt([unitCoord[0]+1,unitCoord[1]+1]);
    var currTerr2=gridWorld.getTerrInt([unitCoord[0],unitCoord[1]+1]);
    if(currTerr1&&currTerr2&&(!currTerr1.blockVis&&!currTerr2.blockVis)||flying){
      startSeer(gridWorld,seeArr,currUnit,unitCoord,1,2,[0,0,vertDist,0,diagDist,0,0,0],flying);}
    currTerr1=gridWorld.getTerrInt([unitCoord[0]+1,unitCoord[1]+1]);
    currTerr2=gridWorld.getTerrInt([unitCoord[0]+1,unitCoord[1]]);
    if(currTerr1&&currTerr2&&(!currTerr1.blockVis&&!currTerr2.blockVis)||flying){
      startSeer(gridWorld,seeArr,currUnit,unitCoord,2,1,[horizDist,0,0,0,diagDist,0,0,0],flying);}
    currTerr1=gridWorld.getTerrInt([unitCoord[0]-1,unitCoord[1]+1]);
    currTerr2=gridWorld.getTerrInt([unitCoord[0],unitCoord[1]+1]);
    if(currTerr1&&currTerr2&&(!currTerr1.blockVis&&!currTerr2.blockVis)||flying){
      startSeer(gridWorld,seeArr,currUnit,unitCoord,-1,2,[0,0,vertDist,0,0,diagDist,0,0],flying);}
    currTerr1=gridWorld.getTerrInt([unitCoord[0]-1,unitCoord[1]+1]);
    currTerr2=gridWorld.getTerrInt([unitCoord[0]-1,unitCoord[1]]);
    if(currTerr1&&currTerr2&&(!currTerr1.blockVis&&!currTerr2.blockVis)||flying){
      startSeer(gridWorld,seeArr,currUnit,unitCoord,-2,1,[0,horizDist,0,0,0,diagDist,0,0],flying);}
    currTerr1=gridWorld.getTerrInt([unitCoord[0]+1,unitCoord[1]-1]);
    currTerr2=gridWorld.getTerrInt([unitCoord[0],unitCoord[1]-1]);
    if(currTerr1&&currTerr2&&(!currTerr1.blockVis&&!currTerr2.blockVis)||flying){
      startSeer(gridWorld,seeArr,currUnit,unitCoord,1,-2,[0,0,0,vertDist,0,0,diagDist,0],flying);}
    currTerr1=gridWorld.getTerrInt([unitCoord[0]+1,unitCoord[1]-1]);
    currTerr2=gridWorld.getTerrInt([unitCoord[0]+1,unitCoord[1]]);
    if(currTerr1&&currTerr2&&(!currTerr1.blockVis&&!currTerr2.blockVis)||flying){
      startSeer(gridWorld,seeArr,currUnit,unitCoord,2,-1,[horizDist,0,0,0,0,0,diagDist,0],flying);}
    currTerr1=gridWorld.getTerrInt([unitCoord[0]-1,unitCoord[1]-1]);
    currTerr2=gridWorld.getTerrInt([unitCoord[0],unitCoord[1]-1]);
    if(currTerr1&&currTerr2&&(!currTerr1.blockVis&&!currTerr2.blockVis)||flying){
      startSeer(gridWorld,seeArr,currUnit,unitCoord,-1,-2,[0,0,0,vertDist,0,0,0,diagDist],flying);}
    currTerr1=gridWorld.getTerrInt([unitCoord[0]-1,unitCoord[1]-1]);
    currTerr2=gridWorld.getTerrInt([unitCoord[0]-1,unitCoord[1]]);
    if(currTerr1&&currTerr2&&(!currTerr1.blockVis&&!currTerr2.blockVis)||flying){
      startSeer(gridWorld,seeArr,currUnit,unitCoord,-2,-1,[0,horizDist,0,0,0,0,0,diagDist],flying);}}};

var seeUnitsTeam=function(teamI){
  if(!this.visible&&(this.world.getTerr(this.loc).visible[teamI]||this.team==teamI)){this.reveal();}
  else if(this.visible&&!this.world.getTerr(this.loc).visible[teamI]&&this.team!=teamI){this.hide();}};

var seeTerrAndUnits=function(RTSGame,gridWorld,numTeams){
  for(var teamI=0;teamI<numTeams;teamI++){
    var seeArr=RTSGame.teamSeeArrs[teamI];
    for(var x=0;x<gridWorld.terrDims[0];x++){
      for(var y=0;y<gridWorld.terrDims[1];y++){
        var seeArrI=getSeeArrI([x,y],gridWorld);
        var visibleTerr=gridWorld.getTerrInt([x,y],gridWorld);
        var myPlayer=RTSGame.myPlayer();
        if(seeArr[seeArrI]){
          if(!visibleTerr.visible[teamI]){
            visibleTerr.visible[teamI]=true;
            if(RTSGame.myPlayer().myTeam==teamI){
              visibleTerr.addColor([0.1,0.1,0.1]);}}}
        else{
          if(visibleTerr.visible[teamI]){
            visibleTerr.visible[teamI]=false;
            if(RTSGame.myPlayer().myTeam==teamI){
              visibleTerr.addColor([-0.1,-0.1,-0.1]);}}}}}}
  if(gridWorld.things&&gridWorld.things[RTSGame.myTeam()]){
    gridWorld.things[RTSGame.myTeam()].apply(seeUnitsTeam,[RTSGame.myPlayer().myTeam]);
    gridWorld.things[RTSGame.otherTeam()].apply(seeUnitsTeam,[RTSGame.myPlayer().myTeam]);}};
//#

//$CALC PRICES CODE

var bound=function(val,min,max){return Math.log(val+1);};

var coeffStats=function(unitInfo){
  var ret=[];
  ret.push(bound(unitInfo.atkDmg/(unitInfo.cooldown+1),MIN_ATTACK/(MAX_COOLDOWN+1),MAX_ATTACK/(MIN_COOLDOWN+1),true));
  ret.push(bound(unitInfo.range,MIN_RANGE,MAX_RANGE,true));
  ret.push(bound(unitInfo.splash*0.75+2,MIN_SPLASH,MAX_SPLASH,true));
  ret.push(bound(unitInfo.armor,MIN_ARMOR,MAX_ARMOR,true));
  ret.push(bound(unitInfo.maxHP,MIN_HEALTH,MAX_HEALTH,true));
  ret.push(bound(unitInfo.healing,MIN_HEALING,MAX_HEALING,true));
  return ret;
};

var sendInfo=function(RTSGame,unit){
  //RTSGame.peer.send('Server',{tag:"unitInfo",attack:unit.attackOrig,cooldown:unit.cooldown,range:unit.range,armor:unit.armor,health:unit.maxHP,speed:unit.speed,healing:unit.healing,flying:unit.flying,damage:unit.damageCounter,splash:unit.splash});
var data=coeffStats(unit);
data.push(Math.log(unit.damageCounter+1));
data.push(Math.log(unit.damageAbsorbed));
RTSGame.peer.send('Server',{tag:"unitInfo",data:data});};

var makeRandyUnit=function(world,player){
  var center=world.getCenter();
  var myLoc=[];
  var mySpeed=getRandVal(0.1,1);
  myLoc[0]=Math.random()*world.terrDims[0]*world.terrSize[0];
  if(player.playerNum===0){myLoc[1]=(center[1]+(center[1])*(mySpeed-1/20));}
  else{myLoc[1]=(center[1]-(center[1])*(mySpeed-1/20));}
  //  var mySplash=Math.random()*8;
  //  mySplash=mySplash<MAX_SPLASH?mySplash:0;
  //  var myHealing=Math.random()*2;
  //  myHealing=myHealing<MAX_HEALING?myHealing:0;
  var myflying=Math.random();
  myflying=myflying<0.7?0:1;
  var ret=new Unit(
    world,
    myLoc,
    player.color,
    player,
    player.myTeam,
    //getRandVal(MIN_ATTACK,MAX_ATTACK,4),
    getRandVal(MIN_ATTACK,MAX_ATTACK,2.5),
    //getRandVal(MIN_COOLDOWN,MAX_COOLDOWN,1.5),
    getRandVal(MIN_COOLDOWN,MAX_COOLDOWN,2),
    getRandVal(MIN_RANGE,MAX_RANGE,7),
    getRandVal(MIN_SPLASH,MAX_SPLASH,3),
    //getRandVal(MIN_HEALTH,MAX_HEALTH,4),
    getRandVal(MIN_HEALTH,MAX_HEALTH,2.5),
    //getRandVal(MIN_ARMOR,MAX_ARMOR,2),
    getRandVal(MIN_ARMOR,MAX_ARMOR,1.5),
    getRandVal(MIN_HEALING,MAX_HEALING,3),
    mySpeed,
    myflying,
  0);
  ret.mode='aggressive';
  ret.dest=[myLoc[0],world.getCenter()[1]];
  return ret;};

var getRandVal=function(min,max,exp){
  var randy=Math.random();
  if(exp){randy=Math.pow(randy,exp);}
  return min+randy*(max-min);};

//#

//$ FACTORY CODE
var Factory=function(RTSGame,terr,player,startHP,activationCounter,startTime){
  //factory has max unit health... make it look slightly different...
  //every tick that unit is not healing, add buildpoints
  var ret=new Unit(RTSGame.world,terr.center,player.color,player,player.myTeam,20,10,50,0,999,5,0,0,0,1);
  ret.activationCounter=activationCounter;
  ret.addColor([-0.4,-0.4,-0.4]);
  if(ret.activationCounter===0){
    ret.activated=true;
    ret.addColor([0.4,0.4,0.4]);}
  else{
    ret.activated=false;
    ret.activating=false;}
  this.RTSGame=RTSGame;
  var world=RTSGame.world;
  if(!terr.factory){console.error("spawning factory in non-factory location");}
  var worldCenter=world.getCenter();
  ret.ralleyPt=vecAdd(ret.loc,normDirScaled(ret.loc,worldCenter,world.terrSize[0]*0.99));
  ret.ralleyPath=[ret.ralleyPt];
  ret.spawnPt=vecAdd(ret.loc,normDirScaled(ret.loc,worldCenter,world.terrSize[0]*0.2));
  ret.healingFactory=undefined;
  ret.build=undefined;
  if(startTime){ret.buildPoints=startTime;}
  else{ret.buildPoints=0;}
  ret.currHP=startHP;
  if(ret.activationCounter){
    ret.activationHeal=(1+ret.maxHP-ret.currHP)/ret.activationCounter;}
  ret.set=function(){
    this.player.RTSGame.peer.TakeMove('SetBuild',{name:this.build,ID:this.ID});};
  return ret;};

var ArcObj=function(world,startP,endP,threeColor){
  var v1=new THREE.Vector3(startP[0],startP[1],startP[2]);
  var v2=new THREE.Vector3((startP[0]+endP[0])/2,dist([startP[0],startP[2]],[endP[0],endP[2]])/2,(startP[2]+endP[2])/2);
  var v3=new THREE.Vector3(endP[0],endP[1],endP[2]);
  this.geometry=new THREE.Geometry();
  this.geometry.vertices.push(v1);
  this.geometry.vertices.push(v2);
  this.geometry.vertices.push(v3);
  this.material=new THREE.LineBasicMaterial({
    color:threeColor});
  this.mesh=new THREE.Line(this.geometry,this.material);
  world.scene.add(this.mesh);};
ArcObj.prototype.destroy=function(world){
  world.scene.remove(this.mesh);
  this.material.dispose();
  this.geometry.dispose();};

Thing.prototype.factoryHeal=function(healMe){
  this.healingFactory=healMe;
  if(this.healArc){
    this.healArc.destroy(this.world);}
  if(this.player.playerNum===this.player.RTSGame.myPlayer().playerNum){
    this.healArc=new ArcObj(this.world,[this.loc[0],0,this.loc[1]],[healMe.loc[0],0,healMe.loc[1]],this.player.threeColor);}};

Thing.prototype.factoryPlan=function(){
  if(this.activating){
    this.currHP=Math.min(this.currHP+this.activationHeal,this.maxHP);
    if(this.activationCounter<=0){
      this.activated=true;
      this.activating=false;
      this.player.activating=false;
      this.addColor([-1.5,-1.5,-1.5]);
      this.addColor([0.4,0.4,0.4]);}
    this.activationCounter-=1;}
  if(this.build==undefined&&this.player.myUnitNames.length){
    this.build=this.player.myUnitNames[0];}
  this.mode='guarding';
  if(this.activated){
    if(this.dest){
      //all that needs to happen with clicking is setting up dest, or setting to repair/build another factory
      var clickedLoc=this.world.getTerr(this.dest);
      if(clickedLoc.walkable){
        this.ralleyPt=this.dest.slice();
        this.spawnPt=vecAdd(this.loc,normDirScaled(this.loc,this.ralleyPt,this.world.terrSize[0]*0.5));
        this.ralleyPath=getPath(this.loc,this.ralleyPt,this.world,this);
        if(this.player.ralleyPath){
          destroyPath(this.world,this.player.ralleyPath);}
        if(this.player==this.player.RTSGame.myPlayer()){
          this.player.ralleyPath=drawPath(this.world,this.ralleyPath.concat([this.loc]),[1,1,1]);}
        this.healingFactory=undefined;}
      this.dest=undefined;}
    //      console.log(this.ralleyPt);
    //      console.log(this.player.myUnits);
    //      console.log(this.player.myUnitNames);
    //    console.log(this.producing);
    if(!this.producing&&this.player.myUnits){
      this.producing=this.player.myUnits[0];
      this.producingName=this.player.myUnitNames[0];}
    if(this.healingFactory){
      if(this.healingFactory.currHP<this.healingFactory.maxHP){ 
        this.healingFactory.currHP=Math.min(this.healingFactory.currHP+1,this.healingFactory.maxHP);}
      else{
        if(this.healArc){
          this.healArc.destroy(this.world);
          this.healArc=undefined;}
        this.healingFactory=undefined;}}
    if(!this.healingFactory&&!this.player.activating){
      this.buildPoints+=1;}
    if(!this.player.activating&&this.producing&&this.buildPoints>=this.producing.time){
      this.buildPoints-=this.producing.time;
      this.makeUnit();}}};

Thing.prototype.makeUnit=function(){
  var model=this.producing;
  //var Unit=function(world,loc,color,player,team,attack,cooldown,range,splash,health,armor,healing,speed,flying){
    var ret=new Unit(this.world,this.spawnPt.slice(),stringToColor(this.producing.color),this.player,this.team,model.atkDmg,model.cooldown,model.range,model.splash,model.maxHP,model.armor,model.healing,model.speed,model.flying,0);
    ret.mode='moving';
    ret.dest=this.ralleyPt.slice();
    if(!ret.flying){ret.path=this.ralleyPath.slice();}
    else{ret.path=this.ralleyPath[0].slice();}
    return ret;};



//#

//$ INIT UNIT
var Unit=function(world,loc,color,player,team,atkDmg,cooldown,range,splash,maxHP,armor,healing,speed,flying,factory){
  //note: make see range proprtional to move speed!
  //need formula for calculating radius from HP
  var radius=1.5+Math.pow(((maxHP-30)/1000),0.7)*3.5;
  //  if(radius>SQUARE_SIZE/2){
    //    radius=SQUARE_SIZE/2.1;
    //  }
var ret=new Thing(world,loc,radius,color,[team],undefined,1,flying,factory);
ret.player=player;
if(!team){
  ret.team=0;
  ret.otherTeam=1;}
else{
  ret.team=1;
  ret.otherTeam=0;}
ret.ThoughtTimer=new ThoughtTimer(world.thinkInterval,world.thinkInterval%ret.ID);
ret.healing=healing;
ret.planVec=[0,0];
ret.flying=flying;
ret.splash=splash;
ret.moveVec=[0,0];
ret.orderList=new LL();//when order finishes, call getNextOrder
ret.dest=undefined;
ret.arrivedLoc=undefined;
ret.path=[];//if no length, and dest exists, calc path!
ret.targeted=undefined;
ret.donePlan=false;
ret.alive=true;
ret.cooldown=cooldown;
ret.coolTime=0;
ret.armor=armor;
ret.mode='aggressive';
ret.bumpCalc=false;
ret.attackLines=new LL();
ret.atkDmg=atkDmg;
ret.maxHP=maxHP;
ret.currHP=maxHP;
ret.speed=speed;
ret.seeRange=35+(ret.speed/MAX_SPEED)*25;
ret.range=range;
ret.damageCounter=0;
ret.damageAbsorbed=0;
ret.PathChecker=new ProgressChecker(ret,100,2);
return ret;};

var ProgressChecker=function(myUnit,tickDelta,moveDelta){
  this.tickDelta=tickDelta;
  this.currTick=0;
  this.myUnit=myUnit;
  this.moveDelta=moveDelta;};
ProgressChecker.prototype.checkProgress=function(){
  if(this.currTick==this.tickDelta){
    this.currTick=0;
    if(this.lastPos){
      if(dist(this.myUnit.loc,this.lastPos)<this.moveDelta){
        this.lastPos=this.myUnit.loc;
        this.currTick+=1;
        return false;}}
    this.lastPos=this.myUnit.loc.slice();}
  this.currTick+=1;
  return true;};
ProgressChecker.prototype.resetChecker=function(){
  this.currTick=0;
  this.lastPos=undefined;};

var ThoughtTimer=function(valRange,start){
  this.valRange=valRange;
  this.curr=start;};
ThoughtTimer.prototype.checkThink=function(){
  return this.curr?false:true;};
ThoughtTimer.prototype.advanceThink=function(){
  if(this.curr){this.curr-=1;}};
ThoughtTimer.prototype.resetThink=function(){this.curr=this.valRange;};
//#

//$ UNIT ACTIVITIES

Thing.prototype.attack=function(enemy,attackDuration){
  if(this.donePlan){console.error("attacking after plans already made");}
  if(this.coolTime>0){console.error("attacking before cooldown is over");}
  this.coolTime=this.cooldown;
  //may need to include path in these stipulations
  //controversial, but fine for nao!
  if(!enemy.targeted&&!enemy.path.length&&enemy.mode=='aggressive'){
    enemy.targeted=this;
    enemy.getAttackPath();
    enemy.untarget();}
    //cannot chase what cannot be seen!
    //var potentialPosse=[];
    //enemy.path[enemy.path.length-1]=enemy.path[enemy.path.length-1].slice();
    //if(enemy.path.length>1){
      //  var compareTerr=enemy.path[enemy.path.length-1];
      //}
  //else{
    //  var compareTerr=this.world.getTerr(enemy.loc).center;
    //}
//var compareTerrString=compareTerr[0]+','+compareTerr[1];
//this.world.getThingsWithinRadius(enemy.loc,30,[enemy.team],potentialPosse);
//for(var potentialMemberI in potentialPosse){
//  var potentialMember=potentialPosse[potentialMemberI];
//  var potentialTerr=this.world.getTerr(potentialMember.loc);
//  if((potentialMember!=enemy&&!potentialMember.attacking&&!potentialMember.path&&!potentialMember.dest&&potentialMember.mode=='aggressive')&&potentialMember.flying||(enemy.path.length==1&&TerrRayCheck(potentialTerr,this.world.getTerr(enemy.path[0]),this.world))||(enemy.path.length>1&&potentialTerr.accessibleObj[compareTerrString])){
//    potentialMember.path=enemy.path.slice();
//    potentialMember.prevDest=enemy.prevDest;
//  }
//}
if (!this.world.laserActive){
  if(this.world.laserI==this.world.lasers.length){
    this.world.laserI=0;}
  playAudio(this.world.lasers[this.world.laserI]);
  this.world.laserI+=1;
  this.world.laserActive=5;}
//used for attacking, draw attack and if enemy unit is sitting still, set it's movement to our position and set attacking to me!
var attackDist=dist(this.loc,enemy.loc);
var atkDmg=armorDamageCalc(this.atkDmg,enemy.armor);
this.damageCounter+=atkDmg;
enemy.damageAbsorbed+=this.atkDmg;
enemy.currHP-=atkDmg;
if(this.splash){
  var splashed=[]
  this.world.getThingsWithinRadius(enemy.loc,this.splash,[this.otherTeam],splashed);
  for(var splashedI in splashed){
    var splashedUnit=splashed[splashedI];
    if(splashedUnit!=enemy&&splashedUnit.team!=this.team){
      var atkDmg=(this.atkDmg/((dist(splashedUnit.loc,enemy.loc)-splashedUnit.radius)*0.375+1));
      var atkDmgRed=armorDamageCalc(atkDmg,splashedUnit.armor);
      splashedUnit.currHP-=atkDmgRed;
      this.damageCounter+=atkDmgRed;
      splashedUnit.damageAbsorbed+=atkDmg;
      var ret={}
      ret.destroyTimer=attackDuration;
      var v1=new THREE.Vector3(enemy.loc[0],enemy.mesh.position.y,enemy.loc[1]);
      var v2=new THREE.Vector3((enemy.loc[0]+splashedUnit.loc[0])/2,dist(enemy.loc,splashedUnit.loc)/2,(enemy.loc[1]+splashedUnit.loc[1])/2);
      var v3=new THREE.Vector3(splashedUnit.loc[0],splashedUnit.mesh.position.y,splashedUnit.loc[1]);
      ret.geometry=new THREE.Geometry();
      ret.geometry.vertices.push(v1);
      ret.geometry.vertices.push(v2);
      ret.geometry.vertices.push(v3);
      ret.material=new THREE.LineBasicMaterial({color:this.player.threeColor});
      ret.mesh=new THREE.Line(ret.geometry,ret.material);
      ret.myElem=this.attackLines.push(ret);
      this.world.scene.add(ret.mesh);}}}
//create draw elements for attack
var ret={}
ret.destroyTimer=attackDuration;
var v1=new THREE.Vector3(this.loc[0],this.mesh.position.y,this.loc[1]);
var v2=new THREE.Vector3((this.loc[0]+enemy.loc[0])/2,attackDist/2,(this.loc[1]+enemy.loc[1])/2);
var v3=new THREE.Vector3(enemy.loc[0],enemy.mesh.position.y,enemy.loc[1]);
ret.geometry=new THREE.Geometry();
ret.geometry.vertices.push(v1);
ret.geometry.vertices.push(v2);
ret.geometry.vertices.push(v3);
ret.material=new THREE.LineBasicMaterial({
  color:this.player.threeColor});
ret.mesh=new THREE.Line(ret.geometry,ret.material);
ret.myElem=this.attackLines.push(ret);
this.world.scene.add(ret.mesh);
//note: draw some kind of attack visualization, attack duration should also be a variable!
//also make units come after attacker!
this.donePlan=true;};

Thing.prototype.getAttackPath=function(){
  //don't do every tick!
  if(this.world.getTerr(this.targeted.loc).walkable){
    this.path=getPath(this.loc,this.targeted.loc,this.world,this);
    this.ThoughtTimer.resetThink();}};
var checkMove=function(unit){return unit.planVec[0]||unit.planVec[1]?true:false;};
Thing.prototype.calcBumps=function(){
  var unitList=[];
  this.world.getThingsWithinRadius(this.loc,this.radius,[this.team,this.otherTeam],unitList);
  for(var unitI in unitList){
    var currUnit=unitList[unitI];
    if(currUnit!==this){
      var betweenDist=dist(currUnit.loc,this.loc);
      var penetrationDepth=dist(currUnit.loc,this.loc)/(this.radius+currUnit.radius);
      if(penetrationDepth<1){
        if(this.path.length){
          var lastPathPos=this.path[this.path.length-1];}
        if(currUnit.team==this.team&&!currUnit.path.length&&this.dest&&currUnit.arrivedLoc&&currUnit.arrivedLoc[0]==this.dest[0]&&currUnit.arrivedLoc[1]==this.dest[1]){
          //hit unit that already arrived at destination, consider unit to have arrived as well.
          this.mode='aggressive';
          this.path=[];
          this.arrivedLoc=this.dest;
          this.dest=undefined;}
        //if(this.moved&&this.mode!='guarding'&&currUnit.team!=this.team&&!this.flying&&!currUnit.flying&&(this.planVec[0]||this.planVec[1])||(!this.planVec[0]&&!this.planVec[1]&&!currUnit.planVec[0]&&!currUnit.planVec[1])){
          var bumpComps=[0,0];
          bumpMag=(1-(penetrationDepth*penetrationDepth));
          if(this.mode!='guarding'&&currUnit.team!=this.team&&!this.flying&&!currUnit.flying&&checkMove(this)){
            //make units bounce off each other if they are moving, or if they are both still, if one is moving it bounces off the other.
            bumpComps=normDirScaled(currUnit.loc,this.loc,bumpMag*ENEMY_BUMP_FORCE);
            this.moveVec[0]+=bumpComps[0];
            this.moveVec[1]+=bumpComps[1];}
          else if(currUnit.team==this.team&&!((this.flying&&!currUnit.flying)||(currUnit.flying&&!this.flying))&&(checkMove(this)||!this.donePlan)){
            //bump event happens here!
            if(currUnit.coolTime){bumpComps=normDirScaled(currUnit.loc,this.loc,bumpMag*BUMP_FORCE_FIGHTING);}
            else{bumpComps=normDirScaled(currUnit.loc,this.loc,bumpMag*BUMP_FORCE);}
            this.moveVec[0]+=bumpComps[0];
            this.moveVec[1]+=bumpComps[1];}}}}};
Thing.prototype.targetable=function(team){
  var myTerr=this.world.getTerr(this.loc);
  return this.alive&&myTerr.visible[team]&&this.team!=team;};
Thing.prototype.pathMove=function(){
  if((!this.PathChecker.checkProgress()&&this.dest)||(!this.path.length&&this.dest)){
    if(this.path.length&&dist(this.loc,this.path[0])<this.world.terrSize[0]*2){
      this.arrivedLoc=this.path[0];
      if(this.path[0]==this.dest){
        if(!this.orderList.length){this.mode='aggressive';}
        this.dest=undefined;}
      this.path.pop();}
    if(this.dest){this.path=getPath(this.loc,this.dest,this.world,this);}}
  if(this.path.length>1&&dist(this.loc,this.path[this.path.length-1])-this.radius<this.world.terrSize[0]*0.5){
    //arrived at non-dest path point
    this.path.pop();}
  else if(this.path.length==1&&dist(this.loc,this.path[this.path.length-1])-this.radius<0){
    //arrived at dest
    this.arrivedLoc=this.path[0];
    if(this.path[0]==this.dest){
      if(!this.orderList.length){
        this.mode='aggressive';}
      this.dest=undefined;}
    this.path.pop();}
  if(this.path.length){
    var nextLoc=this.path[this.path.length-1];
    this.planVec=normDirScaled(this.loc,this.path[this.path.length-1],this.speed);
    this.donePlan=true;}};
Thing.prototype.untarget=function(){
  this.targeted=undefined;
  if(this.path.length){
    this.path[0]=this.path.slice(0,1);}}
Thing.prototype.runOrder=function(nextOrder){
  //all orders start here, orders have modes, destinations, and units
  if(nextOrder.a.dest){
    var nextTerr=this.world.getTerr(nextOrder.a.dest);
    if(!nextTerr.walkable&&!this.flying){
      //prevent walking to unreachable location!
      return;}}
  this.mode=nextOrder.a.mode;
  this.dest=nextOrder.a.dest;
  if(this.mode=='chasing'&&this.world.UnitsByID[nextOrder.a.unit].targetable(this.team)){
    this.targeted=this.world.UnitsByID[nextOrder.a.unit];
    this.getAttackPath();
    this.dest=this.path[0];}
  if(this.dest){
    this.path=getPath(this.loc,this.dest,this.world,this);
    this.ThoughtTimer.resetThink();}};


var destroyAttack=function(world){
  world.scene.remove(this.mesh);
  this.material.dispose();
  this.geometry.dispose();
  this.myElem.rem();};

var attackDrawCountdown=function(world){
  if(this.destroyTimer<=0){
    world.scene.remove(this.mesh);
    this.material.dispose();
    this.geometry.dispose();
    this.myElem.rem();}
  else{this.destroyTimer-=1;}};
//#

//$ PLAN AND ACT
Thing.prototype.plan=function(){
  if(isNaN(this.currHP)){console.log(this);}
  if(this.currHP<this.maxHP&&!this.dmgColored){
    this.addColor([-0.4,-0.4,-0.4]);
    this.dmgColored=true;}
  if(this.dmgColored&&this.maxHP==this.currHP){
    this.addColor([0.4,0.4,0.4]);
    this.dmgColored=false;}
  if(this.currHP<this.maxHP){this.currHP=Math.min(this.currHP+this.healing,this.maxHP);}
  if(!this.factory||this.activated){
    var myTerr=this.world.getTerr(this.loc);
    if(myTerr.factory&&myTerr.factoryOwner==-1){
      var newFactory=new Factory(this.player.RTSGame,myTerr,this.player,10,500);
      myTerr.factoryOwner=this.player.playerNum;}
    //advance state counters
    this.planVec[0]=0;
    this.planVec[1]=0;
    this.moveVec[0]=0;
    this.moveVec[1]=0;
    this.attackLines.apply(attackDrawCountdown,[this.world]);
    this.ThoughtTimer.advanceThink();
    if(this.coolTime<=0){
      this.donePlan=false;}
    else{this.coolTime-=1;}
    //global updates to state depending on orders and targets
    if(!this.dest&&this.orderList.length){this.runOrder(this.orderList.pop());}
    if(this.dest&&!this.path.length){
      this.path=getPath(this.loc,this.dest,this.world,this);
      this.ThoughtTimer.resetThink();}
    if(this.targeted&&!this.targeted.alive){
      if(this.mode=='aggressive'){this.findTarget(Math.max(this.range,this.seeRange)+SQUARE_SIZE/2);}}
    if(this.targeted&&!this.targeted.targetable(this.team)){
      this.untarget();}

    //mode specific code
    if(this.mode=='moving'){this.planMove();}
    else if(this.mode=='guarding'){this.planGuard();}
    else if(this.mode=='chasing'){this.planChase();}
    if(this.mode=='aggressive'){this.planAggressive();}}
  //calculate collisions and wrapup
  if(!this.factory){
    this.calcBumps();}
  if(this.factory){
    this.factoryPlan()}
  if(this.planVec[0]||this.planVec[1]){
    this.moveVec[0]+=this.planVec[0];
    this.moveVec[1]+=this.planVec[1];}};

Thing.prototype.act=function(){
  //move and kill dead
  if(this.moveVec[0]||this.moveVec[1]){
    if(magSq(this.moveVec)>this.speed*this.speed){
      this.move(rescale(this.moveVec,this.speed));}
    else{this.move(this.moveVec);}}
  if(this.currHP<0){
    this.attackLines.apply(destroyAttack,[this.world]);
    if(!this.world.splatActive){
      if(this.world.splatI==this.world.splats.length){
        this.world.splatI=0;}
      playAudio(this.world.splats[this.world.splatI]);
      this.world.splatActive=3;
      this.world.splatI+=1;}
    //    sendInfo(this.player.RTSGame,this);
    this.die();}};
//
var terrMark=function(RTSGame){
  this.RTSGame=RTSGame;
  this.terrMarks=[];
  this.prevMarks=[];
  this.numTerrs=RTSGame.world.Terrains.length;
  for(var terrI in RTSGame.world.Terrains){
    this.terrMarks.push([]);
    this.prevMarks.push([]);}};
//unoccupy all terr
terrMark.prototype.unOccupy=function(){
  for(var i=0;i<this.numTerrs;i++){
    var currMark=this.terrMarks[i];
    var prevMark=this.prevMarks[i];
    for(var j=0;j<this.RTSGame.Players.length;j++){
      prevMark[j]=currMark[j];
      currMark[j]=false;}}};
//occupy terrs with units on them
terrMark.prototype.occupy=function(){
  //generally somewhat inefficient, but what choice do I have, need to understand this keyword
  var unitList=this.RTSGame.world.UnitsByID;
  for(var unitI in unitList){
    var currUnit=unitList[unitI];
    if(currUnit&&currUnit.alive){
      //laughably inefficient, computes i to get terr, then gets i from terr... but it works!
      var terr=this.RTSGame.world.getTerr(currUnit.loc);
      var terrI=terr.i;
      if(terr.visible[this.RTSGame.myTeam()]){
        var currMark=this.terrMarks[terrI];
        currMark[currUnit.player.playerNum]=true;}}}};
//color based on delta occupation
terrMark.prototype.changeColors=function(){
  for(var i=0;i<this.numTerrs;i++){
    var currTerrMarks=this.terrMarks[i];
    var prevTerrMarks=this.prevMarks[i];
    for(var j=0;j<this.RTSGame.Players.length;j++){
      if(prevTerrMarks[j]&&!currTerrMarks[j]){
        var deltaColor=this.RTSGame.Players[j].color;
        this.RTSGame.world.Terrains[i].addColor([deltaColor[0]*-0.3,deltaColor[1]*-0.3,deltaColor[2]*-0.3]);}
      else if(!prevTerrMarks[j]&&currTerrMarks[j]){
        var deltaColor=this.RTSGame.Players[j].color;
        this.RTSGame.world.Terrains[i].addColor([deltaColor[0]*0.3,deltaColor[1]*0.3,deltaColor[2]*0.3]);}}}};


Thing.prototype.findTarget=function(searchRad){
  var engageUnits=[];
  var closestEnemy;
  this.world.getThingsWithinRadius(this.loc,searchRad,[this.otherTeam],engageUnits);
  for(var unitI in engageUnits){
    var currUnit=engageUnits[unitI];
    var currDist=dist(this.loc,currUnit.loc);
    if(currDist<searchRad&&currUnit.targetable(this.team)){
      searchRad=currDist;
      closestEnemy=currUnit;}
    if(closestEnemy){this.targeted=closestEnemy;}}
  if(this.targeted){this.getAttackPath();}
  this.ThoughtTimer.resetThink();};

Thing.prototype.checkTarget=function(){
  return this.targeted.alive&&this.targeted.targetable(this.team);};

Thing.prototype.canAttack=function(){
  return this.targeted&&this.targeted.targetable(this.team)&&dist(this.targeted.loc,this.loc)<this.range+this.targeted.radius+this.radius;};

Thing.prototype.planMove=function(){
  if(!this.donePlan){
    this.pathMove();}};

Thing.prototype.planAggressive=function(){
  if(this.ThoughtTimer.checkThink()&&!this.canAttack()){
    this.findTarget(Math.max(this.range+this.radius,this.seeRange+this.radius)+this.world.terrSize[0]/2);}
  if(!this.donePlan&&this.canAttack()){
    this.attack(this.targeted,5);}
  if(!this.donePlan&&this.path.length){
    this.pathMove();}}

Thing.prototype.planGuard=function(){
  if(this.ThoughtTimer.checkThink()&&!this.canAttack()){
    //need to check how this stuff is calculated to not waste!
    this.findTarget(this.range+this.world.terrSize[0]/2);}
  if(!this.donePlan&&this.canAttack()){
    this.attack(this.targeted,5);}};

Thing.prototype.planChase=function(){
  if(!this.targeted||!this.checkTarget()){
    this.mode='aggressive';
    this.untarget();
    this.path=[];
    this.dest=undefined;}
  else if(!this.donePlan&&this.canAttack()){
    this.attack(this.targeted,5);}
  else if(this.ThoughtTimer.checkThink){
    this.getAttackPath();}
  if(!this.donePlan){
    this.pathMove();}};


//#

//$ AUDIO

var HowlObj=function(path,volume,len,pos){
  pos=pos*len;
  var ret=new Howl({urls:[path],
    volume:volume,
    sprite:{
      complete:[0,len],
      start:[pos,len]},
    onend:function(){
      this.play('complete');}});
  ret.play('start');
  return ret;}

var AudioObj=function(path,volume){
  var ret=new Audio(path);
  ret.volume=volume;
  return ret;}
var playAudio=function(AudioObj){
  AudioObj.play();};
//#

//$ RTSGAME INITIALIZATION AND SOME FUNCTIONS
var RTSGame=function(peer,lockstep,mode){
  this.lasers=[];
  for(var i=0;i<10;i++){
    this.lasers.push(AudioObj("/ZAP.wav",0.1,false));}
  this.splats=[];
  for(var i=0;i<10;i++){
    this.splats.push(AudioObj("/SPLAT.wav",0.2,false));}
  this.mode='None'
  //this.music=HowlObj("/FTLSoundTrack.mp3",0.7,5580000,Math.random());
  this.music=AudioObj("/FTLSoundTrack.mp3",0.7);
  this.music.loop=true;
  playAudio(this.music);
  this.mode=mode;
  this.peer=peer;
  this.lockstep=lockstep;

  //  this.peer.tagFuncs.addFunc('PlayerNum',function(dataObj){
    //    console.log("got PlayerNum"+dataObj.num);
    //    this.playerNum=dataObj.num;
    //  }.bind(this)),

this.peer.tagFuncs.addFunc('editor',function(dataObj){
  this.mode='editor';
  this.editorInfo=new terrEdit(this);
  this.terrEditorGui=new terrEditGui(this.editorInfo);
}.bind(this));
this.peer.tagFuncs.addFunc('map',function(dataObj){
  if(dataObj.data){
    var mapData=JSON.parse(dataObj.data);
    importMap(mapData,this);}
  else{ mportMap(undefined,this);}
  this.mode='Game';
  //this.terrEditorGui.destroy();
}.bind(this));
this.peer.tagFuncs.addFunc('coeffs',function(dataObj){
  this.coeffs=(dataObj.data);
  this.coeffs=JSON.parse(this.coeffs).coeffs;
  console.log("coeffs recieved!");}.bind(this));};
//#

//$ RTSGAME PROTO AND GAME CONTROLS
RTSGame.prototype={
  myTeam:function(){
    return this.Players[this.peer.playerNum].myTeam;},
  otherTeam:function(){
    return this.Players[this.peer.playerNum].otherTeam;},
  myPlayer:function(){
    //return this.Players[this.peer.PeerID-1];
    return this.Players[this.peer.playerNum];},
  findSelectionSquare:function(){
    var selectionSquareCoords=undefined;
    if(this.leftDownPoint&&this.leftSquareRay){
      var completingIntersection=this.getRayIntersection(this.leftSquareRay,this.world.giantMesh)
      if(completingIntersection){
        selectionSquareCoords=getBLTR(this.leftDownPoint,completingIntersection.point);}}
    this.selectionSquareCoords=selectionSquareCoords;},
  initSelectionSquare:function(){
    var v1=new THREE.Vector3(this.world.getCenter[0]-1,-20,this.world.getCenter[0]-1);
    var v2=new THREE.Vector3(this.world.getCenter[0]+1,-20,this.world.getCenter[0]-1);
    var v3=new THREE.Vector3(this.world.getCenter[0]+1,-20,this.world.getCenter[0]+1);
    var v4=new THREE.Vector3(this.world.getCenter[0]-1,-20,this.world.getCenter[0]+1);
    this.selectionSquare=setupSquare(v1,v2,v3,v4,0xFF0000,true,[0,0.5,0],true);

    this.selectionSquare.drawDown=true;
    this.world.scene.add(this.selectionSquare.mesh);},

  moveSelectionSquare:function(){
    if(!this.selectionSquareCoords&&!this.selectionSquare.drawDown){
      this.selectionSquare.drawDown=true;
      var v1=new THREE.Vector3(this.world.getCenter[0]-1,-20,this.world.getCenter[0]-1);
      var v2=new THREE.Vector3(this.world.getCenter[0]+1,-20,this.world.getCenter[0]-1);
      var v3=new THREE.Vector3(this.world.getCenter[0]+1,-20,this.world.getCenter[0]+1);
      var v4=new THREE.Vector3(this.world.getCenter[0]-1,-20,this.world.getCenter[0]+1);
      this.selectionSquare.mesh.geometry.vertices=[v1,v2,v3,v4,v1];
      this.selectionSquare.mesh.geometry.verticesNeedUpdate=true;}
    else if(this.selectionSquareCoords){
      this.selectionSquare.drawDown=false;
      var v1=new THREE.Vector3(this.selectionSquareCoords.bl[0],0.5,this.selectionSquareCoords.bl[1]);
      var v2=new THREE.Vector3(this.selectionSquareCoords.tr[0],0.5,this.selectionSquareCoords.bl[1]);
      var v3=new THREE.Vector3(this.selectionSquareCoords.tr[0],0.5,this.selectionSquareCoords.tr[1]);
      var v4=new THREE.Vector3(this.selectionSquareCoords.bl[0],0.5,this.selectionSquareCoords.tr[1]);
      this.selectionSquare.mesh.geometry.vertices=[v1,v2,v3,v4,v1];
      this.selectionSquare.mesh.geometry.verticesNeedUpdate=true;
      this.selectionSquare.material.depthTest=false;}},
  //  selectionSquareDraw:function(){
    //    if(this.selectionSquare){
      //      this.world.scene.remove(this.selectionSquare.mesh);
      //      this.selectionSquare.mesh.material.dispose();
      //      this.selectionSquare.mesh.geometry.dispose();
      //      this.selectionSquare=undefined;
      //    }
//  },
onMouseUp:function(e){
  switch(e.button){
    case 0:
      var newSel=[];
      if(this.leftDownPoint){
        var currRay=this.mouseRay(e);
        var leftUpIntersection=this.getRayIntersection(currRay,this.world.giantMesh);
        if(leftUpIntersection){
          if(this.myPlayer().ralleyPath){
            destroyPath(this.world,this.myPlayer().ralleyPath);
            this.myPlayer.ralleyPath=undefined;}
          if(dist(leftUpIntersection.point,this.leftDownPoint)<SELECTSQUARE_MARGIN){
            var chosenUnitIntersection=this.getRayIntersection(currRay,this.world.clickables);
            if(chosenUnitIntersection && chosenUnitIntersection.thing.player==this.myPlayer()){
              if(chosenUnitIntersection.thing.factory&&chosenUnitIntersection.thing.activated){
                this.myPlayer().ralleyPath=drawPath(this.world,chosenUnitIntersection.thing.ralleyPath.concat([chosenUnitIntersection.thing.loc]),[1,1,1]);}
              if(chosenUnitIntersection.thing.factory&&chosenUnitIntersection.thing.producing&&chosenUnitIntersection.thing.player==this.myPlayer()&&this.myPlayer().myUnitNames.length>1){
                if(this.buildGui){
                  this.buildGui.destroy();
                  this.buildGui=undefined;}
                this.buildGui=new SelectBuildGui(chosenUnitIntersection.thing);}
              if(chosenUnitIntersection&&chosenUnitIntersection.thing.factory&&chosenUnitIntersection.thing.player==this.myPlayer()&&!chosenUnitIntersection.thing.activated&&!this.myPlayer().activationChosen){
                this.myPlayer().activationChosen=true;
                this.peer.TakeMove('FactoryActivating',{chosen:chosenUnitIntersection.thing.ID});}
              newSel.push(chosenUnitIntersection.thing.ID);}}
          else{
            var unitContainer=[];
            this.findSelectionSquare();
            if(this.selectionSquareCoords){
              this.world.getThingsWithinSquare(this.selectionSquareCoords.bl,this.selectionSquareCoords.tr,[this.myTeam()],unitContainer);
              for(var containerI in unitContainer){
                if(unitContainer[containerI].player==this.myPlayer()&&!unitContainer[containerI].factory){
                  newSel.push(unitContainer[containerI].ID);}}}}
          //if new units were picked, select them and remove old selection!
          changeSelected(this.world,this.myPlayer(),newSel);}}
      this.leftDownPoint=undefined;
      this.leftSquareRay=undefined;
      break;}},
onMouseMove:function(e){
  //if left mouse is down and mouse is far enough, draw selection square
  if(this.leftDownPoint){
    this.leftSquareRay=this.mouseRay(e);}},
onMouseDown:function(e) {
  switch(e.button){
    case 0:
      //record left click for unit selection
      var leftDownRay=this.mouseRay(e);
      var leftDownIntersection=this.getRayIntersection(leftDownRay,this.world.giantMesh);
      if(leftDownIntersection){
        this.leftDownPoint=leftDownIntersection.point;}
      if(this.mode=='editor'){
        var terrIntersection=this.getRayIntersection(leftDownRay,this.world.TerrMeshes);
        if(terrIntersection){
          var currTerr=terrIntersection.thing.mesh.myObj;
          currTerr.setColor([this.editorInfo.red,this.editorInfo.green,this.editorInfo.blue]);
          currTerr.walkable=this.editorInfo.walkable;
          currTerr.height=this.editorInfo.height;
          currTerr.mesh.position.y=this.editorInfo.height;
          currTerr.factory=this.editorInfo.factory;
          if(this.editorInfo.factory){
            currTerr.factoryOwner=this.editorInfo.factoryOwner;}
          currTerr.blockVis=this.editorInfo.blockVis;}}
      break;
    case 2:
      //right click orders all selected units to move either to a position or another unit
      if(this.mode=='editor'&&this.terrEditorGui){
        var rightDownRay=this.mouseRay(e);
        var terrIntersection=this.getRayIntersection(rightDownRay,this.world.TerrMeshes);
        if(terrIntersection){
          var currTerr=terrIntersection.thing.mesh.myObj;
          this.editorInfo.red=currTerr.color[0];
          this.editorInfo.blue=currTerr.color[2];
          this.editorInfo.green=currTerr.color[1];
          this.editorInfo.height=currTerr.height;
          this.editorInfo.blockVis=currTerr.blockVis;
          this.editorInfo.walkable=currTerr.walkable;
          this.editorInfo.factory=currTerr.factory;
          this.editorInfo.factoryOwner=currTerr.factoryOwner;
          for(var i in this.terrEditorGui.__controllers){
            this.terrEditorGui.__controllers[i].updateDisplay();}}}
      if(this.myPlayer().selected){
        var rightDownPos=this.mouseRay(e);
        var rightDownUnitMesh=this.getRayIntersection(rightDownPos,this.world.clickables);
        if(rightDownUnitMesh){
          var rightDownUnit=rightDownUnitMesh.thing;
          if(rightDownUnit.targetable(this.myPlayer().myTeam)){
            this.peer.TakeMove('UnitOrder',{mode:'chasing',sel:this.myPlayer().selected,dest:rightDownUnit.loc,unit:rightDownUnit.ID,queue:this.myPlayer().shiftKey});}
          if(!this.myPlayer().activating&&this.world.UnitsByID[this.myPlayer().selected[0]].factory&&rightDownUnit.factory&&rightDownUnit.team==this.myPlayer().myTeam&&rightDownUnit.currHP<rightDownUnit.maxHP&&this.world.UnitsByID[this.myPlayer().selected[0]].activated&&rightDownUnit.activated){
            this.peer.TakeMove('FactoryHeal',{from:this.myPlayer().selected[0],to:rightDownUnit.ID});}}
        else{
          var rightDownDest=this.getRayIntersection(rightDownPos,this.world.TerrMeshes);
          if(rightDownDest&&this.myPlayer().AKey){
            this.peer.TakeMove('UnitOrder',{mode:'aggressive',sel:this.myPlayer().selected,dest:rightDownDest.point,queue:this.myPlayer().shiftKey});}
          else if(rightDownDest){
            this.peer.TakeMove('UnitOrder',{mode:'moving',sel:this.myPlayer().selected,dest:rightDownDest.point,queue:this.myPlayer().shiftKey});}}} 
      break;}},
onKeyDown:function(event){
  if(event.keyCode==65){
    this.myPlayer().AKey=true;}
  else if(event.keyCode==16){
    this.myPlayer().shiftKey=true;}
  else if(event.keyCode==17){
    this.myPlayer().CtrlKey=true;}
  else if(event.keyCode>=48&&event.keyCode<=57){
    var keyNum=event.keyCode-48;
    if(this.myPlayer().CtrlKey&&checkAliveUnitList(this.world,this.myPlayer().selected)){
      this.myPlayer().controlGroups[keyNum]=this.myPlayer().selected.slice();
      event.preventDefault();
      event.stopPropagation();}
    else if(!this.myPlayer().CtrlKey&&checkAliveUnitList(this.world,this.myPlayer().controlGroups[keyNum])){
      changeSelected(this.world,this.myPlayer(),this.myPlayer().controlGroups[keyNum]);}}
  else if(event.keyCode==68){
    this.peer.TakeMove('UnitOrder',{mode:'guarding',sel:this.myPlayer().selected,queue:this.myPlayer().shiftKey});
}
  //    if(event.keyCode!=122){
    //    }
//this.peer.TakeMove('keyDown',String.fromCharCode(event.keyCode));
},
onKeyUp:function(event){
  if(event.keyCode==17){this.myPlayer().CtrlKey=false;}
  else if(event.keyCode==65){this.myPlayer().AKey=false;}
  else if(event.keyCode==16){this.myPlayer().shiftKey=false;}},
//this.peer.TakeMove('keyUp',String.fromCharCode(event.keyCode));
//SHUT DOWN ALL NORMAL KEYBOARD EVENTS (except fullscreen)
//#

//$ CRITICAL LOOP FUNCTIONS
unitAct:function(){this.act();},
unitPlan:function(){this.plan();},
run:function(moves){
  //  console.log(this.peer.goConnections);
  if(!this.gameOn&&this.world.tick==0){
    this.UnitGuiInfo=new UnitGuiInfo(this,this.myPlayer());
    this.UnitEditGui=new UnitEditGui(this.UnitGuiInfo);
    this.world.tick+=1;}
  if(!this.gameOn){
    var allDone=true;
    for(var playerI in this.Players){
      if(this.Players[playerI].numUnits==0){allDone=false;}}}
  if(allDone){this.gameOn=true;}
  //agreableness, concientiousness, emotional stability
  //extroverstion, openness
  //        if(!this.nextRunTick){
    //          this.nextRunTick=4;
    //        }
//        if(this.world.tick%this.nextRunTick==0){
//          makeRandyUnit(this.world,this.Players[0]);
//          makeRandyUnit(this.world,this.Players[1]);
//        }
//        if(this.world.tick%2000==0){
//          this.nextRunTick=~~(Math.random()*20+4);
//          for(var i in this.world.UnitsByID){
//            if(this.world.UnitsByID[i].alive){
//              this.world.UnitsByID[i].attackLines.apply(destroyAttack,[this.world]);
//              this.world.UnitsByID[i].die();
//            }
//          }
//        }
if(this.world.tick==2){
  for(var infoI in this.displayAtStart){
    var ret=new UnitInfoDisp(this.displayAtStart[infoI])}}
if((this.world.tick%this.world.seeInterval)==0){
  for(var i=0;i<NUM_TEAMS;i++){
    calcVisibleTeam(this.world,i,this.teamSeeArrs[i]);}
  seeTerrAndUnits(this,this.world,NUM_TEAMS);
  this.terrMarks.unOccupy();
  this.terrMarks.occupy();
  this.terrMarks.changeColors();}
if(this.world.laserActive){
  this.world.laserActive-=1;};
if(this.world.splatActive){
  this.world.splatActive-=1;};
if(moves.length){
  for(var moveI in moves){
    var currMove=moves[moveI];
    if(currMove.t==='FactoryActivating'){
      var chosenFactory=this.world.UnitsByID[currMove.a.chosen];
      if(!chosenFactory.player.activating){
        chosenFactory.addColor([1.5,1.5,1.5]);//white factory is currently being activated!
        chosenFactory.activating=true;
        chosenFactory.player.activating=chosenFactory;
        chosenFactory.player.activationChosen=false;}
        //activation chosen variable used to make sure player does not activate two factories at the same time.
      else{
        //cancel activation
        var stopMe=chosenFactory.player.activating;
        stopMe.activating=false;
        stopMe.addColor([-1.5,-1.5,-1.5]);
        chosenFactory.player.activating=undefined;
        chosenFactory.player.activationChosen=false;}}
    if(currMove.t==='FactoryHeal'){
      this.world.UnitsByID[currMove.a.from].factoryHeal(this.world.UnitsByID[currMove.a.to]);}
    if(currMove.t==='SetBuild'){
      var changeMe=this.world.UnitsByID[currMove.a.ID];
      var nextUnit;
      for(var unitI in changeMe.player.myUnits){
        if(changeMe.player.myUnits[unitI].name==currMove.a.name){
          changeMe.producing=changeMe.player.myUnits[unitI];}}}
    if(currMove.t==='NewUnitType'){
      var unitOwner=this.Players[currMove.a.info.player];
      var unitInfo=currMove.a.info;
      //          var uniqueName=false;
      //          var currName=unitInfo.name;
      //          while(!uniqueName){
        //            uniqueName=true;
        //            var ownerNames=unitOwner.myUnitNames;
        //            //do the same for color
        //            for(var unitNameI in ownerNames){
          //              if(currName==unitOwner.myUnitNames[unitNameI]){
            //                currName=currName+'*';
            //                uniqueName=false;
            //              }
    //            }
  //          }
//          unitInfo.name=currName;
//          unitInfo.color;
//          var ownerUnits=unitOwner.myUnits;
//          for(var unitI in ownerUnits){
//            if(unitInfo.color==ownerUnits[unitI].color){
//              unitInfo.color=unitOwner.leftColors[unitOwner.leftColors.length-1];
//              unitOwner.leftColors.pop;
//            }
//          }
//          console.log(unitInfo);
unitOwner.myUnits.push(unitInfo);
unitOwner.myUnitNames.push(unitInfo.name);
unitOwner.numUnits+=1;
if(!this.gameOn){
  if(this.myPlayer().playerNum==unitOwner.playerNum){var ret=new UnitInfoDisp(unitInfo);}
  else{this.displayAtStart.push(unitInfo);}}
else{var ret=new UnitInfoDisp(unitInfo);}}
if(currMove.t==='UnitOrder'){
  for(var UnitI in currMove.a.sel){
    var currUnit=this.world.UnitsByID[currMove.a.sel[UnitI]];
    if(currUnit.alive&&!currMove.a.queue){
      currUnit.dest=undefined;
      currUnit.orderList.empty();}
    currUnit.orderList.pushEnd(currMove);}}}}
if(this.gameOn){
  this.world.tick+=1;
  if(this.world.things){
    if(this.world.things[0]){
      this.world.things[0].apply(this.unitPlan);
      this.world.things[0].apply(this.unitAct);}
    if(this.world.things[1]){
      this.world.things[1].apply(this.unitPlan);
      this.world.things[1].apply(this.unitAct);}}}},
draw:function(RTSGame){
  //    if(!this.selectionSquare){
    //      this.initSelectionSquare();
    //    }
//    this.findSelectionSquare();
//    this.moveSelectionSquare();
//    this.selectionSquareDraw();
//    console.log(this.selectionSquare.geometry.__dirtyVirtices);
//    console.log(this.selectionSquare.geometry.dynamic);
//    console.log(this.selectionSquare.geometry.verticesNeedUpdate);
//    console.log(this.selectionSquare.geometry.lineDistancesNeedUpdate);
this.world.draw();
//    console.log(this.selectionSquare.geometry.verticesNeedUpdate);
},
//#

//$GAME CONTROL FUNCTIONS
mouseRay:function(e){
  var rayCaster=new THREE.Raycaster();
  var mouse=new THREE.Vector2();
  mouse.x=(e.clientX/window.innerWidth)*2-1;
  mouse.y=-(e.clientY/window.innerHeight)*2+1;
  rayCaster.setFromCamera(mouse,this.world.camera);
  return rayCaster;
},
getRayIntersection:function(mouseRay,intersectObjects){
  var closestThing=mouseRay.intersectObjects(intersectObjects);
  if(closestThing.length){
    return {thing:closestThing[0].object.myObj,point:[closestThing[0].point.x,closestThing[0].point.z]};}
  return null;},

zoomInOut2:function(Vector3Loc,zoomDist){
  var currPos=this.world.camera.position;
  var newY=currPos.y-zoomDist;
  if(newY>this.world.terrDims[0]*20){
    newY=this.world.terrDims[0]*20;}
  else if(newY<50){
    newY=50;}
  this.world.camera.position.set(Vector3Loc.x,newY,Vector3Loc.z-(newY*(this.cameraLookPt.z-currPos.z))/currPos.y);
  this.cameraLookPt=Vector3Loc;},
zoomInOut:function(vector3Loc,zoomDist,direction,currLook){
  //cap camera y position
  var currPos=this.world.camera.position;
  if(!direction){
    var DirVec=getNormDir3d(currPos,vector3Loc);
    var newCoords=[currPos.x,currPos.y+DirVec.y*zoomDist,currPos.z+DirVec.z*zoomDist];
    if(newCoords[1]>600){
      newCoords[1]=600;}
    else if(newCoords[1]<50){
      newCoords[1]=50;}
    this.world.camera.position.set(newCoords[0],newCoords[1],newCoords[2]);
    this.world.camera.lookAt(vector3Loc);}
  else{
    this.world.camera.position.set(currPos.x+vector3Loc.x*zoomDist,currPos.y+vector3Loc.y*zoomDist,currPos.z+vector3Loc.z*zoomDist);}},

onWheelRoll2:function(e){
  if(!this.currLook){
    this.currLook=new THREE.Vector3(0,0,-1);
    this.currLook.applyQuaternion(this.world.camera.quaternion);}
  if(!this.lastZoom.time||Date.now()-this.lastZoom.time>SCROLL_LOCK_MS){
    var mouseRay=this.mouseRay(e);
    var intersectPoint=this.getRayIntersection(mouseRay,this.world.TerrMeshes);
    if(intersectPoint&&e.wheelDeltaY>0){
      //if not zoomed in a while, zoom to mouse point
      var pointVec=new THREE.Vector3(intersectPoint.point[0],0,intersectPoint.point[1]);
      this.zoomInOut2(pointVec,e.wheelDeltaY/2);
      this.lastZoom.clientX=e.clientX;
      this.lastZoom.clientY=e.clientY;}
    else{
      this.zoomInOut2(this.cameraLookPt,e.wheelDeltaY/2);}}
  else{
    //        var zoomAdjustment={clientX:e.clientX-this.lastZoom.clientX+window.innerWidth/2,clientY:e.clientY-this.lastZoom.clientY+window.innerHeight/2};
  //        var adjustementMouseRay=this.mouseRay(zoomAdjustment);
  //        var adjustementPoint=this.getRayIntersection(adjustementMouseRay,this.world.TerrMeshes);
  //        if(adjustementPoint){
    //          var pointVec=new THREE.Vector3(adjustementPoint.point[0],0,adjustementPoint.point[1]);
    //          this.zoomInOut2(pointVec,e.wheelDeltaY/2);
    //        }
//        else{
this.zoomInOut2(this.cameraLookPt,e.wheelDeltaY/2);
//        }
}
this.lastZoom.time=Date.now();
},

onWheelRoll:function(e){
  currLook=new THREE.Vector3(0,0,-1);
  currLook.applyQuaternion(this.world.camera.quaternion);
  var currLook;
  if(!this.lastZoom.time||Date.now()-this.lastZoom.time>SCROLL_LOCK_MS){
    var mouseRay=this.mouseRay(e);
    var intersectPoint=this.getRayIntersection(mouseRay,this.world.TerrMeshes);
    if(intersectPoint){
      //if not zoomed in a while, zoom to mouse point
      var pointVec=new THREE.Vector3(intersectPoint.point[0],0,intersectPoint.point[1]);
      this.zoomInOut(pointVec,e.wheelDeltaY/2,false);
      this.lastZoom.clientX=e.clientX;
      this.lastZoom.clientY=e.clientY;}
    else{
      //if no mouse point, zoom to current middle
      currLook=new THREE.Vector3(0,0,-1);
      currLook.applyQuaternion(this.world.camera.quaternion);
      this.zoomInOut(currLook,e.wheelDeltaY/2,true);}}
  else{
    //if we have zoomed recently, get mouse adjust to change zoom orientation
    var zoomAdjustment={clientX:e.clientX-this.lastZoom.clientX+window.innerWidth/2,clientY:e.clientY-this.lastZoom.clientY+window.innerHeight/2};
    var adjustementMouseRay=this.mouseRay(zoomAdjustment);
    var adjustementPoint=this.getRayIntersection(adjustementMouseRay,this.world.TerrMeshes);
    if(adjustementPoint){
      this.zoomInOut(new THREE.Vector3(adjustementPoint.point[0],0,adjustementPoint.point[1]),e.wheelDeltaY/20,false);
      this.lastZoom.clientX=e.clientX;
      this.lastZoom.clientY=e.clientY;}
    else{
      //if no adjust point, zoom to current middle
      currLook=new THREE.Vector3(0,0,-1);
      currLook.applyQuaternion(this.world.camera.quaternion);
      this.zoomInOut(currLook,e.wheelDeltaY/2,true);}}
  this.lastZoom.time=Date.now();}};
//#

//$ GUI CODE
var UnitInfo=function(name,atkDmg,cooldown,range,splash,maxHP,armor,healing,speed,flying,color,price,player){
  //the information needed to completely specify a unit. send this to all other clients during unit creation phase.
  this.name=name;
  this.atkDmg=atkDmg;
  this.cooldown=cooldown;
  this.range=range;
  this.splash=splash?1:0;
  this.maxHP=maxHP;
  this.armor=armor;
  this.healing=healing;
  this.speed=speed;
  this.flying=flying?1:0;
  this.color=color;
  this.time=price;
  this.player=player;}

var stringToColor=function(colorStr){
  if(colorStr=='Black'){return [0,0,0];}
  else if(colorStr=='Gray'){return [0.5,0.5,0.5];}
  else if(colorStr=='White'){return [1,1,1];}
  else if(colorStr=='Red'){return [.9,0,0];}
  else if(colorStr=='Orange'){return [.9,0.6,0];}
  else if(colorStr=='Yellow'){return [.9,.9,0];}
  else if(colorStr=='Slime'){return [.5,.9,0];}
  else if(colorStr=='Green'){return [0,.9,0];}
  else if(colorStr=='Cyan'){return [0,.9,.9];}
  else if(colorStr=='Blue'){return [0,0,.9];}
  else if(colorStr=='Purple'){return [0.6,0,.9];}
  else if(colorStr=='Pink'){return [.9,0,0.65];}}

var processUnitVals=function(unitGuiInfo,player){
  var index=player.leftColors.indexOf(unitGuiInfo.color);
  if(index==-1){
    unitGuiInfo.color=player.leftColors[0];
    index=0;}
  player.leftColors.splice(index,1);
  index=player.myUnitNames.indexOf(unitGuiInfo.name);
  while(index!=-1){
    unitGuiInfo.name=unitGuiInfo.name+'*';
    index=player.myUnitNames.indexOf(unitGuiInfo.name);}};

var modifyPrice=function(unitGuiInfo,currPrice){
  //deals with speed, flying, and healing considerations not taken into account by the coefficients in the calcPrice function
  //  currPrice=currPrice*(1+unitGuiInfo.splash*0.02);
  currPrice=Math.exp(currPrice);
  //currPrice=Math.pow(currPrice,1.2);
  currPrice=currPrice*(0.5+unitGuiInfo.speed*0.8);
  currPrice=currPrice*(unitGuiInfo.flying?1.3:1);
  currPrice=Math.pow(currPrice/2,0.9);
  //currPrice=currPrice*(1+unitGuiInfo.healing*0.05);
  //currPrice=currPrice*(1+unitGuiInfo.splash*0.05);
  //  currPrice=currPrice*(1+unitGuiInfo.healing*0.5);
  return currPrice;}

var calcPrice=function(RTSGame,unitGuiInfo,UnitEditGui){
  var acceptable=false;
  while(!acceptable){
    //var arr=[Math.sqrt(unitGuiInfo.attack/(unitGuiInfo.cooldown+1)),Math.sqrt(unitGuiInfo.range),unitGuiInfo.splash,unitGuiInfo.armor,Math.sqrt(unitGuiInfo.maxHP),unitGuiInfo.healing]
    var arr=coeffStats(unitGuiInfo);
    var cost=RTSGame.coeffs[0];
    for(var i=0;i<6;i++){
      cost+=arr[i]*RTSGame.coeffs[i+1];
      if(isNaN(cost)){
        console.log(arr[i]);
        console.log(i);
        console.log(RTSGame.coeffs[i+1]);}}
    for(var i=0;i<6;i++){
      for(var j=0;j<6;j++){
        cost+=arr[i]*arr[j]*RTSGame.coeffs[7+i*6+j];
        if(isNaN(cost)){
          console.log(i,j);
          console.log(i);
          console.log(RTSGame.coeffs[i+1]);}}}
    for(var i=0;i<6;i++){
      for(var j=0;j<6;j++){
        for(var k=0;k<6;k++){
          cost+=arr[i]*arr[j]*arr[k]*RTSGame.coeffs[43+i*36+j*6+k];
          if(isNaN(cost)){
            console.log(i,j,k);
            console.log(i);
            console.log(RTSGame.coeffs[i+1]);}}}}
cost=modifyPrice(unitGuiInfo,cost);
acceptable=true;
if(cost>MAX_COST){
  acceptable=false;
  var newatkDmg=unitGuiInfo.atkDmg*0.9;
  if(newatkDmg>MIN_ATTACK){
    unitGuiInfo.atkDmg=newatkDmg;}
  var newCooldown=unitGuiInfo.cooldown*1.1
  if(newCooldown<MAX_COOLDOWN){
    unitGuiInfo.cooldown=newCooldown;}
  var newmaxHP=unitGuiInfo.maxHP*0.9;
  if(newmaxHP>MIN_HEALTH){
    unitGuiInfo.maxHP=newmaxHP;}
  var newRange=unitGuiInfo.range*0.9;
  if(newRange>MIN_RANGE){
    unitGuiInfo.range=newRange;}}
if(cost<MIN_COST){
  acceptable=false;
  var newatkDmg=unitGuiInfo.atkDmg*1.1;
  if(newatkDmg<MAX_ATTACK){
    unitGuiInfo.atkDmg=newatkDmg;}
  var newCooldown=unitGuiInfo.cooldown*0.9
  if(newCooldown>MIN_COOLDOWN){
    unitGuiInfo.cooldown=newCooldown;}
  var newmaxHP=unitGuiInfo.maxHP*1.1;
  if(newmaxHP<MAX_HEALTH){
    unitGuiInfo.maxHP=newmaxHP;}
  var newRange=unitGuiInfo.range*1.1;
  if(newRange<MAX_RANGE){
    unitGuiInfo.range=newRange;}}}
unitGuiInfo.time=cost;
for(var i in UnitEditGui.__controllers){UnitEditGui.__controllers[i].updateDisplay();}};

var randomInRange=function(min,max){
  var randVal=Math.pow(Math.random(),3);
  console.log(randVal);
  return randVal*(max-min)+min;};

var UnitGuiInfo=function(RTSGame,player){
  //holds info used in unit creation
  this.RTSGame=RTSGame;
  this.name='unit'
  this.atkDmg=randomInRange(MIN_ATTACK,MAX_ATTACK);
  this.cooldown=randomInRange(MIN_COOLDOWN,MAX_COOLDOWN);
  this.range=1;
  this.splash=randomInRange(MIN_SPLASH,MAX_SPLASH);
  this.maxHP=randomInRange(MIN_HEALTH,MAX_HEALTH);
  this.armor=randomInRange(MIN_ARMOR,MAX_ARMOR);
  this.healing=randomInRange(MIN_HEALING,MAX_HEALING);
  this.speed=randomInRange(MIN_SPEED,MAX_SPEED);
  this.flying=Math.random()>0.5?true:false;
  this.player=player;
  this.time=0;
  this.color=this.player.leftColors[0];
  this.calcTime=function(){calcPrice(this.RTSGame,this,this.RTSGame.UnitEditGui);}
  this.publish=function(){
    processUnitVals(this,player);
    calcPrice(this.RTSGame,this,this.RTSGame.UnitEditGui);
    //can only make up to 3 unit types!
    if(this.RTSGame.numPlayers<3){
      if(this.RTSGame.myPlayer().numUnits==2){
        this.RTSGame.UnitEditGui.destroy();}}
    else{
      if(this.RTSGame.myPlayer().numUnits==1){
        this.RTSGame.UnitEditGui.destroy();}}
    //remove color of new unit from possibilities
    this.RTSGame.peer.TakeMove('NewUnitType',{info:new UnitInfo(this.name,this.atkDmg,this.cooldown,this.range,this.splash,this.maxHP,this.armor,this.healing,this.speed,this.flying,this.color,this.time,this.player.playerNum) });};};

var UnitInfoDisp=function(unitInfo){
  //used to display unit information
  var ret=new dat.GUI();
  ret.add(unitInfo,'name',[unitInfo.name]);
  ret.add(unitInfo,'player',unitInfo.player,unitInfo.player);
  ret.add(unitInfo,'color',[unitInfo.color]);
  ret.add(unitInfo,'time',unitInfo.time,unitInfo.time);
  ret.add(unitInfo,'atkDmg',unitInfo.atkDmg,unitInfo.atkDmg);
  ret.add(unitInfo,'cooldown',unitInfo.cooldown,unitInfo.cooldown);
  ret.add(unitInfo,'range',unitInfo.range,unitInfo.range);
  ret.add(unitInfo,'splash',unitInfo.splash,unitInfo.splash);
  ret.add(unitInfo,'maxHP',unitInfo.maxHP,unitInfo.maxHP);
  ret.add(unitInfo,'armor',unitInfo.armor,unitInfo.armor);
  ret.add(unitInfo,'healing',unitInfo.healing,unitInfo.healing);
  ret.add(unitInfo,'speed',unitInfo.speed,unitInfo.speed);
  ret.add(unitInfo,'flying',unitInfo.flying,unitInfo.flying);
  //  ret.add(unitInfo,'makeUnit');
  ret.width=135;};

var SelectBuildGui=function(factory){
  //return after forcing publication of first unit
  var ret=new dat.GUI();
  ret.add(factory,'ID',factory.ID,factory.ID);
  var setBuild=ret.add(factory,'build',factory.player.myUnitNames);
  setBuild.onFinishChange(function(value){
    if(factory.alive&&factory.producing.name!=value){
      factory.player.RTSGame.peer.TakeMove('SetBuild',{name:value,ID:factory.ID});}});
  //ret.add(factory,'set');
  ret.width=135;
  return ret;};

var UnitEditGui=function(unitGuiInfo){
  //gui used to create units
  var ret=new dat.GUI();
  ret.add(unitGuiInfo,'name');
  ret.add(unitGuiInfo,'color',unitGuiInfo.player.leftColors);
  ret.add(unitGuiInfo,'atkDmg',MIN_ATTACK,MAX_ATTACK);
  ret.add(unitGuiInfo,'cooldown',MIN_COOLDOWN,MAX_COOLDOWN);
  ret.add(unitGuiInfo,'range',MIN_RANGE,MAX_RANGE);
  ret.add(unitGuiInfo,'splash',MIN_SPLASH,MAX_SPLASH);
  ret.add(unitGuiInfo,'maxHP',MIN_HEALTH,MAX_HEALTH);
  ret.add(unitGuiInfo,'armor',MIN_ARMOR,MAX_ARMOR);
  ret.add(unitGuiInfo,'healing',MIN_HEALING,MAX_HEALING);
  ret.add(unitGuiInfo,'speed',MIN_SPEED,MAX_SPEED);
  ret.add(unitGuiInfo,'flying');
  ret.add(unitGuiInfo,'time');
  ret.add(unitGuiInfo,'calcTime');
  ret.add(unitGuiInfo,'publish');
  //  ret.__closeButton.hidden=true;
  ret.width=513;
  return ret;};

var terrEdit=function(RTSGame){
  this.red=0.1;
  this.blue=0.1;
  this.green=0.1;
  this.height=0;
  this.factory=false;
  this.walkable=true;
  this.blockVis=false;
  this.name='untitled';
  this.factoryOwner=-1;
  this.rightLeftFlip=function(){
    //for now make sure dimensions are even!
    var world=RTSGame.world;
    var midline=(world.terrDims[0]/2);
    var worldEnd=world.terrDims[0];
    for(var x=0;x<midline;x++){
      for(var y=0;y<world.terrDims[1];y++){
        copyTerr(world.getTerrInt([x,y]),world.getTerrInt([worldEnd-1-x,y]));}}}
  this.rightLeftFlip2=function(){
    //for now make sure dimensions are even!
    var world=RTSGame.world;
    var midline=world.terrDims[0]/2;
    var worldEnd=world.terrDims[0];
    for(var x=0;x<midline;x++){
      for(var y=0;y<world.terrDims[1];y++){
        copyTerr(world.getTerrInt([x,y]),world.getTerrInt([worldEnd-1-x,worldEnd-1-y]));}}}
  this.brtlDiagonalFlip=function(){//copy all terrains over diagonal, skip diagonal itself
    var world=RTSGame.world;
    for(var y=0;y<world.terrDims[1];y++){
      for(var x=0;x<world.terrDims[0];x++){
        copyTerr(world.getTerrInt([x,y]),world.getTerrInt([world.terrDims[0]-1-y,world.terrDims[1]-1-x]));}}}
  this.brtlDiagonalFlip2=function(){
    //requires square world!
    var world=RTSGame.world;
    for(var i=0;i<world.terrDims[0];i++){
      for(var y=0;y<=i;y++){
        for(var x=0;x<=i;x++){
          copyTerr(world.getTerrInt([x,y]),world.getTerrInt([world.terrDims[0]-1-x,world.terrDims[1]-1-y]));}}}}
  this.brAllCornersFlip=function(){
    var world=RTSGame.world;
    //all 4 corners copy the br
    var midlineX=world.terrDims[0]/2;
    var midlineY=world.terrDims[0]/2;
    for(var x=0;x<midlineX;x++){
      for(var y=0;y<midlineY;y++){
        var brTerr=world.getTerrInt([x,y]);
        copyTerr(brTerr,world.getTerrInt([world.terrDims[0]-1-x,world.terrDims[1]-1-y]));
        copyTerr(brTerr,world.getTerrInt([x,world.terrDims[1]-1-y]));
        copyTerr(brTerr,world.getTerrInt([world.terrDims[0]-1-x,y]));}}}
  this.setAllColor=function(){
    world=RTSGame.world;
    //applies settings to all squares that share current color
    for(var i=0;i<world.terrDims[0]*world.terrDims[1];i++){
      var currTerr=world.Terrains[i];
      if(currTerr.color[0]==this.red&&currTerr.color[1]==this.green&&currTerr.color[2]==this.blue){
        currTerr.walkable=this.walkable;
        currTerr.height=this.height;
        currTerr.mesh.position.y=this.height;
        currTerr.factory=this.factory;
        if(currTerr.factory){
          currTerr.factoryOwner=this.factoryOwner;}
        currTerr.blockVis=this.blockVis;}}}
  this.exportMap=function(){
    var info={};
    info.name=this.name;
    info.terrDims=RTSGame.world.terrDims;
    info.terrSize=RTSGame.world.terrSize;
    info.sqDims=RTSGame.world.sqDims;
    info.SquareSize=RTSGame.world.SquareSize;
    info.TerrInfos=[];
    var corners=getCorners(RTSGame.world);
    setupPathing(RTSGame.world,corners);
    info.numPlayers=0;
    for(var x=0;x<info.terrDims[0];x++){
      for(var y=0;y<info.terrDims[1];y++){
        var currTerrInfo=getTerrInfo(RTSGame.world,[x,y]);
        if(currTerrInfo.factoryOwner>-1){
          console.log(currTerrInfo.factoryOwner);}
        info.TerrInfos.push(currTerrInfo);
        if(currTerrInfo.factory&&currTerrInfo.factoryOwner>info.numPlayers){
          info.numPlayers=currTerrInfo.factoryOwner;
          console.log(info.numPlayers);
          console.log(currTerr.factoryOwner);}}}
    info.numPlayers=Number(info.numPlayers)+1;
    console.log(info.numPlayers);
    RTSGame.peer.send('Server',{tag:"mapInfo",name:info.name,data:info});};}


var terrEditGui=function(terrEdit){
  var ret=new dat.GUI();
  ret.add(terrEdit,'red',0,0.7);
  ret.add(terrEdit,'green',0,0.7);
  ret.add(terrEdit,'blue',0,0.7);
  ret.add(terrEdit,'height',-10,10);
  ret.add(terrEdit,'factory');
  ret.add(terrEdit,'factoryOwner',[-1,0,1,2,3]);
  //-1 means owned by no player!
  ret.add(terrEdit,'walkable');
  ret.add(terrEdit,'blockVis');
  ret.add(terrEdit,'setAllColor');
  ret.add(terrEdit,'rightLeftFlip');
  ret.add(terrEdit,'brtlDiagonalFlip');
  ret.add(terrEdit,'rightLeftFlip2');
  ret.add(terrEdit,'brtlDiagonalFlip2');
  ret.add(terrEdit,'brAllCornersFlip');
  ret.add(terrEdit,'name');
  ret.add(terrEdit,'exportMap');
  return ret;}

var copyTerr=function(startTerr,endTerr){
  endTerr.setColor(startTerr.color.slice());
  endTerr.walkable=startTerr.walkable;
  endTerr.height=startTerr.height;
  endTerr.mesh.position.y=startTerr.height;
  endTerr.factory=startTerr.factory;
  endTerr.factoryOwner=startTerr.factoryOwner;
  endTerr.blockVis=startTerr.blockVis;}
//#

//$ MAIN MENU CODE
var mainMenuObj=function(){
  this.PLAY=function(){};//send go/choose map request
  this.mapEditor=function(){};//send map editor request
  this.instructions=function(){};}//display instructions or something
//#

//$ MAP PROCESSING AND SAVING
var importMap=function(mapData,RTSGame){
  RTSGame.numPlayers=2;
  if(mapData&&mapData.numPlayers){
    RTSGame.numPlayers=mapData.numPlayers;}
  RTSGame.Players=[];
  RTSGame.displayAtStart=[];
  RTSGame.gameOn=false;
  RTSGame.lastZoom={};
  if(!mapData){
    RTSGame.world=new GridWorld(WORLD_DIMS,TERR_DIMS,SQUARE_SIZE,[0.4,0.4,0.4]);}
  else{
    RTSGame.world=new GridWorld(mapData.sqDims,mapData.terrDims,mapData.SquareSize,[0.4,0.4,0.4]);}
  RTSGame.terrMarks=new terrMark(RTSGame);
  RTSGame.world.tick=0;
  RTSGame.world.seeInterval=5;
  RTSGame.world.thinkInterval=10;
  RTSGame.world.laserI=0;
  RTSGame.world.splats=RTSGame.splats;
  RTSGame.world.lasers=RTSGame.lasers;
  RTSGame.world.laserActive=false;
  RTSGame.world.splatI=0;
  RTSGame.world.splatActive=false;
  RTSGame.teamSeeArrs=[];
  RTSGame.teamSeeArrs.push(new Array(RTSGame.world.terrDims[0]*RTSGame.world.terrDims[1]));
  RTSGame.teamSeeArrs.push(new Array(RTSGame.world.terrDims[0]*RTSGame.world.terrDims[1]));
  RTSGame.Players.push(new Player(0,100,0,RTSGame.world,RTSGame.teamSeeArrs[0],[1,0,0],RTSGame));
  RTSGame.Players.push(new Player(1,100,1,RTSGame.world,RTSGame.teamSeeArrs[1],[0,0,1],RTSGame));
  if(RTSGame.numPlayers>2){
    RTSGame.Players.push(new Player(2,100,0,RTSGame.world,RTSGame.teamSeeArrs[0],[0,1,0],RTSGame));}
  if(RTSGame.numPlayers>3){
    RTSGame.Players.push(new Player(3,100,1,RTSGame.world,RTSGame.teamSeeArrs[0],[1,1,0],RTSGame));}

  //RTSGame.terrEditorGui.destroy();
  RTSGame.world.camera.position.set(RTSGame.world.sqDims[0]*RTSGame.world.SquareSize/2,RTSGame.world.terrDims[0]*20,CAMERA_DISTZ);
  RTSGame.cameraLookPt=new THREE.Vector3(RTSGame.world.sqDims[0]*RTSGame.world.SquareSize/2,0,RTSGame.world.sqDims[1]*RTSGame.world.SquareSize/2);
  RTSGame.world.camera.lookAt(RTSGame.cameraLookPt);



  //may have to dispose of mesh too, not sure why that function does not exist...
  var selectionSquareCoords=RTSGame.findSelectionSquare();
  if(selectionSquareCoords){
    var v1=new THREE.Vector3(selectionSquareCoords.bl[0],0,selectionSquareCoords.bl[1]);
    var v2=new THREE.Vector3(selectionSquareCoords.tr[0],0,selectionSquareCoords.bl[1]);
    var v3=new THREE.Vector3(selectionSquareCoords.tr[0],0,selectionSquareCoords.tr[1]);
    var v4=new THREE.Vector3(selectionSquareCoords.bl[0],0,selectionSquareCoords.tr[1]);
    RTSGame.selectionSquare=setupSquare(v1,v2,v3,v4,0xFF0000,true,[0,0.5,0]);
    RTSGame.selectionSquare.selectionSquareCoords=selectionSquareCoords;
    RTSGame.world.scene.add(RTSGame.selectionSquare.mesh);};
  if(mapData){
    var gridWorld=RTSGame.world;
    //temporary, eventually full map will be initialized using data from file
    if(gridWorld.terrDims[0]!=mapData.terrDims[0]||gridWorld.terrDims[1]!=mapData.terrDims[1]){console.error("terrdims mismatch!");}
    if(gridWorld.terrSize[0]!=mapData.terrSize[0]||gridWorld.terrSize[1]!=mapData.terrSize[1]){console.error("terrsize mismatch!");}
    if(gridWorld.sqDims[0]!=mapData.sqDims[0]||gridWorld.sqDims[1]!=mapData.sqDims[1]){console.error("terrdims mismatch!");}
    if(gridWorld.SquareSize!=mapData.SquareSize){console.error("SquareSize mismatch!");}
    for(var infoI in mapData.TerrInfos){
      var currInfo=mapData.TerrInfos[infoI];
      var currTerr=gridWorld.Terrains[infoI];
      currTerr.walkable=currInfo.walkable;
      currTerr.color=currInfo.color;
      currTerr.setColor(currTerr.color);
      currTerr.blockVis=currInfo.blockVis;
      currTerr.factory=currInfo.factory;
      currTerr.factoryOwner=Number(currInfo.factoryOwner);
      if(currTerr.factoryOwner>-1){
        var factoryPlayer=RTSGame.Players[currTerr.factoryOwner];
        var ret=new Factory(RTSGame,currTerr,factoryPlayer,1000,0,150);}
      if(currInfo.height){
        currTerr.mesh.position.y=currInfo.height;
        currTerr.height=currInfo.height;}
      currTerr.corner=currInfo.corner;
      currTerr.accessible=[];
      currTerr.accessibleObj={};
      for(var accessibleI in currInfo.accessible){
        var currAccess=currInfo.accessible[accessibleI];
        var currAccessTerr=gridWorld.getTerrInt(currAccess.point);
        currTerr.accessible.push({terr:currAccessTerr,dist:currAccess.dist});
        currTerr.accessibleObj[currAccessTerr.center[0]+','+currAccessTerr.center[1]]=true;}}
    console.log("map loaded!");}};

var getTerrInfo=function(gridWorld,loc){
  var inputTerr=gridWorld.getTerrInt(loc);
  var terrInfo={};
  terrInfo.walkable=inputTerr.walkable;
  terrInfo.corner=inputTerr.corner;
  terrInfo.accessible=inputTerr.accessible;
  terrInfo.color=inputTerr.color;
  terrInfo.blockVis=inputTerr.blockVis;
  terrInfo.height=inputTerr.height;
  terrInfo.factory=inputTerr.factory;
  terrInfo.factoryOwner=inputTerr.factoryOwner;
  return terrInfo;};

var negCompareFunc=function(a1,a2){return a2.dist-a1.dist;};
var pathingCompareFunc=function(a1,a2){return a1.dist-a2.dist;};

var setupPathing=function(gridWorld,corners){
  //iterate over all squares and check if all corners are accessible
  for(var x=0;x<gridWorld.terrDims[0];x++){
    for(var y=0;y<gridWorld.terrDims[1];y++){
      currTerr=gridWorld.getTerrInt([x,y]);
      currTerr.accessible=[];
      for(var cornerI in corners){
        var currCorner=corners[cornerI];
        if(currTerr!=currCorner && TerrRayCheck(currTerr,currCorner,gridWorld,true)){
          currTerr.accessible.push({point:[currCorner.x,currCorner.y],dist:dist(currTerr.center,currCorner.center)});}}}}};

var getCorners=function(gridWorld){
  var ret=[];
  for(var x=0;x<gridWorld.terrDims[0];x++){
    for(var y=0;y<gridWorld.terrDims[1];y++){
      var currTerr=gridWorld.getTerrInt([x,y]);
      if(gridWorld.getWalkableInt([x,y])){
        //find corners, 4 cases, then check if each corner is useful or not
        if(!gridWorld.getWalkableInt([x+1,y+1])&&gridWorld.getWalkableInt([x,y+1])&&gridWorld.getWalkableInt([x+1,y])&&(gridWorld.getWalkableInt([x+2,y])||gridWorld.getWalkableInt([x,y+2]))){
          ret.push(currTerr);
          currTerr.corner=true;}
        else if(!gridWorld.getWalkableInt([x-1,y-1])&&gridWorld.getWalkableInt([x,y-1])&&gridWorld.getWalkableInt([x-1,y])&&(gridWorld.getWalkableInt([x-2,y])||gridWorld.getWalkableInt([x,y-2]))){
          ret.push(currTerr);
          currTerr.corner=true;}
        else if(!gridWorld.getWalkableInt([x+1,y-1])&&gridWorld.getWalkableInt([x,y-1])&&gridWorld.getWalkableInt([x+1,y])&&(gridWorld.getWalkableInt([x+2,y])||gridWorld.getWalkableInt([x,y-2]))){
          ret.push(currTerr);
          currTerr.corner=true;}
        else if(!gridWorld.getWalkableInt([x-1,y+1])&&gridWorld.getWalkableInt([x,y+1])&&gridWorld.getWalkableInt([x-1,y])&&(gridWorld.getWalkableInt([x-2,y])||gridWorld.getWalkableInt([x,y+2]))){
          ret.push(currTerr);
          currTerr.corner=true;}}}}
  return ret;
};
//#

//$ MAIN RUN CODE
var prevTime=Date.now();

var runStep=function(myGame,moves){
  //uncomment to see fps!
  //  var currTime=Date.now()
  //  console.log(currTime-prevTime);
  //  prevTime=currTime;
  if(moves){
    myGame.run(moves);
}
  else{
    myGame.run([]);
}
  window.requestAnimationFrame(myGame.draw.bind(myGame));
};

var myGame=new RTSGame(new LSPeer(8001,function(moves){
  runStep(myGame,moves);
  //    runStep(myGame);
  //    runStep(myGame);
  //    runStep(myGame);
  //    runStep(myGame);
  //    runStep(myGame);
  //    runStep(myGame);
  //    runStep(myGame);
  //    runStep(myGame);
  //    runStep(myGame);
  //    runStep(myGame);
  //    runStep(myGame);
  //    runStep(myGame);
  setTimeout(function(){runStep(myGame);},50);
  setTimeout(function(){runStep(myGame);},100);
  setTimeout(function(){runStep(myGame);},150);
  //    setTimeout(function(){runStep(myGame);},200);
//    setTimeout(function(){runStep(myGame);},250);
//  setTimeout(function(){runStep(myGame);},10);
//  setTimeout(function(){runStep(myGame);},20);
//  setTimeout(function(){runStep(myGame);},30);
//  setTimeout(function(){runStep(myGame);},40);
}),1,'editor');
document.addEventListener('mousedown', myGame.onMouseDown.bind(myGame), false);
document.addEventListener('mouseup', myGame.onMouseUp.bind(myGame), false);
document.addEventListener('mousemove', myGame.onMouseMove.bind(myGame), false);
document.addEventListener('keydown',myGame.onKeyDown.bind(myGame),false);
document.addEventListener('keyup',myGame.onKeyUp.bind(myGame),false);
document.addEventListener('wheel',myGame.onWheelRoll2.bind(myGame),false);
//#

//$ A-STAR FUNCTIONS
//var LosCheck=function(startPt,endPt,world,stopAtCorners){
//  if(startPt[0]>endPt[0]){
//    var realStartPt
//  }
//};
var TerrRayCheck=function(startTerr,endTerr,world,stopAtCorners){
  //checks from middle of startTerr to middle of endTerr, which is close enough for me provided that terrain sizes are small. (which they will be!)
  var currX=startTerr.x;
  var currY=startTerr.y;
  var dirX=endTerr.x-currX;
  var dirY=endTerr.y-currY;
  var dx=Math.abs(dirX);
  var dy=Math.abs(dirY);
  var incX=currX<endTerr.x?1:-1;
  var incY=currY<endTerr.y?1:-1;
  var error=dx-dy;
  if(world.getTerrInt([currX+1,currY+1])&&!world.getTerrInt([currX+1,currY+1]).walkable&&dirX>0&&dirY>0){
    return false;}
  if(world.getTerrInt([currX-1,currY+1])&&!world.getTerrInt([currX-1,currY+1]).walkable&&dirX<0&&dirY>0){
    return false;}
  if(world.getTerrInt([currX+1,currY-1])&&!world.getTerrInt([currX+1,currY-1]).walkable&&dirX>0&&dirY<0){
    return false;}
  if(world.getTerrInt([currX-1,currY-1])&&!world.getTerrInt([currX-1,currY-1]).walkable&&dirX<0&&dirY<0){
    return false;}
  var dx2=dx*2;
  var dy2=dy*2;
  var nChecks=0;
  var totChecks=dx+dy;
  if(!stopAtCorners){
    while(nChecks<totChecks){
      if(error>0){
        currX+=incX;
        error-=dy2;
        nChecks+=1;}
      else if(error<0){
        currY+=incY;
        error+=dx2;
        nChecks+=1;}
      else if(nChecks<totChecks){
        if(!world.getTerrInt([currX+incX,currY]).walkable){
          return false;}
        if(!world.getTerrInt([currX,currY+incY]).walkable){
          return false;}
        currX+=incX;
        currY+=incY;
        error+=dx2-dy2;
        nChecks+=2;}
      if(!world.getTerrInt([currX,currY]).walkable){
        return false;}}
    return true;}
  else{
    while(nChecks<totChecks){
      if(error>0){
        currX+=incX;
        error-=dy2;
        nChecks+=1;}
      else if(error<0){
        currY+=incY;
        error+=dx2;
        nChecks+=1;}
      else if(nChecks<totChecks){
        if((!world.getTerrInt([currX+incX,currY]).walkable||world.getTerrInt([currX+incX,currY]).corner)&&(!world.getTerrInt([currX,currY+incY]).walkable||world.getTerrInt([currX+incX,currY]).corner)){
          return false;}
        currX+=incX;
        currY+=incY;
        error+=dx2-dy2;
        nChecks+=2;}
      if(!world.getTerrInt([currX,currY]).walkable||(nChecks!=totChecks&&world.getTerrInt([currX,currY]).corner)){
        return false;}}
    return true;}};
var getFScore=function(e1,e2){return e1.fScore-e2.fScore;}//used to order by smallest f score 

var getPath=function(startP,endP,gridWorld,unit){
  var startT=gridWorld.getTerr(startP);
  var endT=gridWorld.getTerr(endP);
  if(unit.flying||TerrRayCheck(startT,endT,gridWorld)){return [endP];}
  if(!endT.walkable){return [];}
  var friendsNearby=[];
  var myTerr=gridWorld.getTerr(startP);
  gridWorld.getThingsWithinRadius(unit.loc,20,[unit.team],friendsNearby);
  for(var friendI in friendsNearby){
    var currFriend=friendsNearby[friendI];
    for(var i=3;i>0;i--){
      if(currFriend!=unit&&currFriend.dest==endP&&currFriend.path.length>i){
        var friendNextLoc=currFriend.path[currFriend.path.length-i];
        var friendSqStr=friendNextLoc[0]+','+friendNextLoc[1];
        if(myTerr.accessibleObj[friendSqStr]){
          var sliceI=-(i-1);
          if(sliceI){var ret=currFriend.path.slice(0,sliceI);}
          else{var ret=currFriend.path.slice();}
          if(currFriend.targeted){
            unit.targeted=currFriend.targeted;
            ret[0]=currFriend.path[0];}
          return ret;}}}}
  var closedList={};
  var openList={};
  var endList={};
  for(var endConnI in endT.accessible){
    endList[endT.accessible[endConnI].terr.i]=endT.accessible[endConnI].dist;
}
  var openHeap=new Heap(getFScore);
  var primerElem=new aStarElem(startT,undefined,0,endT);
  openHeap.push(primerElem);
  while(openHeap.size()){
    var bestSoFar=openHeap.pop();
    if(bestSoFar.myTerr==endT){
      //compile path into LIFO stack
      var ret=[endP];
      bestSoFar=bestSoFar.prev;
      while(bestSoFar.prev){
        ret.push(bestSoFar.myTerr.center);
        bestSoFar=bestSoFar.prev;}
      return ret;}
    if(endList[bestSoFar.myTerr.i]){
      //add endP to openHeap if connecting terr is met
      var solutionElem=new aStarElem(endT,bestSoFar,endList[bestSoFar.myTerr.i],endT);
      openHeap.push(solutionElem);}
    closedList[bestSoFar.myTerr.i]=true;
    var fromHere=bestSoFar.myTerr.accessible;
    for(var connectionI in fromHere){
      var currConn=fromHere[connectionI];
      currConnI=currConn.terr.i;
      if(!closedList[currConnI]){
        var toHere=bestSoFar.gScore;
        if(!openList[currConnI]||openList[currConnI]<toHere){
          openList[currConnI]=toHere;
          openHeap.push(new aStarElem(fromHere[connectionI].terr,bestSoFar,fromHere[connectionI].dist,endT));}}}}
  return [];};

var aStarElem=function(myTerr,prev,distTo,endP){
  var prevGScore=0;
  if(prev){
    prevGScore=prev.gScore;}
  this.myTerr=myTerr;
  this.prev=prev;
  this.gScore=prevGScore+distTo;
  var hScore=dist(myTerr.center,endP.center)*0.8;
  this.fScore=this.gScore+hScore;};

//#

//$ MISC FUNCTIONS

//used to check pathing
var drawPath=function(gridWorld,path,color){
  var ret={};
  ret.geometry=new THREE.Geometry();
  for(var vertI in path){
    var currVert=path[vertI];
    ret.geometry.vertices.push(new THREE.Vector3(currVert[0],0.4,currVert[1]));
}
  ret.geometry.vertices.push(new THREE.Vector3(currVert[0],0.4,currVert[1]));
  ret.material=new THREE.LineBasicMaterial({
    color:new THREE.Color(color[0],color[1],color[2])
});
  ret.mesh=new THREE.Line(ret.geometry,ret.material);
  gridWorld.scene.add(ret.mesh);
  return ret;
}
var destroyPath=function(gridWorld,drawnPath){
  gridWorld.scene.remove(drawnPath.mesh);
  drawnPath.material.dispose();
  drawnPath.geometry.dispose();
}
var drawConnections=function(gridWorld,currTerr){
  for(var terrI in currTerr.accessible){
    var accessibleTerr=currTerr.accessible[terrI].terr;
    var v1=new THREE.Vector3(currTerr.center[0],1,currTerr.center[1]);
    var v2=new THREE.Vector3(accessibleTerr.center[0],1,accessibleTerr.center[1]);
    var ret={};
    ret.geometry=new THREE.Geometry();
    ret.geometry.vertices.push(v1);
    ret.geometry.vertices.push(v2);
    ret.material=new THREE.LineBasicMaterial({color:new THREE.Color(1,0,1)});
    ret.mesh=new THREE.Line(ret.geometry,ret.material);
    gridWorld.scene.add(ret.mesh);}};

var changeSelected=function(world,player,newUnitList){
  if(checkAliveUnitList(world,newUnitList)){
    for(var selectedI in player.selected){
      //note, addcolor event may be problematic if units are dead!
      var currUnit=world.UnitsByID[player.selected[selectedI]];
      if(currUnit.alive){
        currUnit.addColor([-0.1,-0.1,-0.1]);}}
    player.selected=newUnitList.slice();
    for(var selectedI in player.selected){
      var currUnit=world.UnitsByID[player.selected[selectedI]];
      if(currUnit.alive){
        currUnit.addColor([0.1,0.1,0.1]);}}}};
var checkAliveUnitList=function(world,unitList){
  //used for hotkeying, checks if there are any living units for number assignment, etc.
  for(var unitI in unitList){
    var currUnit=unitList[unitI];
    if(world.UnitsByID[currUnit].alive){return true;}}
  return false;};
//#

//$ CODE GRAVEYARD
// right click code to test 2D line of sight function
//        var checkRay=this.mouseRay(e);
//      var terrIntersect=this.getRayIntersection(checkRay,this.world.TerrMeshes);
//      if(terrIntersect){
//        if(!this.checkStart){
//          this.checkStart=terrIntersect;
//          this.checkStart.thing.mesh.myObj.setColor(0x00FF00);
//        }
//        else if(!this.checkEnd){
//          this.checkEnd=terrIntersect;
//          this.checkEnd.thing.mesh.myObj.setColor(0xFF0000);
//          console.log(TerrRayCheck(this.checkStart.thing.mesh.myObj,this.checkEnd.thing.mesh.myObj,this.world));
//        }
//        else{
//          this.checkEnd.thing.mesh.myObj.setColor(0xCCFFFF);
//          this.checkStart.thing.mesh.myObj.setColor(0xCCFFFF);
//          this.checkEnd=undefined;
//          this.checkStart=undefined;
//        }
//      }
//
//#
//
//
//MORE TESTS

