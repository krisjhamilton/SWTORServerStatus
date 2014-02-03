function HTTPGET(url) {
	var req = new XMLHttpRequest();
	req.open("GET", url, false);
	req.send(null);
	return req.responseText;
}

var getWeather = function() {
	//Get weather info
	var response = HTTPGET("http://www.forswor.com/forswor_services/prod/serverwidget/getSWTORStatusjson.php");
		
	//Convert to JSON
	var json = JSON.parse(response);
	
	//Extract the data
	//var temperature = Math.round(json.main.temp - 273.15);
	var state = json.SWTORServerState.server_state;
	var name = json.SWTORServerState.server_name;
	var description = json.SWTORServerState.server_desc;
	var population = json.SWTORServerState.server_load_txt;
	
	//Console output to check all is working.
	//console.log("It is " + temperature + " degrees in " + location + " today!");
	
	//Construct a key-value dictionary	
	var dict = {"KEY_STATE" : state, "KEY_NAME": name, "KEY_DESCRIPTION": description, "KEY_POPULATION": population};
	
	//Send data to watch for display
	Pebble.sendAppMessage(dict);
};

Pebble.addEventListener("ready",
  function(e) {
	//App is ready to receive JS messages
	getWeather();
  }
);

Pebble.addEventListener("appmessage",
  function(e) {
	//Watch wants new data!
	getWeather();
  }
);