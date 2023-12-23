local digits = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine" }
local digit_regex = "%d"
for _, pattern in pairs(digits) do
	digit_regex = digit_regex .. "|" .. pattern
end

local contains = function(table, elem)
	for i, e in pairs(table) do
		if e == elem then
			return true
		end
	end
	return false
end

local to_num = function(num)
	for index, value in ipairs(digits) do
		if num == value then
			return index
		end
		if num == index then
			return index
		end
	end
	if 0 < num and num < 10 then
		return num
	end
	return nil
end

local get_num = function(nums)
	local tens = nums[1]
	local ones = nums[#nums]
	return 10 * to_num(tens) + to_num(ones)
end

local read_nums = function()
	local line = io.read("L")
	if line == nil then return nil end
	local ret = {}
	for num in string.gmatch(line, digit_regex) do
		ret[#ret + 1] = num
	end
	print(ret)
	return ret
end

-- lines = io.read("a", "1.in")

print(digit_regex)
local sum = 0
while true do
	local nums = read_nums()
	if nums == nil then break end
	for i, value in ipairs(nums) do
		print(value)
	end
	sum = sum + get_num(nums)
end

print(sum)
