# Test performance of sort
option fail 0
option malloc 0
new
ih RAND 50000
time
lsort
time
free