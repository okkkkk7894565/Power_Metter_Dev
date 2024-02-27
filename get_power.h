#include <PZEM004Tv30.h>
PZEM004Tv30 pzem(D5, D6);

//get vol
float getVol() {
  float voltage = pzem.voltage();
  if (isnan(voltage)) {
    return -100;
  } else {
    return voltage;
  }
}

//GET AMP
float getAmp() {
  float current = pzem.current();
  if (isnan(current)) {
    return -100;
  } else {
    return current;
  }
}

//get cosPi
float getPF() {
  float PF = pzem.pf();
  if (isnan(PF)) {
    return -100;
  } else {
    return PF;
  }
}

//get Wat
float getWat() {
  float power = pzem.power();
  if (isnan(power)) {
    return -100;
  } else {
    return power;
  }
}

//get Frequency
float getFre() {
  float fre = pzem.frequency();
  if (isnan(fre)) {
    return -100;
  } else {
    return fre;
  }
}


//get vol
float getEnergy() {
  float energy = pzem.energy();
  if (isnan(energy)) {
    return -100;
  } else {
    return energy;
  }
}
void showData(float volt,float ampe ,float PF,float wat,float Frequency,float Energy ){
      Serial.print("volt: ");
      Serial.print(volt);
      Serial.println(" V");

      Serial.print("ampe: ");
      Serial.print(ampe);
      Serial.println(" A");

      Serial.print("Wat: ");
      Serial.print(wat);
      Serial.println("W");

      Serial.print("PF: ");
      Serial.println(PF);

      Serial.print("Fre: ");
      Serial.print(Frequency);
      Serial.println("Hz");

      Serial.print("Ene: ");
      Serial.print(Energy);
      Serial.println("kwh");
      Serial.println();
}


