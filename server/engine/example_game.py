import engine
import sys
import gc
import subprocess

class ExampleGame:
    def setupGame(self, players):
        assert len(players) == 2
        self.p1 = players[0]
        self.p2 = players[1]
        self.p1_score = 0
        self.p2_score = 0
        self.gameover = False
        # non output for init

    def playRound(self, turn):
        self.p1.writeToBot(str(turn))
        p1_value = int(self.p1.getResponse(0.2)) # 200ms for response
        
        self.p2.writeToBot(str(turn))
        p2_value = int(self.p1.getResponse(0.2)) # 200ms for response

        diff = p1_value - p2_value

        if diff == -1 or diff == 2:
            self.p1_score += 1
        elif diff == 1 or diff == -2:
            self.p2_score += 1

        if turn >= 100:
            self.gameover = True

    def isGameOver(self):
        return self.gameover

    def finish(self):
        # nothing to clear
        pass

    # get game result: 0 - tie/draw, 1 - p1 win, 2 - p2 win
    def gameResult(self):
        if self.p1_score == self.p2_score:
            return 0
        elif self.p1_score < self.p2_score:
            return 2
        else:
            return 1

def main_once():
    # play once
    game = ExampleGame()
    e = engine.Engine(game)
    e.addPlayer('player1', 'stub/bot0.exe')
    e.addPlayer('player2', 'stub/bot0.exe')
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
        exit(main_once())
    else:
        main_stats(100)
