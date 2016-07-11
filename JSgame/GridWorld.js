//$GLOBALS
var GIANT_SQUARE_MARGIN=100
//#

//$ GRIDWORLD INIT
var GridWorld=function(sqDims,terrDims,SquareSize,defaultTerrColor){
  this.UnitsByID={};
  this.IDCount=1;
  this.clock=new THREE.Clock();
  this.scene = new THREE.Scene();
  this.canv=document.getElementById('myCanvas');
  this.canv.style.display='block';
  //var newInnerHeight=window.innerHeight*1
  //this.camera = new THREE.PerspectiveCamera( 75, window.innerWidth/newInnerHeight, 0.1, 1000 );
  this.camera = new THREE.PerspectiveCamera( 75, window.innerWidth/window.innerHeight, 0.1, 2000 );

  this.renderer = new THREE.WebGLRenderer({canvas:this.canv});
  //this.renderer.setSize( window.innerWidth, newInnerHeight );
  this.renderer.setSize( window.innerWidth, window.innerHeight );

  this.winResize=function(){
    //var newInnerHeight=window.innerHeight*1
    //this.renderer.setSize( window.innerWidth, newInnerHeight );
    this.renderer.setSize( window.innerWidth, window.innerHeight );
    //this.camera.aspect=window.innerWidth/newInnerHeight;
    this.camera.aspect=window.innerWidth/window.innerHeight;
    this.camera.updateProjectionMatrix();
  }.bind(this);
  window.addEventListener('resize',this.winResize,false);
  document.body.appendChild( this.renderer.domElement );
  this.sqDims=sqDims;
  this.SquareSize=SquareSize;
  this.terrDims=terrDims;
  this.terrSize=[this.SquareSize*this.sqDims[0]/this.terrDims[0],this.SquareSize*this.sqDims[1]/this.terrDims[1]];
  this.Squares=[];
  this.Terrains=[];
  this.TerrMeshes=[];
  this.things={};
  this.giantSquare=this.setupGiantSquare();
  this.giantMesh=[this.giantSquare.mesh];
  //this.scene.add(this.giantMesh[0]);
  this.thingMeshes={};
  this.clickables=[];
  this.deadClickableSlots=[];
  //  this.viewRatio=viewRatio;
  for(var i=0;i<this.sqDims[0];i++){
    for(var j=0;j<this.sqDims[1];j++){
      this.Squares.push(new Square(i,j,this));
    }
  }
  var allGeoms=new THREE.Geometry();
  for(var i=0;i<this.terrDims[0];i++){
    for(var j=0;j<this.terrDims[1];j++){
      this.Terrains.push(new Terrain(i,j,this,defaultTerrColor,true));
      this.TerrMeshes.push(this.Terrains[this.Terrains.length-1].mesh);
      //allGeoms.updateMatrix();
      allGeoms.merge(this.TerrMeshes[this.Terrains.length-1].geometry,this.TerrMeshes[this.Terrains.length-1].mesh);
    }
  }
  //  for(var i=0;i<this.terrDims[0]*this.terrDims[1];i++){
  //    THREE.GeometryUtils.merge(this.Terrains[i].geometry,this.TerrMeshes[i].mesh);
  //  }
  //  for(var i=0;i<this.terrDims[0]*this.terrDims[1]-1;i++){
  //    THREE.GeometryUtils.merge(this.TerrMeshes[i],this.TerrMeshes[i+1]);
  //  }
  };
  //#

  //$ GRIDWORLD FUNCTIONS
  GridWorld.prototype={
    getNewID:function(newThing){
      this.IDCount+=1;
      this.UnitsByID[this.IDCount]=newThing;
      return this.IDCount;
    },
    draw:function(){
      var delta=this.clock.getDelta();
      //this.cameraControls.update(delta);
      this.renderer.render(this.scene,this.camera);
    },
    //    for(i=0;i<this.Squares.length;i++){
    //      this.Squares[i].draw(this.ctx);
    //    }
    //    var drawFunc=function(ctx){
    //      this.draw(ctx);
    //    },
    //    for(var thing in this.things){
    //      this.things[thing].apply(drawFunc,[this.ctx]);
    //    }
    getSquareCoord:function(loc){
      var index=loc[0]*this.sqDims[0]+loc[1];
      if(index>=0&&index<this.Squares.length){
        return this.Squares[index];
      }
    },
    setupGiantSquare:function(){
      var v1=new THREE.Vector3(-GIANT_SQUARE_MARGIN,-1,-GIANT_SQUARE_MARGIN);
      var v2=new THREE.Vector3(this.sqDims[0]*this.SquareSize+GIANT_SQUARE_MARGIN,-1,-GIANT_SQUARE_MARGIN);
      var v3=new THREE.Vector3(this.sqDims[0]*this.SquareSize+GIANT_SQUARE_MARGIN,-1,this.sqDims[1]*this.SquareSize+GIANT_SQUARE_MARGIN);
      var v4=new THREE.Vector3(-GIANT_SQUARE_MARGIN,-1,this.sqDims[1]*this.SquareSize+GIANT_SQUARE_MARGIN);
      var ret=setupSquare(v1,v2,v3,v4,0xFFFFFF,false)
      return ret;
    },
    getSquare:function(loc){
      var index=~~(loc[0]/this.SquareSize)*this.sqDims[0]+~~(loc[1]/this.SquareSize);
      if(loc[0]>=0&&loc[1]>=0&&loc[0]<this.sqDims[0]*this.SquareSize&&loc[1]<this.sqDims[1]*this.SquareSize&&index>=0&&index<this.Squares.length){
        return this.Squares[index];
      }
    },
    getTerrInt:function(loc){
      var index=loc[0]*this.terrDims[0]+loc[1];
      if(index>=0&&index<this.Terrains.length){
        return this.Terrains[index];
      }
    },
    getTerr:function(loc){
      var index=~~(loc[0]/this.terrSize[0])*this.terrDims[0]+~~(loc[1]/this.terrSize[1]);
      if(loc[0]>=0&&loc[1]>=0&&loc[0]<this.terrDims[0]*this.terrSize[0]&&loc[1]<this.terrDims[1]*this.terrSize[1]&&index>=0&&index<this.Terrains.length){
        return this.Terrains[index];
      }
    },
    getCenter:function(){
      return [(this.terrDims[0]/2)*this.terrSize[0],(this.terrDims[1]/2)*this.terrSize[1]];
    },
    addThingType:function(thing,typeName){
      if(!this.things[typeName]){ this.things[typeName]=new LL(); }
      if(!this.thingMeshes[typeName]){ this.thingMeshes[typeName]=new LL(); }
      return [this.things[typeName].push(thing),this.thingMeshes[typeName].push(thing)];
    },
    addClickable:function(thingMesh){
      if(this.deadClickableSlots.length){
        var myClickableIndex=this.deadClickableSlots.pop();
        this.clickables[myClickableIndex]=thingMesh;
        return myClickableIndex;
      }
      else{
      this.clickables.push(thingMesh);
      return this.clickables.length-1;
      }
    },
    remThingType:function(entry,typeName){
      entry.rem();
    },
    remThingTypeMesh:function(entry,typeName){
    },
    moveThing:function(thing){
      thing.rem();
      var newSq=this.getSquare([thing.x,thing.y]);
      var newTerr=this.getTerr([thing.x,thing,y]);
      thing.sqElem=newSq.addthing(thing);
      thing.currSq=newSq;
      thing.currTerr=newTerr;
    },
    //  CanvasToWorldCoords:function(x,y){
    //    var WorldCoords={x:(x-this.x)/this.viewRatio,y:(this.sqY*this.SquareSize*this.viewRatio-(y-this.y))/this.viewRatio};
    //    if(this.getSquare(WorldCoords.x,WorldCoords.y)){ return WorldCoords; }
    //  },
    //  WorldToCanvasCoords:function(x,y){
    //    return {x:(this.x+(x*this.viewRatio)),y:this.y+this.sqY*this.SquareSize*this.viewRatio-y*this.viewRatio};
    //  },
    withinDist:function(retContainer,loc,dist){
      var distToThing=this.getDist(loc);
      if(distToThing<dist){
        retContainer.push(this);
      }
    },
    withinSquare:function(retContainer,bl,tr){
      if(this.loc[0]+this.radius>bl[0]&&this.loc[0]-this.radius<tr[0]&&this.loc[1]+this.radius>bl[1]&&this.loc[1]-this.radius<tr[1]){
        retContainer.push(this);
      }
    },
    getThingsWithinSquare:function(bl,tr,type,retContainer){
      var minX=~~(bl[0]/this.SquareSize);
      var maxX=~~(tr[0]/this.SquareSize);
      var minY=~~(bl[1]/this.SquareSize);
      var maxY=~~(tr[1]/this.SquareSize);
      for(var sqX=minX;sqX<=maxX;sqX++){
        for(var sqY=minY;sqY<=maxY;sqY++){
          var currSq=this.getSquareCoord([sqX,sqY]);
          if(currSq&&currSq.things[type]){
            currSq.things[type].apply(this.withinSquare,[retContainer,bl,tr]);
          }
        }
      }
    },
    getThingsWithinRadius:function(loc,radius,typeList,retContainer){
      var checkRadius=(this.SquareSize/2)+radius;
      var minX=~~((loc[0]-checkRadius)/this.SquareSize);
      var maxX=~~((loc[0]+checkRadius)/this.SquareSize);
      var minY=~~((loc[1]-checkRadius)/this.SquareSize);
      var maxY=~~((loc[1]+checkRadius)/this.SquareSize);
      for(var sqX=minX;sqX<=maxX;sqX++){
        for(var sqY=minY;sqY<=maxY;sqY++){
          var currSq=this.getSquareCoord([sqX,sqY]);
          for(var typeI in typeList){
            var type=typeList[typeI];
            if(currSq&&currSq.things[type]){
              currSq.things[type].apply(this.withinDist,[retContainer,loc,radius]);
            }
          }
        }
      }
    },
    insideWorldInt:function(p){
      if(p[0]>=0 && p[0]<this.terrDims[0]&&p[1]>=0 &&p[1]<this.terrDims[1]){
        return true
      }
      return false;
    },
    getWalkableInt:function(p){
      if(this.insideWorldInt(p)&&this.getTerrInt(p).walkable){
        return true;
      }
      return false;
    }
  };
  //#

  //$ TERRAIN INIT
  var Terrain=function(x,y,world,color,numTeams,walkable){
    this.x=x;
    this.y=y;
    this.factory=false;
    this.factoryOwner=-1;
    this.height=0;
    this.i=x*world.terrDims[0]+y;
    this.walkable=true;
    this.blockVis=false;
    this.visible=[];
    for(var i=0;i<numTeams;i++){
      this.visible.push(false);
    }
    this.world=world;
    this.color=[color[0],color[1],color[2]];
    var terrSize=this.world.terrSize;
    this.center=[terrSize[0]*(x+0.5),terrSize[1]*(y+0.5)];
    var v1=new THREE.Vector3(0,0,0);
    var v2=new THREE.Vector3(terrSize[0],0,0);
    var v3=new THREE.Vector3(terrSize[0],0,terrSize[1]);
    var v4=new THREE.Vector3(0,0,terrSize[1]);
    var loc=[terrSize[0]*this.x,0,terrSize[1]*this.y];
    var drawStuff=setupSquare(v1,v2,v3,v4,this.color,false,loc);
    this.geometry=drawStuff.geometry;
    this.material=drawStuff.material;
    this.mesh=drawStuff.mesh;
    this.mesh.myObj=this;
    this.setColor(this.color);
    this.world.scene.add(drawStuff.mesh);
  }
  //#

  //$ TERRAIN PROTO
  Terrain.prototype={
    setColor:function(color){
      this.color=[color[0],color[1],color[2]];
      this.mesh.material.color.set(new THREE.Color(color[0],color[1],color[2]));
    },
    addColor:function(dcolor){
      if(this.factory){
        console.log(this.color[0]+','+this.color[1]+','+this.color[2]);
        console.log(dcolor[0]+','+dcolor[1]+','+dcolor[2]);
        console.log(this.ID);
      }
      this.color[0]+=dcolor[0];
      this.color[1]+=dcolor[1];
      this.color[2]+=dcolor[2];
      this.mesh.material.color.set(new THREE.Color(this.color[0],this.color[1],this.color[2]));
    }
  };
  //#

  //$ SQUARE INIT
  var Square=function(x,y,world){
    this.x=x;
    this.y=y;
    this.world=world;
    this.things={};
  };
  //#

  //$ SQUARE PROTO
  Square.prototype={
    //  draw:function(ctx){
    //    ctx.fillStyle=this.color;
    //    var SqSize=this.world.SquareSize;
    //    var MyWorldCoords=this.world.WorldToCanvasCoords(this.x*SqSize,this.y*SqSize+SqSize);
    //    ctx.fillRect(MyWorldCoords.x,MyWorldCoords.y,SqSize*this.world.viewRatio,SqSize*this.world.viewRatio);
    //  },
    addThingType:function(thing,typeName){
      if(!this.things[typeName]){ this.things[typeName]=new LL(); }
      return this.things[typeName].push(thing);
    },
    remThingType:function(entry,typeName){
      entry.rem();
    },
    setupGeometry:function(color,scene){
      var sqSize=this.world.SquareSize;
      this.mesh.myObj=this;
      this.world.scene.add(this.mesh);
    }
    //  getThingsofType:function(typeNames){
    //    var shortest=Number.MAX_VALUE;
    //    var lookin;
    //    for(var i=0;i<typeNames.length;i++){
    //      var currLL=this.things[typeNames[i]];
    //      if(currLL){
    //        if(currLL.length<shortest){
    //          lookin=currLL;
    //        }
    //      }
    //      else{return false;}
    //    }
    //    lookin=new LL()
    //  }
  };
  //#

  //$ THING INIT
  var Thing=function(world,loc,radius,color,typesArr,moveCheck,clickable,flying,factory){
    this.factory=factory;
    this.flying=flying;
    this.world=world;
    this.ID=this.world.getNewID(this);
    this.loc=loc;
    this.radius=radius;
    if(this.radius>=this.world.SquareSize/2){
      console.error("Thing Radius too large!"+this);
    }
    this.color=[color[0],color[1],color[2]];
    this.typeEntries={};
    if(moveCheck){
      this.moveCheck=moveCheck;
    }
    this.currSq=this.world.getSquare(this.loc);
    this.currTerr=this.world.getTerr(this.loc);
    if(this.currSq&&this.moveCheck(this.currTerr)){
      for(var i=0;i<typesArr.length;i++){
        this.addType(typesArr[i]);
      }
      var drawStuff;
      if(!this.factory){
      drawStuff=setupSphere(this.radius,2,2,this.color,[this.loc[0],0,this.loc[1]]);
      }
      else{
      drawStuff=setupSphere(this.radius,5,5,this.color,[this.loc[0],0,this.loc[1]]);
      }
      this.mesh=drawStuff.mesh;
      this.material=drawStuff.material;
      this.geometry=drawStuff.geometry;
      this.mesh.myObj=this;
      if(flying){
        this.mesh.position.y=10;
      }
      this.visible=false;
      this.setColor(this.color);
      if(clickable){
        this.clickableIndex=this.world.addClickable(this.mesh);
      }
    }
    else{console.error("Thing unplaceable! x:"+this.loc[0]+" y:"+this.loc[1]);}
  };
  //#

  //$ THING FUNCTIONS
  Thing.prototype={
    getDist:function(loc){
      var xcomp=this.loc[0]-loc[0];
      var ycomp=this.loc[1]-loc[1];
      return (Math.sqrt(xcomp*xcomp,ycomp*ycomp))-this.radius;
    },
    setColor:function(color){
      this.color[0]=color[0];
      this.color[1]=color[1];
      this.color[2]=color[2];
      this.mesh.material.color.set(new THREE.Color(color[0],color[1],color[2]));
    },
    addColor:function(dcolor){
      this.color[0]+=dcolor[0];
      this.color[1]+=dcolor[1];
      this.color[2]+=dcolor[2];
      this.mesh.material.color.set(new THREE.Color(this.color[0],this.color[1],this.color[2]));
    },
    //  draw:function(ctx){
    //    var myCanvCoords=this.world.WorldToCanvasCoords(this.x,this.y);
    //    ctx.fillStyle=this.color;
    //    ctx.beginPath();
    //    ctx.arc(myCanvCoords.x,myCanvCoords.y,this.radius*this.world.viewRatio,0,2*Math.PI);
    //    ctx.fill();
    //  },
    addType:function(typeName){//adds and removes types to and from thing!
      if(this.typeEntries[typeName]){
        console.error("thing already has type being added, Thing: "+this+" typeName: "+typeName);
      }
      else{
        worldEntryandMesh=this.world.addThingType(this,typeName);
        this.typeEntries[typeName]={SquareEntry:this.currSq.addThingType(this,typeName),WorldEntry:worldEntryandMesh[0],meshEntry:worldEntryandMesh[1]};
      }
    },
    remType:function(typeName){
      if(!this.typeEntries[typeName]){console.error("type being removed does not exist, Thing: "+this+" typeName: "+typeName);}
      else{
        this.currSq.remThingType(this.typeEntries[typeName].SquareEntry,typeName);
        this.world.remThingType(this.typeEntries[typeName].WorldEntry,typeName);
        this.world.remThingTypeMesh(this.typeEntries[typeName].MeshEntry,typeName);
      }
    },
    die:function(){
      for(var entry in this.typeEntries){
        this.remType(entry);
      }
      this.world.scene.remove(this.mesh);
      this.mesh.material.dispose();
      this.mesh.geometry.dispose();
      if(this.clickableIndex>=0){
        this.world.clickables[this.clickableIndex]=null;
      }
      if(this.factory){
        if(this.activating){
          this.player.activating=undefined;
        }
        this.world.getTerr(this.loc).factoryOwner=-1;
      }
      this.alive=false;
    },
    moveCheck:function(nextTerr,flying){//default to check if next square is walkable
      if(nextTerr.walkable||flying){ return true; }
      return false;
    },
    reveal:function(){
      this.visible=true;
      this.world.scene.add(this.mesh);
    },
    hide:function(){
      this.visible=false;
      this.world.scene.remove(this.mesh);
    },
    put:function(loc){//used to replace things
      var nextTerr=this.world.getTerr(loc);
      if(nextTerr&&nextTerr!=this.currTerr){
        if(!this.moveCheck(nextTerr,this.flying)){
          return false;
        }
      }
      var nextSq=this.world.getSquare(loc);
      if(nextSq&&nextSq===this.currSq){
        this.loc[0]=loc[0];
        this.loc[1]=loc[1];
        this.mesh.position.x=this.loc[0];
        this.mesh.position.z=this.loc[1];
        return true;
      }
      if(nextSq&&nextSq!=this.currSq){
        this.loc[0]=loc[0];
        this.loc[1]=loc[1];
        this.currSq=nextSq;
        for(var entry in this.typeEntries){
          this.typeEntries[entry].SquareEntry.rem();
          this.typeEntries[entry].SquareEntry=this.currSq.addThingType(this,entry);
        }
        return true;
      }
      return false;
    },
    move:function(delta){//specifically used for thing motion
      //need to edit to make units move fast around obstacles
      var thisMag;
      if(this.put([this.loc[0]+delta[0],this.loc[1]+delta[1]])){
        return;
      }
      var mag=Math.sqrt(delta[0]*delta[0]+delta[1]*delta[1]);
      if(Math.abs(delta[0])>0){
        thisMag= delta[0]>0 ? mag : -mag;
        if(this.put([this.loc[0]+thisMag,this.loc[1],this.moveCheck])){
          return;
        }
      }
      if(Math.abs(delta[1])>0){
        thisMag= delta[1]>0 ? mag : -mag;
        if(this.put([this.loc[0],this.loc[1]+thisMag,this.moveCheck])){
          return;
        }
      }
      //console.error("A unit was unable to move!");
    }
  };
  //#

  //$ MESH FUNCTIONS
  var setupSphere=function(radius,wSegments,hSegments,color,loc){
    var ret={};
    ret.geometry=new THREE.SphereGeometry(radius,wSegments,hSegments,0,Math.PI*2,0,Math.PI/2);
    ret.material=new THREE.MeshBasicMaterial({color:color});
    ret.mesh=new THREE.Mesh(ret.geometry,ret.material);
    ret.mesh.position.set(loc[0],loc[1],loc[2]);
    return ret;
  };

//  var generateCircularSprite=function(color,radius){
//    var canvas=document.createElement('canvas');
//    canvas.height=100;
//    canvas.width=100;
//    var context=canvas.getContext('2d');
//    var centerX=50;
//    var centerY=50;
//    var radius=radius;
//    ctx.beginPath();
//    ctx.
//  };

  var setupSprite=function(radius,color,loc){
    var ret=new THREE.Sprite(new THREE.SpriteMaterial({map:new THREE.Texture(generateCircularSprite(color))}));
  };

  var setupSquare=function(v1,v2,v3,v4,color,empty,loc,dirty){
    var ret={};
    ret.geometry=new THREE.Geometry();
    if(dirty){
      ret.geometry.__dirtyVirtices=true;
      ret.geometry.dynamic=true;
    }
    ret.geometry.vertices.push(v1);
    ret.geometry.vertices.push(v2);
    ret.geometry.vertices.push(v3);
    ret.geometry.vertices.push(v4);
    if(empty){
      ret.geometry.vertices.push(v1);
      ret.material=new THREE.LineBasicMaterial({
        color:color
      });
      ret.mesh=new THREE.Line(ret.geometry,ret.material);
    }
    else{
      ret.geometry.faces.push(new THREE.Face3(0,1,2));
      ret.geometry.faces.push(new THREE.Face3(0,2,3));
      ret.material=new THREE.MeshBasicMaterial({
        color:color,
        side:THREE.DoubleSide
      });
      ret.mesh=new THREE.Mesh(ret.geometry,ret.material);
    }
    if(loc){
      ret.mesh.position.set(loc[0],loc[1],loc[2]);
    }
    return ret;
  };
  //#
