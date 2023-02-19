% This script is intended to verify the parallels between a z-transfer
% fucntion and its associated difference equation and to ensure that the
% two are equivalent.

clc; clear; close all;
format compact; 

runtime_sec = 1800;
sample_rate_sec = 2;
sys = tf([0, 0.003419683395759, -0.003404915952542],...
         [1, -1.987115795703846, 0.987183059301988], sample_rate_sec);

uk = 1; % Step input;



sample_size = runtime_sec/sample_rate_sec;

uk_m1 = 0;
uk_m2 = 0;
yk_m1 = 0;
yk_m2 = 0;
yk = 0;

yk_record = nan(1,sample_size);

for i = 1:sample_size+1
    yk = 1.987115795703846*yk_m1 - 0.987183059301988*yk_m2 + 0.003419683395759*uk_m1...
        - 0.003404915952542*uk_m2;
    yk_record(i) = yk;
    yk_m2 = yk_m1;
    yk_m1 = yk;
    uk_m2 = uk_m1;
    uk_m1 = uk;
end

figure(1);
hold on;
step(sys);
plot(0:sample_rate_sec:runtime_sec, yk_record, 'rx-');
hold off;


