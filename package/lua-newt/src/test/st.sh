#!/bin/bash

{ fname=$( { { ./fm.lua; } 1>&4 ; } 3>&1); } 4>&1

echo "$fname"



