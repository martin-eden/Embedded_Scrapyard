#pragma once

class me_PourDurationCalculator {
  public:
    int32_t calculatePouringDuration_ms(
      float pupmingVolume_mlPerMinute,
      float potDiameter_mm,
      float height_mm
    );
  private:
    float calculateCylinderVolume_cm3(
      float potDiameter_mm,
      float height_mm
    );
};

int32_t me_PourDurationCalculator::calculatePouringDuration_ms(
  float pupmingVolume_mlPerMinute,
  float potDiameter_mm,
  float height_mm
) {
  return calculateCylinderVolume_cm3(potDiameter_mm, height_mm) / pupmingVolume_mlPerMinute * 60 * 1000;
}

float me_PourDurationCalculator::calculateCylinderVolume_cm3(
  float potDiameter_mm,
  float height_mm
) {
  return potDiameter_mm * potDiameter_mm / 4 * PI * height_mm / 1000;
}
