clc; clear; close all;
format compact;

ENABLE_PRELIMINARY_TRANSFERFUNCTION = false;
ENABLE_SAVING_DATA = false;

sample_time = 0.2;

data_directory = "../Mat_Data";
filename = "data-2-16-2023--10-9-39.mat";
data_path = strcat(data_directory, filename);
load(data_path); 
strain_data = double(SD); % This may be unnecessary since datatype is float in Python
valve_data = double(VD);
time_data_sec = double(TD)/1000;

% Made custom functions to parse through data and remove duplicant
% independent variables (time) and their associated output (valve/strain) 
% (Arduino occasionally is too fast at data collection and transmission)
[time_data_sec, removed_indicies] = duplicate_filter(time_data_sec);
strain_data = delete_elements(strain_data, removed_indicies);
valve_data = delete_elements(valve_data, removed_indicies);

valve_fit = fit(time_data_sec', valve_data', 'linearinterp');
strain_fit = fit(time_data_sec', strain_data', 'linearinterp');
dense_time_data_sec = time_data_sec(1):sample_time:time_data_sec(numel(time_data_sec));
dense_valve_data = feval(valve_fit, dense_time_data_sec);
dense_strain_data = feval(strain_fit, dense_time_data_sec);

figure(1);

subplot(2,1,1);
hold on;
grid on;
plot(time_data_sec, valve_data, '.b--');
plot(dense_time_data_sec, dense_valve_data, 'r--');
xlabel("Time (sec)");
ylabel("Valve (0-255)")
hold off;

subplot(2,1,2);
hold on;
grid on;
plot(time_data_sec, strain_data, 'xb--');
plot(dense_time_data_sec, dense_strain_data, 'r--');
xlabel("Time (sec)");
ylabel("Strain (0-N/A)")
hold off;

data = iddata(dense_strain_data, dense_valve_data, sample_time);
if ENABLE_SAVING_DATA == true
    save(strcat("Saved-Processed-Data\iddata-", filename), 'data');
end

if ENABLE_PRELIMINARY_TRANSFERFUNCTION == true
    Gs = tfest(data, 4)
    [n,d] = tfdata(Gs,'V');
    disp("zeros:");
    disp(roots(n));
    disp("poles:")
    disp(roots(d));
    
    figure(2);
    rlocus(Gs);
    title('Continuous Root Locus')

    Gz = c2d(Gs, sample_time);
    [nz,dz] = tfdata(Gz,'V');
    figure(3);
    rlocus(Gz);
    title('Discrete Root Locus (w ZOH)')
end