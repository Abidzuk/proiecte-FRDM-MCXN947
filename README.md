# MCXN947 Embedded Project Multi-App System

Acest proiect a fost realizat in cadrul cursului "NXP MCX Embedded Programing" Course. El reprezinta o aplicatie embedded dezvoltata pentru placa de la 
NXP FRDM-MCXN947, utilizand un Shield cu diverse periferice (OLED, LED-uri, Senzori, Joystick, Encoder). Aplicatia implementeaza un sistem de meniuri 
bazat pe o masina de stari (State Machine), controlat prin DIP Switches, permitand rularea a 4 sub-aplicatii distincte.

### Startup & Animatie Custom
La pornire, sistemul ruleaza o animatie frame-by-frame, afisand succesiv 8 cadre pe ecranul OLED.

### Utilitar Conversie Imagini (Python)
Pentru a realiza aceasta animatie, proiectul include un script custom "convertor.py" care proceseaza imaginile:
* Functie: Transforma imagini standard in vectori de date C.
* Procesare: Redimensioneaza imaginea la 128x64, o converteste in format alb-negru si genereaza matricea de bytes necesara driverului OLED.

### Cerintele Proiectului (Functionalitate)

Dupa animatia de boot, sistemul intra in meniul principal. Navigarea intre aplicatii se face exclusiv prin activarea unui singur switch.

### 1. LED Ring (Selectat cu DP1)
* Descriere: Aprinderea consecutiva a LED-urilor de pe shield (efect de rotire).
* Control:
    * Viteza: Controlata prin Potentiometru (citire ADC + Timer Interrupt).
    * Directie: Schimbarea sensului de rotatie la apasarea butonului SW1.

### 2. Sensors (Selectat cu DP2)
* Descriere: Citirea si afisarea valorilor de la senzorii de pe placă.
* Functionalitate:
    * Afiseaza pe ecranul OLED valorile in timp real pentru Senzorul de Lumina si Termistor.
    * Foloseste citire ADC.

### 3. Rotary Encoder Control (Selectat cu DP4)
* Descriere: Controlul LED-urilor folosind encoderul rotativ.
* Functionalitate:
    * Mod Single LED: Rotirea encoderului plimba un singur LED aprins pe cerc.
    * Mod Bar Graph: Apasarea butonului encoderului aprinde toate LED-urile. Rotirea controleaza numarul de LED-uri active (crestere/descrestere).
    * Suporta control bidirectional prin intreruperi GPIO.

### 4. Joystick Control (Selectat cu DP8)
* Descriere: Controlul LED-urilor folosind Joystick-ul.
* Functionalitate:
    * Activeaza LED-ul corespunzator directiei in care este impins joystick-ul.

### Gestionarea Erorilor
* Daca utilizatorul activeaza mai mult de un DIP Switch simultan, sistemul intra intr-o stare de Eroare.
* Se afiseaza un mesaj de avertizare pe OLED.
* Revenirea in meniul principal este posibila doar dupa resetarea tuturor switch-urilor pe pozitia OFF.

## Hardware Utilizat
* Placa de baza: NXP FRDM-MCXN947
* Shield: NXP Application Shield (OLED 128x64, LED Ring, Sensors, Joystick, Encoder)

## Software Utilizat
* Limbaj: C, Python
* IDE: VSCode
* SDK: NXP MCUXpresso SDK
* Drivere: GPIO, ADC (LPADC), Timer (CTIMER), I2C (pentru OLED), Interrupts.
