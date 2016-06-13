# coding: utf-8

import ioplayer
class Engine:
    def __init__(self, logic):
        self.logic = logic
        self.isRunning = False
        self.players = []

    def addPlayer(self, name, command):
        player = ioplayer.IOPlayer(command, name)
        self.players.append(player)
        return player

    def start(self):
        turn = 0
        self.isRunning = True
        self.logic.setupGame(self.players)

        while self.isRunning:
            turn += 1
            self.logic.playRound(turn)
            if self.logic.isGameOver():
                self.isRunning = False
                self.logic.finish()
                for player in self.players:
                    player.finish()

# logic function used:
#       setupGame( [ Players ] )
#       playRound( round_number )
#       isGameOver()
#       finish()
    
