# LED als Sensor

Proof of Concept zur Nutzung einer LED als Sensor für die Helligkeit.
Die LED kann normal zur Ausgabe plus zur Ermittlung der Helligkeit genutzt werden.  
Bei dieser Lösung werden keine zusätzlichen Bauteile oder Transistoren benötigt.

Leuchtet man eine grüne LED mit einer starken LED z.B. vom Smartphone an, dann kann man mit dem Multimeter bis zu 1.5 Volt messen. Der erzeugte Strom von 3 Mikroampere ist aber viel zu gering, um direkt und zuverlässig am ADC-Pin gelesen werden zu können.

Am ADC-Pin wird eine Schwankung durch dauernden Wechsel zwischen HIGH mit Input-Pullup und LOW mit Output erzwungen. Mit diesem Trick kann nun die Helligkeit sehr zuverlässig ermittelt werden. Bei Dunkelheit erzeugt die LED keine Spannung und keinen Strom, weshalb die Schwankungen sehr stark sind. Bei sehr hellem Licht glättet die LED die Schwankung trotz geringer Stromstärke.

Der Helligkeitswert wird über die Summe der Schwankungen von mehreren hintereinander durchgeführten Messungen abgeleitet. Das funktioniert erstaunlich gut, wie die Beispiele mit dem Blinklicht bzw. Morsecode (siehe PNG-Dateien) verdeutlichen.
