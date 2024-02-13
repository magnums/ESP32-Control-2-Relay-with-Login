#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "PhayouneWIFI";
const char* password = "PhayounE2014";

WebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", loginPage());
  });

  server.on("/login", HTTP_POST, []() {
    if (server.arg("username") == "admin" && server.arg("password") == "admin") {
      server.send(200, "text/html", homePage());
    } else {
      server.send(403, "text/html", "<h1>Forbidden</h1>");
    }
  });

  server.on("/logout", HTTP_GET, []() {
    // This is a simple redirect to the login page for demo purposes
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });


server.on("/toggle", HTTP_GET, []() {
  if (server.hasArg("relay")) {
    int relay = server.arg("relay").toInt();
    bool state = false;
    if (relay == 1) {
      digitalWrite(26, !digitalRead(26));
      state = digitalRead(26);
    } else if (relay == 2) {
      digitalWrite(27, !digitalRead(27));
      state = digitalRead(27);
    }
    server.send(200, "application/json", "{\"relay\":" + String(relay) + ", \"state\":" + (state ? "true" : "false") + "}");
  } else {
    server.send(400, "text/html", "<h1>Bad Request</h1>");
  }
});


  // Catch-all handler for undefined routes
  server.onNotFound([]() {
    server.sendHeader("Location", "/", true); // Redirect to the root path
    server.send(302, "text/plain", ""); // 302 Found response code
  });


  server.begin();
}

void loop() {
  server.handleClient();
}

String loginPage() {
  return "<!DOCTYPE html><html><head><title>Login</title><style>body { font-family: Arial, sans-serif; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; background-color: #f0f0f0; } form { background-color: #ffffff; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); } input[type=text], input[type=password] { width: 100%; padding: 10px; margin: 10px 0; border: 1px solid #ddd; border-radius: 4px; box-sizing: border-box; } input[type=submit] { width: 100%; background-color: #4CAF50; color: white; padding: 14px 20px; margin: 8px 0; border: none; border-radius: 4px; cursor: pointer; } input[type=submit]:hover { background-color: #45a049; }</style></head><body><form action='/login' method='POST'><h2>Login</h2>Username:<br><input type='text' name='username' required><br>Password:<br><input type='password' name='password' required><br><input type='submit' value='Login'></form></body></html>";
}

String homePage() {
  return "<!DOCTYPE html><html><head><title>Home</title><style>body { font-family: Arial, sans-serif; text-align: center; padding: 50px; background-color: #f0f0f0; } h1 { color: #333; } .button { background-color: #4CAF50; border: none; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; border-radius: 4px; } .button-on { background-color: #f44336; } .button:hover { opacity: 0.8; } .logout { position: fixed; right: 20px; top: 20px; }</style></head><body><h1>Welcome!</h1><button id='relay1' class='button' onclick=\"toggleRelay(1)\">Relay 1</button><button id='relay2' class='button' onclick=\"toggleRelay(2)\">Relay 2</button><a href='/logout' class='button button-red logout'>Logout</a><script>function toggleRelay(relay) { fetch('/toggle?relay=' + relay).then(function(response) { return response.json(); }).then(function(data) { var button = document.getElementById('relay' + relay); if(data.state) { button.classList.add('button-on'); button.textContent = 'Relay ' + relay + ' ON'; } else { button.classList.remove('button-on'); button.textContent = 'Relay ' + relay; } }); }</script></body></html>";
}
