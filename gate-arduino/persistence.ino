#define EEPROM_MARKER 127

void initPersistence() {
  EEPROM.update(0, EEPROM_MARKER);
}

void persistBoolean(boolean value) {
  int i = findEepromMarker();
  if (i == EEPROM.length() - 1) {
    i = 0;
  }
  EEPROM.update(i, value ? 1 : 0);
  EEPROM.update(i + 1, EEPROM_MARKER);
}

boolean loadBoolean() {
  int i = findEepromMarker();
  if (i == 0) { 
    return false;
  }
  return EEPROM.read(i - 1) == 1;
}

int findEepromMarker() {
  for (int i = 0; i < EEPROM.length(); i++) {
    if (EEPROM.read(i) == EEPROM_MARKER) {
      return i;
    }
  }
  return 0;
}
