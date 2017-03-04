/*
 Name:		esp_cli.ino
 Created:	3/4/2017 12:25:10 PM
 Author:	agmun
*/

#include "CLI2.h"

CLI::Handler cli;

bool on_command(CLI::Handler &h, Print &prn) {
	prn.print("Command: ");
	prn.println(h.argv(0));
	prn.print("Argument count: ");
	Serial.println(h.argc());
	return true;
}

void setup() {
	Serial.begin(115200);
	cli.add("cmd", "Short help for cmd", "Long help for cmd and extra args", on_command);

}

String serBuf = "";
void onSerial() {
	while (Serial.available()) {
		if (128 < serBuf.length()) {
			Serial.println("\n\rERR");
			serBuf = "";
		}
		char c = (char)Serial.read();
		if (c == '\t') c = ' ';
		Serial.write(c);
		if (c == '\r') continue;
		if (c == '\n') {
			Serial.println();
			if (!cli.handle(Serial, serBuf)) {
				Serial.println("\nERR");
			}
			serBuf = "";
			break;
		}
		serBuf += c;
	}
}

void loop() {
	if (Serial.available()) onSerial();
	delay(10);
}