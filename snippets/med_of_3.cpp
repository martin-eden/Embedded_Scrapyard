int16_t med_of_3(int16_t a, int16_t b, int16_t c) {
  int16_t result;
  if (a < b) {
    if (a < c) {
      if (b < c)
        result = b;
      else
        result = c;
    }
    else {
      result = a;
    }
  }
  else {
    if (a > c) {
      if (b > c)
        result = b;
      else
        result = c;
    }
    else {
      result = a;
    }
  }
  return result;
}
