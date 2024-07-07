classdef SamplingRegulator < handle
    properties
        sampling_period;
        time_reference;
    end

    properties (Access = private)
        prior_target_time = 0;
    end
    
    methods
        function self = SamplingRegulator(sampling_period)
            self.sampling_period = sampling_period;
            self.time_reference = tic;    
        end

        function run_time = get_run_time(self)
            run_time = toc(self.time_reference);
        end
        
        function reset(self)
            self.prior_target_time = 0;
            self.time_reference = tic;
        end

        function hold(self) 
            target_time = self.prior_target_time + self.sampling_period;
            while self.get_run_time() >  target_time
                target_time = target_time + self.sampling_period;
            end
            self.prior_target_time = target_time;
            
            while (self.get_run_time() < target_time)
                % Pass  
            end
        end
    end
end

