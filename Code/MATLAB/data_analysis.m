% Go into "Saved-Data" directory and load the data first

% transfer-function configurations
sample_period_s = 0.1;
n_zeros = 0;
n_poles = 1;

figure(1);
subplot(2,1,1);
plot(time_s,strain,'bx-');
grid on;
subplot(2,1,2);
plot(time_s,input,'rx-');
ylim([0,255]);
grid on;

plant_model_dt = tfest(input,strain,n_poles,n_zeros,'Ts',sample_period_s) %#ok<NOPTS>
figure(2);
step(255*plant_model_dt);

[controller_p, info] = pidtune(plant_model_dt,"P") %#ok<ASGLU,NOPTS>
[controller_pi, info] = pidtune(plant_model_dt,"PI") %#ok<ASGLU,NOPTS>
[controller_pid, info] = pidtune(plant_model_dt,"PID") %#ok<NOPTS>
