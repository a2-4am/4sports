-- Some games need custom scripts to check for a successful launch,
-- generally because they wait for input before switching to
-- graphics mode. The game-specific function here must include all
-- commands, including the appropriate wait-for function(s).
--
-- Games not listed here use the default script, which just waits
-- for graphics mode.

replay = require("replay")
apple2 = require("apple2")

return {
  ["Bejeweled"] = function()
    replay.WaitForAddressEquals(0x0413, 0xC2) --'B' in 'Bejeweled'
    apple2.Type("R")
    replay.WaitForGraphicsMode()
  end,
  ["Craps"] = function()
    replay.WaitForScreenContains("SOFTAPE")
    apple2.ReturnKey()
    replay.WaitForGraphicsMode()
  end,
  ["Family Feud"] = function()
    replay.WaitForScreenContains("SHAREDATA")
    apple2.Type(" ")
    replay.WaitForGraphicsMode()
  end,
  ["Fastgammon"] = function()
    replay.WaitForScreenContains("NEW SEQUENCE OF ROLLS")
    apple2.Type("1")
    replay.WaitForGraphicsMode()
  end,
  ["Go Four It"] = function()
    replay.WaitForScreenContains("NEW GAME")
    apple2.ReturnKey()
    replay.WaitForGraphicsMode()
  end,
  ["Roulette"] = function()
    replay.WaitForScreenContains("ROULETTE")
    apple2.ReturnKey()
    replay.WaitForScreenContains("INSTRUCTIONS")
    apple2.ReturnKey()
    replay.WaitForScreenContains("BET")
    apple2.ReturnKey()
    replay.WaitForGraphicsMode()
  end,
  ["Sudoku"] = function()
    replay.WaitForScreenContains("NEW GAME")
    apple2.ReturnKey()
    replay.WaitForGraphicsMode()
  end,



--TODO:
-- 13/Card Sharks
-- 28/Flight Simulator II
-- 56/Pitstop II
-- 86/The Dam Busters
-- 89/The Games: Winter Edition
-- 97/World Games
-- 98/Wipeout
-- 99/World Karate Championship



}
