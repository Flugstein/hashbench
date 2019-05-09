#!/bin/bash

COUNTER=0
while [  $COUNTER -le 30  ]; do
    openssl rand -out testfiles/2_${COUNTER} -base64 $(( 2**${COUNTER} * 3/4 ))
    let COUNTER=COUNTER+1
done