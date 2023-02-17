function [filtered_data, filtered_indices] = duplicate_filter(unfiltered_data)
% [IMPORTANT] input array cannot have a int datatype as elements will be
% left behind as a 0 rather than being outright removed (this is for NaN's
% get converted into 0 for int datatype in MATLAB). Will force input to be
% a double to prevent this scenario.

unfiltered_data = double(unfiltered_data);

% Intakes an array with duplicate elements and removes them except the
% first instance. New non-duplicant array is returned. Additional output is
% the indicies removed from the unfiltered array so that associated arrays
% to the unfiltered may also have their associated terms deleted
    
    % ---Inital search to determine amount of duplicates---
    % [NOTE] repeated counting of dupliates can/will occur
    count = 0;
    for i = 1:numel(unfiltered_data)
        x = find(unfiltered_data == unfiltered_data(i));
        if numel(x) > 1
            count = count + 1;
        end
    end
    
    % Prior step done to preallocate correct memory below
    value_duplicate = nan(1,count); 
    count = 0;
    for i = 1:numel(unfiltered_data)
        x = find(unfiltered_data == unfiltered_data(i));
        if numel(x) > 1
            count = count + 1;
            value_duplicate(count) = unfiltered_data(x(1));
        end
    end

    % Remove the double counting of what values are duplicants
    value_duplicate = unique(value_duplicate);
    % ---Remove the nonfirst repeated elements from the unfiltered_data---
    count = 0;
    for i = 1:numel(value_duplicate)
        x = find(unfiltered_data == value_duplicate(i));
        %disp("value: ");
        %disp(value_duplicate(i))
        for k = 2:numel(x)
            count = count + 1;   
        end
    end
    
    removed_indicies = nan(1,count);
    count = 0;
    for i = 1:numel(value_duplicate)
        x = find(unfiltered_data == value_duplicate(i));
        for k = 2:numel(x)
            count = count + 1;
            removed_indicies(count) = x(k);
            unfiltered_data(x(k)) = NaN;
        end
    end

    nan_indicies = find(isnan(unfiltered_data) == true);
    % Reverse order needed to not shift indicies when deleting elements and
    % causing wrong elements to be deleted
    for i = numel(nan_indicies):-1:1
        unfiltered_data(nan_indicies(i)) = [];
    end
    
    filtered_data = unfiltered_data;
    if nargout == 2
        filtered_indices = sort(removed_indicies);
    end
end