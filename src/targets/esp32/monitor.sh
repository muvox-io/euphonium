#!/bin/bash
cd app
idf.py monitor -p $1
cd ..
