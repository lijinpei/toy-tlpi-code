local function get_define()
  local process = require("process")
  local chd = process.exec("cpp", {"-dM"})
  chd:stdin("#include <errno.h>")
  process.close(chd:fds())

  local datas = ""
  repeat
    local data, again
    data, err, again = chd:stdout()
    if data then
      datas = datas .. data
    end
  until not data and (not err and not again)

  local table = {}
  for line in datas:gmatch("[^\n]*\n?") do
    p1, p2 = line:match("#define +(E[A-Z0-9]*) +([A-Z0-9]+)")
    if p1 then
      n2 = tonumber(p2)
      if n2 then
        table[n2] = p1
      else
        if table[p2] then
          table[p2] = p1 .. "/" .. table[p2]
          print(p1 .. "/" .. table[p2])
        else
          table[p2] = p1
        end
      end
    end
  end
  return table
end

local function generate_c(table, output)
  local nr = #table
  local function format1(n)
    return string.format("    /* %3d */", n)
  end
  local function format2(n)
    local e1 = table[n] or ""
    local e2 = table[e1]
    local str = " \""
    if e2 then str = str .. e1 .. "/" .. e2 else str = str .. e1 end
    if n < nr then
      str = str .. "\","
    else
      str = str .. "\""
    end
    return str
  end
  local max_ln = 80
  local next_nr = 1
  local line = format1(next_nr)
  output:write("static const char *ename[] = {\n")
  output:write(format1(0) .. " \"\",\n")
  repeat
    local str = format2(next_nr)
    if #line + #str + 2 > max_ln then
      output:write(line .. "\n")
      line = format1(next_nr)
    end
    line = line .. str
    next_nr = next_nr + 1
  until next_nr >= nr
  output:write(line .. "\n")
  output:write([[};
const int MAX_ENAME = ]] .. nr .. ";\n")
end

local function generate_scheme(table, output)
  output:write("'((0 . \"\")\n")
  for nr = 1,#table do
    local e1 = table[nr] or ""
    local e2 = table[e1]
    local str
    if e2 then
      str = e1 .. "/" .. e2
    else
      str = e1
    end
    str = "\"" .. str .. "\""
    output:write(string.format("(%d . %s)\n", nr, str))
  end
  output:write(")")
end

if not arg[1] or not arg[2] then
  print("Usage: ", arg[0], "[c|scheme] output_file")
  os.exit(false)
end

local generate_funcs = {c = generate_c, scheme = generate_scheme}
local func = generate_funcs[arg[1]]

if not func then
  print("Error: unable to generate output the specified language ", arg[1])
  os.exit(false)
end

local table = get_define()
local output = io.open(arg[2], "w+")
func(table, output)
output:close()

