#!/bin/bash

# Verify the quantity of inputs to generate
echo "How many inputs do you want to generate?"
echo "1 - Just one"
echo "2 - A range"
read -p "Option (Choose 1 or 2): " CHOICE

# Verify the size of the grid to generate
echo "What is the size of the grid?"
read -p "Rows: " ROW
read -p "Columns: " COL

EXEC=generateInput

# File name
read -p "Enter the file name: " FILE_NAME

# Verify the user choice
    # Generate just one input
if [[ "$CHOICE" == "1" ]]; then
    ./$EXEC $ROW $COL $FILE_NAME.in

    # Generate a range of inputs
elif [[ "$CHOICE" == "2" ]]; then
    read -p "Enter the quantity of files to generate: " QTD
    
    for((i = 1; i <= QTD; i++))
    do
        ./$EXEC $ROW $COL ${FILE_NAME}_$i.in
    done
else
    echo "Invalid option"
fi