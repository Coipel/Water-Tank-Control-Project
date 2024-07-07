classdef PI_Controller < handle
    properties
        Kp;
        Ki;
        sampling_period;
        offset = 0;   
    end

    properties (Access = private)
        prior_error = 0;
        prior_x = 0;
        lowerbound;
        upperbound;
    end
    
    methods
        function self = PI_Controller(Kp, Ki, sampling_period, bounds, offset)
            self.Kp = Kp;
            self.Ki = Ki;
            self.sampling_period = sampling_period;

            self.lowerbound = bounds(1);
            self.upperbound = bounds(2);
            
            self.offset = offset;
        end

        function actuation = calculate(self, error)
            x = self.prior_x + self.Kp*error + (self.Ki*self.sampling_period - self.Kp)*self.prior_error;
            self.prior_error = error;
            self.prior_x = x;
    
            x = x + self.offset;

            if x < self.lowerbound
                x = self.lowerbound;
            elseif x > self.upperbound
                x = self.upperbound;
            else
                % x is unchanged
            end
            actuation = x;
        end
    end
end

