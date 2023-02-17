function filtered_data = delete_elements(unfiltered_data, removing_indicies)
    if numel(removing_indicies) ~= numel(unique(removing_indicies))
        error("Repeating indicies to remove not allowed");
    end
    
    for i = numel(removing_indicies):-1:1
        unfiltered_data(removing_indicies(i)) = [];
    end
    
    filtered_data = unfiltered_data;
end

