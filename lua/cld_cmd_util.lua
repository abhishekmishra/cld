local cld_cmd_util = {}

cld_cmd_util.table_sep = "  "

function cld_cmd_util.display_table(o)
    headers = o.headers
    table_data = o.data
    column_widths = o.column_widths

    fmtspecs = {}
    if column_widths ~= nil then
        for k, v in pairs(column_widths) do
            fmtspecs[k] = "%-" .. (v+1) .. "." .. v .. "s"
        end
    end
    items = 0
    for _, k in ipairs(headers) do
        if fmtspecs[k] ~= nil then
            io.write(string.format(fmtspecs[k], k) .. cld_cmd_util.table_sep)
        else
            io.write(k .. cld_cmd_util.table_sep)
        end
        if items == 0 then
            items = #table_data[k]
        end
    end
    io.write("\n")
    for i = 1, items do
        for _, k in ipairs(headers) do
            if fmtspecs[k] ~= nil then
                io.write(string.format(fmtspecs[k], table_data[k][i]) .. cld_cmd_util.table_sep)
            else
                io.write(table_data[k][i] .. cld_cmd_util.table_sep)
            end
        end
        io.write("\n")
    end
end

return cld_cmd_util