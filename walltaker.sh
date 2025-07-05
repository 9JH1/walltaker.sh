#!/bin/sh

# kill other runnin variants 
mypid=$$
for pid in $(pgrep -f ${0##*/}); do
	[[ $pid -ne $mypid ]] && kill $pid
  sleep 1
done 

id="46554" # default 
save_wallpaper_path="$HOME/Pictures/Wallpapers/.walltaker/" # leave empty to not save wallpapers 
delay=10
user_agent="Wallpaper Engine"
cache=""
notifications="1"
debug="0"
wallpaper_set_command='$HOME/.config/i3/src/wal.sh --custom "$location"'

# process arguments 
if [[ "$1" = "--id" ]]; then
	if [[ -n "$2" ]]; then 
		id="$2";
	fi 
elif [[ "$1" = "--path" ]];then
	if [[ -n "$2" ]]; then 
		save_wallpaper_path="$2"
	fi 
elif [[ "$1" = "--delay" ]];then 
	if [[ -n "$2" ]]; then 
		delay="$2"
	fi 
elif [[ "$1" = "--no-i3" ]];then 
	i3="0"
elif [[ "$1" = "--no-notify" ]];then 
	notifications="0"
elif [[ "$1" = "--user-agent" ]];then
	if [[ -n "$2" ]];then
		user_agent="$2";
	fi 
fi

while [[ 1 ]]; do
	response=$(curl -s "https://walltaker.joi.how/api/links/"$id".json" -0 --header "User-Agent: $user_agent")
  url=$(echo $response      | grep -oE '"post_url":".*"'   | sed -e 's/"post_url":"//'   -e 's/".*//')
  set_by=$(echo $response   | grep -oE '"set_by":".*"'     | sed -e 's/"set_by":"//'     -e 's/".*//')
	#time_raw=$(echo $response | grep -oE '"updated_at":".*"' | sed -e 's/"updated_at":"//' -e 's/".*//')
	#time_date=$(echo $time_raw | tr "T" " " | awk '{printf $1}'); 	
	#time_time=$(echo $time_raw | tr "T" " " | awk '{printf $2}' | tr "." " " | awk '{printf $1}');
	description=$(echo $response | grep -oE '"post_description":".*"' | sed -e 's/"post_description":"//' -e 's/".*//')

	if [[ "$debug" = "1" ]]; then echo $response; fi
  if [ -z "$set_by" ]; then set_by="Anonymous"; fi
	
  if [ "$url" != "$cache" ]; then
  	cache=$url
    filename=$(echo $url | sed -e 's/.*\///g')
		extension="${filename##*.}"
		curl -O "$url" &>/dev/null
		location="";
		if [[ "$save_wallpaper_path" = "" ]];then 
			tmp=$(mktemp --suffix ".$extension");
			location=$tmp
		else 
			# seeing as the time vars are from when the wallpaper 
			# was last checked ive had to change this to not include 
			# a timestamp or anything like that, its a bit weird that 
			# there is no response value saying when the wallpaper 
			# was last changed but ah well this works too.
			#
			# location="$save_wallpaper_path$set_by"_"$time_date"_"$time_time.$extension"
			
			location="$save_wallpaper_path$filename"
		fi 
		
		# move the file 
		(mv -f "$filename" "$location" &>/dev/null)
		echo "Wallpaper Saved To: $location"

		echo "Requesting wallpaper refresh"
		eval $wallpaper_set_command &>/dev/null &)
		if [[ "$notifications" = "1" ]];then
			# in my case this script is used along side another
			# script that also sets the color theme of my wm so 
			# having this notification AFTER we set the background 
			# is important as it will show with the NEW wallpapers 
			# color scheme instead of the old one.
			notify-send "New Wallpaper From $set_by" "$description"
		else 
			echo "New Wallpaper From $set_by"
			echo $description
		fi 
	fi
	fi
	sleep $delay
done
