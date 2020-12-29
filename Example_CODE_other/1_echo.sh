#!/bin/sh
 
#these if statements will check input and place default values if no input is given
#they will also check if input is a number so you can call 
#this script with just a time and it will still work correctly
 
if [ "$1" = '' ]; then
    name="You"
else
    if echo "$1" | egrep -q '^[0-9]+$'; then
        name="You"
    else
        name="$1"
    fi
fi
 
if [ "$2" = '' ]; then
    every="5"
else
    every="$2"
fi
 
if echo "$1" | egrep -q '^[0-9]+$'; then
    every="$1"
fi
 
#endless loop, will print the message every X seconds as indicated in the $every variable
 
while [ 1 ]; do 
    echo "Hey, $name, it's time to get up"
    sleep $every
done
 
exit 0