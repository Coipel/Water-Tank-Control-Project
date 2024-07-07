classdef ShifterArray < handle
    properties
        array = [];
        recent_value = [];
    end

    properties (Access = private)
        size;
    end
    
    methods
        function self = ShifterArray(size)
            self.size = size;
            self.array = nan(1,size);   
        end

        function initialize(self,value)
            self.array = value.*ones(1,self.size);
        end
        
        function insert(self,entry)
            temp = nan(1,self.size);
            temp(1:end-1) = self.array(2:end);
            temp(end) = entry;
            self.recent_value = entry;
            self.array = temp;      
        end

        function clear(self)
            self.array = nan(1,self.size);
        end

        function full = isfull(self)
            if any(isnan(self.array))
                full = false;
            else
                full = true;
            end
        end
    end
end

