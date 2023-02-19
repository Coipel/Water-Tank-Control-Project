clc; clear; close all;
format compact; 

runtime_sec = 1800;
sample_rate_sec = 2;
sample_size = runtime_sec/sample_rate_sec;

plant_sys = tf([0, 0.003419683395759, -0.003404915952542],...
         [1, -1.987115795703846, 0.987183059301988], sample_rate_sec);

plant = diff_eq(plant_sys);
plant_record = nan(1,sample_size);
actuation_record = nan(1,sample_size);

reference = 10;
error = 0;
P = 226.779344667868;
for i = 1:sample_size+1
    actuation = P*error;
    
    % Saturation:
    if actuation > 137
        actuation = 137;
    elseif actuation < 0
        actuation = 0;
    end
    
    actuation_record(i) = actuation;
    plant_output = plant.evaluate(actuation);
    plant_record(i) = plant_output;
    error = reference - plant_output;
end

time_domain = 0:sample_rate_sec:runtime_sec;
figure(1);
subplot(2,1,1);
plot(time_domain, plant_record, 'bx-');
grid on;
subplot(2,1,2);
plot(time_domain, actuation_record, 'bx-');
grid on;
