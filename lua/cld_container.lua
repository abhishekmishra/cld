json = require("json")
docker = require("luaclibdocker")
cld_cmd_util = require("cld_cmd_util")

local cld_cmd_container = {}

function cld_cmd_container.dummy(d) print("Running dummy fn") end

function cld_cmd_container.ls(d, options, args)
    local all = cld_cmd_util.option_val(options, "all")
    local filter = cld_cmd_util.option_val(options, "filter")
    local format = cld_cmd_util.option_val(options, "format")
    local size = cld_cmd_util.option_val(options, "size")
    local last = cld_cmd_util.option_val(options, "last")
    local latest = cld_cmd_util.option_val(options, "latest")
    local no_trunc = cld_cmd_util.option_val(options, "no-trunc")
    local quiet = cld_cmd_util.option_val(options, "quiet")

    local filters_ls = nil
    if filters ~= nil then filters_ls = cld_cmd_util.filters_to_list(filter) end

    local ctr_ls_str = d:container_ls_filter(all, cld_cmd_util.option_val(
                                                 options, "last"),
                                             cld_cmd_util.option_val(options,
                                                                     "size"),
                                             json.encode(filters_ls))
    local ctr_ls = json.decode(ctr_ls_str)

    local output = {}
    for k, v in ipairs(ctr_ls) do
        local c = {}
        c["Command"] = v.Command
        c["CreatedAt"] = v.Created
        c["ID"] = v.Id
        c["Image"] = v.Image
        c["Labels"] = v.Labels
        c["LocalVolumes"] = v.Mounts
        c["Mounts"] = v.Mounts
        c["Names"] = v.Names
        c["Networks"] = v.NetworkSettings.Networks
        c["Ports"] = v.Ports
        c["RunningFor"] = nil
        c["Size"] = v.SizeRootFs
        c["Status"] = v.Status

        table.insert(output, c)
    end

    local o = nil
    if cld_cmd_util.option_val(options, "quiet") then
        o = cld_cmd_container.ls_format_quiet(output)
    else
        o = cld_cmd_container.ls_format(output)
    end

    cld_cmd_util.display_table(o)

    return json.encode(o)
end

function cld_cmd_container.ls_format(output, options)
    local o = {
        headers = {
            "CONTAINER ID", "IMAGE", "COMMAND", "CREATED", "STATUS", "PORTS",
            "NAMES"
        },
        data = {
            ["CONTAINER ID"] = {},
            ["IMAGE"] = {},
            ["COMMAND"] = {},
            ["CREATED"] = {},
            ["STATUS"] = {},
            ["PORTS"] = {},
            ["NAMES"] = {}
        },
        column_widths = {
            ["CONTAINER ID"] = 15,
            ["IMAGE"] = 15,
            ["COMMAND"] = 25,
            ["CREATED"] = 25,
            ["STATUS"] = 20,
            ["PORTS"] = 25,
            ["NAMES"] = 50
        },
        show_headers = true,
        truncate = false
    }
    for k, c in ipairs(output) do
        local ports_str = ""
        local count = 1
        for _, p in ipairs(c["Ports"]) do
            ports_str = ports_str .. p.IP .. ":" .. p.PublicPort .. "->" ..
                            p.PrivatePort .. "/" .. p.Type
            if count < #c["Ports"] then ports_str = ports_str .. ", " end
        end

        local names_str = ""
        count = 1
        for _, p in ipairs(c["Names"]) do
            names_str = names_str .. p
            if count < #c["Names"] then names_str = names_str .. ", " end
        end

        table.insert(o.data["CONTAINER ID"], c["ID"])
        table.insert(o.data["IMAGE"], c["Image"])
        table.insert(o.data["COMMAND"], c["Command"])
        table.insert(o.data["CREATED"],
                     os.date("%d-%m-%Y:%H:%M:%S", c["CreatedAt"]))
        table.insert(o.data["STATUS"], c["Status"])
        table.insert(o.data["PORTS"], ports_str)
        table.insert(o.data["NAMES"], names_str)
    end
    return o
end

function cld_cmd_container.ls_format_quiet(output, options)
    local o = {
        headers = {"CONTAINER ID"},
        data = {["CONTAINER ID"] = {}},
        column_widths = {["CONTAINER ID"] = 15},
        show_headers = false,
        truncate = false
    }
    for k, c in ipairs(output) do
        table.insert(o.data["CONTAINER ID"], c["ID"])
    end
    return o
end

function cld_cmd_container.top(d, options, args)
    local id = cld_cmd_util.option_val(args, "Container")
    local ctr_ps_str = d:container_top(id)
    -- io.write(ctr_ps_str)
    local ctr_ps = json.decode(ctr_ps_str)
    local o = {
        headers = ctr_ps["Titles"],
        data = {
        },
        column_widths = {
        },
        show_headers = true,
        truncate = false
    }

    for k,v in ipairs(o.headers) do
        o.data[v] = {}
        for k1, v1 in ipairs(ctr_ps["Processes"]) do
            o.data[v][k1] = v1[k]
        end
    end

    for k,v in ipairs(o.headers) do
        o.column_widths[k] = 10
    end

    cld_cmd_util.display_table(o)

    return json.encode(o)
end

return cld_cmd_container
