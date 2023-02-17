% Have time_data_sec variable in the workspace
clc;
format long

time_difference_sec = nan(1,numel(time_data_sec)-1);
for i = 1:numel(time_difference_sec)
    time_difference_sec(i) = time_data_sec(i+1) - time_data_sec(i);
end

disp("---Time-Data-Stats---");
disp("Max Time Difference (sec):"); disp(max(time_difference_sec));
disp("Min Time Difference (sec):"); disp(min(time_difference_sec));
disp("Average Time Difference (sec):"); disp(mean(time_difference_sec));
disp("Standard Deviation Time Difference (sec):"); disp(std(time_difference_sec));
