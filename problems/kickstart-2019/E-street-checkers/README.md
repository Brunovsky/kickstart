# kickstart-2019 - E-street-checkers

Solved

Time: several hours, pretty hard

Had an initial bug in `sieve()` where `k` was `(p + L - 1) / L`
instead of `(p + L - 1) / p`. Caused me a lot of headaches.
It wasn't a correctness issue because of the loop that followed and was
slowing things down considerably.

But once fixed the program went mega turbo, it takes mere milliseconds
to solve test set 2.
