(function(){var imported=document.createElement("script");imported.src="sprite.js";document.head.appendChild(imported);})();

var block_t=function(x,y)
{
	this.x=x;
	this.y=y;
	this.spr=new sprite_t("block.png",1);

	this.draw=function(simulation)
	{
		if(simulation)
		{
			simulation.ctx.save();
			simulation.ctx.translate(this.x,this.y);
			this.spr.draw(simulation);
			simulation.ctx.restore();
		}
	};
};