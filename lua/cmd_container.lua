json = require("json")
docker = require("luaclibdocker")

function cmd_ctr_ls(d, options, args)
    all = false
    limit = 0
    size = true

    if options ~= nil then
        if options["--all"] ~= nil then
            all = options["--all"].val
        end
    end

    ctr_ls_str = d:container_ls(all, limit, size)
    ctr_ls = json.decode(ctr_ls_str)
    -- print(ctr_ls_str)

    output = {}
    for k, v in ipairs(ctr_ls) do
        ctr = {}
        ctr["Command"] = v.Command
        ctr["CreatedAt"] = v.Created
        ctr["ID"] = v.Id
        ctr["Image"] = v.Image
        ctr["Labels"] = v.Labels
        ctr["LocalVolumes"] = v.Mounts
        ctr["Mounts"] = v.Mounts
        ctr["Names"] = v.Names
        ctr["Networks"] = v.NetworkSettings.Networks
        ctr["Ports"] = v.Ports
        ctr["RunningFor"] = nil
        ctr["Size"] = v.SizeRootFs
        ctr["Status"] = v.Status

        table.insert(output, ctr)
        -- print("Container #" .. k .. " is " .. v.Names[1])
    end
    return output
end
