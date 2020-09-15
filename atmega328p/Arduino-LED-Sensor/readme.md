# LED als Sensor

Proof of Concept zur Nutzung einer LED als Sensor für die Helligkeit. Die LED kann normal zur Ausgabe plus zur Ermittlung der Helligkeit genutzt werden. Bei dieser Lösung werden außer dem 1-kOhm Widerstand und der grünen LED keine zusätzlichen Bauteile oder Transistoren benötigt, d.h. jede LED kann quasi grundsätzlich an einem ADC auch als Sensor genutzt werden.

Leuchtet man eine grüne LED mit einer starken Lampe z.B. LED vom Smartphone an, dann kann man mit dem Multimeter bis zu 1.5 Volt messen. Der erzeugte Strom von 3 Mikroampere ist aber viel zu gering, um direkt und zuverlässig analog am ADC-Pin gelesen werden zu können.

![Sensor](https://github.com/iotool/arduino/blob/master/atmega328p/Arduino-LED-Sensor/Arduino-LED-Sensor.png?raw=true)

Mit einem Trick kann die Helligkeit sehr zuverlässig ermittelt werden. Am ADC-Pin wird eine Schwankung durch den dauernden Wechsel zwischen HIGH mit Input-Pullup und LOW mit Output erzwungen. Bei Dunkelheit erzeugt die LED keine Spannung und keinen Strom, weshalb die am ADC gemessenen Schwankungen sehr stark bleiben. Bei hellem Licht glättet die LED messbar die erzeugte Schwankung trotz der geringen Stromstärke. Die Schwankung wird gezielt erzeugt, um zufälliges Floating am ADC zu verhindern und reproduzierbare Messergebnisse zu erhalten.

Der Helligkeitswert wird über die Summe der Schwankungen von mehreren hintereinander durchgeführten Messungen abgeleitet. Das funktioniert erstaunlich gut, wie die Beispiele mit dem Blinklicht bzw. Morsecode verdeutlichen. Eine LED kann somit zusätzlich als Sensor für die Helligkeit der Umgebung, ein Blink- oder Morsecode genutzt werden.

## Anwendung Morsecode

Mit einer LED können Daten per Morsecode vom Microcontroller empfangen werden. 

![Morsen](https://github.com/iotool/arduino/blob/master/atmega328p/Arduino-LED-Sensor/Arduino-LED-Sensor-Morse3.png?raw=true)

Das Beispiel zeigt das Muster für die Ziffer 3 (Morsecode ..---) mit der Geschwindigkeit drei Worte pro Minute.

## Anwendung Blinkcode

Mit einer LED kann ein Blinkcode erkannt werden, d.h. ob die LED mit einem Blinkcode angestrahlt wird. Viele Taschenlampen unterstützen einen Blinkcode mit z.B. 12 Herz. Die Erkennung funktioniert auch bei ungünstigen Lichtverhältnissen. Nur bei direkter Sonneneinstrahlung auf die LED ist das Blinklicht zu schwach, um erkannt zu werden.

![Blinklicht](https://github.com/iotool/arduino/blob/master/atmega328p/Arduino-LED-Sensor/Arduino-LED-Sensor_L4-room-light.png)

Über den Blinkcode kann die LED als kontaktloser Schalter genutzt werden. Zur Datenübertragung könnten die Zustände LO und HI über verschiedene Blinkfrequenzen (z.B. 6 Herz und 12 Herz) modelliert werden.

## Anwendung Helligkeit

Mit einer LED kann die Helligkeit der Umgebung erkannt werden. Bei direktem Sonnenlicht gibt es kaum noch Schwankungen, weshalb der Messwert von 1800 bei Dunkelheit auf 100 bei Sonnenlicht fällt.

![Helligkeit](https://github.com/iotool/arduino/blob/master/atmega328p/Arduino-LED-Sensor/Arduino-LED-Sensor_Lx-compare.png)

Der ermittelte Helligkeitswert (rote Linie) entspricht der Summe der Schwankungen. Bei Dunkelheit erzeugt die LED keine Spannung bzw. Strom, wodurch das Umschalten zwischen LOW/HIGH-Werten am ADC-Pin maximale Schwankungswerte ergeben und der Messwert für die Varianz am höchten ist. Bei direktem Sonnenlicht ist die Leistung der LED als Entergiequelle am höchsten und die Schwankungen am ADC-Pin werden fast vollständig geklättet, d.h. der Messwert für die Varianz ist am geringsten.
