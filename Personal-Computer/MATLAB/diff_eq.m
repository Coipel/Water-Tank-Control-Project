classdef diff_eq < handle
    % difference equation evaluator for maximum second order system (for
    % now) where order of input is less than output (stability condition)
    
    % [NOTE TO SELF] the "< handle" syntax results in this class working
    % exactly as I am used to for other programming languages (in that
    % attributes are modifable and not copies inside methods like MATLABs 
    % default classes)

    properties
        Gz; % Discrete transfer function (must contain sample rate info)
        prior_output; % A vector of two elements [y[k-1], y[k-2] when k=0];
        prior_input; % A vector of two elements [u[k-1], u[k-2] when k=0];

        numerator;
        denominator
    end
    
    methods
        function self = diff_eq(Gz, prior_output_inital, prior_input_inital)
            if nargin == 3
                self.prior_output = prior_output_inital;
                self.prior_input = prior_input_inital;
            elseif nargin == 2
                error("Missing input inital values");
            else
                self.prior_output = [0,0];
                self.prior_input = [0,0];
            end
                
            [numerator, denominator] = tfdata(Gz,'V');

            % Ensure normalized numerator and denominator (where a0 = 1)
            self.numerator = numerator./denominator(1);
            self.denominator = denominator./denominator(1);
        end
        
        function output = evaluate(self, input)
            % evaluates difference equation for a given input with
            % consideration to prior inputs and prior outputs tracked in
            % the object
            

            output = -self.denominator(2)*self.prior_output(1)...
                     -self.denominator(3)*self.prior_output(2)...
                     +self.numerator(2)*self.prior_input(1)...
                     +self.numerator(3)*self.prior_input(2);
            
            self.prior_output(2) = self.prior_output(1);
            self.prior_output(1) = output;
            self.prior_input(2) = self.prior_input(1);
            self.prior_input(1) = input;
        end
    end
end

