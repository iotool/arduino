# LED als Sensor

Proof of Concept zur Nutzung einer LED als Sensor für die Helligkeit. Die LED kann normal zur Ausgabe plus zur Ermittlung der Helligkeit genutzt werden. Bei dieser Lösung werden keine zusätzlichen Bauteile oder Transistoren benötigt, d.h. jede LED kann quasi grundsätzlich an einem ADC auch als Sensor genutzt werden.

Leuchtet man eine grüne LED mit einer starken Lampe z.B. LED vom Smartphone an, dann kann man mit dem Multimeter bis zu 1.5 Volt messen. Der erzeugte Strom von 3 Mikroampere ist aber viel zu gering, um direkt und zuverlässig analog am ADC-Pin gelesen werden zu können.

Mit einem Trick kann die Helligkeit sehr zuverlässig ermittelt werden. Am ADC-Pin wird eine Schwankung durch dauernden Wechsel zwischen HIGH mit Input-Pullup und LOW mit Output erzwungen. Bei Dunkelheit erzeugt die LED keine Spannung und keinen Strom, weshalb die Schwankungen sehr stark bleiben. Bei sehr hellem Licht glättet die LED messbar die Schwankung trotz der geringen Stromstärke.

Der Helligkeitswert wird nun über die Summe der Schwankungen von mehreren hintereinander durchgeführten Messungen abgeleitet. Das funktioniert erstaunlich gut, wie die Beispiele mit dem Blinklicht bzw. Morsecode (siehe PNG-Dateien) verdeutlichen. Mit einer LED kann somit die Helligkeit der Umgebung, ein Blink- oder Morsecode erkannt werden.
