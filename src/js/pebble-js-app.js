Pebble.addEventListener("ready", function(e) {
	console.log("JavaScript is go!");
	setTimeout(function() {
		Pebble.sendAppMessage({
			"message":"omg from js"
		});
		console.log("Message sent");
	}, 2000);
});