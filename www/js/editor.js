function load_js(js)
{
	var scr=document.createElement("script");
	scr.src=js;
	document.head.appendChild(scr);
};

function load_link(link)
{
	var ln=document.createElement("link");
	ln.rel="stylesheet";
	ln.href=link;
	document.head.appendChild(ln);
};

function load_style(style)
{
	var sty=document.createElement("style");
	sty.appendChild(document.createTextNode(style));
	document.head.appendChild(sty);
};

function load_dependencies()
{
	load_js("http://robotmoose.com/js/xmlhttp.js");
	load_js("http://robotmoose.com/js/codemirror/clike_arduino_nxt.js");
	load_js("http://robotmoose.com/js/codemirror/addon/edit/matchbrackets.js");
	load_js("http://robotmoose.com/js/codemirror/addon/dialog/dialog.js");
	load_js("http://robotmoose.com/js/codemirror/addon/search/search.js");
	load_js("http://robotmoose.com/js/codemirror/addon/search/searchcursor.js");
	load_link("http://robotmoose.com/js/codemirror/codemirror.css");
	load_link("http://robotmoose.com/js/codemirror/addon/dialog/dialog.css");
	load_style(".CodeMirror{border:1px solid #000000;}");
	load_style(".lint-error{background:#ff8888;color:#a00000;padding:1px}\r\n.lint-error-icon{background:#ff0000;color:#ffffff;border-radius:50%;margin-right:7px;}");
};

(function(){load_dependencies()})();

function editor_t()
{
	var myself=this;
	myself.div=null;
	myself.statusarea=null;
	myself.textarea=null;
	myself.widgets=[];
	myself.editor=null;
	myself.compilable=false;
	myself.timeout=null;
	myself.compile_time=1000;

	myself.on_change=null;
	myself.on_compiled=null;

	myself.create=function(div)
	{
		if(div)
		{
			myself.div=div;
			myself.div.innerHTML="";

			myself.textarea=document.createElement("textarea");
			myself.set_status("");
			myself.div.appendChild(myself.textarea);

			myself.statusarea=document.createElement("div");
			myself.statusarea.innerHTML=" ";
			myself.div.appendChild(myself.statusarea);

			myself.editor=CodeMirror.fromTextArea(myself.textarea,
				{indentUnit:4,indentWithTabs:true,lineNumbers:true,matchBrackets:true,mode:"text/x-arduino"});

			myself.editor.on("change",myself.interval_reset);
			myself.compilable=true;

			if(myself.editor)
				return true;
			else
				myself.destroy();
		}

		return false;
	};

	myself.interval_reset=function()
	{
		if(myself.editor&&myself.compilable)
		{
			myself.clear_errors();
			myself.set_status("");

			if(myself.timeout)
			{
				window.clearTimeout(myself.timeout);
				myself.timeout=null;
			}

			myself.timeout=setTimeout(myself.compile,myself.compile_time);

			if(myself.on_change)
				myself.on_change();
		}
	};

	myself.destroy=function()
	{
		myself.clear_errors();
		myself.div=null;
		myself.statusarea=null;
		myself.textarea=null;
		myself.editor=null;
		myself.compilable=false;

		if(myself.timeout)
		{
			window.clearTimeout(myself.timeout);
			myself.timeout=null;
		}
	};

	myself.set_status=function(text)
	{
		if(myself.statusarea)
		{
			if(text.length==0)
				myself.statusarea.innerHTML="&nbsp;";
			else
				myself.statusarea.innerHTML=text;
		}
	};

	myself.set_value=function(value)
	{
		if(myself.editor&&value)
			myself.editor.setValue(value);
	};

	myself.get_value=function()
	{
		return myself.editor.getValue();
	};

	myself.set_size=function(width,height)
	{
		if(myself.editor)
		{
			if(width)
				myself.editor.setSize(width,null);

			if(height)
				myself.editor.setSize(null,height);
		}
	};

	myself.clear_errors=function()
	{
		if(myself.editor)
		{
			for(var ii=0;ii<myself.widgets.length;++ii)
				myself.editor.removeLineWidget(myself.widgets[ii]);
			myself.widgets.length=0;
		}
	};

	myself.add_error=function(line,text)
	{
		if(myself.editor)
		{
			var msg=document.createElement("div");
			var icon=msg.appendChild(document.createElement("span"));
			icon.innerHTML="!!";
			icon.className="lint-error-icon";
			msg.appendChild(document.createTextNode(text));
			msg.className="lint-error";

			var line_number=line-1;

			if(line_number<0)
				line_number=myself.editor.lineCount()-1;

			myself.widgets.push(myself.editor.addLineWidget(line_number,msg,{coverGutter:false,noHScroll:true}));
		}
	};

	myself.compile=function()
	{
		if(myself.editor&&myself.compilable)
		{
			myself.set_status("Compiling...");
			send_request("POST","http://robotmoose.com/editor","code","",myself.compile_response,myself.error_response,
				myself.get_value(),"application/octet-stream");
		}
	};

	myself.compile_response=function(response)
	{
		if(myself.editor)
		{
			myself.set_status("");

			try
			{
				var json=JSON.parse(response);

				if(json)
				{
					myself.clear_errors();

					for(var ii=0;ii<json.errors.length;++ii)
						myself.add_error(json.errors[ii].line,json.errors[ii].text);

					if(myself.on_compiled&&json.errors.length==0)
						myself.on_compiled();
				}
			}
			catch(e)
			{
				console.log(response);
				console.log(e);
				myself.set_status("Server error...");
			}
		}
	};

	myself.error_response=function(error_code)
	{
		if(myself.editor)
			myself.set_status("Server error ("+error_code+")");
	};
}
