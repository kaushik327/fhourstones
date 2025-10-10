#!/bin/bash
./SearchGame < test-inputs > test.log

if diff test-outputs <(grep -oE "\(\+\)|\(=\)|\(-\)" test.log); then
    echo "Tests passed!"
else
    echo "Tests failed :(" 
fi
