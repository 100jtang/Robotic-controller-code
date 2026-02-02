// полное демо
#include <Arduino.h>
// #define EB_NO_FOR           // отключить поддержку pressFor/holdFor/stepFor и счётчик степов (экономит 2 байта оперативки)
// #define EB_NO_CALLBACK      // отключить обработчик событий attach (экономит 2 байта оперативки)
// #define EB_NO_COUNTER       // отключить счётчик энкодера (экономит 4 байта оперативки)
// #define EB_NO_BUFFER        // отключить буферизацию энкодера (экономит 1 байт оперативки)

// #define EB_DEB_TIME 50      // таймаут гашения дребезга кнопки (кнопка)
// #define EB_CLICK_TIME 500   // таймаут ожидания кликов (кнопка)
// #define EB_HOLD_TIME 600    // таймаут удержания (кнопка)
// #define EB_STEP_TIME 200    // таймаут импульсного удержания (кнопка)
// #define EB_FAST_TIME 30     // таймаут быстрого поворота (энкодер)

#include <EncButton.h>
EncButton eb(22,23);
// EncButton eb(2, 3, 4, INPUT); // + режим пинов энкодера
// EncButton eb(2, 3, 4, INPUT, INPUT_PULLUP); // + режим пинов кнопки

void isr() {
  eb.tickISR();
}

void setup() {
    Serial.begin(115200);

    // показаны значения по умолчанию

    eb.setEncReverse(0);
    eb.setEncType(EB_STEP4_HIGH);
    eb.setFastTimeout(10);

    // сбросить счётчик энкодера
    eb.counter = 0;

    attachInterrupt(22, isr, CHANGE);
    attachInterrupt(23, isr, CHANGE);
    eb.setEncISR(true);
}

void loop() {
    eb.tick();

    // обработка поворота общая
    if (eb.turn()) {
        Serial.print("turn: dir ");
        Serial.print(eb.dir());
        Serial.print(", fast ");
        Serial.print(eb.fast());
        Serial.print(", hold ");
        Serial.print(eb.pressing());
        Serial.print(", counter ");
        Serial.println(eb.counter);
    }


    // состояния
    // Serial.println(eb.pressing());
    // Serial.println(eb.holding());
    // Serial.println(eb.busy());
    // Serial.println(eb.waiting());

}