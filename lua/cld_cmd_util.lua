local cld_cmd_util = {}

cld_cmd_util.table_sep = "  "

function cld_cmd_util.display_table(o)
    headers = o.headers
    table_data = o.data
    column_widths = o.column_widths

    fmtspecs = {}
    if column_widths ~= nil then
        for k, v in pairs(column_widths) do
            fmtspecs[k] = "%-" .. (v + 1) .. "." .. v .. "s"
        end
    end
    items = 0
    for _, k in ipairs(headers) do
        if o.show_headers then
            if fmtspecs[k] ~= nil then
                io.write(string.format(fmtspecs[k], k) .. cld_cmd_util.table_sep)
            else
                io.write(k .. cld_cmd_util.table_sep)
            end
        end
        if items == 0 then items = #table_data[k] end
    end
    if o.show_headers then io.write("\n") end
    for i = 1, items do
        for _, k in ipairs(headers) do
            if fmtspecs[k] ~= nil then
                io.write(string.format(fmtspecs[k], table_data[k][i]) ..
                             cld_cmd_util.table_sep)
            else
                io.write(table_data[k][i] .. cld_cmd_util.table_sep)
            end
        end
        io.write("\n")
    end
end

function cld_cmd_util.print_options(options)
    for opt, opt_val in pairs(options) do
        io.write(opt .. " [")
        io.write("name=" .. opt_val["name"] .. " ")
        if opt_val["short_name"] ~= nil then
            io.write("short_name=" .. tostring(opt_val["short_name"]) .. " ")
        end
        if opt_val["val"] ~= nil then
            io.write("value=" .. tostring(opt_val["val"]) .. " ")
        end
        if opt_val["default_val"] ~= nil then
            io.write("default=" .. tostring(opt_val["default_val"]) .. " ")
        end
        if opt_val["description"] ~= nil then
            io.write("description=" .. tostring(opt_val["description"]) .. " ")
        end
        io.write(']\n')
    end
end

function cld_cmd_util.has_option(options, key) return options[key] ~= nil end

function cld_cmd_util.option_val(options, key)
    if cld_cmd_util.has_option(options, key) then
        return options[key]["val"]
    else
        return nil
    end
end

function cld_cmd_util.filters_to_list(filters)
    filters_ls = {}
    for w in string.gmatch(filters, "[%w<>:-/@]+=[%w<>:-/@]+") do 
        count = 0
        key = nil
        val = nil
        for k in string.gmatch(w, "[%w<>:-/@]+") do
            if count == 0 then 
                key = k
            else
                val = k
            end
            count = count +1
        end
        if filters_ls[key] == nil then
            filters_ls[key] = {}          
        end
        table.insert(filters_ls[key], val)
    end
    return filters_ls
end

return cld_cmd_util
