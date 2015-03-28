var url_path="/"; // http://robotmoose.com/";

function load_js(js)
{
	var scr=document.createElement("script");
	scr.src=js;
	document.head.appendChild(scr);
};

function load_dependencies()
{
	load_js(url_path+"js/3d.js");
};

(function(){load_dependencies()})();

function roomba_t(renderer)
{
	var myself=this;

	myself.model=new Array();

	myself.left=0;
	myself.right=0;

	var model_path=url_path+"models/roomba/body.obj";
	myself.model=null;
	var model_color=0xcc0000;
	
	// Positions of Left and Right wheels
	this.wheelbase=250; // mm
	this.wheel=[];
	this.wheel[0]=new vec3(-0.5*this.wheelbase,0,0);
	this.wheel[1]=new vec3(+0.5*this.wheelbase,0,0);
	// Robot coordinate system:
	this.P=new vec3(0,0,0); // position
	this.UP=new vec3(0,0,1); // Z is up
	this.LR=new vec3(1,0,0); // left-to-right wheel
	this.FW=new vec3(0,1,0); // drive forward

	myself.model=renderer.load_obj(model_path);
	myself.model.rotation.set(0,0,0);
	myself.model.position.set(0,0,0);
	myself.model.scale.set(1,1,1);
	myself.model.set_color(model_color);
	myself.model.castShadow=true;
	myself.model.receiveShadow=true;

	myself.set_position=function(x,y,z)
	{
		myself.model.position.set(x,y,z);
	};

	myself.get_position=function()
	{
		return myself.model.position;
	};

	myself.set_rotation=function(x,y,z)
	{
		myself.model.rotation.set(x,y,z);
	};

	myself.get_rotation=function()
	{
		return myself.model.rotation;
	};

	myself.drive=function(left,right)
	{
		myself.left=left;
		myself.right=right;
	};
};


roomba_t.prototype.loop=function(dt)
{
	dt = dt || 0.001; // weird startup, null dt?
	var mm=1; // from mm to model coordinates

	// Limit motor power
	var limit=function (power) {
		if (power>500) return 500;
		if (power<-500) return -500;
		return power;
	}
	var L=limit(this.left);
	var R=limit(this.right);
	
	var speed=mm*dt; // world units per step
	
	// Drive wheels forward
	this.wheel[0].pe(this.FW.t(L*speed));
	this.wheel[1].pe(this.FW.t(R*speed));

	// Enforce wheelbase
	this.P=this.wheel[0].p(this.wheel[1]).te(0.5);
	this.LR=this.wheel[1].m(this.wheel[0]); // left-to-right
	this.LR.normalize();
	this.FW=new vec3(-this.LR.y,this.LR.x,0.0); // forward
	var wheeloff=this.LR.clone();
	wheeloff.te(this.wheelbase*0.5);
	this.wheel[0]=this.P.m(wheeloff);
	this.wheel[1]=this.P.p(wheeloff);
	

	// Robot's Z rotation rotation, in radians
	var orient=Math.atan2(this.LR.y,this.LR.x);
	// console.log("P="+this.P+" and orient="+orient+" rads");
	this.model.rotation.z=orient;

	// Update robot center position
	this.model.position.copy(this.P); 

};

