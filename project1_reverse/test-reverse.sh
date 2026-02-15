#! /bin/bash

if ! [[ -x reverse ]]; then
    echo "reverse executable does not exist"
    exit 1
fi

./run-tests.sh $*


