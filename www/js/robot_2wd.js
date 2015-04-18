/*
This file has utility routines to support a 2-wheel drive robot:
	- Two wheel locations (3D points)
	- A wheelbase
	- Functions to drive the wheels
	- Functions to access points in the robot's coordinate system
*/

/* Tracks the wheel positions onscreen */
function wheel_tracker_t(renderer,lineOptions)
{
	this.max=10000; // maximum vertex count
	this.cur=0; // current vertex number

	this.geom=new THREE.Geometry();
	/* for some reason THREE.js doesn't let you change
	   vertex counts, so preallocate lots of vertices. */
	for (var x=0;x<this.max;x++)
	{
		this.geom.vertices.push(new vec3(0,0,0));
	}

	this.line=new THREE.Line(this.geom,
	  new THREE.LineBasicMaterial(lineOptions)
	);
	renderer.scene.add(this.line);
}

/* Add a new wheel location to this chart. */
wheel_tracker_t.prototype.add=function(pos) {
  if (!this.last || pos.m(this.last).length()>10.0) { // moved enough to be visible:
    if (this.cur<this.max) { // still have space to write
	// stupid: write all future vertices as this point
	for (var i=this.cur++;i<this.max;i++)
	     this.geom.vertices[i]=pos;
	this.geom.verticesNeedUpdate=true;
	this.geom.computeBoundingSphere();
    }
    this.last=pos;
  }
}

wheel_tracker_t.prototype.reset=function() {
  this.cur=0;
  this.last=undefined;
}

/**
  This class is the superclass of robots with two wheel tank drive.
*/
function robot_2wd(wheelbase) {
	this.wheelbase=wheelbase;
	this.wheel=[];
	this.reset_wheels();
}


// Reset positions of wheels:
robot_2wd.prototype.reset_wheels=function()
{
	this.wheel[0]=new vec3(0.0,-0.5*this.wheelbase,0.01);
	this.wheel[1]=new vec3(0.0,+0.5*this.wheelbase,0.01);
	
	// Robot coordinate system:
	this.P=new vec3(0,0,0); // position of robot (between wheels)
	this.UP=new vec3(0,0,1); // Z is up
	this.LR=new vec3(0,-1,0); // left-to-right wheel
	this.FW=new vec3(1,0,0); // drive forward
	
	this.drive_wheels(0.0,0.0); // set up coordinates
}

// Drive the robot's wheels this far.
robot_2wd.prototype.drive_wheels=function(L,R)
{	
	// Drive wheels forward
	this.wheel[0].pe(this.FW.t(L));
	this.wheel[1].pe(this.FW.t(R));

	// Enforce wheelbase
	this.P=this.wheel[0].p(this.wheel[1]).te(0.5);
	this.LR=this.wheel[1].m(this.wheel[0]); // left-to-right
	this.LR.normalize();
	this.FW=new vec3(0,0,1).cross(this.LR).normalize();  // forward
	var wheeloff=this.LR.clone();
	wheeloff.te(this.wheelbase*0.5);
	this.wheel[0]=this.P.m(wheeloff);
	this.wheel[1]=this.P.p(wheeloff);
	
	// Robot's Z rotation rotation, in radians
	this.angle_rad=Math.atan2(this.FW.y,this.FW.x);
	this.angle=180.0/Math.PI*this.angle_rad;
	// console.log("Robot_2wd P="+this.P+" mm and angle="+this.angle+" degrees");
}


// Return the world-coordinates XYZ of this robot polar coordinates position
robot_2wd.prototype.world_from_robot=function(radius,angle_rad,height)
{
	var W=new vec3(
		radius*Math.cos(angle_rad+this.angle_rad),
		radius*Math.sin(angle_rad+this.angle_rad),
		height || 0.0
	);
	W.pe(this.P); // robot-relative
	return W;
}



