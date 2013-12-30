Pebble.addEventListener("ready", function(e) {
	console.log("JavaScript is go!");
	Pebble.sendAppMessage({	"message":"omg from js" });
	console.log("Message sent");
});

Pebble.addEventListener("appmessage", function(e) {
	console.log("Received message: " + e.payload);
	Pebble.sendAppMessage({ "message":"you pushed a button"});
});