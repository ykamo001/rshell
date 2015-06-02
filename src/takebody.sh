#! /bin/bash

# Will take in two parameters, receiver, and sender

receiver=$1
sender=$2

domain=`cut -d "@" -f 2 <<< "$receiver"`

echo "open mail.cs.ucr.edu 25"
sleep 2

echo "helo $domain"
sleep 2

echo "mail from: $2"
sleep 2

echo "rcpt to: $1"
sleep 2

echo "data"
sleep 2

echo "Subject: This email is spam"
sleep 2

echo ""
sleep 2

echo "$bodytemp"
sleep 2

echo "."
sleep 2

echo ""
sleep 2

echo "quit"
sleep 2
