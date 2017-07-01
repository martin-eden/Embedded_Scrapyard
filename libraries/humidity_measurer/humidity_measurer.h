#pragma once

class humidity_measurer {
  public:
    int sensor_pin;
    int min_value;
    int max_value;
    bool power_off_between_measures;
    bool high_means_dry;
    int power_pin;
    humidity_measurer();
    int get_raw_value();
    int get_value();
    bool is_line_problem;
};
