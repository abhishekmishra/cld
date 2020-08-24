local class = require 'lib.middleclass'

local CLD = class('CLD')

function CLD:initialize(docker_conn)
    self.docker_conn = docker_conn
    self.d = self.docker_conn
end

CLD.static.container = require('cld_container')
CLD.static.ctr = CLD.static.container

function CLD:run(module, command, options, args)
    return CLD.static[module][command](self.d, options, args)
end

return CLD

-- OLD IMPLEMENTATION
-- local cld_cmd = {}

-- cld_cmd.ctr = require('cld_container')
-- cld_cmd.container = cld_cmd.ctr

-- return cld_cmd
