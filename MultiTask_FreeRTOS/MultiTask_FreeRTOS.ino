// Simple demo of three threads
// LED blink thread, print thread, and idle loop
//the library for freertos needs to be installed from github
#include <FreeRTOS_AVR.h>
#include <Wire.h>
//this library also needs to be hunted down and installed 
#include <LiquidCrystal_I2C.h>

const uint8_t LED_PIN = 13;
LiquidCrystal_I2C lcd(0x27, 16, 2);

volatile uint32_t count = 0;
volatile uint32_t count1 = 0;

// handle for blink task
TaskHandle_t blink;

//handle for lcd task
TaskHandle_t lcdI2C;

//------------------------------------------------------------------------------
// high priority for blinking LED
static void vLEDFlashTask(void *pvParameters) {
	pinMode(LED_PIN, OUTPUT);

	// Flash led every 1000 ms.
	for (;;) {
		// Turn LED on.
		digitalWrite(LED_PIN, HIGH);

		// Sleep for 500 milliseconds.
		vTaskDelay((500L * configTICK_RATE_HZ) / 1000L);

		// Turn LED off.
		digitalWrite(LED_PIN, LOW);

		// Sleep for 500 milliseconds.
		vTaskDelay((500L * configTICK_RATE_HZ) / 1000L);
	}
}
//------------------------------------------------------------------------------
static void vPrintTask(void *pvParameters) {
	while (1) {
		// Sleep for one second.
		vTaskDelay(configTICK_RATE_HZ);

		// Print count for previous second.
		Serial.print(F("Count: "));
		Serial.print(count);

		Serial.print(F(", Unused Stack: LED_Blink:"));
		Serial.print(uxTaskGetStackHighWaterMark(blink));
		Serial.print(" LCD:");
		Serial.print(uxTaskGetStackHighWaterMark(lcdI2C));
		Serial.print(" Serial:");
		Serial.print(uxTaskGetStackHighWaterMark(0));
		Serial.print(" H:");
		Serial.println(freeHeap());

		// Zero count.
		count = 0;
	}
}

static void vLCDPrintTask(void *pvParameters){
	while (1){
		//Sleep for one second.
		vTaskDelay(configTICK_RATE_HZ);
		lcd.setCursor(0, 0);
		lcd.print("Count: ");
		lcd.print(count1);
		lcd.setCursor(0, 1);
		lcd.print("T3:");
		lcd.print(uxTaskGetStackHighWaterMark(0));
		lcd.print(" H:");
		lcd.print(freeHeap());

		// Zero count.
		count1 = 0;
	}
}
//------------------------------------------------------------------------------
void setup() {
	Serial.begin(9600);
	// initialize the lcd 
	lcd.init();                     
	lcd.backlight();
	// wait for Leonardo
	while (!Serial) {}

	// create blink task
	xTaskCreate(vLEDFlashTask,
		"Task1",
		configMINIMAL_STACK_SIZE + 100,
		NULL,
		tskIDLE_PRIORITY + 2,
		&blink);

	// create print task
	xTaskCreate(vPrintTask,
		"Task2",
		configMINIMAL_STACK_SIZE + 100,
		NULL,
		tskIDLE_PRIORITY + 1,
		NULL);

	// create print LCD task
	xTaskCreate(vLCDPrintTask,
		"Task3",
		configMINIMAL_STACK_SIZE + 100,
		NULL,
		tskIDLE_PRIORITY + 1,
		&lcdI2C);
	// start FreeRTOS
	vTaskStartScheduler();

	// should never return
	Serial.println(F("Die"));
	while (1);
}
//------------------------------------------------------------------------------
// WARNING idle loop has a very small stack (configMINIMAL_STACK_SIZE)
// loop must never block
void loop() {
	while (1) {
		// must insure increment is atomic
		// in case of context switch for print
		noInterrupts();
		count++;
		count1++;
		interrupts();
	}
}
