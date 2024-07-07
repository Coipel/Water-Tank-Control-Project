function tank_app
    % --- User Configurations ---
    % Connection Settings
    serial_port = "/dev/ttyUSB0"; % Windows uses "COM#" system instead
    baudrate = 57600;
    
    % Display and Data Saving Setting
    samples = 64;

    % Operation Rate Settings
    sample_period_s = 2;
    motor_pause_s = 0.7;

    % Unit Conversion Settings
    strain_to_ml_factor = 34;
    
    % Filtering Settings
    moving_average_samples = 1;
    strain_lowerbound = 0;
    strain_upperbound = 3;
    
    % Proportional-Integral Controller Settings
    Kp = 40;
    Ki = 3;
    controller_lowerbound = 180;
    controller_upperbound = 255;
    controller_offset = 180;
    % --- User Configurations ---
    
    
    % --- App Initalization/GUI ---
    figure = uifigure;
    figure.HandleVisibility = "on";
    figure.Name = "Tank-Application";
    
    grid_layout = uigridlayout(figure, [7,2]);
    grid_layout.RowHeight = {'4x','1x','4x','1x','2x','1x','1x'};
    grid_layout.ColumnWidth = {'1x','1x'};
    
    axes_sensor = uiaxes(grid_layout);
    axes_sensor.Layout.Row = 1;
    axes_sensor.Layout.Column = [1,2];
    axes_sensor.XGrid = "on";
    axes_sensor.YGrid = "on";
    axes_sensor.XLabel.String = "Time (seconds)";

    dropdown_units = uidropdown(grid_layout);
    dropdown_units.Layout.Row = 2;
    dropdown_units.Layout.Column = [1,2];
    dropdown_units.Items = ["Strain","Milliliteres"];
  
    axes_power = uiaxes(grid_layout);
    axes_power.Layout.Row = 3;
    axes_power.Layout.Column = [1,2];
    axes_power.XGrid = "on";
    axes_power.YGrid = "on";
    axes_power.XLabel.String = "Time (seconds)";
    axes_power.YLabel.String = "Power (uint8)";

    label = uilabel(grid_layout);
    label.Layout.Row = 4;
    label.Layout.Column = [1,2];
    label.HorizontalAlignment = "center";
    label.Text = "Power (uint8)";
    label.FontSize = 16;

    slider = uislider(grid_layout);
    slider.Limits = [0,255];
    slider.Layout.Row = 5;
    slider.Layout.Column = [1,2];

    data_collection_button = uibutton(grid_layout,"state");
    data_collection_button.Text = "Start Data Collection";
    data_collection_button.Layout.Row = 6;
    data_collection_button.Layout.Column = 1;
    data_collection_enable = false;

    control_system_button = uibutton(grid_layout,"state");
    control_system_button.Text = "Start Control System";
    control_system_button.Layout.Row = 6;
    control_system_button.Layout.Column = 2;
    control_system_enable = false;

    end_application_button = uibutton(grid_layout,"state");
    end_application_button.Text = "Terminate Application";
    end_application_button.Layout.Row = 7;
    end_application_button.Layout.Column = [1,2];
    % --- App Initalization/GUI ---
    
    controller = PI_Controller(Kp, Ki, sample_period_s, [controller_lowerbound, controller_upperbound], controller_offset);
    tank = Tank();
    tank.set_motor_pause_s(motor_pause_s);
    tank.set_samples(samples);
    tank.set_moving_average_samples(moving_average_samples);
    tank.set_bounds([strain_lowerbound, strain_upperbound]);
    tank.open_connection(serial_port, baudrate);
    sampling_regulator = SamplingRegulator(sample_period_s);
    while true
        if (end_application_button.Value)
            tank.set_power(0);
            tank.close_connection();
            close(figure);
            return;
        end
  
        if data_collection_button.Value || control_system_button.Value 
            if data_collection_button.Value
                data_collection_enable = true;
                control_system_enable = false;
                slider.Limits = [0,255];
                slider.MajorTicks = 0:15:255;
                label.Text = "Power (uint8)";
                disp("Data Collection Started...");
            elseif control_system_button.Value
                control_system_enable = true;
                data_collection_enable = false;
                label.Text = "Setpoint (Strain/Milliliters)";
                disp("Control System Started...");
            end
            data_collection_button.Value = false;
            control_system_button.Value = false;
            tank.clear();
            sampling_regulator.reset();
        end
            
            tank.time_record.insert(sampling_regulator.get_run_time());

            if control_system_enable
                if dropdown_units.Value == "Strain"
                    slider.Limits = [0, strain_upperbound];
                    slider.MajorTicks = 0:0.2:strain_upperbound;
                    tank.setpoint_record.insert(slider.Value);
                else
                    slider.Limits = [0, strain_to_ml_factor*strain_upperbound];
                    slider.MajorTicks = 0:5:strain_to_ml_factor*strain_upperbound;
                    tank.setpoint_record.insert(slider.Value/strain_to_ml_factor);
                end
                error = tank.setpoint_record.recent_value - tank.strain_record.recent_value;
                tank.power_record.insert(controller.calculate(error));
                strain = tank.get_strain_filtered();
                tank.set_power(tank.power_record.recent_value);
            else
                tank.power_record.insert(slider.Value); 
                strain = tank.get_strain_w_pause();
                tank.set_power(tank.power_record.recent_value);
            end 
            tank.strain_record.insert(strain);
            

            if (data_collection_enable && tank.time_record.isfull())
                data_collection_enable = false;
                save_data(tank.time_record.array, ...
                          tank.strain_record.array, ...
                          tank.power_record.array);
                disp("Data has been Saved...");     
            end
    
        min_time = min(tank.time_record.array);
        max_time = max(tank.time_record.array);
        if min_time ~= max_time
            axes_sensor.XLim = [min_time; max_time];
            axes_power.XLim = [min_time; max_time];
        end
        
        if dropdown_units.Value == "Strain"
            axes_sensor.YLabel.String = "Strain (unit-less)";
            sensor_data = tank.strain_record.array;
            setpoint_data = tank.setpoint_record.array;      
        else
            axes_sensor.YLabel.String = "Volume (mL)";
            sensor_data = strain_to_ml_factor*tank.strain_record.array;
            setpoint_data = strain_to_ml_factor*tank.setpoint_record.array;   
        end
        
        plot(axes_sensor, ... 
             tank.time_record.array, ... 
             sensor_data, ...
             'bx-', ...
             tank.time_record.array, ... 
             setpoint_data, ...
             'go-', ...
             LineWidth = 2);
                  
        plot(axes_power, ...
            tank.time_record.array, ...
            tank.power_record.array, ...
            'rx-', ...
            LineWidth = 2);

        sampling_regulator.hold();             
    end
end


function save_data(time, strain, power)
    if (~exist("Saved-Data","dir"))
        mkdir("Saved-Data");
    end
    date = datetime("now");
    date.Format = "MMddyy_HHmmss";
    filename = string(date);
    path = strcat("./Saved-Data/", filename);
    save(path,"time","strain","power");
end