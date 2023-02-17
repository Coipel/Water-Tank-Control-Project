clc; clear; close all;
format compact;

lower_bound = -10;
upper_bound = 100;

filename = "data-2-13-2023--21-15-24.mat";
load(filename);

strain_data = double(SD); % This may be unnecessary since datatype is float in Python
time_data_sec = double(TD)/1000;

[time_data_sec, removed_indicies] = duplicate_filter(time_data_sec);
strain_data = delete_elements(strain_data, removed_indicies);

figure(1);
plot(time_data_sec, strain_data, 'bx-');

lower_violations = numel(find(strain_data < lower_bound));
upper_violations = numel(find(strain_data > upper_bound));
total_violations = lower_violations + upper_violations;
total_data_points = numel(strain_data);

disp("'Absurd spikes percentage: ");
disp((total_violations*100)/total_data_points);


