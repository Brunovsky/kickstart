# CodeJam 2019 - Pottery Lottery

## [Pottery Lottery (23pts)](https://codingcompetitions.withgoogle.com/codejam/round/0000000000051679/00000000001461c8#)

Solved

Read V "good vases" (initially all good vases), pick the V/2 with the highest
expected number of tickets on day 100, and discard them into the set of "bad vases", so
we'll have V/2 good vases left.
Try to maximize the expected number of players among the bad vases (sabotage them),
and do not touch the good vases. Stop sabotaging the bad vases when the expected
difference in number of tickets between the best good vase and the best bad vase is larger
than a THRESHOLD.
Repeat, until we have one day left or one good vase left.
At the end, put a ticket into the best vase among the good vases.

* Time: 4 hours
* Complexity: -
* Memory: -
