classdef Tank < handle

    properties
        time_record;
        strain_record_raw;
        strain_record;
        power_record;
        setpoint_record;

        strain_offset = 0;
    end

    properties (Access = private)
        connection;
        calibration_samples = 10;
        sampling_period_s = 0.1;
        motor_pause_s;
        lowerbound;
        upperbound;
    end
    
    methods
        function self = Tank()  
            self.set_moving_average_samples(1);
        end

        function set_motor_pause_s(self, motor_pause_s)
            self.motor_pause_s = motor_pause_s;
        end

        function set_samples(self, samples)
            self.time_record = ShifterArray(samples);
            self.strain_record = ShifterArray(samples);
            self.power_record = ShifterArray(samples);
            self.setpoint_record = ShifterArray(samples);  
        end

        function set_moving_average_samples(self, samples)
            self.strain_record_raw = ShifterArray(samples);
            self.strain_record_raw.initialize(0);
        end

        function set_bounds(self, bounds)
            self.lowerbound = bounds(1);
            self.upperbound = bounds(2);
        end
        
        function open_connection(self, serial_port, baudrate)
            self.connection = serialport(serial_port, baudrate);
            self.calibrate_strain_gauge();
        end

        function close_connection(self)
            self.connection = [];
        end

        function strain = get_strain(self)
            % For whatever reason on Windows OS first message sent is junk so
            % data validation was added (good practice anyways)
            data_received = false;
            while ~data_received
                self.connection.flush("input");
                message_raw = self.connection.writeread("arduino:strain");
                data_received = ~boolean(strfind(message_raw, "unknown command"));
            end
            
            message = split(message_raw,':');
            strain_raw = str2double(message(2));
            strain = strain_raw - self.strain_offset;   
        end

        function strain = get_strain_w_pause(self)
            self.set_power(0);
            pause(self.motor_pause_s);
            strain = self.get_strain();
        end

        function strain_filtered = get_strain_filtered(self)
            strain = self.get_strain_w_pause();
            strain = self.clipper(strain);
            self.strain_record_raw.insert(strain);
            strain = mean(self.strain_record_raw.array);
            strain_filtered = strain;
        end

        function set_calibration_settings(self, calibration_samples, sampling_period_s)
            self.calibration_samples = calibration_samples;
            self.sampling_period_s = sampling_period_s;
        end

        function calibrate_strain_gauge(self)
            disp("Calibrating Strain Gauge...");
            self.set_power(0);
            pause(self.motor_pause_s);
            self.strain_offset = 0;
            sum = 0;
            for i = 1:self.calibration_samples
                strain = self.get_strain();
                pause(self.sampling_period_s);
                sum = sum + strain;
            end
            self.strain_offset = sum/self.calibration_samples;
            disp("Calibration Complete...");
        end

        function set_power(self,value)
            writeline(self.connection,strcat("power:",string(uint8(value))));
        end
        
        function clear(self)
            self.time_record.clear();
            self.strain_record.clear();
            self.power_record.clear();
            self.setpoint_record.clear();
            self.calibrate_strain_gauge();
        end
  
        function clipped_value = clipper(self,value)
            if value < self.lowerbound
                clipped_value = self.lowerbound;
            elseif value > self.upperbound
                clipped_value = self.upperbound;
            else
                clipped_value = value;
            end
        end
    end
end
