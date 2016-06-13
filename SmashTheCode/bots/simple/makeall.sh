flag="-O2 -g -std=c++11"
bots="naive simple simple2"
for bot in $bots
do
    g++ $flag -o $bot $bot.cpp
done
