json = require("json")
docker = require("luaclibdocker")
cld_cmd_util = require("cld_cmd_util")

local cld_cmd_container = {}

function cld_cmd_container.dummy(d) print("Running dummy fn") end

function cld_cmd_container.ls(d, options, args)
    all = cld_cmd_util.option_val(options, "all")
    filter = cld_cmd_util.option_val(options, "filter")
    format = cld_cmd_util.option_val(options, "format")
    size = cld_cmd_util.option_val(options, "size")
    last = cld_cmd_util.option_val(options, "last")
    latest = cld_cmd_util.option_val(options, "latest")
    no_trunc = cld_cmd_util.option_val(options, "no-trunc")
    quiet = cld_cmd_util.option_val(options, "quiet")

    ctr_ls_str = d:container_ls(all, cld_cmd_util.option_val(options, "last"),
                                cld_cmd_util.option_val(options, "size"))
    ctr_ls = json.decode(ctr_ls_str)

    output = {}
    for k, v in ipairs(ctr_ls) do
        c = {}
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

    if cld_cmd_util.option_val(options, "quiet") then
        o = cld_cmd_container.ls_format_quiet(output)
    else
        o = cld_cmd_container.ls_format(output)
    end

    cld_cmd_util.display_table(o)

    return json.encode(o)
end

function cld_cmd_container.ls_format(output, options)
    o = {
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
    o = {
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

return cld_cmd_container
