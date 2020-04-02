local cld_cmd_util = {}

cld_cmd_util.table_sep = "  "

function cld_cmd_util.display_table(hdrs, table_data, colwdths)
    fmtspecs = {}
    if colwdths ~= nil then
        for k, v in pairs(colwdths) do
            fmtspecs[k] = "%-" .. (v+1) .. "." .. v .. "s"
            -- print(fmtspecs[k])
        end
    end
    items = 0
    -- io.write(cld_cmd_util.table_sep)
    for _, k in ipairs(hdrs) do
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
    --print(#hdrs)
    --print(items)
    for i = 1, items do
        --io.write(cld_cmd_util.table_sep)
        for _, k in ipairs(hdrs) do
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