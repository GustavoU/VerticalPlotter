//we page homing

const char homing[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
	<head>
		<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
		<title>Plotter Config</title>
		
		<script type="text/javascript">
			var Boton = (function(container, parameters)
			{
				parameters = parameters || {};
				var title = (typeof parameters.title === "undefined" ? "botonazo" : parameters.title),
				width = (typeof parameters.width === "undefined" ? 0 : parameters.width),
				height = (typeof parameters.height === "undefined" ? 0 : parameters.height);
	  
				// Create Canvas element and add it in the Container object
				var objContainer = document.getElementById(container);
				var canvas = document.createElement("canvas");
				canvas.id = title;
				if(width === 0) { width = objContainer.clientWidth; }
				if(height === 0) { height = objContainer.clientHeight; }
				canvas.width = width;
				canvas.height = height;
				objContainer.appendChild(canvas);
				
	  
				var pressed = 0; // Bool - 1=Yes - 0=No
				

				// Check if the device support the touch or not
				if("ontouchstart" in document.documentElement)
				{
					canvas.addEventListener("touchstart", onTouchStart, false);
					canvas.addEventListener("touchend", onTouchEnd, false);
				}
				else
				{
					canvas.addEventListener("mousedown", onMouseDown, false);
					canvas.addEventListener("mouseup", onMouseUp, false);
				}
				// Draw the object
				var context=canvas.getContext("2d");
				
				

				/**
				* @desc Events for manage touch
				*/
				function onTouchStart(event) 
				{
					pressed = 1;
					objContainer.style.background = "red";					
				}

				function onTouchEnd(event) 
				{
					pressed = 0;
					objContainer.style.background = "#4CAF50";	
				}

				/**
				* @desc Events for manage mouse
				*/
				function onMouseDown(event) 
				{
					pressed = 1;
					objContainer.style.background = "red";	
				}

				function onMouseUp(event) 
				{
					pressed = 0;
					objContainer.style.background = "#4CAF50";	
					
				}

				/******************************************************
				* Public methods
				*****************************************************/

				/**
				* @desc The width of canvas
				* @return Number of pixel width 
				*/
				this.GetWidth = function () 
				{
					return canvas.width;
				};

				/**
				* @desc The height of canvas
				* @return Number of pixel height
				*/
				this.GetHeight = function () 
				{
					return canvas.height;
				};

				

				this.GetPress = function(){
					return pressed;
				};
				  
			});
		</script>
		<style>
			div{
				box-sizing: border-box;
				border-radius:10px;
			}
			div.motor{
				background-color:#4E9AF1;
				padding:0.75em 1em;
				border-radius:1em;
				
				color:#FFFFFF;
				margin:auto;
				font-family:'Roboto',sans-serif;
font-size:14px;
				box-sizing: border-box;
			}
			.speed
		{
			font-family:'Roboto',sans-serif;
			border-radius:10px;
			background:#CCCCCC;
			padding:1em;
			width:17em;
			height:8em;
			margin:auto;
			text-align:center;
			font-weight: bold;
			font-size:20px;
		}
			a.button2{
				display:inline-block;
				vertical-align:middle;
				padding:0.75em 1.5em;
				margin:0.1em 0.1em 0.1em 0;
				border:0.16em solid rgba(255,255,255,0);
				border-radius:1em;
				box-sizing: border-box;
				text-decoration:none;
				font-family:'Roboto',sans-serif;
				font-weight:500;
				color:#FFFFFF;
				background-color:#4E9AF1;
				text-shadow: 0 0.04em 0.04em rgba(0,0,0,0.35);
				text-align:center;
				transition: all 0.2s;
				cursor: pointer;
			}
			
			a.button2:hover{
				border-color: black
			}
			@media all and (max-width:30em){
				a.button2{
					display:block;
					margin:0.2em auto;
				}
			}
			#botonazo
			{
				box-sizing: border-box;
				width:120px;
				height:40px;

				cursor: pointer;position: relative;
			}
			.neo_h2{
				line-height:20px;
				font-size:22px;
				font-family:'Roboto',sans-serif;
				font-weight:bold;
				margin-top:25px;
			}
			
		</style>
	</head>
	<body style="text-align:center;">
		
     	<h2 class="neo_h2">Homing</h2>
		<div>
			<div style="display:inline-flex;margin:10px;">
				<div style="display:inline-block;margin:10px;">
					<h2 class="neo_h2">MOTOR 1</h2>
					<div class="motor">
						<h2>ENROLLAR</h2><div id="M1_E" class="motor" style="background-color:#4CAF50;border:3px solid black;"></div>
						<h2>DESENROLLAR</h2><div id="M1_D" class="motor" style="background-color:#4CAF50;border:3px solid black;"></div>
					</div>
				</div>
				<div style="display:inline-block;margin:10px;" >
					<h2 class="neo_h2">MOTOR 2</h2>
					<div class="motor">
						<h2>ENROLLAR</h2><div id="M2_E" class="motor" style="background-color:#4CAF50;border:3px solid black;"></div>
						<h2>DESENROLLAR</h2><div id="M2_D" class="motor" style="background-color:#4CAF50;border:3px solid black;"></div>
					</div>
				</div>
				</br>
			</div>
			</div>
				<div class="speed" style="display:inline-block" >
				Velocidad
				<div style="display:inline-flex;">
					<a id="masVel" class= "button2" style="background-color:#4CAF50;width:4em;height:4em;margin:auto;padding:1em;font-weight:bold;" onclick="incVel()">+</a></br>
					<a id="velocidad" class="button2" style="background:white;color:black;font-size:22px;padding:1em;margin:1em;border-radius:1em;">1</a></br>
					<a id="minVel" class= "button2" style="background-color:#4CAF50;width:4em;height:4em;margin:auto;padding:1em;font-weight:bold;" onclick="redVel()"> - </a></br>
				</div>
				</div></br>
			<div style="margin:auto;">
				<a id="btHoming" class="button2"  onclick="incHoming()" style="font-size:25px;width:10em;" >Inicio</a></br>
				<a href="/plotter_params" class="button2" style="background-color:#4CAF50;width:10em;">Par&aacutemetros </a></br>
				<a href="/" class="button2" style="background-color:#4CAF50;width:10em;">Joystick</a></br>
				<a href="/canvas_task" class="button2" style="background-color:#4CAF50;width:10em;">Canvas config</a>
			</div>
		
		<script type="text/javascript">
			
						
			// Create JoyStick object into the DIV 'joystickDiv'
			var BotM1_E = new Boton("M1_E");
			var BotM1_D = new Boton("M1_D");
			var BotM2_E = new Boton("M2_E");
			var BotM2_D = new Boton("M2_D");
			var intervalo = 100;
			var homing = "OFF";
			var counter =0;
			var vel = 5;
			var homingStatus=0;
			var homingState=0;
			var posM1=100;
			var posM2=100;
			var flag1=0;
			setInterval(function(){sendJoyAxis();getVel();}, intervalo);
			setInterval(function (){getHoming();}, 500);
				
			function incVel(){
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function() {
					if (this.readyState == 4 && this.status == 200) {
						document.getElementById("velocidad").innerHTML= this.responseText;
					}
				};
				xhttp.open("GET", "/incVel", true);
				xhttp.send();
			}
			function redVel(){
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function() {
					if (this.readyState == 4 && this.status == 200) {
						document.getElementById("velocidad").innerHTML= this.responseText;
					}
				};
				xhttp.open("GET", "/redVel", true);
				xhttp.send();
			}
			function getVel(){
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function() {
					if (this.readyState == 4 && this.status == 200) {
						document.getElementById("velocidad").innerHTML=  this.responseText;
					}
				};
				xhttp.open("GET", "/getVel", true);
				xhttp.send();
			}
			function sendJoyAxis() {
				if(homingState>0){ 
					var P_M1_E = BotM1_E.GetPress();
					var P_M1_D = BotM1_D.GetPress();
					var P_M2_E = BotM2_E.GetPress();
					var P_M2_D = BotM2_D.GetPress();

					if(posM1 != 100 || posM2 != 100 ){
						posM1=100;
						posM2=100;
						flag1=1;
					}

					if(P_M1_E == 1)	
					{
						posM1 = 50;
						
						flag1=1;
					}
					else if(P_M1_D == 1)	
					{
						posM1 = 150;
						
						flag1=1;
					}
					if(P_M2_E == 1)	
					{
						
						posM2 = 50;
						flag1=1;
					}
					else if(P_M2_D == 1)	
					{
						
						posM2 = 150;
						flag1=1;
					}
					if(flag1==1 ){
						var xhr = new XMLHttpRequest();
						xhr.open("GET", "/homing?HX="+posM1+"&HY="+posM2, true); 
						xhr.send();
						flag1=0;
					}
				}
			}

			function getHoming(){
				var xhttp = new XMLHttpRequest();
			  	xhttp.onreadystatechange = function() {
					if (this.readyState == 4 && this.status == 200) {
				  		homingState = this.responseText;
						if(homingState==0){ 		//Inicio
							document.getElementById("btHoming").innerHTML = "Inicio";
							document.getElementById("btHoming").style.background = "#4E9AF1";
						}else if(homingState==1){	//Fin
							document.getElementById("btHoming").innerHTML = "Fin";
							document.getElementById("btHoming").style.background = "#DD1100";
						}
					}
			  	};
			  	xhttp.open("GET", "/getHomingStatus", true);
			  	xhttp.send();
			}

			function homingAction(element) {
				var xhr = new XMLHttpRequest();
				if(element.innerHTML == "INICIO"){ 
					xhr.open("GET", "/action?action=homing&state=1", true);
					element.innerHTML = "FIN";
					element.style.background = "#DD1100";
				}
				else {
					element.innerHTML = "INICIO";
					xhr.open("GET", "/action?action=homing&state=0", true);
					element.style.background = "#4E9AF1";
				}
				xhr.send();
			}

			function incHoming(){
				var xhttp = new XMLHttpRequest();
				var homingState=0;
			  	xhttp.onreadystatechange = function() {
					if (this.readyState == 4 && this.status == 200) {
				  		homingState = this.responseText;
						if(homingState==0){ 		//Inicio
							document.getElementById("btHoming").innerHTML = "Inicio";
							document.getElementById("btHoming").style.background = "#4E9AF1";
						}else if(homingState==1){	//Fin
							document.getElementById("btHoming").innerHTML = "Fin";
							document.getElementById("btHoming").style.background = "#DD1100";
						}
					}
			  	};
			  	xhttp.open("GET", "/incHomingStatus", true);
			  	xhttp.send();
			}
			
			
			
		</script>	
	</body>
</html>
)rawliteral";
const char canvasing[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
	<head>
		<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
		<title>Plotter Config</title>
		
		<script type="text/javascript">
			var Boton = (function(container, parameters)
			{
				parameters = parameters || {};
				var title = (typeof parameters.title === "undefined" ? "botonazo" : parameters.title),
				width = (typeof parameters.width === "undefined" ? 0 : parameters.width),
				height = (typeof parameters.height === "undefined" ? 0 : parameters.height);
	  
				// Create Canvas element and add it in the Container object
				var objContainer = document.getElementById(container);
				var canvas = document.createElement("canvas");
				canvas.id = title;
				if(width === 0) { width = objContainer.clientWidth; }
				if(height === 0) { height = objContainer.clientHeight; }
				canvas.width = width;
				canvas.height = height;
				objContainer.appendChild(canvas);
				
	  
				var pressed = 0; // Bool - 1=Yes - 0=No
				

				// Check if the device support the touch or not
				if("ontouchstart" in document.documentElement)
				{
					canvas.addEventListener("touchstart", onTouchStart, false);
					canvas.addEventListener("touchend", onTouchEnd, false);
				}
				else
				{
					canvas.addEventListener("mousedown", onMouseDown, false);
					canvas.addEventListener("mouseup", onMouseUp, false);
				}
				// Draw the object
				var context=canvas.getContext("2d");
				
				

				/**
				* @desc Events for manage touch
				*/
				function onTouchStart(event) 
				{
					pressed = 1;
					objContainer.style.background = "red";					
				}

				function onTouchEnd(event) 
				{
					pressed = 0;
					objContainer.style.background = "#4CAF50";	
				}

				/**
				* @desc Events for manage mouse
				*/
				function onMouseDown(event) 
				{
					pressed = 1;
					objContainer.style.background = "red";	
				}

				function onMouseUp(event) 
				{
					pressed = 0;
					objContainer.style.background = "#4CAF50";	
					
				}

				/******************************************************
				* Public methods
				*****************************************************/

				/**
				* @desc The width of canvas
				* @return Number of pixel width 
				*/
				this.GetWidth = function () 
				{
					return canvas.width;
				};

				/**
				* @desc The height of canvas
				* @return Number of pixel height
				*/
				this.GetHeight = function () 
				{
					return canvas.height;
				};

				

				this.GetPress = function(){
					return pressed;
				};
				  
			});
		</script>
		<style>
			div{
				box-sizing: border-box;
				border-radius:10px;
			}
			div.motor{
				background-color:#4E9AF1;
				padding:0.75em 1em;
				border-radius:1em;
				
				color:#FFFFFF;
				margin:auto;
				font-family:'Roboto',sans-serif;
				font-size:14px;
				box-sizing: border-box;
			}
			
			a.button2{
				display:inline-block;
				padding:0.75em 1.5em;
				margin:0.1em 0.1em 0.1em 0;
				border:0.16em solid rgba(255,255,255,0);
				border-radius:1em;
				box-sizing: border-box;
				text-decoration:none;
				font-family:'Roboto',sans-serif;
				font-weight:500;
				color:#FFFFFF;
				background-color:#4E9AF1;
				text-shadow: 0 0.04em 0.04em rgba(0,0,0,0.35);
				text-align:center;
				transition: all 0.2s;
				cursor: pointer;
			}
			
			a.button2:hover{
				border-color: black
			}
			@media all and (max-width:30em){
				a.button2{
					display:block;
					margin:0.2em auto;
				}
			}
			#botonazo
			{
				box-sizing: border-box;
				width:120px;
				height:40px;

				cursor: pointer;position: relative;
			}
			.neo_h2{
				line-height:20px;
				font-size:22px;
				font-family:'Roboto',sans-serif;
				font-weight:bold;
				margin-top:25px;
			}
			.speed
			{
				font-family:'Roboto',sans-serif;
				border-radius:10px;
				background:#CCCCCC;
				padding:1em;
				width:17em;
				height:8em;
				margin:auto;
				text-align:center;
				font-weight: bold;
				font-size:20px;
			}
		</style>
	</head>
	<body style="text-align:center;">
		
     	<h2 class="neo_h2">Canvas config</h2>
		<div>
			<div style="display:inline-flex;margin:10px;">
				<div style="display:inline-block;margin:10px;">
					<h2 class="neo_h2">MOTOR 1</h2>
					<div class="motor">
						<h2>ENROLLAR</h2><div id="M1_E" class="motor" style="background-color:#4CAF50;border:3px solid black;"></div>
						<h2>DESENROLLAR</h2><div id="M1_D" class="motor" style="background-color:#4CAF50;border:3px solid black;"></div>
					</div>
				</div>
				<div style="display:inline-block;margin:10px;" >
					<h2 class="neo_h2">MOTOR 2</h2>
					<div class="motor">
						<h2>ENROLLAR</h2><div id="M2_E" class="motor" style="background-color:#4CAF50;border:3px solid black;"></div>
						<h2>DESENROLLAR</h2><div id="M2_D" class="motor" style="background-color:#4CAF50;border:3px solid black;"></div>
					</div>
				</div>
				</br>
			</div>
			<div></div></div>
			<div class="speed" style="display:inline-block" >
				Velocidad
				<div style="display: inline-flex;">
					<a id="masVel" class= "button2" style="background-color:#4CAF50;width:4em;height:4em;margin:auto;padding:1em;font-weight:bold;" onclick="incVel()">+</a></br>
					<a id="velocidad" class="button2" style="background:white;color:black;font-size:22px;padding:1em;margin:1em;border-radius:1em;">1</a></br>
					<a id="minVel" class= "button2" style="background-color:#4CAF50;width:4em;height:4em;margin:auto;padding:1em;font-weight:bold;" onclick="redVel()"> - </a></br>
				</div>
				</div></br>
				<div style="margin:auto;">
				<a id="btCanvas" class="button2" style="font-size:30px;" onclick="incCanvasing()">INICIO</a></br>
				<a href="/plotter_params" class="button2" style="background-color:#4CAF50;width:10em;">Par&aacutemetros </a></br>
				<a href="/homing_task" class="button2" style="background-color:#4CAF50;width:10em;">Homing</a></br>
				<a href="/" class="button2" style="background-color:#4CAF50;width:10em;">Joystick</a>
				</div>
			
		
		<script type="text/javascript">
					
			// Create JoyStick object into the DIV 'joystickDiv'
			var BotM1_E = new Boton("M1_E");
			var BotM1_D = new Boton("M1_D");
			var BotM2_E = new Boton("M2_E");
			var BotM2_D = new Boton("M2_D");
			var intervalo = 100;
			var counter =0;
			var vel = 5;
			var canvasState=0;
			var posM1=100;
			var posM2=100;
			var flag1=0;
			setInterval(function(){sendJoyAxis();getVel();}, intervalo);
			setInterval(function (){getCanvasing();}, 500);
			
			function incVel(){
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function() {
					if (this.readyState == 4 && this.status == 200) {
						document.getElementById("velocidad").innerHTML= this.responseText;
					}
				};
				xhttp.open("GET", "/incVel", true);
				xhttp.send();
			}
			function redVel(){
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function() {
					if (this.readyState == 4 && this.status == 200) {
						document.getElementById("velocidad").innerHTML= this.responseText;
					}
				};
				xhttp.open("GET", "/redVel", true);
				xhttp.send();
			}
			function getVel(){
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function() {
					if (this.readyState == 4 && this.status == 200) {
						document.getElementById("velocidad").innerHTML=  this.responseText;
					}
				};
				xhttp.open("GET", "/getVel", true);
				xhttp.send();
			}
			function sendJoyAxis() {
				if(canvasState>0){ 
					var P_M1_E = BotM1_E.GetPress();
					var P_M1_D = BotM1_D.GetPress();
					var P_M2_E = BotM2_E.GetPress();
					var P_M2_D = BotM2_D.GetPress();

					if(posM1 != 100 || posM2 != 100 ){
						posM1=100;
						posM2=100;
						flag1=1;
					}

					if(P_M1_E == 1)	
					{
						posM1 = 50;
						flag1=1;
					}
					else if(P_M1_D == 1)	
					{
						posM1 = 150;
						flag1=1;
					}
					if(P_M2_E == 1)	
					{
						posM2 = 50;
						flag1=1;
					}
					else if(P_M2_D == 1)	
					{
						posM2 = 150;
						flag1=1;
					}
					if(flag1==1 ){
						var xhr = new XMLHttpRequest();
						xhr.open("GET", "/canvasing?HX="+posM1+"&HY="+posM2, true); 
						xhr.send();
						flag1=0;
					}
				}
			}

			function getCanvasing(){
				var xhttp = new XMLHttpRequest();
			  	xhttp.onreadystatechange = function() {
					if (this.readyState == 4 && this.status == 200) {
				  		canvasState = this.responseText;
						if(canvasState==0){ 		//Inicio
							document.getElementById("btCanvas").innerHTML = "INICIO";
						}else if(canvasState==1){	//PrimeraEsquina
							document.getElementById("btCanvas").innerHTML = "Primera Esquina";
							document.getElementById("btCanvas").style.background = "#DD1100";
						}else if(canvasState==2){	//SegundaEsquina
							document.getElementById("btCanvas").innerHTML = "Segunda Esquina";
							document.getElementById("btCanvas").style.background = "#DD1100";
						}else if(canvasState==3){	//TerceraEsquina
							document.getElementById("btCanvas").innerHTML = "Tercera Esquina - FIN";
							document.getElementById("btCanvas").style.background = "#DD1100";
						}
					}
			  	};
			  	xhttp.open("GET", "/getCanvasStatus", true);
			  	xhttp.send();
			}

			function incCanvasing(){
				var xhttp = new XMLHttpRequest();
			  	xhttp.open("GET", "/incCanvasStatus", true);
			  	xhttp.send();
			}
			
			
		</script>	
	</body>
</html>
)rawliteral";
//params web page
const char params_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
    <title>Plotter Params</title>
    <style>
		a.button2{
			display:inline-block;
			padding:0.75em 1.5em;
			margin:0.1em 0.1em 0.1em 0;
			border:0.16em solid rgba(255,255,255,0);
			border-radius:1em;
			box-sizing: border-box;
			text-decoration:none;
			font-family:'Roboto',sans-serif;
			font-weight:500;
			color:#FFFFFF;
			background-color:#4E9AF1;
			text-shadow: 0 0.04em 0.04em rgba(0,0,0,0.35);
			text-align:center;
			transition: all 0.2s;
			cursor: pointer;
		}
			
		a.button2:hover{
			border-color: black
		}
		@media all and (max-width:30em){
			a.button2{
				display:block;
				margin:0.2em auto;
			}
		}
      	.button 
      	{
			background-color:  #4CAF50;
			border-radius: 5px;
			border: none;
			color: white;
			
			text-align: center;
			text-decoration: none;
			display: inline-block;
			font-size: 16px;
			margin:auto; 
			padding: 15px 22px;
			cursor: pointer;
      	}
      	.tag{
			background-color:  #005FC9;
			border-radius: 5px;
			border: none;
			color: white;
			padding: 10px 10px;
			text-align: center;
			text-decoration: none;
			display: inline-block;
			font-size: 18px;
			margin :2px;
			width:180px;font-weight: bold;
      	}
      	.value{
			font-size:22px;
			font-style:bold;
			background-color:  white;
			border-radius: 5px;
			color: black;
			
			padding:  4px;
      	}
      	.div_holder{
			background-color:  #CCCCCC;
			text-align: center;
			color:black;
			margin: auto;
			padding:0.01em;
			border-radius: 5px;
			width:12em;
			display:block;
			font-size: 22px;
      	}
    </style>
  </head>
  <body style="background-color: #EEEEEE;color:#353535;text-align: center;">
  <h2 style="text-align: center; font-size:25px;">Par&aacutemetros</h2>
    <div class="div_holder"> 
    	<h3>Homing</h3>
      	<p class="tag" id="HX_c">Posici&oacuten X: <input class="value" style="width:75px;font-size:20px;" id="HX_v" value="1250.0"></p>
      	<p class="tag" id="HY_c">Posici&oacuten Y: <input class="value" style="width:75px;font-size:20px;" id="HY_v" value="50.0"></p>
    </div>
	<div class="div_holder"> 
    	<h3>Canvas</h3>
      	<p class="tag" id="CVW_c">Ancho: <input class="value" style="width:75px;font-size:20px;" id="CVW_v" value="1250.0"></p>
      	<p class="tag" id="CVH_c">Alto: <input class="value" style="width:75px;font-size:20px;" id="CVH_v" value="50.0"></p>
		 <a href="/canvas_task" class="button2" style="background-color:#4CAF50">Canvas config</a>
    </div>
	<a id="btGuardar" class="button2" style="font-size:20px;" onclick="updateParams()">GUARDAR</a></br>
    
    <a href="/" class="button2" style="background-color:#4CAF50;width:10em;">Joystick</a></br>
	 <a href="/homing_task" class="button2" style="background-color:#4CAF50;width:10em;">Homing</a>
	 <p></p><p></p>
	

  </body>
  <script type="text/javascript">
		function load() {
			getHomingXParm();
			getHomingYParm();
			getCanvasWidth();
			getCanvasHeight();
		}
		function getHomingXParm(){
			var xhttp = new XMLHttpRequest();
			xhttp.onreadystatechange = function() {
				if (this.readyState == 4 && this.status == 200) {
					document.getElementById("HX_v").value = this.responseText;
				}
			};
			xhttp.open("GET", "getHX", true);
			xhttp.send();
		}
		function getHomingYParm(){
			var xhttp = new XMLHttpRequest();
			xhttp.onreadystatechange = function() {
				if (this.readyState == 4 && this.status == 200) {
					document.getElementById("HY_v").value = this.responseText;
				}
			};
			xhttp.open("GET", "getHY", true);
			xhttp.send();
		}
		function getCanvasWidth(){
			var xhttp = new XMLHttpRequest();
			xhttp.onreadystatechange = function() {
				if (this.readyState == 4 && this.status == 200) {
					document.getElementById("CVW_v").value = this.responseText;
				}
			};
			xhttp.open("GET", "getCVW", true);
			xhttp.send();
		}
		function getCanvasHeight(){
			var xhttp = new XMLHttpRequest();
			xhttp.onreadystatechange = function() {
				if (this.readyState == 4 && this.status == 200) {
					document.getElementById("CVH_v").value = this.responseText;
				}
			};
			xhttp.open("GET", "getCVH", true);
			xhttp.send();
		}
		function updateParams(){
			var px = document.getElementById("HX_v").value;
			var py = document.getElementById("HY_v").value;
			var cw = document.getElementById("CVW_v").value;
			var ch = document.getElementById("CVH_v").value;
			var xhr = new XMLHttpRequest();
  			xhr.open("GET", "/homing_params?PX="+px+"&PY="+py+"&CVW="+cw+"&CVH="+ch, true);  
  			xhr.send();
			load();
		}
		window.onload = load;
  </script>
</html>
)rawliteral";


// HTML web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
  <head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Joystick HTML</title>
    
    
    <script  type="text/javascript">
    

  var JoyStick = (function(container, parameters)
{
  parameters = parameters || {};
  var title = (typeof parameters.title === "undefined" ? "joystick" : parameters.title),
    width = (typeof parameters.width === "undefined" ? 0 : parameters.width),
    height = (typeof parameters.height === "undefined" ? 0 : parameters.height),
    internalFillColor = (typeof parameters.internalFillColor === "undefined" ? "#444444" : parameters.internalFillColor),
    internalLineWidth = (typeof parameters.internalLineWidth === "undefined" ? 2 : parameters.internalLineWidth),
    internalStrokeColor = (typeof parameters.internalStrokeColor === "undefined" ? "#999999" : parameters.internalStrokeColor),
    externalLineWidth = (typeof parameters.externalLineWidth === "undefined" ? 2 : parameters.externalLineWidth),
    externalStrokeColor = (typeof parameters.externalStrokeColor ===  "undefined" ? "#AAAAAA" : parameters.externalStrokeColor),
    autoReturnToCenter = (typeof parameters.autoReturnToCenter === "undefined" ? true : parameters.autoReturnToCenter);
  
  // Create Canvas element and add it in the Container object
  var objContainer = document.getElementById(container);
  var canvas = document.createElement("canvas");
  canvas.id = title;
  if(width === 0) { width = objContainer.clientWidth; }
  if(height === 0) { height = objContainer.clientHeight; }
  canvas.width = width;
  canvas.height = height;
  objContainer.appendChild(canvas);
  var context=canvas.getContext("2d");
  
  var pressed = 0; // Bool - 1=Yes - 0=No
  var press_copy= 0;
  var circumference = 2 * Math.PI;
  var internalRadius = (canvas.width-((canvas.width/2)+10))/2;
  var maxMoveStick = internalRadius + 5;
  var externalRadius = internalRadius + 30;
  var centerX = canvas.width / 2;
  var centerY = canvas.height / 2;
  var directionHorizontalLimitPos = canvas.width / 10;
  var directionHorizontalLimitNeg = directionHorizontalLimitPos * -1;
  var directionVerticalLimitPos = canvas.height / 10;
  var directionVerticalLimitNeg = directionVerticalLimitPos * -1;
  // Used to save current position of stick
  var movedX=centerX;
  var movedY=centerY;
    
  // Check if the device support the touch or not
  if("ontouchstart" in document.documentElement)
  {
    canvas.addEventListener("touchstart", onTouchStart, false);
    canvas.addEventListener("touchmove", onTouchMove, false);
    canvas.addEventListener("touchend", onTouchEnd, false);
  }
  else
  {
    canvas.addEventListener("mousedown", onMouseDown, false);
    canvas.addEventListener("mousemove", onMouseMove, false);
    canvas.addEventListener("mouseup", onMouseUp, false);
  }
  // Draw the object
  drawExternal();
  drawInternal();

  /******************************************************
   * Private methods
   *****************************************************/

  /**
   * @desc Draw the external circle used as reference position
   */
  function drawExternal()
  {
    context.beginPath();
    context.arc(centerX, centerY, externalRadius, 0, circumference, false);
    context.lineWidth = externalLineWidth;
    context.strokeStyle = externalStrokeColor;
    context.stroke();
  }

  /**
   * @desc Draw the internal stick in the current position the user have moved it
   */
  function drawInternal()
  {
    context.beginPath();
    if(movedX<internalRadius) { movedX=maxMoveStick; }
    if((movedX+internalRadius) > canvas.width) { movedX = canvas.width-(maxMoveStick); }
    if(movedY<internalRadius) { movedY=maxMoveStick; }
    if((movedY+internalRadius) > canvas.height) { movedY = canvas.height-(maxMoveStick); }
    context.arc(movedX, movedY, internalRadius, 0, circumference, false);
    // create radial gradient
    var grd = context.createRadialGradient(centerX, centerY, 5, centerX, centerY, 200);
    // Light color
    grd.addColorStop(0, internalFillColor);
    // Dark color
    grd.addColorStop(1, internalStrokeColor);
    context.fillStyle = grd;
    context.fill();
    context.lineWidth = internalLineWidth;
    context.strokeStyle = internalStrokeColor;
    context.stroke();
  }
  
  /**
   * @desc Events for manage touch
   */
  function onTouchStart(event) 
  {
    pressed = 1;
    press_copy =1;
  }

  function onTouchMove(event)
  {
    // Prevent the browser from doing its default thing (scroll, zoom)
    event.preventDefault();
    if(pressed === 1 && event.targetTouches[0].target === canvas)
    {
      movedX = event.targetTouches[0].pageX;
      movedY = event.targetTouches[0].pageY;
      // Manage offset
      if(canvas.offsetParent.tagName.toUpperCase() === "BODY")
      {
        movedX -= canvas.offsetLeft;
        movedY -= canvas.offsetTop;
      }
      else
      {
        movedX -= canvas.offsetParent.offsetLeft;
        movedY -= canvas.offsetParent.offsetTop;
      }
      // Delete canvas
      context.clearRect(0, 0, canvas.width, canvas.height);
      // Redraw object
      drawExternal();
      drawInternal();
    }
  } 

  function onTouchEnd(event) 
  {
    pressed = 0;
    press_copy = 0;
    // If required reset position store variable
    if(autoReturnToCenter)
    {
      movedX = centerX;
      movedY = centerY;
    }
    // Delete canvas
    context.clearRect(0, 0, canvas.width, canvas.height);
    // Redraw object
    drawExternal();
    drawInternal();
    //canvas.unbind('touchmove');
  }

  /**
   * @desc Events for manage mouse
   */
  function onMouseDown(event) 
  {
    pressed = 1;
    press_copy = 1;
  }

  function onMouseMove(event) 
  {
    if(pressed === 1)
    {
      movedX = event.pageX;
      movedY = event.pageY;
      // Manage offset
      if(canvas.offsetParent.tagName.toUpperCase() === "BODY")
      {
        movedX -= canvas.offsetLeft;
        movedY -= canvas.offsetTop;
      }
      else
      {
        movedX -= canvas.offsetParent.offsetLeft;
        movedY -= canvas.offsetParent.offsetTop;
      }
      // Delete canvas
      context.clearRect(0, 0, canvas.width, canvas.height);
      // Redraw object
      drawExternal();
      drawInternal();
    }
  }

  function onMouseUp(event) 
  {
    pressed = 0;
    press_copy = 0;
    // If required reset position store variable
    if(autoReturnToCenter)
    {
      movedX = centerX;
      movedY = centerY;
    }
    // Delete canvas
    context.clearRect(0, 0, canvas.width, canvas.height);
    // Redraw object
    drawExternal();
    drawInternal();
    //canvas.unbind('mousemove');
  }

  /******************************************************
   * Public methods
   *****************************************************/
  
  /**
   * @desc The width of canvas
   * @return Number of pixel width 
   */
  this.GetWidth = function () 
  {
    return canvas.width;
  };
  
  /**
   * @desc The height of canvas
   * @return Number of pixel height
   */
  this.GetHeight = function () 
  {
    return canvas.height;
  };
  
  /**
   * @desc The X position of the cursor relative to the canvas that contains it and to its dimensions
   * @return Number that indicate relative position
   */
  this.GetPosX = function ()
  {
    return movedX;
  };
  
  /**
   * @desc The Y position of the cursor relative to the canvas that contains it and to its dimensions
   * @return Number that indicate relative position
   */
  this.GetPosY = function ()
  {
    return movedY;
  };
  this.GetCenterY=function(){
	return centerY;
  };
  /**
   * @desc Normalizzed value of X move of stick
   * @return Integer from -100 to +100
   */
		this.GetX = function ()
		{
			return (100*((movedX - centerX)/maxMoveStick)).toFixed();
		};

		  /**
		   * @desc Normalizzed value of Y move of stick
		   * @return Integer from -100 to +100
		   */
		this.GetY = function ()
		{
			return ((100*((movedY - centerY)/maxMoveStick))*-1).toFixed();
		};
  
		/**
	   * @desc Get the direction of the cursor as a string that indicates the cardinal points where this is oriented
	   * @return String of cardinal point N, NE, E, SE, S, SW, W, NW and C when it is placed in the center
	   */
		this.GetDir = function()
		{
			var result = "";
			var orizontal = movedX - centerX;
			var vertical = movedY - centerY;
			
			if(vertical >= directionVerticalLimitNeg && vertical <= directionVerticalLimitPos)
			{
			  result = "C";
			}
			if(vertical < directionVerticalLimitNeg)
			{
			  result = "N";
			}
			if(vertical > directionVerticalLimitPos)
			{
			  result = "S";
			}
			
			if(orizontal < directionHorizontalLimitNeg)
			{
			  if(result === "C")
			  { 
				result = "W";
			  }
			  else
			  {
				result += "W";
			  }
			}
			if(orizontal > directionHorizontalLimitPos)
			{
			  if(result === "C")
			  { 
				result = "E";
			  }
			  else
			  {
				result += "E";
			  }
			}
			
			return result;
		  
		};

		this.GetPress = function(){
			return press_copy;
		};
		  
		});
		//FIN JAVASCRIPT
	</script>
	<style>
		*
		{
		  box-sizing: border-box;
		}
		body
		{
			
		  font-family: monospace;
		}  
		.grabacion
		{
			font-family:'Roboto',sans-serif;
			border-radius:10px;
			background:#CCCCCC;
			padding:1em;
			width:15em;
			margin:auto;
			text-align:center;
			font-weight: bold;
			font-size:20px;
		}
		.row
		{
		  display: block;
		  clear: both;
		}

		.joystickSt
		{	
			box-sizing: border-box;
			width:200px;
			height:200px;
			margin:auto;
			background-color: #eeeeee;
			border-radius: 25px;	
		}

		#joystick
		{
			border: 5px solid #4CAF50;
			border-radius: 25px;
			width:200px;
			height:200px;
		}

		.button 
		{
			background-color: #4CAF50;
			border-radius: 5px;
			border: none;
			color: white;
			padding: 10px 20px;
			text-align: center;
			text-decoration: none;
			display: inline-block;
			font-size: 16px;
			margin: 1em;
			cursor: pointer;
		}
		a.button2{
			display:inline-block;
			padding:0.75em 1em;
			width:200px;
			margin:0.1em 0.1em 0.1em 0;
			border:0.16em solid rgba(255,255,255,0);
			border-radius:1em;
			box-sizing: border-box;
			text-decoration:none;
			font-family:'Roboto',sans-serif;
			font-weight:500;
			font-size:20px;
			color:#FFFFFF;
			background-color:#4E9AF1;
			text-shadow: 0 0.04em 0.04em rgba(0,0,0,0.35);
			text-align:center;
			transition: all 0.2s;
			cursor: pointer;
			tittle:OFF;
		}
		
		a.button2:hover{
			border-color: black
		}
		@media all and (max-width:30em){
			a.button2{
				display:block;
				margin:0.2em auto;
			}
		}
		
		.values
		{
			background-color: #4CAF50;
			border:2 px;
			border-color:black;
			border-radius: 5px;
			border-style: solid;
			text-align: center;
			margin:auto;
			margin-top:10px;
			padding:0px 10px ;
			width:auto;
			height:auto;
			line-height:25px;
			font-size: 16px;
			font-weight: bold;
			font-family:'Roboto',sans-serif;
			color:white;
		}
		</style>
	</head>
	<body style="text-align:center;">
    
     	<h2>Plotter Vertical</h2>
		<h2>IdeaLAB</h2>
      	<div style="display:inline-flex;">
			<div class="values"> <p id="PlotterPosX"></p> <p id="PlotterPosY"></p></div>
			<div class="values"> <p id="joy1PosX"></p> <p id="joy1PosY"></p></div>
		</div><a></a></br>
		<div id="joyID"  class="joystickSt" ></div>
		<div >
			<a  class="button2" onclick="updateGPIO(this)"  id="SERVO" >SERVO</a>
		</div>
		<div class="grabacion">
		Grabaci&oacuten</br>
			<a  class="button2" onclick="updateRecord(this)"  id="GRABAR">Grabar</a>
			<a  class="button2" onclick="updatePlay(this)"  id="PLAY">Reproducir</a>
		</div>
		<div style="margin:0.1em;">
		
		<a href="/plotter_params" class="button2" style="background-color:#4CAF50">Par&aacutemetros </a></br>
		<a href="/homing_task" class="button2" style="background-color:#4CAF50">Homing</a></br>
		<a href="/canvas_task" class="button2" style="background-color:#4CAF50">Canvas config</a></br>
		<a  class="button2" onclick="resetESP()" style="background:grey"  id="RESET">Reset ESP</a>
		</div>
   
	   <script type="text/javascript">
			// Create JoyStick object into the DIV 'joystickDiv'
			var Joy1 = new JoyStick("joyID");
			var joy1InputPosX = document.getElementById("joy1PosX");
			var joy1InputPosY = document.getElementById("joy1PosY");

			var intervalo = 50;
			var counter =0;
			setInterval(function(){ document.getElementById("joy1PosX").innerHTML="JoyX: "+(Joy1.GetPosX()-100)/50; 
				document.getElementById("joy1PosY").innerHTML="JoyY: "+(Joy1.GetPosY()-100)/50;
			}, intervalo);
			
			setInterval(function() { sendJoyAxis();}, 50);
			setInterval(function() {  getPlotterPosX();  getPlotterPosY(); getServo();}, 250);
			setInterval(function() {  getPlay(); }, 2000);
			/*METODO PARA ENVIO DE VALORES DE JOYSTICK A ESP32*/
			
			function resetESP(){
				var xhr = new XMLHttpRequest();
				xhr.open("GET", "/reset", true); 
				xhr.send();
			}

			function getPlotterPosX() {
			  var xhttp = new XMLHttpRequest();
			  xhttp.onreadystatechange = function() {
				if (this.readyState == 4 && this.status == 200) {
				  document.getElementById("PlotterPosX").innerHTML = "PosX: " + this.responseText + " mm";
				}
			  };
			  xhttp.open("GET", "getPosX", true);
			  xhttp.send();
			}
			
			function getPlotterPosY() {
			  var xhttp = new XMLHttpRequest();
			  xhttp.onreadystatechange = function() {
				if (this.readyState == 4 && this.status == 200) {
				  document.getElementById("PlotterPosY").innerHTML = "PosY: " + this.responseText + " mm";
				}
			  };
			  xhttp.open("GET", "getPosY", true);
			  xhttp.send();
			}
			function getServo() {
			  var xhttp = new XMLHttpRequest();
			  xhttp.onreadystatechange = function() {
				if (this.readyState == 4 && this.status == 200) {
				  if(this.responseText=="0")
				  {
					  document.getElementById("SERVO").tittle="OFF";
					  document.getElementById("SERVO").style.background = "#4E9AF1";
				  }
				  else if(this.responseText=="1")
				  {
					   document.getElementById("SERVO").tittle="ON";
					   document.getElementById("SERVO").style.background = "#DD1100";
				  }
				}
			  };
			  xhttp.open("GET", "getServo", true);
			  xhttp.send();
			}

			var posXEsp=100;
			var posYEsp=100;
			var posXAnt=100;
			var posYAnt=100;
			function sendJoyAxis() {
				var pressing = Joy1.GetPress();
				posXAnt = posXEsp;
				posYAnt = posYEsp;
				if(posXAnt != 100 || posYAnt !=100 || pressing == 1)	
				{
					posXEsp = Joy1.GetPosX();
					posYEsp = Joy1.GetPosY();
					var xhr = new XMLHttpRequest();
					xhr.open("GET", "/joystick?stateX=" + posXEsp + "&stateY=" + posYEsp, true); 
					xhr.send();
				}
			}

			function updateGPIO(element) {
			  var xhr = new XMLHttpRequest();
			  if(element.tittle == "OFF"){ 
				  xhr.open("GET", "/action?action="+element.id+"&state=1", true); 
				  element.tittle = "ON";
				  element.style.background = "#DD1100";
			  }
				else{ 
					xhr.open("GET", "/action?action="+element.id+"&state=0", true); 
					element.tittle = "OFF";element.style.background = "#4E9AF1";
				}
				xhr.send();
			}
			function updateRecord(element) {
				var xhr = new XMLHttpRequest();
				if(element.innerHTML == "Grabar"){ 
					xhr.open("GET", "/action?action="+element.id+"&state=1", true);
					element.innerHTML = "Fin grabacion";
					element.style.background = "#DD1100";					
				}
				else 
				{ 
					xhr.open("GET", "/action?action="+element.id+"&state=0", true); 
					element.style.background = "#4E9AF1";
					element.innerHTML = "Grabar";
				}
				xhr.send();
			}
			function updatePlay(element){
				var xhr = new XMLHttpRequest();
				if(element.innerHTML == "Reproducir"){ 
					xhr.open("GET", "/action?action="+element.id+"&state=1", true);
					element.innerHTML = "Iniciar";
					element.style.background = "#DD1100";					
				}
				else if(element.innerHTML == "Iniciar"){
					xhr.open("GET", "/action?action="+element.id+"&state=2", true);
					element.innerHTML = "Detener";
					element.style.background = "#DD1100";
				}
				else 
				{ 
					xhr.open("GET", "/action?action="+element.id+"&state=0", true); 
					element.style.background = "#4E9AF1";
					element.innerHTML = "Reproducir";
				}
				xhr.send();
			}

			function getPlay(){
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function() {
					if (this.readyState == 4 && this.status == 200) {

					  	if(this.responseText=="0"){
						 	document.getElementById("PLAY").innerHTML = "Reproducir";
						 	document.getElementById("PLAY").style.background = "#4E9AF1";
					  	}
						else if(this.responseText =="1")
					  	{
						  	document.getElementById("PLAY").innerHTML = "Iniciar";
						 	document.getElementById("PLAY").style.background = "#DD1100";
					  	}
						else if(this.responseText =="2")
						{
							document.getElementById("PLAY").innerHTML = "Detener";
						 	document.getElementById("PLAY").style.background = "#DD1100";
					  	}
					}
			  	};
			  	xhttp.open("GET", "/getPlayState", true);
			  	xhttp.send();
			} 

		</script>	
	</body>
</html>
)rawliteral";