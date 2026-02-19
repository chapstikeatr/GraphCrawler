Homework for my Para Comp class
This file operates by taking in the name for some starting node in a Celebrity DB

first we run make to make our file included in my file is rapidjson which if not in
your directory you can not use make.

if rapidjson is not in the directory use
git clone <https://github.com/Tencent/rapidjson.git>
in the directory and the make file will handle the rest.

EX.) ./graphCrawler "Tom Hanks" 2

which will got to all of the neighbors of Tom Hanks (the movies the actor is in) and then find
the neighbors of those movies (the actors that are in the movies) and so on.

to benchmark we use centaurus and it uses slurm in the file is my benchmarking script
in centaurus.
To run it use:
sbatch bench_graphCrawler.sh

The benchmark took:
3.3 Secs for depth of 2
72.09 Secs for depth of 3
480.07 secs for depth of 4
