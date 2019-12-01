#/bin/bash

# this script creates hard links to a list of files (to be given below)
# this is to avoid using git in eclipse and having the eclipse workspace as the project directory

# the script counts all cpp, h, ino files in a source-eclipse directory
# it creates new links to those files (if they exist)
# and counts the number of completed links
 
separator="--------------------------"
eclipse_project_dir="/home/jconnor/sloeber-workspace/Z_seifenkiste_0/"
git_dir="/home/jconnor/Projects/Seifenkiste_0/"

echo "creating links to eclipse project files"
candidate_count=$(ls -la "$eclipse_project_dir" | grep -E '.cpp|.h|.ino' | grep -v ".ino.cpp" | wc -l)

update_link () {
    file_location="$eclipse_project_dir$1"
    link_location="$git_dir$1"
    echo $separator
    echo "updating "$link_location
    echo "sourced from "$file_location
    if [ -f $file_location ]; then
        echo "target file exists"
    else
        echo "warning - target file does not exist             < -------------------------"
        return 0
    fi

    if [ -f $link_location ]; then
        echo "link exists - delete"
        #return 2
        unlink $link_location
    fi    

    # ln arg1 = original file location      arg2 = link location
    ln "$file_location" "$link_location"
    if [ "$?" -eq "0" ]; then
       return 1
    fi
    return 0
}

filenames="seifenkiste_0.ino
pwm.h
pwm.cpp
portkextint.h
portkextint.cpp
common.h
common.cpp
controllercalibration.h
controllercalibration.cpp
portdtwiandint.h
portdtwiandint.cpp"

#filenames="test.cpp"
copy_list_count=$(echo "$filenames" | wc -w)

result=0
for f in $filenames; do
    #echo $f
    update_link $f
    update_result=$?
    echo "update result = "$update_result
    let "result+=$update_result"
done

echo $separator
echo "found $candidate_count candidates"
echo "copy list contained $copy_list_count"
echo "successfully linked $result"
