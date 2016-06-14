import sys

sys.path.append('../server/engine')

import engine
import gc
import subprocess
#import game

from ctypes import *
gamelib = cdll.LoadLibrary('game.so')

class Game(object):
    def __init__(self):
        self.obj = gamelib.CreateGame()
    
    def process_turn(self, p1c, p1r, p2c, p2r):
        return gamelib.Game_process_turn(self.obj, p1c, p1r, p2c, p2r)

    def get_map_str(self, mapid):
        return gamelib.Game_get_map_str(self.obj, mapid)

    def get_color(self, id, idx):
        return gamelib.Game_get_color(self.obj, id, idx)

    def get_score1(self):
        return gamelib.Game_get_property(self.obj, 0)

    def get_score2(self):
        return gamelib.Game_get_property(self.obj, 1)

    def get_gameover(self):
        return gamelib.Game_get_property(self.obj, 2)

    def get_result(self):
        return gamelib.Game_get_property(self.obj, 3)

    score1 = property(get_score1)
    score2 = property(get_score2)
    gameover = property(get_gameover)
    result = property(get_result)

gamelib.CreateGame.restype = c_void_p
gamelib.DestroyGame.argtypes = [ c_void_p ]
gamelib.Game_process_turn.argtypes = [ c_void_p, c_int, c_int, c_int, c_int ]
gamelib.Game_get_map_str.argtypes = [ c_void_p, c_int]
gamelib.Game_get_map_str.restype = c_char_p
gamelib.Game_get_color.argtypes = [ c_void_p, c_int, c_int ]
gamelib.Game_get_property.argtypes = [ c_void_p, c_int ]

class SmashGame:
    def setupGame(self, players):
        assert len(players) == 2
        self.p1 = players[0]
        self.p2 = players[1]
        self.game = Game()

    def playRound(self, turn):
        colors = []
        for i in range(8):
            colors.append((self.game.get_color(0, i), self.game.get_color(1, i)))

        map1 = self.game.get_map_str(1).rstrip()
        map2 = self.game.get_map_str(2).rstrip()
        score1 = self.game.score1
        score2 = self.game.score2

        for i in range(8):
            self.p1.writeToBot("%d %d" % colors[i])
        self.p1.writeToBot(str(score1))
        self.p1.writeToBot(map1)
        self.p1.writeToBot(str(score2))
        self.p1.writeToBot(map2)
        
        p1_col, p1_rot = map(int, self.p1.getResponse(0.1).split(' '))
        
        for i in range(8):
            self.p2.writeToBot("%d %d" % colors[i])
        self.p2.writeToBot(str(score2))
        self.p2.writeToBot(map2)
        self.p2.writeToBot(str(score1))
        self.p2.writeToBot(map1)
        
        p2_col, p2_rot = map(int, self.p2.getResponse(0.1).split(' '))

        self.game.process_turn(p1_col, p1_rot, p2_col, p2_rot)

    def isGameOver(self):
        return self.game.gameover

    def finish(self):
        # nothing to clear
        pass

    # get game result: 0 - tie/draw, 1 - p1 win, 2 - p2 win
    def gameResult(self):
        return self.game.result

def main_once():
    # play once
    game = SmashGame()
    e = engine.Engine(game)
    e.addPlayer('player1', 'bots/simple/simple')
    e.addPlayer('player2', 'bots/simple/simple2')
    e.start()
    return game.gameResult()

def main_stats(n):
    stats = [0, 0, 0]
    for i in range(n):
        ret = main_once()
        #subprocess.call(['python', 'example_game.py', '1'])
        if i % 10 == 0:
            print "%d/%d" % (i, n)
        stats[ret] += 1
    print stats

if __name__ == '__main__':
    if len(sys.argv) > 1:
        num = int(sys.argv[1])
    else:
        num = 20
        
    main_stats(num)
