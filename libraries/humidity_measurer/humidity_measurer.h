#pragma once

class humidity_measurer {
  public:
    int sensor_pin;
    int power_pin;

    int min_value;
    int max_value;
    bool power_off_between_measures;
    bool high_means_dry;

    humidity_measurer();
    int get_value();
    int get_raw_value();
    bool is_line_problem;
};
