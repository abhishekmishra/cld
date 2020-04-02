local cld_cmd_util = {}

function cld_cmd_util.display_table(table_data)
    hdrs = {}
    items = 0
    io.write("| ")
    for k, _ in pairs(table_data) do
        io.write(k .. " | ")
        table.insert(hdrs, k)
        if items == 0 then
            items = #table_data[k]
        end
    end
    io.write("\n")
    --print(#hdrs)
    --print(items)
    for i = 1, items do
        io.write("| ")
        for k, v in ipairs(hdrs) do
            io.write(table_data[v][i] .. " | ")
        end
        io.write("\n")
    end
end

return cld_cmd_util