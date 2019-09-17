#/bin/bash

git_dir="/home/jconnor/sloeber-workspace/Z_seifenkiste_0/"
eclipse_project_dir="/home/jconnor/Projects/Seifenkiste_0/"

echo "creating links to eclipse project files"
#echo "eclipse project dir = "$eclipse_project_dir
#echo "git repo dir = "$git_dir

fname="seifenkiste_0.ino"
git_name="$git_dir$fname"
source_name="$eclipse_project_dir$fname"
echo "$git_name   ->  $source_name"
ln "$git_name" "$source_name"

fname="pwm.h"
git_name="$git_dir$fname"
source_name="$eclipse_project_dir$fname"
echo "$git_name   ->  $source_name"
ln "$git_name" "$source_name"
